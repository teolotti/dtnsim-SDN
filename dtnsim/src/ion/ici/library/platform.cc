/*
	platform.c:	platform-dependent implementation of common
			functions, to simplify porting.
									*/
/*	Copyright (c) 1997, California Institute of Technology.		*/
/*	ALL RIGHTS RESERVED. U.S. Government Sponsorship		*/
/*	acknowledged.							*/
/*									*/
/*	Author: Scott Burleigh, Jet Propulsion Laboratory		*/
/*									*/
/*	Scalar/SDNV conversion functions written by			*/
/*	Ioannis Alexiadis, Democritus University of Thrace, 2011.	*/
/*									*/
#include "platform.h"

#define	ABORT_AS_REQD		if (_coreFileNeeded(NULL)) sm_Abort()

void	icopy(char *fromPath, char *toPath)
{
#if defined (VXWORKS)
	oK(copy(fromPath, toPath));
#elif defined (RTEMS)
	int	argc = 2;
	char	*argv[2];

	argv[0] = fromPath;
	argv[1] = toPath;
	oK(rtems_shell_main_cp(argc, argv));
#elif defined (mingw)
	oK(CopyFile(fromPath, toPath, 0));
#else
	int	pid = fork();
	int	status;
 
	if (pid)	/*	Parent process.				*/
	{
		waitpid(pid, &status, 0);
	}
	else		/*	Child process.				*/
	{
		execlp("cp", "cp", "--", fromPath, toPath, (char *) 0);
	}
#endif
}

#if defined (VXWORKS)

typedef struct rlock_str
{
	SEM_ID	semaphore;
	int	owner;
	short	count;
	short	init;
} Rlock;		/*	Private-memory semaphore.		*/ 

int	createFile(const char *filename, int flags)
{
	int	result;

	if (filename == NULL)
	{
		ABORT_AS_REQD;
		return ERROR;
	}

	/*	VxWorks open(2) will only create a file on an NFS
	 *	network device.  The only portable flag values are
	 *	O_WRONLY and O_RDWR.  See creat(2) and open(2).		*/

	result = creat(filename, flags);
	if (result < 0)
	{
		putSysErrmsg("can't create file", filename);
	}

	return result;
}

int	initResourceLock(ResourceLock *rl)
{
	Rlock	*lock = (Rlock *) rl;

	if (lock == NULL)
	{
		ABORT_AS_REQD;
		return ERROR;
	}

	if (lock->init)
	{
		return 0;
	}

	lock->semaphore = semBCreate(SEM_Q_PRIORITY, SEM_FULL);
	if (lock->semaphore == NULL)
	{
		return ERROR;
	}

	lock->owner = NONE;
	lock->count = 0;
	lock->init = 1;
	return 0;
}

void	killResourceLock(ResourceLock *rl)
{
	Rlock	*lock = (Rlock *) rl;

	if (lock && lock->init && lock->count == 0)
	{
		oK(semDelete(lock->semaphore));
		lock->semaphore = NULL;
		lock->init = 0;
	}
}

void	lockResource(ResourceLock *rl)
{
	Rlock	*lock = (Rlock *) rl;
	int	tid;

	if (lock && lock->init)
	{
		tid = taskIdSelf();
		if (tid != lock->owner)
		{
			oK(semTake(lock->semaphore, WAIT_FOREVER));
			lock->owner = tid;
		}

		(lock->count)++;
	}
}

void	unlockResource(ResourceLock *rl)
{
	Rlock	*lock = (Rlock *) rl;
	int	tid;

	if (lock && lock->init)
	{
		tid = taskIdSelf();
		if (tid == lock->owner)
		{
			(lock->count)--;
			if (lock->count == 0)
			{
				lock->owner = NONE;
				oK(semGive(lock->semaphore));
			}
		}
	}
}

void	closeOnExec(int fd)
{
	return;		/*	N/A for non-Unix operating system.	*/
}

void	snooze(unsigned int seconds)
{
	struct timespec	ts;

	ts.tv_sec = seconds;
	ts.tv_nsec = 0;
	oK(nanosleep(&ts, NULL));
}

void	microsnooze(unsigned int usec)
{
	struct timespec	ts;

	ts.tv_sec = usec / 1000000;
	ts.tv_nsec = (usec % 1000000) * 1000;
	oK(nanosleep(&ts, NULL));
}

char	*system_error_msg()
{
	return strerror(errno);
}

#ifndef VXWORKS6
int	getpid()
{
	return taskIdSelf();
}
#endif

int	gettimeofday(struct timeval *tvp, void *tzp)
{
	struct timespec	cur_time;

	CHKERR(tvp);

#ifdef FSWTIME
#include "fswtime.c"
#else
	/*	Use the internal POSIX timer.				*/

	clock_gettime(CLOCK_REALTIME, &cur_time);
	tvp->tv_sec = cur_time.tv_sec;
	tvp->tv_usec = cur_time.tv_nsec / 1000;
#endif
	return 0;
}

void	getCurrentTime(struct timeval *tvp)
{
	gettimeofday(tvp, NULL);
}

unsigned long	getClockResolution()
{
	struct timespec	ts;

	clock_getres(CLOCK_REALTIME, &ts);
	return ts.tv_nsec / 1000;
}

#ifdef ION_NO_DNS
#ifdef FSWLAN
#include "fswlan.c"
#endif
#else
unsigned int	getInternetAddress(char *hostName)
{
	int	hostNbr;

	CHKZERO(hostName);
	hostNbr = hostGetByName(hostName);
	if (hostNbr == ERROR)
	{
		putSysErrmsg("can't get address for host", hostName);
		return BAD_HOST_NAME;
	}

	return (unsigned int) ntohl(hostNbr);
}

char	*getInternetHostName(unsigned int hostNbr, char *buffer)
{
	CHKNULL(buffer);
	if (hostGetByAddr((int) hostNbr, buffer) < 0)
	{
		putSysErrmsg("can't get name for host", utoa(hostNbr));
		return NULL;
	}

	return buffer;
}

int	getNameOfHost(char *buffer, int bufferLength)
{
	int	result;

	CHKERR(buffer);
	result = gethostname(buffer, bufferLength);
	if (result < 0)
	{
		putSysErrmsg("can't get local host name", NULL);
	}

	return result;
}

char	*getNameOfUser(char *buffer)
{
	CHKNULL(buffer);
#ifdef FSWUSER
#include "fswuser.c"
#else
	remCurIdGet(buffer, NULL);
	return buffer;
#endif
}

int	reUseAddress(int fd)
{
	int	result;
	int	i = 1;

	result = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &i,
			sizeof i);
	if (result < 0)
	{
		putSysErrmsg("can't make socket's address reusable", NULL);
	}

	return result;
}

int	watchSocket(int fd)
{
	int		result;
	struct linger	lctrl = {0, 0};
	int		kctrl = 1;

	result = setsockopt(fd, SOL_SOCKET, SO_LINGER, (char *) &lctrl,
			sizeof lctrl);
	if (result < 0)
	{
		putSysErrmsg("can't set linger on socket", NULL);
		return result;
	}

	result = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char *) &kctrl,
			sizeof kctrl);
	if (result < 0)
	{
		putSysErrmsg("can't set keepalive on socket", NULL);
	}

	return result;
}
#endif	/*	ION_NO_DNS						*/

int	makeIoNonBlocking(int fd)
{
	int	result;
	int	setting = 1;

	result = ioctl(fd, FIONBIO, (int) &setting);
	if (result < 0)
	{
		putSysErrmsg("can't make IO non-blocking", NULL);
	}

	return result;
}

int	strcasecmp(const char *s1, const char *s2)
{
	register int c1, c2;

	CHKZERO(s1);
	CHKZERO(s2);
	for ( ; ; )
	{
		/* STDC requires tolower(3) to work for all
		** ints. Unfortunately, not all C's are STDC.
		*/
		c1 = *s1; if (isupper(c1)) c1 = tolower(c1);
		c2 = *s2; if (isupper(c2)) c2 = tolower(c2);
		if (c1 != c2) return c1 - c2;
		if (c1 == 0) return 0;
		++s1; ++s2;
	}
}

int	strncasecmp(const char *s1, const char *s2, size_t n)
{
	register int c1, c2;

	CHKZERO(s1);
	CHKZERO(s2);
	for ( ; n > 0; --n)
	{
		/* STDC requires tolower(3) to work for all
		** ints. Unfortunately, not all C's are STDC.
		*/
		c1 = *s1; if (isupper(c1)) c1 = tolower(c1);
		c2 = *s2; if (isupper(c2)) c2 = tolower(c2);
		if (c1 != c2) return c1 - c2;
		if (c1 == 0) return 0;
		++s1; ++s2;
	}

	return 0;
}

#endif	/*	End of #if defined VXWORKS				*/

#if defined (darwin) || defined (freebsd) || defined (mingw)

void	*memalign(size_t boundary, size_t size)
{
	return malloc(size);
}

#endif

#ifndef VXWORKS			/*	Common for all O/S but VXWORKS.	*/

int	createFile(const char *filename, int flags)
{
	int	result;

	/*	POSIX-UNIX creat(2) will only create a file for
	 *	writing.  The only portable flag values are
	 *	O_WRONLY and O_RDWR.  See creat(2) and open(2).		*/

	if (filename == NULL)
	{
		ABORT_AS_REQD;
		return ERROR;
	}

	result = iopen(filename, (flags | O_CREAT | O_TRUNC), 0666);
	if (result < 0)
	{
		putSysErrmsg("can't create file", filename);
	}

	return result;
}

#ifdef _MULTITHREADED

typedef struct rlock_str
{
	pthread_mutex_t	semaphore;
	pthread_t	owner;
	short		count;
	unsigned char	init;		/*	Boolean.		*/
	unsigned char	owned;		/*	Boolean.		*/
} Rlock;		/*	Private-memory semaphore.		*/ 

int	initResourceLock(ResourceLock *rl)
{
	Rlock	*lock = (Rlock *) rl;

	if (lock == NULL)
	{
		ABORT_AS_REQD;
		return ERROR;
	}

	if (lock->init)
	{
		return 0;
	}

	memset((char *) lock, 0, sizeof(Rlock));
	if (pthread_mutex_init(&(lock->semaphore), NULL))
	{
		writeErrMemo("Can't create lock semaphore");
		return -1;
	}

	lock->init = 1;
	return 0;
}

void	killResourceLock(ResourceLock *rl)
{
	Rlock	*lock = (Rlock *) rl;

	if (lock && lock->init && lock->count == 0)
	{
		oK(pthread_mutex_destroy(&(lock->semaphore)));
		lock->init = 0;
	}
}

void	lockResource(ResourceLock *rl)
{
	Rlock		*lock = (Rlock *) rl;
	pthread_t	tid;

	if (lock && lock->init)
	{
		tid = pthread_self();
		if (lock->owned == 0 || !pthread_equal(tid, lock->owner))
		{
			oK(pthread_mutex_lock(&(lock->semaphore)));
			lock->owner = tid;
			lock->owned = 1;
		}

		(lock->count)++;
	}
}

void	unlockResource(ResourceLock *rl)
{
	Rlock		*lock = (Rlock *) rl;
	pthread_t	tid;

	if (lock && lock->init)
	{
		tid = pthread_self();
		if (lock->owned && pthread_equal(tid, lock->owner))
		{
			(lock->count)--;
			if ((lock->count) == 0)
			{
				lock->owned = 0;
				oK(pthread_mutex_unlock(&(lock->semaphore)));
			}
		}
	}
}

#else	/*	Only one thread of control in address space.		*/

int	initResourceLock(ResourceLock *rl)
{
	return 0;
}

void	killResourceLock(ResourceLock *rl)
{
	return;
}

void	lockResource(ResourceLock *rl)
{
	return;
}

void	unlockResource(ResourceLock *rl)
{
	return;
}

#endif	/*	end #ifdef _MULTITHREADED				*/

#if (!defined (linux) && !defined (freebsd) && !defined (darwin) && !defined (RTEMS) && !defined (mingw))
/*	These things are defined elsewhere for Linux-like op systems.	*/

#ifdef solaris
char	*system_error_msg()
{
	return strerror(errno);
}
#else
extern int	sys_nerr;
extern char	*sys_errlist[];

char	*system_error_msg()
{
	if (errno > sys_nerr)
	{
		return "cause unknown";
	}

	return sys_errlist[errno];
}
#endif	/*	end #ifdef solaris					*/

char	*getNameOfUser(char *buffer)
{
	CHKNULL(buffer);
#ifdef FSWUSER
#include "fswuser.c"
#else
	return cuserid(buffer);
#endif
}

#endif	/*	end #if (!defined(linux, freebsd, darwin, RTEMS, mingw))*/

void	closeOnExec(int fd)
{
#ifndef mingw
	oK(fcntl(fd, F_SETFD, FD_CLOEXEC));
#endif
}

#if defined (mingw)

void	snooze(unsigned int seconds)
{
	Sleep(seconds * 1000);
}

void	microsnooze(unsigned int usec)
{
	Sleep(usec / 1000);
}

#endif	/*	end #ifdef mingw					*/

#if (!defined (mingw))			/*	nanosleep is defined.	*/

void	snooze(unsigned int seconds)
{
	struct timespec	ts;

	ts.tv_sec = seconds;
	ts.tv_nsec = 0;
	oK(nanosleep(&ts, NULL));
}

void	microsnooze(unsigned int usec)
{
	struct timespec	ts;

	ts.tv_sec = usec / 1000000;
	ts.tv_nsec = (usec % 1000000) * 1000;
	oK(nanosleep(&ts, NULL));
}

#endif	/*	end #if (!defined(mingw))				*/

void	getCurrentTime(struct timeval *tvp)
{
	CHKVOID(tvp);
	oK(gettimeofday(tvp, NULL));
}

unsigned long	getClockResolution()
{
	/*	Linux clock resolution of Alpha is 1 ms, as is
	 *	Windows XP standard clock resolution, and Solaris
	 *	clock resolution can be configured.  But minimum
	 *	clock resolution in all cases appears to be 10 ms,
	 *	so we use that value since it seems likely to be
	 *	safe in all cases.					*/

	return 10000;
}

#endif	/*	End of #ifndef VXWORKS					*/

#if defined (__SVR4)

int	getNameOfHost(char *buffer, int bufferLength)
{
	struct utsname	name;

	CHKERR(buffer);
	CHKERR(bufferLength > 0);
	if (uname(&name) < 0)
	{
		*buffer = '\0';
		putSysErrmsg("can't get local host name", NULL);
		return -1;
	}

	strncpy(buffer, name.nodename, bufferLength - 1);
	*(buffer + bufferLength - 1) = '\0';
	return 0;
}

int	makeIoNonBlocking(int fd)
{
	int	result;

	result = fcntl(fd, F_SETFL, O_NDELAY);
	if (result < 0)
	{
		putSysErrmsg("can't make IO non-blocking", NULL);
	}

	return result;
}

#if defined (_REENTRANT)	/*	SVR4 multithreaded.		*/

#ifdef ION_NO_DNS
#ifdef FSWLAN
#include "fswlan.c"
#endif
#else
unsigned int	getInternetAddress(char *hostName)
{
	struct hostent	hostInfoBuffer;
	struct hostent	*hostInfo;
	unsigned int	hostInetAddress;
	char		textBuffer[1024];
	int		hostInfoErrno = -1;

	CHKZERO(hostName);
	hostInfo = gethostbyname_r(hostName, &hostInfoBuffer, textBuffer,
			sizeof textBuffer, &hostInfoErrno);
	if (hostInfo == NULL)
	{
		putSysErrmsg("can't get host info", hostName);
		return BAD_HOST_NAME;
	}

	if (hostInfo->h_length != sizeof hostInetAddress)
	{
		putErrmsg("Address length invalid in host info.", hostName);
		return BAD_HOST_NAME;
	}

	memcpy((char *) &hostInetAddress, hostInfo->h_addr, 4);
	return ntohl(hostInetAddress);
}

char	*getInternetHostName(unsigned int hostNbr, char *buffer)
{
	struct hostent	hostInfoBuffer;
	struct hostent	*hostInfo;
	char		textBuffer[128];
	int		hostInfoErrno;

	CHKNULL(buffer);
	hostNbr = htonl(hostNbr);
	hostInfo = gethostbyaddr_r((char *) &hostNbr, sizeof hostNbr, AF_INET,
			&hostInfoBuffer, textBuffer, sizeof textBuffer,
			&hostInfoErrno);
	if (hostInfo == NULL)
	{
		putSysErrmsg("can't get host info", utoa(hostNbr));
		return NULL;
	}

	strncpy(buffer, hostInfo->h_name, MAXHOSTNAMELEN);
	return buffer;
}

int	reUseAddress(int fd)
{
	int	result;
	int	i = 1;

	result = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &i,
			sizeof i);
#if (defined (SO_REUSEPORT))
	result += setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, (char *) &i,
			sizeof i);
#endif
	if (result < 0)
	{
		putSysErrmsg("can't make socket address reusable", NULL);
	}

	return result;
}

int	watchSocket(int fd)
{
	int		result;
	struct linger	lctrl = {0, 0};
	int		kctrl = 1;

	result = setsockopt(fd, SOL_SOCKET, SO_LINGER, (char *) &lctrl,
			sizeof lctrl);
	if (result < 0)
	{
		putSysErrmsg("can't set linger on socket", NULL);
		return result;
	}

	result = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char *) &kctrl,
			sizeof kctrl);
	if (result < 0)
	{
		putSysErrmsg("can't set keepalive on socket", NULL);
	}

	return result;
}
#endif	/*	ION_NO_DNS						*/

#else	/*	SVR4 but not multithreaded.				*/

#ifdef ION_NO_DNS
#ifdef FSWLAN
#include "fswlan.c"
#endif
#else
unsigned int	getInternetAddress(char *hostName)
{
	struct hostent	*hostInfo;
	unsigned int	hostInetAddress;

	CHKZERO(hostName);
	hostInfo = gethostbyname(hostName);
	if (hostInfo == NULL)
	{
		putSysErrmsg("can't get host info", hostName);
		return BAD_HOST_NAME;
	}

	if (hostInfo->h_length != sizeof hostInetAddress)
	{
		putErrmsg("Address length invalid in host info.", hostName);
		return BAD_HOST_NAME;
	}

	memcpy((char *) &hostInetAddress, hostInfo->h_addr, 4);
	return ntohl(hostInetAddress);
}

char	*getInternetHostName(unsigned int hostNbr, char *buffer)
{
	struct hostent	*hostInfo;

	CHKNULL(buffer);
	hostNbr = htonl(hostNbr);
	hostInfo = gethostbyaddr((char *) &hostNbr, sizeof hostNbr, AF_INET);
	if (hostInfo == NULL)
	{
		putSysErrmsg("can't get host info", utoa(hostNbr));
		return NULL;
	}

	strncpy(buffer, hostInfo->h_name, MAXHOSTNAMELEN);
	return buffer;
}

int	reUseAddress(int fd)
{
	int	result;
	int	i = 1;

	result = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof i);
#if (defined (SO_REUSEPORT))
	result += setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &i, sizeof i);
#endif
	if (result < 0)
	{
		putSysErrmsg("can't make socket address reusable", NULL);
	}

	return result;
}

int	watchSocket(int fd)
{
	int		result;
	struct linger	lctrl = {0, 0};
	int		kctrl = 1;

	result = setsockopt(fd, SOL_SOCKET, SO_LINGER, (void *) &lctrl,
			sizeof lctrl);
	if (result < 0)
	{
		putSysErrmsg("can't set linger on socket", NULL);
		return result;
	}

	result = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void *) &kctrl,
			sizeof kctrl);
	if (result < 0)
	{
		putSysErrmsg("can't set keepalive on socket", NULL);
	}

	return result;
}
#endif	/*	ION_NO_DNS						*/

#endif	/*	end of #if defined _REENTRANT				*/

#endif	/*	end of #if defined _SVR4				*/

#if (defined mingw)

int	_winsock(int stopping)
{
	static int	winsockStarted = 0;
	static WSADATA	wsaData;
	WORD		wVersionRequested;
	int		errcode;

	if (stopping)
	{
		if (winsockStarted)
		{
			WSACleanup();
			winsockStarted = 0;
		}

		return 0;
	}

	/*	Starting WinSock.					*/

	if (winsockStarted)
	{
		return 0;	/*	Already started.		*/
	}

	wVersionRequested = MAKEWORD(2, 2);
	errcode = WSAStartup(wVersionRequested, &wsaData);
	if (errcode != 0)
	{
		putErrmsg("Can't start WinSock.", utoa(GetLastError()));
		return -1;
	}

	winsockStarted = 1;
	return 0;
}

char	*system_error_msg()
{
	return strerror(errno);
}

char	*getNameOfUser(char *buffer)
{
	unsigned long	bufsize = 8;

	CHKNULL(buffer);
	if (GetUserName(buffer, &bufsize))
	{
		istrcpy(buffer, "unknown", 8);
	}

	return buffer;
}

unsigned int	getInternetAddress(char *hostName)
{
	struct hostent	*hostInfo;
	unsigned int	hostInetAddress;

	CHKZERO(hostName);
	if (_winsock(0) < 0)
	{
		putErrmsg("Can't start WinSock.", NULL);
		return 0;
	}

	hostInfo = gethostbyname(hostName);
	if (hostInfo == NULL)
	{
		putSysErrmsg("Can't get host info", hostName);
		return BAD_HOST_NAME;
	}

	if (hostInfo->h_length != sizeof hostInetAddress)
	{
		putErrmsg("Address length invalid in host info.", hostName);
		return BAD_HOST_NAME;
	}

	memcpy((char *) &hostInetAddress, hostInfo->h_addr, 4);
	return ntohl(hostInetAddress);
}

char	*getInternetHostName(unsigned int hostNbr, char *buffer)
{
	struct hostent	*hostInfo;

	CHKNULL(buffer);
	if (_winsock(0) < 0)
	{
		putErrmsg("Can't start WinSock.", NULL);
		return 0;
	}

	hostNbr = htonl(hostNbr);
	hostInfo = gethostbyaddr((char *) &hostNbr, sizeof hostNbr, AF_INET);
	if (hostInfo == NULL)
	{
		putSysErrmsg("Can't get host info", utoa(hostNbr));
		return NULL;
	}

	strncpy(buffer, hostInfo->h_name, MAXHOSTNAMELEN);
	return buffer;
}

int	getNameOfHost(char *buffer, int bufferLength)
{
	CHKERR(buffer);
	*buffer = '\0';			/*	Default.		*/
	CHKERR(bufferLength > 0);
	if (_winsock(0) < 0)
	{
		putErrmsg("Can't start WinSock.", NULL);
		return 0;
	}

	return gethostname(buffer, bufferLength);
}

int	reUseAddress(int fd)
{
	int	result;
	int	i = 1;

	if (_winsock(0) < 0)
	{
		putErrmsg("Can't start WinSock.", NULL);
		return 0;
	}

	result = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void *) &i,
			sizeof i);
	if (result < 0)
	{
		putSysErrmsg("Can't make socket address reusable", NULL);
	}

	return result;
}
 
int	makeIoNonBlocking(int fd)
{
	int		result = 0;
	unsigned long	setting = 1;
 
	if (_winsock(0) < 0)
	{
		putErrmsg("Can't start WinSock.", NULL);
		return 0;
	}

	if (ioctlsocket(fd, FIONBIO, &setting) == SOCKET_ERROR)
	{
		putSysErrmsg("Can't make IO non-blocking", NULL);
		result = -1;
	}

	return result;
}

int	watchSocket(int fd)
{
	int		result;
	struct linger	lctrl = {0, 0};
	int		kctrl = 1;

	if (_winsock(0) < 0)
	{
		putErrmsg("Can't start WinSock.", NULL);
		return 0;
	}

	result = setsockopt(fd, SOL_SOCKET, SO_LINGER, (void *) &lctrl,
			sizeof lctrl);
	if (result < 0)
	{
		putSysErrmsg("Can't set linger on socket", NULL);
		return result;
	}

	result = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void *) &kctrl,
			sizeof kctrl);
	if (result < 0)
	{
		putSysErrmsg("Can't set keepalive on socket", NULL);
	}

	return result;
}

#endif	/*	end of #if defined (mingw)				*/

#if (defined (linux) || defined (freebsd) || defined (darwin) || defined (RTEMS))

char	*system_error_msg()
{
	return strerror(errno);
}

char	*getNameOfUser(char *buffer)
{
	CHKNULL(buffer);
#ifdef FSWUSER
#include "fswuser.c"
#else
	uid_t		euid;
	struct passwd	*pwd;

	/*	Note: buffer is in argument list for portability but
	 *	is not used and therefore is not checked for non-NULL.	*/

	euid = geteuid();
	pwd = getpwuid(euid);
	if (pwd)
	{
		return pwd->pw_name;
	}

	return "";
#endif
}

#ifdef ION_NO_DNS
#ifdef FSWLAN
#include "fswlan.c"
#endif
#else
unsigned int	getInternetAddress(char *hostName)
{
	struct hostent	*hostInfo;
	unsigned int	hostInetAddress;

	CHKZERO(hostName);
	hostInfo = gethostbyname(hostName);
	if (hostInfo == NULL)
	{
		putSysErrmsg("can't get host info", hostName);
		return BAD_HOST_NAME;
	}

	if (hostInfo->h_length != sizeof hostInetAddress)
	{
		putErrmsg("Address length invalid in host info.", hostName);
		return BAD_HOST_NAME;
	}

	memcpy((char *) &hostInetAddress, hostInfo->h_addr, 4);
	return ntohl(hostInetAddress);
}

char	*getInternetHostName(unsigned int hostNbr, char *buffer)
{
	struct hostent	*hostInfo;

	CHKNULL(buffer);
	hostNbr = htonl(hostNbr);
	hostInfo = gethostbyaddr((char *) &hostNbr, sizeof hostNbr, AF_INET);
	if (hostInfo == NULL)
	{
		putSysErrmsg("can't get host info", utoa(hostNbr));
		return NULL;
	}

	strncpy(buffer, hostInfo->h_name, MAXHOSTNAMELEN);
	return buffer;
}

int	getNameOfHost(char *buffer, int bufferLength)
{
	int	result;

	CHKERR(buffer);
	CHKERR(bufferLength > 0);
	result = gethostname(buffer, bufferLength);
	if (result < 0)
	{
		putSysErrmsg("can't local host name", NULL);
	}

	return result;
}

int	reUseAddress(int fd)
{
	int	result;
	int	i = 1;

	result = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void *) &i,
			sizeof i);
#if (defined (SO_REUSEPORT))
	result += setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, (char *) &i,
			sizeof i);
#endif
	if (result < 0)
	{
		putSysErrmsg("can't make socket address reusable", NULL);
	}

	return result;
}
#endif	/*	ION_NO_DNS						*/
 
int	makeIoNonBlocking(int fd)
{
	int	result;
	int	setting = 1;
 
	result = ioctl(fd, FIONBIO, &setting);
	if (result < 0)
	{
		putSysErrmsg("can't make IO non-blocking", NULL);
	}

	return result;
}

int	watchSocket(int fd)
{
	int		result;
	struct linger	lctrl = {0, 0};
	int		kctrl = 1;

	result = setsockopt(fd, SOL_SOCKET, SO_LINGER, (void *) &lctrl,
			sizeof lctrl);
	if (result < 0)
	{
		putSysErrmsg("can't set linger on socket", NULL);
		return result;
	}

	result = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void *) &kctrl,
			sizeof kctrl);
	if (result < 0)
	{
		putSysErrmsg("can't set keepalive on socket", NULL);
	}

	return result;
}

#endif	/*	end #if (defined(linux, freebsd, darwin, RTEMS))	*/

/**********************	WinSock adaptations *****************************/

#ifdef mingw
int	iopen(const char *fileName, int flags, int pmode)
{
	CHKERR(fileName);
	flags |= _O_BINARY;
	return _open(fileName, flags, pmode);
}

int	isend(int sockfd, char *buf, int len, int flags)
{
	int	length;
	int	errcode;

	CHKERR(len >= 0);
	CHKERR(buf);
	length = send(sockfd, buf, len, flags);
	if (length == SOCKET_ERROR)
	{
		length = -1;
		errcode = WSAGetLastError();
		switch (errcode)
		{
		case WSAECONNRESET:
		case WSAENETRESET:
		case WSAECONNABORTED:
		case WSAETIMEDOUT:
			errno = EPIPE;	/*	Connection closed.	*/
			break;

		default:
			writeMemoNote("[?] WinSock send error",
					itoa(errcode));
		}
	}
	else
	{
		if (length == 0)
		{
			length = -1;
			errno = EPIPE;	/*	Connection closed.	*/
		}
	}

	return length;
}

int	irecv(int sockfd, char *buf, int len, int flags)
{
	int	length;
	int	errcode;

	CHKERR(len >= 0);
	CHKERR(buf);
	length = recv(sockfd, buf, len, flags);
	if (length < 0)
	{
		errcode = WSAGetLastError();
		switch (errcode)
		{
		case WSAECONNRESET:
		case WSAECONNABORTED:
			errno = ECONNRESET;
			length = 0;	/*	Connection closed.	*/
			break;

		case WSAESHUTDOWN:
			errno = EINTR;	/*	Shut down socket.	*/
			break;

		default:
			writeMemoNote("[?] WinSock recv error",
					itoa(errcode));
		}
	}

	return length;
}

int	isendto(int sockfd, char *buf, int len, int flags,
		const struct sockaddr *to, int tolen)
{
	CHKERR(len >= 0);
	CHKERR(buf);
	CHKERR(to);
	return sendto(sockfd, buf, len, flags, to, tolen);
}

int	irecvfrom(int sockfd, char *buf, int len, int flags,
		struct sockaddr *from, int *fromlen)
{
	int	length;
	int	errcode;

	CHKERR(len >= 0);
	CHKERR(buf);
	CHKERR(from);
	CHKERR(fromlen);
	while (1)	/*	Continue until valid result.		*/
	{
		length = recvfrom(sockfd, buf, len, flags, from, fromlen);
		if (length < 0)
		{
			errcode = WSAGetLastError();
			switch (errcode)
			{
			case WSAECONNRESET:
			case WSAECONNABORTED:
			case WSAESHUTDOWN:
				/*	Ignore; peer socket was closed.	*/

				continue;

			default:
				writeMemoNote("[?] WinSock recvfrom error",
						itoa(errcode));
			}
		}

		break;
	}

	return length;
}
#endif

/******************* platform-independent functions *********************/

void	*acquireSystemMemory(size_t size)
{
	void	*block;

	if (size <= 0)
	{
		return NULL;
	}

	size = size + ((sizeof(void *)) - (size % (sizeof(void *))));
#if defined (RTEMS)
	block = malloc(size);	/*	try posix_memalign?		*/
#else
	block = memalign((size_t) (sizeof(void *)), size);
#endif
	if (block)
	{
		TRACK_MALLOC(block);
		memset((char *) block, 0, size);
	}
	else
	{
		putSysErrmsg("Memory allocation failed", itoa(size));
	}

	return block;
}

static void	watchToStdout(char token)
{
	oK(putchar(token));
	oK(fflush(stdout));
}

static Watcher	_watchOneEvent(Watcher *watchFunction)
{
	static Watcher	watcher = watchToStdout;

	if (watchFunction)
	{
		watcher = *watchFunction;
	}

	return watcher;
}

void	setWatcher(Watcher watchFunction)
{
	if (watchFunction)
	{
		oK(_watchOneEvent(&watchFunction));
	}
}

void	iwatch(char token)
{
	(_watchOneEvent(NULL))(token);
}

static void	logToStdout(char *text)
{
	if (text)
	{
		fprintf(stdout, "%s\n", text);
		fflush(stdout);
	}
}

static Logger	_logOneMessage(Logger *logFunction)
{
	static Logger	logger = logToStdout;

	if (logFunction)
	{
		logger = *logFunction;
	}

	return logger;
}

void	setLogger(Logger logFunction)
{
	if (logFunction)
	{
		oK(_logOneMessage(&logFunction));
	}
}

void	writeMemo(char *text)
{
	if (text)
	{
		(_logOneMessage(NULL))(text);
	}
}

void	writeMemoNote(char *text, char *note)
{
	char	*noteText = note ? note : (char *) "";
	char	textBuffer[1024];

	if (text)
	{
		isprintf(textBuffer, sizeof textBuffer, "%.900s: %.64s",
				text, noteText);
		(_logOneMessage(NULL))(textBuffer);
	}
}

void	writeErrMemo(char *text)
{
	writeMemoNote(text, system_error_msg());
}

char	*iToa(int arg)
{
	static char	itoa_str[33];

	isprintf(itoa_str, sizeof itoa_str, "%d", arg);
	return itoa_str;
}

char	*uToa(unsigned int arg)
{
	static char	utoa_str[33];

	isprintf(utoa_str, sizeof utoa_str, "%u", arg);
	return utoa_str;
}

static int	clipFileName(const char *qualifiedFileName, char **fileName)
{
	int	fileNameLength;
	int	excessLength;

	fileNameLength = strlen(qualifiedFileName);
	excessLength = fileNameLength - MAX_SRC_FILE_NAME;
	if (excessLength < 0)
	{
		excessLength = 0;
	}

	/*	Clip excessLength bytes off the front of the file
	 *	name by adding excessLength to the string pointer.	*/

	(*fileName) = ((char *) qualifiedFileName) + excessLength;
	fileNameLength -= excessLength;
	return fileNameLength;
}

static int	_errmsgs(int lineNbr, const char *qualifiedFileName,
			const char *text, const char *arg, char *buffer)
{
	static char		errmsgs[ERRMSGS_BUFSIZE];
	static int		errmsgsLength = 0;
	static ResourceLock	errmsgsLock;
	static int		errmsgsLockInit = 0;
	int			msgLength;
	int			spaceFreed;
	int			fileNameLength;
	char			*fileName;
	char			lineNbrBuffer[32];
	int			spaceAvbl;
	int			spaceForText;
	int			spaceNeeded;

	if (!errmsgsLockInit)
	{
		memset((char *) &errmsgsLock, 0, sizeof(ResourceLock));
		if (initResourceLock(&errmsgsLock) < 0)
		{
			ABORT_AS_REQD;
			return 0;
		}

		errmsgsLockInit = 1;
	}

	if (buffer)		/*	Retrieving an errmsg.		*/
	{
		if (errmsgsLength == 0)	/*	No more msgs in pool.	*/
		{
			return 0;
		}

		lockResource(&errmsgsLock);
		msgLength = strlen(errmsgs);
		if (msgLength == 0)	/*	No more msgs in pool.	*/
		{
			unlockResource(&errmsgsLock);
			return msgLength;
		}

		/*	Getting a message removes it from the pool,
		 *	releasing space for more messages.		*/

		spaceFreed = msgLength + 1;	/*	incl. last NULL	*/
		memcpy(buffer, errmsgs, spaceFreed);
		errmsgsLength -= spaceFreed;
		memcpy(errmsgs, errmsgs + spaceFreed, errmsgsLength);
		memset(errmsgs + errmsgsLength, 0, spaceFreed);
		unlockResource(&errmsgsLock);
		return msgLength;
	}

	/*	Posting an errmsg.					*/

	if (qualifiedFileName == NULL || text == NULL || *text == '\0')
	{
		return 0;	/*	Ignored.			*/
	}

	fileNameLength = clipFileName(qualifiedFileName, &fileName);
	lockResource(&errmsgsLock);
	isprintf(lineNbrBuffer, sizeof lineNbrBuffer, "%d", lineNbr);
	spaceAvbl = ERRMSGS_BUFSIZE - errmsgsLength;
	spaceForText = 8 + strlen(lineNbrBuffer) + 4 + fileNameLength
			+ 2 + strlen(text);
	spaceNeeded = spaceForText + 1;
	if (arg)
	{
		spaceNeeded += (2 + strlen(arg) + 1);
	}

	if (spaceNeeded > spaceAvbl)	/*	Can't record message.	*/
	{
		if (spaceAvbl < 2)
		{
			/*	Can't even note that it was omitted.	*/

			spaceNeeded = 0;
		}
		else
		{
			/*	Write a single newline message to
			 *	note that this message was omitted.	*/

			spaceNeeded = 2;
			errmsgs[errmsgsLength] = '\n';
			errmsgs[errmsgsLength + 1] = '\0';
		}
	}
	else
	{
		isprintf(errmsgs + errmsgsLength, spaceAvbl,
			"at line %s of %s, %s", lineNbrBuffer, fileName, text);
		if (arg)
		{
			isprintf(errmsgs + errmsgsLength + spaceForText,
				spaceAvbl - spaceForText, " (%s)", arg);
		}
	}

	errmsgsLength += spaceNeeded;
	unlockResource(&errmsgsLock);
	return 0;
}

void	_postErrmsg(const char *fileName, int lineNbr, const char *text,
		const char *arg)
{
	oK(_errmsgs(lineNbr, fileName, text, arg, NULL));
}

void	_putErrmsg(const char *fileName, int lineNbr, const char *text,
		const char *arg)
{
	_postErrmsg(fileName, lineNbr, text, arg);
	writeErrmsgMemos();
}

void	_postSysErrmsg(const char *fileName, int lineNbr, const char *text,
		const char *arg)
{
	char	*sysmsg;
	int	textLength;
	int	maxTextLength;
	char	textBuffer[1024];

	if (text)
	{
		textLength = strlen(text);
		sysmsg = system_error_msg();
		maxTextLength = sizeof textBuffer - (2 + strlen(sysmsg) + 1);
		if (textLength > maxTextLength)
		{
			textLength = maxTextLength;
		}

		isprintf(textBuffer, sizeof textBuffer, "%.*s: %s",
				textLength, text, sysmsg);
		_postErrmsg(fileName, lineNbr, textBuffer, arg);
	}
}

void	_putSysErrmsg(const char *fileName, int lineNbr, const char *text,
		const char *arg)
{
	_postSysErrmsg(fileName, lineNbr, text, arg);
	writeErrmsgMemos();
}

int	getErrmsg(char *buffer)
{
	if (buffer == NULL)
	{
		ABORT_AS_REQD;
		return 0;
	}

	return _errmsgs(0, NULL, NULL, NULL, buffer);
}

void	writeErrmsgMemos()
{
	static ResourceLock	memosLock;
	static int		memosLockInit = 0;
	static char		msgwritebuf[ERRMSGS_BUFSIZE];
	static char		*omissionMsg = "[?] message omitted due to \
excessive length";

	/*	Because buffer is static, it is shared.  So access
	 *	to it must be mutexed.					*/

	if (!memosLockInit)
	{
		memset((char *) &memosLock, 0, sizeof(ResourceLock));
		if (initResourceLock(&memosLock) < 0)
		{
			ABORT_AS_REQD;
			return;
		}

		memosLockInit = 1;
	}

	lockResource(&memosLock);
	while (1)
	{
		if (getErrmsg(msgwritebuf) == 0)
		{
			break;
		}

		if (msgwritebuf[0] == '\n')
		{
			writeMemo(omissionMsg);
		}
		else
		{
			writeMemo(msgwritebuf);
		}
	}

	unlockResource(&memosLock);
}

void	discardErrmsgs()
{
	static char	msgdiscardbuf[ERRMSGS_BUFSIZE];

	/*	The discard buffer is static, therefore shared, but
	 *	its contents are never used for any purpose.  So no
	 *	need to protect it from multiple concurrent users.	*/

	while (1)
	{
		if (getErrmsg(msgdiscardbuf) == 0)
		{
			return;
		}
	}
}

int	_coreFileNeeded(int *ctrl)
{
	static int	coreFileNeeded = CORE_FILE_NEEDED;

	if (ctrl)
	{
		coreFileNeeded = *ctrl;
	}

	return coreFileNeeded;
}

int	_iEnd(const char *fileName, int lineNbr, const char *arg)
{
	_postErrmsg(fileName, lineNbr, "Assertion failed.", arg);
	writeErrmsgMemos();
	printStackTrace();
	if (_coreFileNeeded(NULL))
	{
		sm_Abort();
	}

	return 1;
}

void	printStackTrace()
{
#if (defined(bionic) || defined(uClibc) || !(defined(linux)))
	writeMemo("[?] No stack trace available on this platform.");
#else
#define	MAX_TRACE_DEPTH	100
	void	*returnAddresses[MAX_TRACE_DEPTH];
	size_t	stackFrameCount;
	char	**functionNames;
	int	i;

	stackFrameCount = backtrace(returnAddresses, MAX_TRACE_DEPTH);
	functionNames = backtrace_symbols(returnAddresses, stackFrameCount);
	if (functionNames == NULL)
	{
		writeMemo("[!] Can't print backtrace function names.");
		return;
	}

	writeMemo("[i] Current stack trace:");
	for (i = 0; i < stackFrameCount; i++)
	{
		writeMemoNote("[i] ", functionNames[i]);
	}

	free(functionNames);
#endif
}

void	encodeSdnv(Sdnv *sdnv, uvast val)
{
	static uvast	sdnvMask = ((uvast) -1) / 128;
	uvast		remnant;
	int		i;
	unsigned char	flag = 0;
	unsigned char	*text;

	/*	Get length of SDNV text: one byte for each 7 bits of
	 *	significant numeric value.  On each iteration of the
	 *	loop, until what's left of the original value is zero,
	 *	shift the remaining value 7 bits to the right and add
	 *	1 to the imputed SDNV length.				*/

	CHKVOID(sdnv);
	sdnv->length = 0;
	remnant = val;
	do
	{
		remnant = (remnant >> 7) & sdnvMask;
		(sdnv->length)++;
	} while (remnant > 0);

	/*	Now fill the SDNV text from the numeric value bits.	*/

	text = sdnv->text + sdnv->length;
	i = sdnv->length;
	remnant = val;
	while (i > 0)
	{
		text--;

		/*	Get low-order 7 bits of what's left, OR'ing
		 *	it with high-order bit flag for this position
		 *	of the SDNV.					*/

		*text = (remnant & 0x7f) | flag;

		/*	Shift those bits out of the value.		*/

		remnant = (remnant >> 7) & sdnvMask;
		flag = 0x80;		/*	Flag is now 1.		*/
		i--;
	}
}

int	decodeSdnv(uvast *val, unsigned char *sdnvTxt)
{
	int		sdnvLength = 0;
	unsigned char	*cursor;

	CHKZERO(val);
	CHKZERO(sdnvTxt);
	*val = 0;
	cursor = sdnvTxt;
	while (1)
	{
		sdnvLength++;
		if (sdnvLength > 10)
		{
			return 0;	/*	More than 70 bits.	*/
		}

		/*	Shift numeric value 7 bits to the left (that
		 *	is, multiply by 128) to make room for 7 bits
		 *	of SDNV byte value.				*/

		*val <<= 7;

		/*	Insert SDNV byte value (with its high-order
		 *	bit masked off) as low-order 7 bits of the
		 *	numeric value.					*/

		*val |= (*cursor & 0x7f);
		if ((*cursor & 0x80) == 0)	/*	Last SDNV byte.	*/
		{
			return sdnvLength;
		}

		/*	Haven't reached the end of the SDNV yet.	*/

		cursor++;
	}
}

void	loadScalar(Scalar *s, signed int i)
{
	CHKVOID(s);
	if (i < 0)
	{
		i = 0 - i;
	}

	s->gigs = 0;
	s->units = i;
	while (s->units >= ONE_GIG)
	{
		s->gigs++;
		s->units -= ONE_GIG;
	}
}

void	increaseScalar(Scalar *s, signed int i)
{
	CHKVOID(s);
	if (i < 0)
	{
		i = 0 - i;
	}

	while (i >= ONE_GIG)
	{
		i -= ONE_GIG;
		s->gigs++;
	}

	s->units += i;
	while (s->units >= ONE_GIG)
	{
		s->gigs++;
		s->units -= ONE_GIG;
	}
}

void	reduceScalar(Scalar *s, signed int i)
{
	CHKVOID(s);
	if (i < 0)
	{
		i = 0 - i;
	}

	while (i >= ONE_GIG)
	{
		i -= ONE_GIG;
		s->gigs--;
	}

	while (i > s->units)
	{
		s->units += ONE_GIG;
		s->gigs--;
	}

	s->units -= i;
}

void	multiplyScalar(Scalar *s, signed int i)
{
	double	product;

	CHKVOID(s);
	if (i < 0)
	{
		i = 0 - i;
	}

	product = ((((double)(s->gigs)) * ONE_GIG) + (s->units)) * i;
	s->gigs = (int) (product / ONE_GIG);
	s->units = (int) (product - (((double)(s->gigs)) * ONE_GIG));
}

void	divideScalar(Scalar *s, signed int i)
{
	double	quotient;

	CHKVOID(s);
	CHKVOID(i != 0);
	if (i < 0)
	{
		i = 0 - i;
	}

	quotient = ((((double)(s->gigs)) * ONE_GIG) + (s->units)) / i;
	s->gigs = (int) (quotient / ONE_GIG);
	s->units = (int) (quotient - (((double)(s->gigs)) * ONE_GIG));
}

void	copyScalar(Scalar *to, Scalar *from)
{
	CHKVOID(to);
	CHKVOID(from);
	to->gigs = from->gigs;
	to->units = from->units;
}

void	addToScalar(Scalar *s, Scalar *increment)
{
	CHKVOID(s);
	CHKVOID(increment);
	increaseScalar(s, increment->units);
	s->gigs += increment->gigs;
}

void	subtractFromScalar(Scalar *s, Scalar *decrement)
{
	CHKVOID(s);
	CHKVOID(decrement);
	reduceScalar(s, decrement->units);
	s->gigs -= decrement->gigs;
}

int	scalarIsValid(Scalar *s)
{
	CHKZERO(s);
	return (s->gigs >= 0);
}

void	scalarToSdnv(Sdnv *sdnv, Scalar *scalar)
{
	int		gigs;
	int		units;
	int		i;
	unsigned char	flag = 0;
	unsigned char	*cursor;

	CHKVOID(scalarIsValid(scalar));
	CHKVOID(sdnv);
	sdnv->length = 0;

	/*		Calculate sdnv length				*/

	gigs = scalar->gigs;
	units = scalar->units;
	if (gigs) 
	{
		/*	The scalar is greater than 2^30 - 1, so start
		 *	with the length occupied by all 30 bits of
		 *	"units" in the scalar.  This will occupy 5
		 *	bytes in the sdnv with room for an additional
		 *	5 high-order bits.  These bits will be the
		 *	low-order 5 bits of gigs.  If the value in
		 *	gigs is greater than 2^5 -1, increase sdnv
		 *	length accordingly.				*/

		sdnv->length += 5;			
		gigs >>= 5;
		while (gigs)
		{
			gigs >>= 7;
			sdnv->length++;
		}
	}
	else
	{
		/*	gigs = 0, so calculate the sdnv length from
			units only.					*/

		do
		{
			units >>= 7;
			sdnv->length++;
		} while (units);
	}

	/*		Fill the sdnv text.				*/

	cursor = sdnv->text + sdnv->length;
	i = sdnv->length;
	gigs = scalar->gigs;
	units = scalar->units;
	do
	{
		cursor--;

		/*	Start filling the sdnv text from the last byte.
			Get 7 low-order bits from units and add the
			flag to the high-order bit. Flag is 0 for the
			last byte and 1 for all the previous bytes.	*/

		*cursor = (units & 0x7f) | flag;
		units >>= 7;
		flag = 0x80;		/*	Flag is now 1.		*/
		i--;
	} while (units);

	if (gigs)
	{
		while (sdnv->length - i < 5)
		{
			cursor--;

			/* Fill remaining sdnv bytes corresponding to
			   units with zeroes.				*/

			*cursor = 0x00 | flag;
			i--;
		}

		/*	Place the 5 low-order bits of gigs in the
			current	sdnv byte.				*/

		*cursor |= ((gigs & 0x1f) << 2);
		gigs >>= 5;
		while (i)
		{
			cursor--;

			/*	Now fill the remaining sdnv bytes
				from gigs.				*/

			*cursor = (gigs & 0x7f) | flag;
			gigs >>= 7;
			i--;
		}
	}
}

int	sdnvToScalar(Scalar *scalar, unsigned char *sdnvText)
{
	int		sdnvLength;
	int		i;
	int		numSize = 0; /* Size of stored number in bits.	*/
	unsigned char	*cursor;
	unsigned char	flag;
	unsigned char	k;

	CHKZERO(scalar);
	CHKZERO(sdnvText);
	cursor = sdnvText;

	/*	Find out the sdnv length and size of stored number,
	 *	stripping off all leading zeroes.			*/

	flag = (*cursor & 0x80);/*	Get flag of 1st byte.		*/
	k = *cursor << 1;	/*	Discard the flag bit.		*/
	i = 7;
	while (i)
	{
		if (k & 0x80)
		{
			break;	/*	Loop until a '1' is found.	*/
		}

		i--;
		k <<= 1;
	}

	numSize += i;	/*	Add significant bits from first byte.	*/
	if (flag)	/*	Not end of SDNV.			*/
	{
		/*	Sdnv has more than one byte.  Add 7 bits for
		 *	the last byte and advance cursor to add the
		 *	bits for all intermediate bytes.		*/

		numSize += 7;
		cursor++;
		while (*cursor & 0x80)
		{
			numSize += 7;
			cursor++;
		}
	}

	if (numSize > 61)
	{
		return 0;	/*	Too long to fit in a Scalar.	*/
	}

	sdnvLength = (cursor - sdnvText) + 1;

	/*		Now start filling gigs and units.		*/

	scalar->gigs = 0;
	scalar->units = 0;
	cursor = sdnvText;
	i = sdnvLength;

	while (i > 5)
	{	/*	Sdnv bytes containing gigs only.		*/

		scalar->gigs <<= 7;
		scalar->gigs |= (*cursor & 0x7f);
		cursor++;
		i--;
	}

	if (i == 5)
	{	/* Sdnv byte containing units and possibly gigs too.	*/

		if (numSize > 30)
		{
			/* Fill the gigs bits after shifting out
			   the 2 bits that belong to units.		*/

			scalar->gigs <<= 5;
			scalar->gigs |= ((*cursor >> 2) & 0x1f);
		}

		/*		Fill the units bits.			*/

		scalar->units = (*cursor & 0x03);
		cursor++;
		i--;
	}

	while (i)
	{	/*	Sdnv bytes containing units only.		*/

		scalar->units <<= 7;
		scalar->units |= (*cursor & 0x7f);
		cursor++;
		i--;
	}

	return sdnvLength;
}

uvast	htonv(uvast hostvast)
{
	static const int	fortyTwo = 42;

	if ((*(char *) &fortyTwo) == 0)	/*	Check first byte.	*/
	{
		/*	Small-endian (network byte order) machine.	*/

		return hostvast;
	}

	/*	Must  reverse the byte order of this number.		*/

#if (!LONG_LONG_OKAY)
	return htonl(hostvast);
#else
	static const vast	mask = 0xffffffff;
	unsigned int		big_part;
	unsigned int		small_part;
	uvast			result;

	big_part = hostvast >> 32;
	small_part = hostvast & mask;
	big_part = htonl(big_part);
	small_part = htonl(small_part);
	result = small_part;
	return (result << 32) | big_part;
#endif
}

uvast	ntohv(uvast netvast)
{
	return htonv(netvast);
}

int	fullyQualified(char *fileName)
{
	CHKZERO(fileName);

#if (defined(VXWORKS))
	if (strncmp("host:", fileName, 5) == 0)
	{
		fileName += 5;
	}

	if (isalpha((int)*fileName) && *(fileName + 1) == ':')
	{
		return 1;
	}

	if (*fileName == '/')
	{
		return 1;
	}

	return 0;

#elif (defined(mingw) || defined(DOS_PATH_DELIMITER))
	if (isalpha(*fileName) && *(fileName + 1) == ':')
	{
		return 1;
	}

	return 0;
#else
	if (*fileName == '/')
	{
		return 1;
	}

	return 0;
#endif
}

int	qualifyFileName(char *fileName, char *buffer, int buflen)
{
	char	pathDelimiter = ION_PATH_DELIMITER;
	int	nameLen;
	int	cwdLen;

	CHKERR(fileName);
	CHKERR(buffer);
	CHKERR(buflen> 0);
	nameLen = strlen(fileName);
	if (fullyQualified(fileName))
	{
		if (nameLen < buflen)
		{
			istrcpy(buffer, fileName, buflen);
			return 0;
		}

		writeMemoNote("[?] File name is too long for qual. buffer.",
				fileName);
		return -1;
	}

	/*	This is a relative path name; must insert cwd.		*/

	if (igetcwd(buffer, buflen) == NULL)
	{
		putErrmsg("Can't get cwd.", NULL);
		return -1;
	}

	cwdLen = strlen(buffer);
	if ((cwdLen + 1 + nameLen + 1) > buflen)
	{
		writeMemoNote("Qualified file name would be too long.",
				fileName);
		return -1;
	}

	*(buffer + cwdLen) = pathDelimiter;
	cwdLen++;		/*	cwdname including delimiter	*/
	istrcpy(buffer + cwdLen, fileName, buflen - cwdLen);
	return 0;
}

void	findToken(char **cursorPtr, char **token)
{
	char	*cursor;

	if (token == NULL)
	{
		ABORT_AS_REQD;
		return;
	}

	*token = NULL;		/*	The default.			*/
	if (cursorPtr == NULL || (*cursorPtr) == NULL)
	{
		ABORT_AS_REQD;
		return;
	}

	cursor = *cursorPtr;

	/*	Skip over any leading whitespace.			*/

	while (isspace((int) *cursor))
	{
		cursor++;
	}

	if (*cursor == '\0')	/*	Nothing but whitespace.		*/
	{
		*cursorPtr = cursor;
		return;
	}

	/*	Token delimited by quotes is the complicated case.	*/

	if (*cursor == '\'')	/*	Quote-delimited token.		*/
	{
		/*	Token is everything after this single quote,
		 *	up to (but not including) the next non-escaped
		 *	single quote.					*/

		cursor++;
		while (*cursor != '\0')
		{
			if (*token == NULL)
			{
				*token = cursor;
			}

			if (*cursor == '\\')	/*	Escape.		*/
			{
				/*	Include the escape character
				 *	plus the following (escaped)
				 *	character (unless it's the end
				 *	of the string) in the token.	*/

				cursor++;
				if (*cursor == '\0')
				{
					*cursorPtr = cursor;
					return;	/*	unmatched quote	*/
				}

				cursor++;
				continue;
			}

			if (*cursor == '\'')	/*	End of token.	*/
			{
				*cursor = '\0';
				cursor++;
				*cursorPtr = cursor;
				return;		/*	matched quote	*/
			}

			cursor++;
		}

		/*	If we get here it's another case of unmatched
		 *	quote, but okay.				*/

		*cursorPtr = cursor;
		return;
	}

	/*	The normal case: a simple whitespace-delimited token.
	 *	Token is this character and all successive characters
	 *	up to (but not including) the next whitespace.		*/

	*token = cursor;
	cursor++;
	while (*cursor != '\0')
	{
		if (isspace((int) *cursor))	/*	End of token.	*/
		{
			*cursor = '\0';
			cursor++;
			break;
		}

		cursor++;
	}

	*cursorPtr = cursor;
}

#ifdef ION_NO_DNS
unsigned int	getAddressOfHost()
{
	return 0;
}

char	*addressToString(struct in_addr address, char *buffer)
{
	CHKNULL(buffer);

	*buffer = 0;
	putErrmsg("Can't convert IP address to string.", NULL);
	return buffer;
}

#else

unsigned int	getAddressOfHost()
{
	char	hostnameBuf[MAXHOSTNAMELEN + 1];

	getNameOfHost(hostnameBuf, sizeof hostnameBuf);
	return getInternetAddress(hostnameBuf);
}

char	*addressToString(struct in_addr address, char *buffer)
{
	char	*result;

	CHKNULL(buffer);
	*buffer = 0;
#if defined (VXWORKS)
	inet_ntoa_b(address, buffer);
#else
	result = inet_ntoa(address);
	if (result == NULL)
	{
		putSysErrmsg("inet_ntoa() returned NULL", NULL);
	}
	else
	{
		istrcpy(buffer, result, 16);
	}
#endif
	return buffer;
}
#endif	/*	ION_NO_DNS						*/

#if (defined(FSWLAN) || !(defined(ION_NO_DNS)))
int	parseSocketSpec(char *socketSpec, unsigned short *portNbr,
		unsigned int *ipAddress)
{
	char		*delimiter;
	char		*hostname;
	char		hostnameBuf[MAXHOSTNAMELEN + 1];
	unsigned int	i4;

	CHKERR(portNbr);
	CHKERR(ipAddress);
	*portNbr = 0;			/*	Use default port nbr.	*/
	*ipAddress = INADDR_ANY;	/*	Use local host address.	*/

	if (socketSpec == NULL || *socketSpec == '\0')
	{
		return 0;		/*	Use defaults.		*/
	}

	delimiter = strchr(socketSpec, ':');
	if (delimiter)
	{
		*delimiter = '\0';	/*	Delimit host name.	*/
	}

	/*	First figure out the IP address.  @ is local host.	*/

	hostname = socketSpec;
	if (strlen(hostname) != 0)
	{
		if (strcmp(hostname, "0.0.0.0") == 0)
		{
			*ipAddress = INADDR_ANY;
		}
		else
		{
			if (strcmp(hostname, "@") == 0)
			{
				getNameOfHost(hostnameBuf, sizeof hostnameBuf);
				hostname = hostnameBuf;
			}

			i4 = getInternetAddress(hostname);
			if (i4 < 1)	/*	Invalid hostname.	*/
			{
				writeMemoNote("[?] Can't get IP address",
						hostname);
				if (delimiter)
				{
					/*	Back out the parsing
					 *	of the socket spec.	*/

					*delimiter = ':';
				}

				return -1;
			}
			else
			{
				*ipAddress = i4;
			}
		}
	}

	/*	Now pick out the port number, if requested.		*/

	if (delimiter == NULL)		/*	No port number.		*/
	{
		return 0;		/*	All done.		*/
	}

	*delimiter = ':';		/*	Back out the parsing.	*/
	i4 = atoi(delimiter + 1);	/*	Get port number.	*/
	if (i4 != 0)
	{
		if (i4 < 1024 || i4 > 65535)
		{
			writeMemoNote("[?] Invalid port number.", utoa(i4));
			return -1;
		}
		else
		{
			*portNbr = i4;
		}
	}

	return 0;
}
#else
int	parseSocketSpec(char *socketSpec, unsigned short *portNbr,
		unsigned int *ipAddress)
{
	return 0;
}
#endif	/*	defined(FSWLAN || !(defined(ION_NO_DNS)))		*/

void	printDottedString(unsigned int hostNbr, char *buffer)
{
	CHKVOID(buffer);
	isprintf(buffer, 16, "%u.%u.%u.%u", (hostNbr >> 24) & 0xff,
		(hostNbr >> 16) & 0xff, (hostNbr >> 8) & 0xff, hostNbr & 0xff);
}

/*	Portable implementation of a safe snprintf: always NULL-
 *	terminates the content of the string composition buffer.	*/

#define SN_FMT_SIZE		64

/*	Flag array indices	*/
#define	SN_LEFT_JUST		0
#define	SN_SIGNED		1
#define	SN_SPACE_PREFIX		2
#define	SN_PAD_ZERO		3
#define	SN_ALT_OUTPUT		4

static void	snGetFlags(char **cursor, char *fmt, int *fmtLen)
{
	int	flags[5];

	/*	Copy all flags to field print format.  No flag is
	 *	copied more than once.					*/

	memset((char *) flags, 0, sizeof flags);
	while (1)
	{	
		switch (**cursor)
		{
		case '-':
			if (flags[SN_LEFT_JUST] == 0)
			{
				*(fmt + *fmtLen) = **cursor;
				(*fmtLen)++;
				flags[SN_LEFT_JUST] = 1;
			}

			break;

		case '+':
			if (flags[SN_SIGNED] == 0)
			{
				*(fmt + *fmtLen) = **cursor;
				(*fmtLen)++;
				flags[SN_SIGNED] = 1;
			}

			break;

		case ' ':
			if (flags[SN_SPACE_PREFIX] == 0)
			{
				*(fmt + *fmtLen) = **cursor;
				(*fmtLen)++;
				flags[SN_SPACE_PREFIX] = 1;
			}

			break;

		case '0':
			if (flags[SN_PAD_ZERO] == 0)
			{
				*(fmt + *fmtLen) = **cursor;
				(*fmtLen)++;
				flags[SN_PAD_ZERO] = 1;
			}

			break;

		case '#':
			if (flags[SN_ALT_OUTPUT] == 0)
			{
				*(fmt + *fmtLen) = **cursor;
				(*fmtLen)++;
				flags[SN_ALT_OUTPUT] = 1;
			}

			break;

		default:
			return;	/*	No more flags for field.	*/
		}

		(*cursor)++;
	}
}

static void	snGetNumber(char **cursor, char *fmt, int *fmtLen, int *number)
{
	int	numDigits = 0;
	char	digit;

	while (1)
	{
		digit = **cursor;
		if (digit < '0' || digit > '9')
		{
			return;	/*	No more digits in number.	*/
		}

		/*	Accumulate number value.			*/

		digit -= 48;	/*	Convert from ASCII.		*/
		if ((*number) < 0)	/*	First digit.		*/
		{
			(*number) = digit;
		}
		else
		{
			(*number) = (*number) * 10;
			(*number) += digit;
		}

		/*	Copy to field format if possible.  Largest
		 *	possible value in a 32-bit number is about
		 *	4 billion, represented in 10 decimal digits.
		 *	Largest possible value in a 64-bit number is
		 *	the square of that value, represented in no
		 *	more than 21 decimal digits.  So any number
		 *	of more than 21 decimal digits is invalid.	*/

		numDigits++;
		if (numDigits < 22)
		{
			*(fmt + *fmtLen) = **cursor;
			(*fmtLen)++;
		}

		(*cursor)++;
	}
}

int	_isprintf(char *buffer, int bufSize, char *format, ...)
{
	va_list		args;
	char		*cursor;
	int		stringLength = 0;
	int		printLength = 0;
	char		fmt[SN_FMT_SIZE];
	int		fmtLen;
	int		minFieldLength;
	int		precision;
	char		scratchpad[64];
	int		numLen;
	int		fieldLength;
	int		isLongLong;		/*	Boolean		*/
	int		*ipval;
	char		*sval;
	int		ival;
	long long	llval;
	double		dval;
	void		*vpval;
	uaddr		uaddrval;

	if (buffer == NULL || bufSize < 1)
	{
		ABORT_AS_REQD;
		return 0;
	}

	if (format == NULL)
	{
		ABORT_AS_REQD;
		if (bufSize < 2)
		{
			*buffer = '\0';
		}
		else
		{
			*buffer = '?';
			*(buffer + 1) = '\0';
		}

		return 0;
	}

	va_start(args, format);
	for (cursor = format; *cursor != '\0'; cursor++)
	{
		if (*cursor != '%')
		{
			if ((stringLength + 1) < bufSize)
			{
				*(buffer + stringLength) = *cursor;
				printLength++;
			}

			stringLength++;
			continue;
		}

		/*	We've encountered a variable-length field in
		 *	the string.					*/

		minFieldLength = -1;	/*	Indicates none.		*/
		precision = -1;		/*	Indicates none.		*/

		/*	Start extracting the field format so that
		 *	we can use sprintf to figure out the length
		 *	of the field.					*/

		fmt[0] = '%';
		fmtLen = 1;
		cursor++;

		/*	Copy any flags for field.			*/

		snGetFlags(&cursor, fmt, &fmtLen);

		/*	Copy the minimum length of field, if present.	*/

		if (*cursor == '*')
		{
			cursor++;
			minFieldLength = va_arg(args, int);
			if (minFieldLength < 0)
			{
				minFieldLength = -1;	/*	None.	*/
			}
			else
			{
				sprintf(scratchpad, "%d", minFieldLength);
				numLen = strlen(scratchpad);
				memcpy(fmt + fmtLen, scratchpad, numLen);
				fmtLen += numLen;
			}
		}
		else
		{
			snGetNumber(&cursor, fmt, &fmtLen, &minFieldLength);
		}

		if (*cursor == '.')	/*	Start of precision.	*/
		{
			fmt[fmtLen] = '.';
			fmtLen++;
			cursor++;

			/*	Copy the precision of the field.	*/

			if (*cursor == '*')
			{
				cursor++;
				precision = va_arg(args, int);
				if (precision < 0)
				{
					precision = -1;	/*	None.	*/
				}
				else
				{
					sprintf(scratchpad, "%d", precision);
					numLen = strlen(scratchpad);
					memcpy(fmt + fmtLen, scratchpad,
							numLen);
					fmtLen += numLen;
				}
			}
			else
			{
				snGetNumber(&cursor, fmt, &fmtLen, &precision);
			}
		}

		/*	Copy the field's length modifier, if any.	*/

		isLongLong = 0;
		if ((*cursor) == 'h'		/*	Short.		*/
		|| (*cursor) == 'L')		/*	Long double.	*/
		{
			fmt[fmtLen] = *cursor;
			fmtLen++;
			cursor++;
		}
		else
		{
			if ((*cursor) == 'l')	/*	Long...		*/
			{
				fmt[fmtLen] = *cursor;
				fmtLen++;
				cursor++;
				if ((*cursor) == 'l')	/*	Vast.	*/
				{
					isLongLong = 1;
					fmt[fmtLen] = *cursor;
					fmtLen++;
					cursor++;
				}
			}
			else
			{
				if ((*cursor) == 'I'
				&& (*(cursor + 1)) == '6'
				&& (*(cursor + 2)) == '4')
				{
#ifdef mingw
					isLongLong = 1;
					fmt[fmtLen] = *cursor;
					fmtLen++;
					cursor++;
					fmt[fmtLen] = *cursor;
					fmtLen++;
					cursor++;
					fmt[fmtLen] = *cursor;
					fmtLen++;
					cursor++;
#endif
				}
			}
		}

		/*	Handle a couple of weird conversion characters
		 *	as applicable.					*/

		if (*cursor == 'n')	/*	Report on string size.	*/
		{
			ipval = va_arg(args, int *);
			if (ipval)
			{
				*ipval = stringLength;
			}

			continue;
		}

		if (*cursor == '%')	/*	Literal '%' in string.	*/
		{
			if ((stringLength + 1) < bufSize)
			{
				*(buffer + stringLength) = '%';
				printLength++;
			}

			stringLength++;
			continue;	/*	No argument consumed.	*/
		}

		/*	Ready to compute field length.			*/

		fmt[fmtLen] = *cursor;	/*	Copy conversion char.	*/
		fmtLen++;
		fmt[fmtLen] = '\0';	/*	Terminate format.	*/

		/*	Handle string field conversion character.	*/

		if (*cursor == 's')
		{
			sval = va_arg(args, char *);
			if (sval == NULL)
			{
				continue;
			}

			fieldLength = strlen(sval);

			/*	Truncate per precision.			*/

			if (precision != -1 && precision < fieldLength)
			{
				fieldLength = precision;
			}

			/*	Add padding as per minFieldLength.	*/

			if (minFieldLength != -1
			&& fieldLength < minFieldLength)
			{
				fieldLength = minFieldLength;
			}

			if (stringLength + fieldLength < bufSize)
			{
				sprintf(buffer + stringLength, fmt, sval);
				printLength += fieldLength;
			}

			stringLength += fieldLength;
			continue;
		}

		/*	Handle numeric field conversion character.	*/

		switch (*cursor)
		{
		case 'd':
		case 'u':
		case 'i':
		case 'o':
		case 'x':
		case 'X':
		case 'c':
			if (isLongLong)
			{
				llval = va_arg(args, long long);
				sprintf(scratchpad, fmt, llval);
			}
			else
			{
				ival = va_arg(args, int);
				sprintf(scratchpad, fmt, ival);
			}

			break;

		case 'f':
		case 'e':
		case 'E':
		case 'g':
		case 'G':
			dval = va_arg(args, double);
			sprintf(scratchpad, fmt, dval);
			break;

		case 'p':
			vpval = va_arg(args, void *);
			uaddrval = (uaddr) vpval;
			sprintf(scratchpad, ADDR_FIELDSPEC, uaddrval);
			break;

		default:		/*	Bad conversion char.	*/
			continue;	/*	No argument consumed.	*/
		}

		fieldLength = strlen(scratchpad);
		if (stringLength + fieldLength < bufSize)
		{
			memcpy(buffer + stringLength, scratchpad, fieldLength);
			printLength += fieldLength;
		}

		stringLength += fieldLength;
	}

	va_end(args);

	/*	NULL-terminate the buffer contents, one way or another.	*/

	if (stringLength < bufSize)
	{
		*(buffer + stringLength) = '\0';
	}
	else
	{
		*(buffer + printLength) = '\0';
	}

	return stringLength;
}

/*	*	*	Other portability adaptations	*	*	*/

size_t	istrlen(const char *from, size_t maxlen)
{
	size_t	length;
	const char	*cursor;

	if (from == NULL)
	{
		ABORT_AS_REQD;
		return 0;
	}

	length = 0;
	if (maxlen > 0)
	{
		for (cursor = from; *cursor; cursor++)
		{
			length++;
			if (length == maxlen)
			{
				break;
			}
		}
	}

	return length;
}

char	*istrcpy(char *buffer, const char *from, size_t bufSize)
{
	int	maxText;
	int	copySize;

	if (buffer == NULL || from == NULL || bufSize < 1)
	{
		ABORT_AS_REQD;
		return NULL;
	}

	maxText = bufSize - 1;
	copySize = istrlen(from, maxText);
	memcpy(buffer, from, copySize);
	*(buffer + copySize) = '\0';
	return buffer;
}

char	*istrcat(char *buffer, char *from, size_t bufSize)
{
	int	maxText;
	int	currTextSize;
	int	maxCopy;
	int	copySize;

	if (buffer == NULL || from == NULL || bufSize < 1)
	{
		ABORT_AS_REQD;
		return NULL;
	}

	maxText = bufSize - 1;
	currTextSize = istrlen(buffer, maxText);
	maxCopy = maxText - currTextSize;
	copySize = istrlen(from, maxCopy);
	memcpy(buffer + currTextSize, from, copySize);
	*(buffer + currTextSize + copySize) = '\0';
	return buffer;
}

char	*igetcwd(char *buf, size_t size)
{
#ifdef FSWWDNAME
#include "wdname.c"
#else
	char	*cwdName;

	CHKNULL(buf);
	CHKNULL(size > 0);
	cwdName = getcwd(buf, size);
	if (cwdName == NULL)
	{
		putSysErrmsg("Can't get CWD name", itoa(size));
	}

	return cwdName;
#endif
}

#ifdef POSIX_TASKS

#ifndef SIGNAL_RULE_CT
#define SIGNAL_RULE_CT	100
#endif

typedef struct
{
	pthread_t	tid;
	int		signbr;
	SignalHandler	handler;
} SignalRule;

static SignalHandler	_signalRules(int signbr, SignalHandler handler)
{
	static SignalRule	rules[SIGNAL_RULE_CT];
	static int		rulesInitialized = 0;
	int			i;
	pthread_t		tid = sm_TaskIdSelf();
	SignalRule		*rule;

	if (!rulesInitialized)
	{
		memset((char *) rules, 0, sizeof rules);
		rulesInitialized = 1;
	}

	if (handler)	/*	Declaring a new signal rule.		*/
	{
		/*	We take this as an opportunity to clear out any
 		 *	existing rules that are no longer needed, due to
 		 *	termination of the threads that declared them.	*/

		for (i = 0, rule = rules; i < SIGNAL_RULE_CT; i++, rule++)
		{
			if (rule->tid == 0)	/*	Cleared.	*/
			{
				if (handler == NULL)	/*	Noted.	*/
				{
					continue;
				}

				/*	Declare new signal rule here.	*/

				rule->tid = tid;
				rule->signbr = signbr;
				rule->handler = handler;
				handler = NULL;		/*	Noted.	*/
				continue;
			}

			/*	This is a declared signal rule.		*/

			if (rule->tid == tid)
			{
				/*	One of thread's own rules.	*/

				if (rule->signbr != signbr)
				{
					continue;	/*	Okay.	*/
				}

				/*	New handler for tid/signbr.	*/

				if (handler)	/*	Not noted yet.	*/
				{
					rule->handler = handler;
					handler = NULL;	/*	Noted.	*/
				}
				else	/*	Noted in another rule.	*/
				{
					rule->tid = 0;	/*	Clear.	*/
				}

				continue;
			}

			/*	Signal rule for another thread.		*/

			if (!sm_TaskExists(rule->tid))
			{
				/*	Obsolete rule; thread is gone.	*/

				rule->tid = 0;		/*	Clear.	*/
			}
		}

		return NULL;
	}

	/*	Just looking up applicable signal rule for tid/signbr.	*/

	for (i = 0, rule = rules; i < SIGNAL_RULE_CT; i++, rule++)
	{
		if (rule->tid == tid && rule->signbr == signbr)
		{
			return rule->handler;
		}
	}

	return NULL;	/*	No applicable signal rule.		*/
}

static void	threadSignalHandler(int signbr)
{
	SignalHandler	handler = _signalRules(signbr, NULL);

	if (handler)
	{
		handler(signbr);
	}
}
#endif	/*	end of #ifdef POSIX_TASKS				*/

#ifdef mingw
void	isignal(int signbr, void (*handler)(int))
{
	oK(signal(signbr, handler));
}

void	iblock(int signbr)
{
	oK(signal(signbr, SIG_IGN));	/*	No thread granularity!	*/
}

#else					/*	Any POSIX O/S.		*/

void	isignal(int signbr, void (*handler)(int))
{
	struct sigaction	action;
#ifdef POSIX_TASKS
	sigset_t		signals;

	oK(sigemptyset(&signals));
	oK(sigaddset(&signals, signbr));
	oK(pthread_sigmask(SIG_UNBLOCK, &signals, NULL));
	oK(_signalRules(signbr, handler));
	handler = threadSignalHandler;
#endif	/*	end of #ifdef POSIX_TASKS				*/
	memset((char *) &action, 0, sizeof(struct sigaction));
	action.sa_handler = handler;
	oK(sigaction(signbr, &action, NULL));
#ifdef freebsd
	oK(siginterrupt(signbr, 1));
#endif
}

void	iblock(int signbr)
{
	sigset_t	signals;

	oK(sigemptyset(&signals));
	oK(sigaddset(&signals, signbr));
	oK(pthread_sigmask(SIG_BLOCK, &signals, NULL));
}
#endif	/*	end of #ifdef mingw					*/

char	*igets(int fd, char *buffer, int buflen, int *lineLen)
{
	char	*cursor = buffer;
	int	maxLine = buflen - 1;
	int	len;

	if (fd < 0 || buffer == NULL || buflen < 1 || lineLen == NULL)
	{
		ABORT_AS_REQD;
		putErrmsg("Invalid argument(s) passed to igets().", NULL);
		return NULL;
	}

	len = 0;
	while (1)
	{
		switch (read(fd, cursor, 1))
		{
		case 0:		/*	End of file; also end of line.	*/
			if (len == 0)		/*	Nothing more.	*/
			{
				*(buffer + len) = '\0';
				*lineLen = len;
				return NULL;	/*	Indicate EOF.	*/
			}

			/*	End of last line.			*/

			break;			/*	Out of switch.	*/

		case -1:
			if (errno == EINTR)	/*	Treat as EOF.	*/
			{
				*(buffer + len) = '\0';
				*lineLen = 0;
				return NULL;
			}

			putSysErrmsg("Failed reading line", itoa(len));
			*(buffer + len) = '\0';
			*lineLen = -1;
			return NULL;

		default:
			if (*cursor == 0x0a)		/*	LF (nl)	*/
			{
				/*	Have reached end of line.	*/

				if (len > 0
				&& *(buffer + (len - 1)) == 0x0d)
				{
					len--;		/*	Lose CR	*/
				}

				break;		/*	Out of switch.	*/
			}

			/*	Have not reached end of line yet.	*/

			if (len == maxLine)	/*	Must truncate.	*/
			{
				break;		/*	Out of switch.	*/
			}

			/*	Okay, include this char in the line...	*/

			len++;

			/*	...and read the next character.		*/

			cursor++;
			continue;
		}

		break;				/*	Out of loop.	*/
	}

	*(buffer + len) = '\0';
	*lineLen = len;
	return buffer;
}

int	iputs(int fd, char *string)
{
	int	totalBytesWritten = 0;
	int	length;
	int	bytesWritten;

	if (fd < 0 || string == NULL)
	{
		ABORT_AS_REQD;
		putErrmsg("Invalid argument(s) passed to iputs().", NULL);
		return -1;
	}

	length = strlen(string);
	while (totalBytesWritten < length)
	{
		bytesWritten = write(fd, string + totalBytesWritten,
				length - totalBytesWritten);
		if (bytesWritten < 0)
		{
			putSysErrmsg("Failed writing line",
					itoa(totalBytesWritten));
			return -1;
		}

		totalBytesWritten += bytesWritten;
	}

	return totalBytesWritten;
}

/*	*	*	Standard TCP functions	*	*	*	*/

#ifndef mingw
void	itcp_handleConnectionLoss()
{
	isignal(SIGPIPE, (void(*)(int)) itcp_handleConnectionLoss);
}
#endif

int	itcp_connect(char *socketSpec, unsigned short defaultPort, int *sock)
{
	unsigned short		portNbr;
	unsigned int		hostNbr;
	struct sockaddr		socketName;
	struct sockaddr_in	*inetName;
	char			dottedString[16];
	char			socketTag[32];

	CHKERR(socketSpec);
	CHKERR(sock);
	*sock = -1;		/*	Default value.			*/
	if (*socketSpec == '\0')
	{
		return 0;	/*	Don't try to connect.		*/
	}

	/*	Construct socket name.					*/

	parseSocketSpec(socketSpec, &portNbr, &hostNbr);
	if (hostNbr == 0)
	{
		putErrmsg("Can't get IP address for host.", socketSpec);
		return 0;
	}

	if (portNbr == 0)
	{
		portNbr = defaultPort;
	}

	printDottedString(hostNbr, dottedString);
	isprintf(socketTag, sizeof socketTag, "%s:%hu", dottedString, portNbr);
	hostNbr = htonl(hostNbr);
	portNbr = htons(portNbr);
	memset((char *) &socketName, 0, sizeof socketName);
	inetName = (struct sockaddr_in *) &socketName;
	inetName->sin_family = AF_INET;
	inetName->sin_port = portNbr;
	memcpy((char *) &(inetName->sin_addr.s_addr), (char *) &hostNbr, 4);
	*sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (*sock < 0)
	{
		putSysErrmsg("Can't open TCP socket", socketTag);
		return -1;
	}

	if (connect(*sock, &socketName, sizeof(struct sockaddr)) < 0)
	{
		putSysErrmsg("Can't connect to TCP socket", socketTag);
		closesocket(*sock);
		*sock = -1;
		return 0;
	}

	writeMemoNote("[i] Connected to TCP socket", socketTag);
	return 1;	/*	Connected to remote socket.		*/
}

static int	itcpSendBytes(int *sock, char *from, int length)
{
	int	bytesWritten;

	/*	This is a single transmission.  It's in a loop only
	 *	so that we can deal with interruptions.			*/

	while (1)	/*	Continue until not interrupted.		*/
	{
		if (*sock == -1)	/*	Socket has been closed.	*/
		{
			return 0;
		}

		bytesWritten = isend(*sock, from, length, 0);
		if (bytesWritten < 0)
		{
			switch (errno)
			{
			case EINTR:	/*	Interrupted; retry.	*/
				continue;

			case EPIPE:	/*	Lost connection.	*/
			case EBADF:
			case ETIMEDOUT:
			case ECONNRESET:
			case EHOSTUNREACH:
				bytesWritten = 0;
			}

			putSysErrmsg("isend error on TCP socket", itoa(*sock));
		}

		return bytesWritten;
	}
}

int	itcp_send(int *sock, char *from, int length)
{
	int	totalBytesSent = 0;
	int	bytesToSend = length;
	int	bytesSent;

	CHKERR(sock);
	CHKERR(from);
	CHKERR(length > 0);

	/*	It's valid for TCP to accept for transmission only
	 *	a subset of the data presented, so we have to loop
	 *	until the entire buffer has been transmitted.		*/

	while (bytesToSend > 0)
	{
		bytesSent = itcpSendBytes(sock, from, bytesToSend);
		switch (bytesSent)
		{
		case -1:	/*	Big problem; shut down.		*/
			return -1;

		case 0:		/*	Connection closed.		*/
			return 0;

		default:
			totalBytesSent += bytesSent;
			from += bytesSent;
			bytesToSend -= bytesSent;
		}
	}

	return totalBytesSent;
}

int	itcp_recv(int *sock, char *into, int length)
{
	int	totalBytesReceived = 0;
	int	bytesToRecv = length;
	int	bytesRead;

	CHKERR(sock);
	CHKERR(into);
	CHKERR(length > 0);

	/*	It's valid for TCP to deliver on demand only a
	 *	subset of the data received, so we have to loop
	 *	until the entire buffer has been acquired.		*/

	while (bytesToRecv > 0)
	{
		if (*sock == -1)	/*	Socket has been closed.	*/
		{
			return 0;
		}

		bytesRead = irecv(*sock, into, bytesToRecv, 0);
		switch (bytesRead)
		{
		case -1:
			switch (errno)
			{
			/*	The recv() call may have been
			 *	interrupted by arrival of SIGTERM,
			 *	in which case reception should simply
			 *	report that it's time to shut down.	*/
			case EINTR:		/*	Shutdown.	*/
			case EBADF:
			case ECONNRESET:
				bytesRead = 0;

			/*	Intentional fall-through to default.	*/

			default:
				putSysErrmsg("irecv() error on TCP socket",
						NULL);
				return bytesRead;
			}

		case 0:			/*	Connection closed.	*/
			return 0;

		default:
			totalBytesReceived += bytesRead;
			into += bytesRead;
			bytesToRecv -= bytesRead;
		}
	}

	return totalBytesReceived;
}

static void	takeIpcLock();
static void	giveIpcLock();

/************************* Shared-memory services *****************************/

	/*	sm_ShmAttach returns have the following meanings:
	 *		1 - created a new memory segment
	 *		0 - memory segment already existed
	 *	       -1 - could not attach to memory segment
	 */

#ifdef RTOS_SHM

	/* ---- Shared Memory services (RTOS) ------------------------- */

#define nShmIds	50

typedef struct
{
	int		key;
	char		*ptr;
	unsigned int	freeNeeded:1;		/*	Boolean.	*/
	unsigned int	nUsers:31;
} SmShm;

static SmShm	*_shmTbl()
{
	static SmShm	shmTable[nShmIds];

	return shmTable;
}

int
sm_ShmAttach(int key, int size, char **shmPtr, uaddr *id)
{
	int	i;
	SmShm	*shm;

	CHKERR(shmPtr);
	CHKERR(id);

    /* If shared memory segment exists, return its location */
	if (key != SM_NO_KEY)
	{
		for (i = 0, shm = _shmTbl(); i < nShmIds; i++, shm++)
		{
			if (shm->key == key)
			{
				*shmPtr = shm->ptr;
				shm->nUsers++;
				*id = i;
				return 0;
			}
		}
	}

    /* create a new "shared memory segment" */
	for (i = 0, shm = _shmTbl(); i < nShmIds; i++, shm++)
	{
		if (shm->ptr == NULL)
		{
			/*	(To prevent dynamic allocation of
			 *	the required memory segment, pre-
			 *	allocate it and place a pointer to
			 *	the previously allocated memory
			 *	into *shmPtr.)				*/

			if (*shmPtr == NULL)
			{
				*shmPtr = (char *) acquireSystemMemory(size);
				if (*shmPtr == NULL)
				{
					putErrmsg("Memory attachment failed.",
							NULL);
					return -1;
				}

				shm->freeNeeded = 1;
			}
			else
			{
				shm->freeNeeded = 0;
			}

			shm->ptr = *shmPtr;
			shm->key = key;
			shm->nUsers = 1;
			*id = i;
			return 1;
		}
	}

	putErrmsg("Too many shared memory segments.", itoa(nShmIds));
	return -1;
}

void
sm_ShmDetach(char *shmPtr)
{
	int	i;
	SmShm	*shm;

	for (i = 0, shm = _shmTbl(); i < nShmIds; i++, shm++)
	{
		if (shm->ptr == shmPtr)
		{
			shm->nUsers--;
			return;
		}
	}
}

void
sm_ShmDestroy(uaddr i)
{
	SmShm	*shm;

	CHKVOID(i >= 0);
	CHKVOID(i < nShmIds);
	shm = _shmTbl() + i;
	if (shm->freeNeeded)
	{
		TRACK_FREE(shm->ptr);
		free(shm->ptr);
		shm->freeNeeded = 0;
	}

	shm->ptr = NULL;
	shm->key = SM_NO_KEY;
	shm->nUsers = 0;
}

#endif			/*	end of #ifdef RTOS_SHM			*/

#ifdef MINGW_SHM

static int	trackIpc(int type, int key)
{
	char	*pipeName = "\\\\.\\pipe\\ion.pipe";
	DWORD	keyDword = (DWORD) key;
	char	msg[1 + sizeof(DWORD)];
	int	startedWinion = 0;
	HANDLE	hPipe;
	DWORD	dwMode;
	BOOL	fSuccess = FALSE;
	DWORD	bytesWritten;
	char	reply[1];
	DWORD	bytesRead;

	msg[0] = type;
	memcpy(msg + 1, (char *) &keyDword, sizeof(DWORD));

	/*	Keep trying to open pipe to winion until succeed.	*/

	while (1)
	{
      		if (WaitNamedPipe(pipeName, 100) == 0) 	/*	Failed.	*/
		{
			if (GetLastError() != ERROR_FILE_NOT_FOUND)
			{
				putErrmsg("Timed out opening pipe to winion.",
						NULL);
				return -1;
			}

			/*	Pipe doesn't exist, so start winion.	*/

			if (startedWinion)	/*	Already did.	*/
			{
				putErrmsg("Can't keep winion runnning.", NULL);
				return -1;
			}

			startedWinion = 1;
			pseudoshell("winion");
			Sleep(100);	/*	Let winion start.	*/
			continue;
		}

		/*	Pipe exists, winion is waiting for connection.	*/

		hPipe = CreateFile(pipeName, GENERIC_READ | GENERIC_WRITE,
				0, NULL, OPEN_EXISTING, 0, NULL);
		if (hPipe != INVALID_HANDLE_VALUE)
		{
			break; 		/*	Got it.			*/
		}

		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			putErrmsg("Can't open pipe to winion.",
					itoa(GetLastError()));
			return -1;
		}
	}

	/*	Connected to pipe.  Change read-mode to message(?!).	*/

	dwMode = PIPE_READMODE_MESSAGE;
	fSuccess = SetNamedPipeHandleState(hPipe, &dwMode, NULL, NULL);
	if (!fSuccess)
	{
		putErrmsg("Can't change pipe's read mode.",
				itoa(GetLastError()));
		return -1;
	}

	fSuccess = WriteFile(hPipe, msg, sizeof msg, &bytesWritten, NULL);
	if (!fSuccess)
	{
		putErrmsg("Can't write to pipe.", itoa(GetLastError()));
		return -1;
	}

	fSuccess = ReadFile(hPipe, reply, 1, &bytesRead, NULL);
	if (!fSuccess)
	{
		putErrmsg("Can't read from pipe.", itoa(GetLastError()));
		return -1;
	}

	CloseHandle(hPipe);
	if (reply[0] == 0 && type != '?')
	{
		return -1;
	}

	return 0;
}

	/* ---- Shared Memory services (mingw -- Windows) ------------- */

typedef struct
{
	char	*shmPtr;
	int	key;
} SmSegment;

#define	MAX_SM_SEGMENTS	20

static void	_smSegment(char *shmPtr, int *key)
{
	static SmSegment	segments[MAX_SM_SEGMENTS];
	static int		segmentsNoted = 0;
	int			i;

	CHKVOID(key);
	for (i = 0; i < segmentsNoted; i++)
	{
		if (segments[i].shmPtr == shmPtr)
		{
			/*	Segment previously noted.		*/

			if (*key == SM_NO_KEY)	/*	Lookup.		*/
			{
				*key = segments[i].key;
			}

			return;
		}
	}

	/*	This is not a previously noted shared memory segment.	*/

	if (*key == SM_NO_KEY)		/*	No key provided.	*/
	{
		return;			/*	Can't record segment.	*/
	}

	/*	Newly noting a shared memory segment.			*/

	if (segmentsNoted == MAX_SM_SEGMENTS)
	{
		puts("No more room for shared memory segments.");
		return;
	}

	segments[segmentsNoted].shmPtr = shmPtr;
	segments[segmentsNoted].key = *key;
	segmentsNoted += 1;
}

int
sm_ShmAttach(int key, int size, char **shmPtr, uaddr *id)
{
	char	memName[32];
	int	minSegSize = 16;
	HANDLE	mappingObj;
	void	*mem;
	int	newSegment = 0;

	CHKERR(shmPtr);
	CHKERR(id);

    	/*	If key is not specified, make one up.			*/

	if (key == SM_NO_KEY)
	{
		key = sm_GetUniqueKey();
	}

	sprintf(memName, "%d.mmap", key);
	if (size != 0)	/*	Want to create segment if not present.	*/
	{
		if (size < minSegSize)
		{
			size = minSegSize;
		}
	}

	/*	Locate the shared memory segment.  If doesn't exist
	 *	yet, create it.						*/

	mappingObj = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, memName);
	if (mappingObj == NULL)		/*	Not found.		*/
	{
		if (size == 0)		/*	Just attaching.		*/
		{
			putErrmsg("Can't open shared memory segment.",
					utoa(GetLastError()));
			return -1;
		}

		/*	Need to create this shared memory segment.	*/

		mappingObj = CreateFileMapping(INVALID_HANDLE_VALUE, NULL,
				PAGE_READWRITE, 0, size, memName);
		if (mappingObj == NULL)
		{
			putErrmsg("Can't create shared memory segment.",
					utoa(GetLastError()));
			return -1;
		}

		if (trackIpc(WIN_NOTE_SM, key) < 0)
		{
			putErrmsg("Can't preserve shared memory.", NULL);
			return -1;
		}

		newSegment = 1;
	}

	mem = MapViewOfFile(mappingObj, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (mem == NULL)
	{
		putErrmsg("Can't map shared memory segment.",
				utoa(GetLastError()));
		return -1;
	}

	/*	Record the ID of this segment in case the segment
	 *	must be detached later.					*/

	_smSegment(mem, &key);
	*shmPtr = (char *) mem;
	*id = (uaddr) mappingObj;
	if (newSegment)
	{
		memset(mem, 0, size);	/*	Initialize to zeroes.	*/
		return 1;
	}

	return 0;
}

void
sm_ShmDetach(char *shmPtr)
{
	return;		/*	Closing last handle detaches segment.	*/
}

void
sm_ShmDestroy(uaddr id)
{
	return;		/*	Closing last handle destroys mapping.	*/
}

#endif			/*	end of #ifdef MINGW_SHM			*/

#ifdef SVR4_SHM

	/* ---- Shared Memory services (Unix) ------------------------- */

int
sm_ShmAttach(int key, int size, char **shmPtr, uaddr *id)
{
	int		minSegSize = 16;
	int		result;
	char		*mem;
	struct shmid_ds	stat;

	CHKERR(shmPtr);
	CHKERR(id);

    /* if key is not specified, make up one */
	if (key == SM_NO_KEY)
	{
		key = sm_GetUniqueKey();
	}

	if (size != 0)	/*	Want to create region if not present.	*/
	{
		if (size < minSegSize)
		{
			size = minSegSize;
		}
	}

    /* create a new shared memory segment, or attach to an existing one */
	if ((*id = shmget(key, size, IPC_CREAT | 0666)) == -1)
	{
		putSysErrmsg("Can't get shared memory segment", itoa(size));
		return -1;
	}

    /* determine if the segment has been initialized yet */
	if (shmctl(*id, IPC_STAT, &stat) == -1)
	{
		putSysErrmsg("Can't get status of shared memory segment",
				itoa(key));
		return -1;
	}

	result = (stat.shm_atime == 0);	/*	If never attached, 1.	*/

	/*	Normally, *shmPtr should be set to NULL prior to
	 *	calling sm_ShmAttach, to let shmat determine the
	 *	attachment point for the memory segment.		*/

	if ((mem = (char *) shmat(*id, *shmPtr, 0)) == ((char *) -1))
	{
		putSysErrmsg("Can't attach shared memory segment", itoa(key));
		return -1;
	}

	if (result == 1)	/*	Newly allocated data segment.	*/
	{
		memset(mem, 0, size);	/*	Initialize to zeroes.	*/
	}

	*shmPtr = mem;
	return result;
}

void
sm_ShmDetach(char *shmPtr)
{
	if (shmdt(shmPtr) < 0)
	{
		putSysErrmsg("Can't detach shared memory segment", NULL);
	}
}

void
sm_ShmDestroy(uaddr id)
{
	if (shmctl(id, IPC_RMID, NULL) < 0)
	{
		putSysErrmsg("Can't destroy shared memory segment", itoa(id));
	}
}

#endif			/*	End of #ifdef SVR4_SHM			*/

/****************** Argument buffer services **********************************/

#ifdef ION_LWT

#define	ARG_BUFFER_CT	256
#define	MAX_ARG_LENGTH	63

typedef struct
{
	int		ownerTid;
	char		arg[MAX_ARG_LENGTH + 1];
} ArgBuffer;

static ArgBuffer	*_argBuffers()
{
	static ArgBuffer argBufTable[ARG_BUFFER_CT];

	return argBufTable;
}

static int	_argBuffersAvbl(int *val)
{
	static int	argBufsAvbl = -1;
	ArgBuffer	*argBuffer;
	int		i;

	if (argBufsAvbl < 0)	/*	Not initialized yet.		*/
	{
		/*	Initialize argument copying.			*/

		argBuffer = _argBuffers();
		for (i = 0; i < ARG_BUFFER_CT; i++)
		{
			argBuffer->ownerTid = 0;
			argBuffer++;
		}

		argBufsAvbl = ARG_BUFFER_CT;
	}

	if (val == NULL)
	{
		return argBufsAvbl;
	}

	argBufsAvbl = *val;
	return 0;
}

static int	copyArgs(int argc, char **argv)
{
	int		i;
	int		j;
	ArgBuffer	*buf;
	char		*arg;
	int		argLen;

	if (argc > _argBuffersAvbl(NULL))
	{
		putErrmsg("No available argument buffers.", NULL);
		return -1;
	}

	/*	Copy each argument into the next available argument
	 *	buffer, tagging each consumed buffer with -1 so that
	 *	it can be permanently tagged when the ownerTid is
	 *	known, and replace each original argument with a
	 *	pointer to its copy in the argBuffers.			*/

	for (i = 0, buf = _argBuffers(), j = 0; j < argc; j++)
	{
		arg = argv[j];
		argLen = strlen(arg);
		if (argLen > MAX_ARG_LENGTH)
		{
			argLen = MAX_ARG_LENGTH;
		}

		while (1)
		{
			CHKERR(i < ARG_BUFFER_CT);
			if (buf->ownerTid != 0)	/*	Unavailable.	*/
			{
				i++;
				buf++;
				continue;
			}

			/*	Copy argument into this buffer.		*/

			memcpy(buf->arg, arg, argLen);
			buf->arg[argLen] = '\0';
			buf->ownerTid = -1;
			argv[j] = buf->arg;

			/*	Skip over this buffer for next arg.	*/

			i++;
			buf++;
			break;
		}
	}

	return 0;
}

static void	tagArgBuffers(int tid)
{
	int		avbl;
	int		i;
	ArgBuffer	*buf;

	avbl = _argBuffersAvbl(NULL);
	for (i = 0, buf = _argBuffers(); i < ARG_BUFFER_CT; i++, buf++)
	{
		if (buf->ownerTid == -1)
		{
			buf->ownerTid = tid;
			if (tid != 0)
			{
				avbl--;
			}
		}
#if !(defined (VXWORKS))
		else	/*	An opportunity to release arg buffers.	*/
		{
			if (buf->ownerTid != 0 && !sm_TaskExists(buf->ownerTid))
			{
				buf->ownerTid = 0;
				avbl++;
			}
		}
#endif
	}

	oK(_argBuffersAvbl(&avbl));
}

#endif		/*	End of #ifdef ION_LWT				*/

/****************** Semaphore services **********************************/

#ifdef VXWORKS_SEMAPHORES

	/* ---- IPC services access control (VxWorks) ----------------- */

#include <vxWorks.h>
#include <semLib.h>
#include <taskLib.h>
#include <timers.h>
#include <sysSymTbl.h>
#include <taskVarLib.h>
#include <dbgLib.h>

#define nSemIds 200

typedef struct
{
	int	key;
	SEM_ID	id;
	int	ended;
} SmSem;

static SmSem	*_semTbl()
{
	static SmSem	semTable[nSemIds];

	return semTable;
}

	/* ---- Semaphore services (VxWorks) --------------------------- */

static void	releaseArgBuffers(WIND_TCB *pTcb)
{
	int		tid = (int) pTcb;
	int		avbl;
	int		i;
	ArgBuffer	*buf;

	avbl = _argBuffersAvbl(NULL);
	for (i = 0, buf = _argBuffers(); i < ARG_BUFFER_CT; i++, buf++)
	{
		if (buf->ownerTid == tid)
		{
			buf->ownerTid = 0;
			avbl++;
		}
	}

	oK(_argBuffersAvbl(&avbl));
}

static int	initializeIpc()
{
	SmSem		*semTbl = _semTbl();
	SmSem		*sem;
	int		i;
	SmShm		*shmTbl = _shmTbl();
	SmShm		*shm;

	for (i = 0, sem = semTbl; i < nSemIds; i++, sem++)
	{
		sem->key = SM_NO_KEY;
		sem->id = NULL;
		sem->ended = 0;
	}

	for (i = 0, shm = shmTbl; i < nShmIds; i++, shm++)
	{
		shm->key = SM_NO_KEY;
		shm->ptr = NULL;
		shm->freeNeeded = 0;
		shm->nUsers = 0;
	}

	/*	Note: we are abundantly aware that the
	 *	prototype for the function that must be
	 *	passed to taskDeleteHookAdd, according to
	 *	the VxWorks 5.4 Reference Manual, is NOT
	 *	of the same type as FUNCPTR, which returns
	 *	int rather than void.  We do this cast only
	 *	to get rid of a compiler warning which is,
	 *	at bottom, due to a bug in Wind River's
	 *	function prototype for taskDeleteHookAdd.	*/

	if (taskDeleteHookAdd((FUNCPTR) releaseArgBuffers) == ERROR)
	{
		putSysErrmsg("Can't register releaseArgBuffers", NULL);
		return -1;
	}

	giveIpcLock();
	return 0;
}

/*	Note that the ipcSemaphore is allocated using the VxWorks
 *	semBLib functions directly rather than the ICI VxWorks
 *	semaphore system.  This is necessary for bootstrapping the
 *	ICI semaphore system: only after the ipcSemaphore exists
 *	can we initialize the semaphore tables, enabling subsequent
 *	semaphores to be allocated in a more portable fashion.		*/

static SEM_ID	_ipcSemaphore(int stop)
{
	static SEM_ID	ipcSem = NULL;

	if (stop)
	{
		if (ipcSem)
		{
			semDelete(ipcSem);
			ipcSem = NULL;
		}

		return NULL;
	}

	if (ipcSem == NULL)
	{
		ipcSem = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
		if (ipcSem == NULL)
		{
			putSysErrmsg("Can't initialize IPC semaphore", NULL);
		}
		else
		{
			if (initializeIpc() < 0)
			{
				semDelete(ipcSem);
				ipcSem = NULL;
			}
		}
	}

	return ipcSem;
}

int	sm_ipc_init()
{
	SEM_ID	sem = _ipcSemaphore(0);

	if (sem == NULL)
	{
		putErrmsg("Can't initialize IPC.", NULL);
		return -1;
	}

	return 0;
}

void	sm_ipc_stop()
{
	oK(_ipcSemaphore(1));
}

static void	takeIpcLock()
{
	semTake(_ipcSemaphore(0), WAIT_FOREVER);
}

static void	giveIpcLock()
{
	semGive(_ipcSemaphore(0));
}

sm_SemId	sm_SemCreate(int key, int semType)
{
	SmSem	*semTbl = _semTbl();
	SmSem	*sem;
	SEM_ID	semId;
	int	i;

	/*	If key is not specified, invent one.			*/

	if (key == SM_NO_KEY)
	{
		key = sm_GetUniqueKey();
	}

	takeIpcLock();
    /* If semaphore exists, return its ID */
	for (i = 0; i < nSemIds; i++)
	{
		if (semTbl[i].key == key)
		{
			giveIpcLock();
			return i;
		}
	}

    /* create a new semaphore */
	for (i = 0, sem = semTbl; i < nSemIds; i++, sem++)
	{
		if (sem->id == NULL)
		{
			if (semType == SM_SEM_PRIORITY)
			{
				semId = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);
			}
			else
			{
				semId = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
			}

			if (semId == NULL)
			{
				giveIpcLock();
				putSysErrmsg("Can't create semaphore",
						itoa(key));
				return SM_SEM_NONE;
			}

			sem->id = semId;
			sem->key = key;
			sem->ended = 0;
			sm_SemGive(i);	/*	(First taker succeeds.)	*/
			giveIpcLock();
			return i;
		}
	}

	giveIpcLock();
	putErrmsg("Too many semaphores.", itoa(nSemIds));
	return SM_SEM_NONE;
}

void	sm_SemDelete(sm_SemId i)
{
	SmSem	*semTbl = _semTbl();
	SmSem	*sem;

	CHKVOID(i >= 0);
	CHKVOID(i < nSemIds);
	sem = semTbl + i;
	takeIpcLock();
	if (semDelete(sem->id) == ERROR)
	{
		giveIpcLock();
		putSysErrmsg("Can't delete semaphore", itoa(i));
		return;
	}

	sem->id = NULL;
	sem->key = SM_NO_KEY;
	giveIpcLock();
}

int	sm_SemTake(sm_SemId i)
{
	SmSem	*semTbl = _semTbl();
	SmSem	*sem;

	CHKERR(i >= 0);
	CHKERR(i < nSemIds);
	sem = semTbl + i;
	if (semTake(sem->id, WAIT_FOREVER) == ERROR)
	{
		putSysErrmsg("Can't take semaphore", itoa(i));
		return -1;
	}

	return 0;
}

void	sm_SemGive(sm_SemId i)
{
	SmSem	*semTbl = _semTbl();
	SmSem	*sem;

	CHKVOID(i >= 0);
	CHKVOID(i < nSemIds);
	sem = semTbl + i;
	if (semGive(sem->id) == ERROR)
	{
		putSysErrmsg("Can't give semaphore", itoa(i));
	}
}

void	sm_SemEnd(sm_SemId i)
{
	SmSem	*semTbl = _semTbl();
	SmSem	*sem;

	CHKVOID(i >= 0);
	CHKVOID(i < nSemIds);
	sem = semTbl + i;
	sem->ended = 1;
	sm_SemGive(i);
}

int	sm_SemEnded(sm_SemId i)
{
	SmSem	*semTbl = _semTbl();
	SmSem	*sem;
	int	ended;

	CHKZERO(i >= 0);
	CHKZERO(i < nSemIds);
	sem = semTbl + i;
	ended = sem->ended;
	if (ended)
	{
		sm_SemGive(i);	/*	Enable multiple tests.		*/
	}

	return ended;
}

void	sm_SemUnend(sm_SemId i)
{
	SmSem	*semTbl = _semTbl();
	SmSem	*sem;

	CHKVOID(i >= 0);
	CHKVOID(i < nSemIds);
	sem = semTbl + i;
	sem->ended = 0;
}

int	sm_SemUnwedge(sm_SemId i, int timeoutSeconds)
{
	SmSem	*semTbl = _semTbl();
	SmSem	*sem;
	int	ticks;

	CHKERR(i >= 0);
	CHKERR(i < nSemIds);
	sem = semTbl + i;
	if (timeoutSeconds < 1) timeoutSeconds = 1;
	ticks = sysClkRateGet() * timeoutSeconds;
	if (semTake(sem->id, ticks) == ERROR)
	{
		if (errno != S_objLib_OBJ_TIMEOUT)
		{
			putSysErrmsg("Can't unwedge semaphore", itoa(i));
			return -1;
		}
	}

	if (semGive(sem->id) == ERROR)
	{
		putSysErrmsg("Can't unwedge semaphore", itoa(i));
		return -1;
	}

	return 0;
}

#endif			/*	End of #ifdef VXWORKS_SEMAPHORES	*/

#ifdef MINGW_SEMAPHORES

	/* ---- Semaphore services (mingw) --------------------------- */

#ifndef SM_SEMKEY
#define SM_SEMKEY	(0xee01)
#endif
#ifndef SM_SEMTBLKEY
#define SM_SEMTBLKEY	(0xee02)
#endif

typedef struct
{
	int	key;
	int	inUse;
	int	ended;
} IciSemaphore;

typedef struct
{
	IciSemaphore	semaphores[SEMMNS];
	int		semaphoresCreated;
} SemaphoreTable;

static SemaphoreTable	*_semTbl(int stop)
{
	static SemaphoreTable	*semaphoreTable = NULL;
	static uaddr		semtblId = 0;

	if (stop)
	{
		if (semaphoreTable != NULL)
		{
			sm_ShmDetach((char *) semaphoreTable);
			semaphoreTable = NULL;
		}

		return NULL;
	}

	if (semaphoreTable == NULL)
	{
		switch(sm_ShmAttach(SM_SEMTBLKEY, sizeof(SemaphoreTable),
				(char **) &semaphoreTable, &semtblId))
		{
		case -1:
			putErrmsg("Can't create semaphore table.", NULL);
			break;

		case 0:
			break;		/*	Semaphore table exists.	*/

		default:		/*	New SemaphoreTable.	*/
			memset((char *) semaphoreTable, 0,
					sizeof(semaphoreTable));
		}
	}

	return semaphoreTable;
}

int	sm_ipc_init()
{
	char	semName[32];
	HANDLE	ipcSemaphore;

	oK(_semTbl(0));

	/*	Create the IPC semaphore and preserve it.		*/

	sprintf(semName, "%d.event", SM_SEMKEY);
	ipcSemaphore = CreateEvent(NULL, FALSE, FALSE, semName);
	if (ipcSemaphore == NULL)
	{
		putErrmsg("Can't create IPC semaphore.", NULL);
		return -1;
	}

	if (GetLastError() != ERROR_ALREADY_EXISTS)
	{
		oK(SetEvent(ipcSemaphore));

		/*	Preserve the IPC semaphore.			*/

		if (trackIpc(WIN_NOTE_SEMAPHORE, SM_SEMKEY) < 0)
		{
			putErrmsg("Can't preserve IPC semaphore.", NULL);
			sm_ipc_stop();
			return -1;
		}
	}

	return 0;
}

void	sm_ipc_stop()
{
	oK(trackIpc(WIN_STOP_ION, 0));
}

static HANDLE	getSemaphoreHandle(int key)
{
	char	semName[32];

	sprintf(semName, "%d.event", key);
	return OpenEvent(EVENT_ALL_ACCESS, FALSE, semName);
}

static void	takeIpcLock()
{
	HANDLE	ipcSemaphore = getSemaphoreHandle(SM_SEMKEY);

	oK(WaitForSingleObject(ipcSemaphore, INFINITE));
	CloseHandle(ipcSemaphore);
}

static void	giveIpcLock()
{
	HANDLE	ipcSemaphore = getSemaphoreHandle(SM_SEMKEY);

	oK(SetEvent(ipcSemaphore));
	CloseHandle(ipcSemaphore);
}

sm_SemId	sm_SemCreate(int key, int semType)
{
	SemaphoreTable	*semTbl;
	int		i;
	IciSemaphore	*sem;
	char		semName[32];
	HANDLE		semId;

	/*	If key is not specified, invent one.			*/

	if (key == SM_NO_KEY)
	{
		key = sm_GetUniqueKey();
	}

	/*	Look through list of all existing ICI semaphores.	*/

	takeIpcLock();
	semTbl = _semTbl(0);
	if (semTbl == NULL)
	{
		giveIpcLock();
		putErrmsg("No semaphore table.", NULL);
		return SM_SEM_NONE;
	}

	for (i = 0, sem = semTbl->semaphores; i < semTbl->semaphoresCreated;
			i++, sem++)
	{
		if (sem->key == key)
		{
			giveIpcLock();
			return i;	/*	already created		*/
		}
	}

	/*	No existing semaphore for this key; allocate new one.	*/

	for (i = 0, sem = semTbl->semaphores; i < SEMMNS; i++, sem++)
	{
		if (sem->inUse)
		{
			continue;
		}

		sprintf(semName, "%d.event", key);
		semId = CreateEvent(NULL, FALSE, FALSE, semName);
		if (semId == NULL)
		{
			giveIpcLock();
			putErrmsg("Can't create semaphore.",
					utoa(GetLastError()));
			return SM_SEM_NONE;
		}

		if (GetLastError() != ERROR_ALREADY_EXISTS)
		{
			if (trackIpc(WIN_NOTE_SEMAPHORE, key) < 0)
			{
				CloseHandle(semId);
				giveIpcLock();
				putErrmsg("Can't preserve semaphore.", NULL);
				return SM_SEM_NONE;
			}
		}

		CloseHandle(semId);
		sem->inUse = 1;
		sem->key = key;
		sem->ended = 0;
		if (!(i < semTbl->semaphoresCreated))
		{
			semTbl->semaphoresCreated++;
		}

		sm_SemGive(i);		/*	(First taker succeeds.)	*/
		giveIpcLock();
		return i;
	}

	giveIpcLock();
	putErrmsg("Can't add any more semaphores.", NULL);
	return SM_SEM_NONE;
}

void	sm_SemDelete(sm_SemId i)
{
	SemaphoreTable	*semTbl = _semTbl(0);
	IciSemaphore	*sem;

	CHKVOID(i >= 0);
	CHKVOID(i < SEMMNS);
	sem = semTbl->semaphores + i;
	takeIpcLock();
	if (sem->inUse)
	{
		if (trackIpc(WIN_FORGET_SEMAPHORE, sem->key) < 0)
		{
			putErrmsg("Can't detach from semaphore.", NULL);
		}

		sem->inUse = 0;
		sem->key = SM_NO_KEY;
	}

	giveIpcLock();
}

int	sm_SemTake(sm_SemId i)
{
	SemaphoreTable	*semTbl = _semTbl(0);
	IciSemaphore	*sem;
	HANDLE		semId;

	CHKERR(i >= 0);
	CHKERR(i < SEMMNS);
	sem = semTbl->semaphores + i;
	CHKERR(sem->inUse);
	semId = getSemaphoreHandle(sem->key);
	if (semId == NULL)
	{
		putSysErrmsg("Can't take semaphore", itoa(i));
		return -1;
	}

	oK(WaitForSingleObject(semId, INFINITE));
	CloseHandle(semId);
	return 0;
}

void	sm_SemGive(sm_SemId i)
{
	SemaphoreTable	*semTbl = _semTbl(0);
	IciSemaphore	*sem;
	HANDLE		semId;

	CHKVOID(i >= 0);
	CHKVOID(i < SEMMNS);
	sem = semTbl->semaphores + i;
	CHKVOID(sem->inUse);
	semId = getSemaphoreHandle(sem->key);
	if (semId == NULL)
	{
		putSysErrmsg("Can't give semaphore", itoa(i));
		return;
	}

	oK(SetEvent(semId));
	CloseHandle(semId);
}

void	sm_SemEnd(sm_SemId i)
{
	SemaphoreTable	*semTbl = _semTbl(0);
	IciSemaphore	*sem;

	CHKVOID(i >= 0);
	CHKVOID(i < SEMMNS);
	sem = semTbl->semaphores + i;
	CHKVOID(sem->inUse);
	sem->ended = 1;
	sm_SemGive(i);
}

int	sm_SemEnded(sm_SemId i)
{
	SemaphoreTable	*semTbl = _semTbl(0);
	IciSemaphore	*sem;
	int		ended;

	CHKZERO(i >= 0);
	CHKZERO(i < SEMMNS);
	sem = semTbl->semaphores + i;
	CHKZERO(sem->inUse);
	ended = sem->ended;
	if (ended)
	{
		sm_SemGive(i);	/*	Enable multiple tests.		*/
	}

	return ended;
}

void	sm_SemUnend(sm_SemId i)
{
	SemaphoreTable	*semTbl = _semTbl(0);
	IciSemaphore	*sem;

	CHKVOID(i >= 0);
	CHKVOID(i < SEMMNS);
	sem = semTbl->semaphores + i;
	CHKVOID(sem->inUse);
	sem->ended = 0;
}

int	sm_SemUnwedge(sm_SemId i, int timeoutSeconds)
{
	SemaphoreTable	*semTbl = _semTbl(0);
	IciSemaphore	*sem;
	HANDLE		semId;
	DWORD		millisec;

	CHKERR(i >= 0);
	CHKERR(i < SEMMNS);
	sem = semTbl->semaphores + i;
	CHKERR(sem->inUse);
	semId = getSemaphoreHandle(sem->key);
	if (semId == NULL)
	{
		putSysErrmsg("Can't unwedge semaphore", itoa(i));
		return -1;
	}

	if (timeoutSeconds < 1) timeoutSeconds = 1;
	millisec = timeoutSeconds * 1000;
	oK(WaitForSingleObject(semId, millisec));
	oK(SetEvent(semId));
	CloseHandle(semId);
	return 0;
}

#endif			/*	End of #ifdef MINGW_SEMAPHORES		*/

#ifdef POSIX_SEMAPHORES

	/* ---- Semaphore services (POSIX, including RTEMS) ---------	*/

typedef struct
{
	int		key;
	sem_t		semobj;
	sem_t		*id;
	int		ended;
} SmSem;

static SmSem	*_semTbl()
{
	static SmSem	semTable[SEM_NSEMS_MAX];
	static int	semTableInitialized = 0;

	if (!semTableInitialized)
	{
		memset((char *) semTable, 0, sizeof semTable);
		semTableInitialized = 1;
	}

	return semTable;
}

static sem_t	*_ipcSemaphore(int stop)
{
	static sem_t	ipcSem;
	static int	ipcSemInitialized = 0;

	if (stop)
	{
		if (ipcSemInitialized)
		{
			oK(sem_destroy(&ipcSem));
			ipcSemInitialized = 0;
		}

		return NULL;
	}

	if (ipcSemInitialized == 0)
	{
		if (sem_init(&ipcSem, 0, 0) < 0)
		{
			putSysErrmsg("Can't initialize IPC semaphore", NULL);
			return NULL;
		}

		/*	Initialize the semaphore system.		*/

		oK(_semTbl());
		ipcSemInitialized = 1;
		giveIpcLock();
	}

	return &ipcSem;
}

int	sm_ipc_init()
{
	if (_ipcSemaphore(0) == NULL)
	{
		putErrmsg("Can't initialize IPC.", NULL);
		return -1;
	}

	return 0;
}

void	sm_ipc_stop()
{
	oK(_ipcSemaphore(1));
}

static void	takeIpcLock()
{
	oK(sem_wait(_ipcSemaphore(0)));

}

static void	giveIpcLock()
{
	oK(sem_post(_ipcSemaphore(0)));
}

sm_SemId	sm_SemCreate(int key, int semType)
{
	SmSem	*semTbl = _semTbl();
	int	i;
	SmSem	*sem;

	/*	If key is not specified, invent one.			*/

	if (key == SM_NO_KEY)
	{
		key = sm_GetUniqueKey();
	}

	takeIpcLock();
	for (i = 0, sem = semTbl; i < SEM_NSEMS_MAX; i++, sem++)
	{
		if (sem->key == key)
		{
			giveIpcLock();
			return i;
		}
	}

	for (i = 0, sem = semTbl; i < SEM_NSEMS_MAX; i++, sem++)
	{
		if (sem->id == NULL)	/*	Not in use.		*/
		{
			if (sem_init(&(sem->semobj), 0, 0) < 0)
			{
				giveIpcLock();
				putSysErrmsg("Can't init semaphore", NULL);
				return SM_SEM_NONE;
			}

			sem->id = &sem->semobj;
			sem->key = key;
			sem->ended = 0;
			sm_SemGive(i);	/*	(First taker succeeds.)	*/
			giveIpcLock();
			return i;
		}
	}

	giveIpcLock();
	putErrmsg("Too many semaphores.", itoa(SEM_NSEMS_MAX));
	return SM_SEM_NONE;
}

void	sm_SemDelete(sm_SemId i)
{
	SmSem	*semTbl = _semTbl();
	SmSem	*sem = semTbl + i;

	CHKVOID(i >= 0);
	CHKVOID(i < SEM_NSEMS_MAX);
	takeIpcLock();
	if (sem_destroy(&(sem->semobj)) < 0)
	{
		giveIpcLock();
		putSysErrmsg("Can't destroy semaphore", itoa(i));
		return;
	}

	sem->id = NULL;
	sem->key = SM_NO_KEY;
	giveIpcLock();
}

int	sm_SemTake(sm_SemId i)
{
	SmSem	*semTbl = _semTbl();
	SmSem	*sem = semTbl + i;

	CHKERR(i >= 0);
	CHKERR(i < SEM_NSEMS_MAX);
	while (sem_wait(sem->id) < 0)
	{
		if (errno == EINTR)
		{
			continue;
		}

		putSysErrmsg("Can't take semaphore", itoa(i));
		return -1;
	}

	return 0;
}

void	sm_SemGive(sm_SemId i)
{
	SmSem	*semTbl = _semTbl();
	SmSem	*sem = semTbl + i;

	CHKVOID(i >= 0);
	CHKVOID(i < SEM_NSEMS_MAX);
	if (sem_post(sem->id) < 0)
	{
		putSysErrmsg("Can't give semaphore", itoa(i));
	}
}

void	sm_SemEnd(sm_SemId i)
{
	SmSem	*semTbl = _semTbl();
	SmSem	*sem = semTbl + i;

	CHKVOID(i >= 0);
	CHKVOID(i < SEM_NSEMS_MAX);
	sem->ended = 1;
	sm_SemGive(i);
}

int	sm_SemEnded(sm_SemId i)
{
	SmSem	*semTbl = _semTbl();
	SmSem	*sem = semTbl + i;
	int	ended;

	CHKZERO(i >= 0);
	CHKZERO(i < SEM_NSEMS_MAX);
	ended = sem->ended;
	if (ended)
	{
		sm_SemGive(i);	/*	Enable multiple tests.		*/
	}

	return ended;
}

void	sm_SemUnend(sm_SemId i)
{
	SmSem	*semTbl = _semTbl();
	SmSem	*sem = semTbl + i;

	CHKVOID(i >= 0);
	CHKVOID(i < SEM_NSEMS_MAX);
	sem->ended = 0;
}

int	sm_SemUnwedge(sm_SemId i, int timeoutSeconds)
{
	SmSem		*semTbl = _semTbl();
	SmSem		*sem = semTbl + i;
	struct timespec	timeout;

	CHKERR(i >= 0);
	CHKERR(i < SEM_NSEMS_MAX);
	if (timeoutSeconds < 1) timeoutSeconds = 1;
	oK(clock_gettime(CLOCK_REALTIME, &timeout));
	timeout.tv_sec += timeoutSeconds;
	while (sem_timedwait(sem->id, &timeout) < 0)
	{
		switch (errno)
		{
		case EINTR:
			continue;

		case ETIMEDOUT:
			break;	/*	Out of switch.			*/

		default:
			putSysErrmsg("Can't unwedge semaphore", itoa(i));
			return -1;
		}

		break;		/*	Out of loop.			*/
	}

	if (sem_post(sem->id) < 0)
	{
		putSysErrmsg("Can't unwedge semaphore", itoa(i));
		return -1;
	}

	return 0;
}

#endif			/*	End of #ifdef POSIX_SEMAPHORES		*/

#ifdef SVR4_SEMAPHORES

	/* ---- Semaphore services (SVR4) -----------------------------	*/

#ifndef SM_SEMKEY
#define SM_SEMKEY	(0xee01)
#endif
#ifndef SM_SEMBASEKEY
#define SM_SEMBASEKEY	(0xee02)
#endif

/*	Note: one semaphore set is consumed by the ipcSemaphore.	*/
#define MAX_SEM_SETS	(SEMMNI - 1)

typedef struct
{
	int		semid;
	int		idsAllocated;
} IciSemaphoreSet;

/*	Note: we can actually always compute a semaphore's semSetIdx
 *	and semNbr from a sm_SemId (they are sm_SemId/SEMMSL and
 *	sm_SemId%SEMMSL), but we store the precomputed values to
 *	avoid having to do all that integer division; should make
 *	taking and releasing semaphores somewhat faster.		*/

typedef struct
{
	int		key;
	int		semSetIdx;
	int		semNbr;
	int		inUse;
	int		ended;
} IciSemaphore;

typedef struct
{
	IciSemaphoreSet	semSets[MAX_SEM_SETS];
	int		currSemSet;
	IciSemaphore	semaphores[SEMMNS];
	int		idsAllocated;
} SemaphoreBase;

static SemaphoreBase	*_sembase(int stop)
{
	static SemaphoreBase	*semaphoreBase = NULL;
	static uaddr		sembaseId = 0;
	int			semSetIdx;
	IciSemaphoreSet		*semset;
	int			i;

	if (stop)
	{
		if (semaphoreBase != NULL)
		{
			semSetIdx = 0;
			while (semSetIdx < MAX_SEM_SETS)
			{
				semset = semaphoreBase->semSets + semSetIdx;
				oK(semctl(semset->semid, 0, IPC_RMID, NULL));
            			semSetIdx++;
			}

			sm_ShmDestroy(sembaseId);
			semaphoreBase = NULL;
		}

		return NULL;
	}

	if (semaphoreBase == NULL)
	{
		switch (sm_ShmAttach(SM_SEMBASEKEY, sizeof(SemaphoreBase),
				(char **) &semaphoreBase, &sembaseId))
		{
		case -1:
			putErrmsg("Can't create semaphore base.", NULL);
			break;

		case 0:
			break;		/*	SemaphoreBase exists.	*/

		default:		/*	New SemaphoreBase.	*/
			semaphoreBase->idsAllocated = 0;
			semaphoreBase->currSemSet = 0;
			for (i = 0; i < MAX_SEM_SETS; i++)
			{
				semaphoreBase->semSets[i].semid = -1;
			}

			/*	Acquire initial semaphore set.		*/

			semset = semaphoreBase->semSets
					+ semaphoreBase->currSemSet;
			semset->semid = semget(sm_GetUniqueKey(), SEMMSL,
					IPC_CREAT | 0666);
			if (semset->semid < 0)
			{
				putSysErrmsg("Can't get initial semaphore set",
						NULL);
				sm_ShmDestroy(sembaseId);
				semaphoreBase = NULL;
				break;
			}

			semset->idsAllocated = 0;
		}
	}

	return semaphoreBase;
}

/*	Note that the ipcSemaphore gets an entire semaphore set for
 *	itself.  This is necessary for bootstrapping the ICI svr4-
 *	based semaphore system: only after the ipcSemaphore exists
 *	can we initialize the semaphore base, enabling subsequent
 *	semaphores to be allocated more efficiently.			*/

static int	_ipcSemaphore(int stop)
{
	static int	ipcSem = -1;

	if (stop)
	{
		oK(_sembase(1));
		if (ipcSem != -1)
		{
			oK(semctl(ipcSem, 0, IPC_RMID, NULL));
			ipcSem = -1;
		}

		return ipcSem;
	}

	if (ipcSem == -1)
	{
		ipcSem = semget(SM_SEMKEY, 1, IPC_CREAT | 0666);
		if (ipcSem == -1)
		{
			putSysErrmsg("Can't initialize IPC semaphore",
					itoa(SM_SEMKEY));
		}
		else
		{
			if (_sembase(0) == NULL)
			{
				oK(semctl(ipcSem, 0, IPC_RMID, NULL));
				ipcSem = -1;
			}
		}
	}

	return ipcSem;
}

int	sm_ipc_init()
{
	if (_ipcSemaphore(0) == -1)
	{
		putErrmsg("Can't initialize IPC.", NULL);
		return -1;
	}

	return 0;
}

void	sm_ipc_stop()
{
	oK(_ipcSemaphore(1));
}

static void	takeIpcLock()
{
	struct sembuf	sem_op[2] = { {0,0,0}, {0,1,0} };

	oK(semop(_ipcSemaphore(0), sem_op, 2));
}

static void	giveIpcLock()
{
	struct sembuf	sem_op = { 0, -1, IPC_NOWAIT };

	oK(semop(_ipcSemaphore(0), &sem_op, 1));
}

sm_SemId	sm_SemCreate(int key, int semType)
{
	SemaphoreBase	*sembase;
	int		i;
	IciSemaphore	*sem;
	IciSemaphoreSet	*semset;
	int		semSetIdx;
	int		semid;

	/*	If key is not specified, invent one.			*/

	if (key == SM_NO_KEY)
	{
		key = sm_GetUniqueKey();
	}

	/*	Look through list of all existing ICI semaphores.	*/

	takeIpcLock();
	sembase = _sembase(0);
	if (sembase == NULL)
	{
		giveIpcLock();
		putErrmsg("No semaphore base.", NULL);
		return SM_SEM_NONE;
	}

	for (i = 0, sem = sembase->semaphores; i < sembase->idsAllocated;
			i++, sem++)
	{
		if (sem->key == key)
		{
			giveIpcLock();
			return i;	/*	already created		*/
		}
	}

	/*	No existing semaphore for this key; repurpose one
	 *	that is unused or allocate the next one in the current
	 *	semaphore set.						*/

	semset = sembase->semSets + sembase->currSemSet;
	for (i = 0, sem = sembase->semaphores; i < SEMMNS; i++, sem++)
	{
		if (sem->inUse)
		{
			continue;
		}

		/*	Found available slot in table.			*/

		sem->inUse = 1;
		sem->key = key;
		sem->ended = 0;
		if (i >= sembase->idsAllocated)
		{
			/*	Must allocate new semaphore ID in slot.	*/

			sem->semSetIdx = sembase->currSemSet;
			sem->semNbr = semset->idsAllocated;
			semset->idsAllocated++;
			sembase->idsAllocated++;
		}

		sm_SemGive(i);		/*	(First taker succeeds.)	*/

		/*	Acquire next semaphore set if necessary.	*/

		if (semset->idsAllocated == SEMMSL)
		{
			/*	Must acquire another semaphore set.	*/

			semSetIdx = sembase->currSemSet + 1;
			if (semSetIdx == MAX_SEM_SETS)
			{
				giveIpcLock();
				putErrmsg("Too many semaphore sets, can't \
manage the new one.", NULL);
				return SM_SEM_NONE;
			}

			semid = semget(sm_GetUniqueKey(), SEMMSL,
					IPC_CREAT | 0666);
			if (semid < 0)
			{
				giveIpcLock();
				putSysErrmsg("Can't get semaphore set", NULL);
				return SM_SEM_NONE;
			}

			sembase->currSemSet = semSetIdx;
			semset = sembase->semSets + semSetIdx;
			semset->semid = semid;
			semset->idsAllocated = 0;
		}

		giveIpcLock();
		return i;
	}

	giveIpcLock();
	putErrmsg("Can't add any more semaphores; table full.", NULL);
	return SM_SEM_NONE;
}


void	sm_SemDelete(sm_SemId i)
{
	SemaphoreBase	*sembase = _sembase(0);
	IciSemaphore	*sem;

	CHKVOID(sembase);
	CHKVOID(i >= 0);
	CHKVOID(i < sembase->idsAllocated);
	sem = sembase->semaphores + i;
	takeIpcLock();

	/*	Note: the semSetIdx and semNbr in the semaphore
	 *	don't need to be deleted; they constitute a
	 *	semaphore ID that will be reassigned later when
	 *	this semaphore object is allocated to a new use.	*/

	sem->inUse = 0;
	sem->key = SM_NO_KEY;
	giveIpcLock();
}

int	sm_SemTake(sm_SemId i)
{
	SemaphoreBase	*sembase = _sembase(0);
	IciSemaphore	*sem;
	IciSemaphoreSet	*semset;
	struct sembuf	sem_op[2] = { {0,0,0}, {0,1,0} };

	CHKERR(sembase);
	CHKERR(i >= 0);
	CHKERR(i < sembase->idsAllocated);
	sem = sembase->semaphores + i;
	if (sem->key == -1)	/*	semaphore deleted		*/
	{
		putErrmsg("Can't take deleted semaphore.", itoa(i));
		return -1;
	}

	semset = sembase->semSets + sem->semSetIdx;
	sem_op[0].sem_num = sem_op[1].sem_num = sem->semNbr;
	while (semop(semset->semid, sem_op, 2) < 0)
	{
		if (errno == EINTR)
		{
			/*Retry on Interruption by signal*/
			continue;
		} else {
			putSysErrmsg("Can't take semaphore", itoa(i));
			return -1;
		}
	}

	return 0;
}

void	sm_SemGive(sm_SemId i)
{
	SemaphoreBase	*sembase = _sembase(0);
	IciSemaphore	*sem;
	IciSemaphoreSet	*semset;
	struct sembuf	sem_op = { 0, -1, IPC_NOWAIT };

	CHKVOID(sembase);
	CHKVOID(i >= 0);
	CHKVOID(i < sembase->idsAllocated);
	sem = sembase->semaphores + i;
	if (sem->key == -1)	/*	semaphore deleted		*/
	{
		putErrmsg("Can't give deleted semaphore.", itoa(i));
		return;
	}

	semset = sembase->semSets + sem->semSetIdx;
	sem_op.sem_num = sem->semNbr;
	if (semop(semset->semid, &sem_op, 1) < 0)
	{
		if (errno != EAGAIN)
		{
			putSysErrmsg("Can't give semaphore", itoa(i));
		}
	}
}

void	sm_SemEnd(sm_SemId i)
{
	SemaphoreBase	*sembase = _sembase(0);
	IciSemaphore	*sem;

	CHKVOID(sembase);
	CHKVOID(i >= 0);
	CHKVOID(i < sembase->idsAllocated);
	sem = sembase->semaphores + i;
	sem->ended = 1;
	sm_SemGive(i);
}

int	sm_SemEnded(sm_SemId i)
{
	SemaphoreBase	*sembase = _sembase(0);
	IciSemaphore	*sem;
	int		ended;

	CHKZERO(sembase);
	CHKZERO(i >= 0);
	CHKZERO(i < sembase->idsAllocated);
	sem = sembase->semaphores + i;
	ended = sem->ended;
	if (ended)
	{
		sm_SemGive(i);	/*	Enable multiple tests.		*/
	}

	return ended;
}

void	sm_SemUnend(sm_SemId i)
{
	SemaphoreBase	*sembase = _sembase(0);
	IciSemaphore	*sem;

	CHKVOID(sembase);
	CHKVOID(i >= 0);
	CHKVOID(i < sembase->idsAllocated);
	sem = sembase->semaphores + i;
	sem->ended = 0;
}

static void	handleTimeout()
{
	return;
}

int	sm_SemUnwedge(sm_SemId i, int timeoutSeconds)
{
	SemaphoreBase	*sembase = _sembase(0);
	IciSemaphore	*sem;
	IciSemaphoreSet	*semset;
	struct sembuf	sem_op[3] = { {0,0,0}, {0,1,0}, {0,-1,IPC_NOWAIT} };

	CHKERR(sembase);
	CHKERR(i >= 0);
	CHKERR(i < sembase->idsAllocated);
	sem = sembase->semaphores + i;
	if (sem->key == -1)	/*	semaphore deleted		*/
	{
		putErrmsg("Can't unwedge deleted semaphore.", itoa(i));
		return -1;
	}

	semset = sembase->semSets + sem->semSetIdx;
	sem_op[0].sem_num = sem_op[1].sem_num = sem_op[2].sem_num = sem->semNbr;
	if (timeoutSeconds < 1) timeoutSeconds = 1;
	isignal(SIGALRM, (void(*)(int)) handleTimeout);
	oK(alarm(timeoutSeconds));
	if (semop(semset->semid, sem_op, 2) < 0)
	{
		if (errno != EINTR)
		{
			putSysErrmsg("Can't take semaphore", itoa(i));
			return -1;
		}
		/*Intentionally don't retry if EINTR... That means the
		 *alarm we just set went off... We're going to proceed anyway.*/
	}

	oK(alarm(0));
	isignal(SIGALRM, SIG_DFL);
	if (semop(semset->semid, sem_op + 2, 1) < 0)
	{
		if (errno != EAGAIN)
		{
			putSysErrmsg("Can't give semaphore", itoa(i));
			return -1;
		}
	}

	return 0;
}

#endif			/*	End of #ifdef SVR4_SEMAPHORES		*/

/************************* Symbol table services  *****************************/

#ifdef PRIVATE_SYMTAB

extern FUNCPTR	sm_FindFunction(char *name, int *priority, int *stackSize);

#if defined (FSWSYMTAB) || defined (GDSSYMTAB)
#include "mysymtab.c"
#else
#include "symtab.c"
#endif

#endif

/****************** Task control services *************************************/

#ifdef VXWORKS_TASKS

	/* ---- Task Control services (VxWorks) ----------------------- */

int	sm_TaskIdSelf()
{
	return (taskIdSelf());
}

int	sm_TaskExists(int task)
{
	if (taskIdVerify(task) == OK)
	{
		return 1;
	}

	return 0;
}

void	*sm_TaskVar(void **arg)
{
	static void	*value;

	if (arg != NULL)
	{
		/*	Set value by dereferencing argument.		*/

		value = *arg;
		taskVarAdd(0, (int *) &value);
	}

	return value;
}

void	sm_TaskSuspend()
{
	if (taskSuspend(sm_TaskIdSelf()) == ERROR)
	{
		putSysErrmsg("Can't suspend task (self)", NULL);
	}
}

void	sm_TaskDelay(int seconds)
{
	if (taskDelay(seconds * sysClkRateGet()) == ERROR)
	{
		putSysErrmsg("Can't pause task", itoa(seconds));
	}
}

void	sm_TaskYield()
{
	taskDelay(0);
}

int	sm_TaskSpawn(char *name, char *arg1, char *arg2, char *arg3,
		char *arg4, char *arg5, char *arg6, char *arg7, char *arg8,
		char *arg9, char *arg10, int priority, int stackSize)
{
	char	namebuf[33];
	FUNCPTR	entryPoint;
	int	result;
#ifdef PRIVATE_SYMTAB

	CHKERR(name);
	if ((entryPoint = sm_FindFunction(name, &priority, &stackSize)) == NULL)
	{
		isprintf(namebuf, sizeof namebuf, "_%s", name);
		if ((entryPoint = sm_FindFunction(namebuf, &priority,
				&stackSize)) == NULL)
		{
			putErrmsg("Can't spawn task; function not in private \
symbol table; must be added to mysymtab.c.", name);
			return -1;
		}
	}
#else
	SYM_TYPE	type;

	CHKERR(name);
	if (symFindByName(sysSymTbl, name, (char **) &entryPoint, &type)
			== ERROR)
	{
		isprintf(namebuf, sizeof namebuf, "_%s", name);
		if (symFindByName(sysSymTbl, namebuf, (char **) &entryPoint,
				&type) == ERROR)
		{
			putSysErrmsg("Can't spawn task; function not in \
VxWorks symbol table", name);
			return -1;
		}
	}

	if (priority <= 0)
	{
		priority = ICI_PRIORITY;
	}

	if (stackSize <= 0)
	{
		stackSize = 32768;
	}
#endif

#ifdef FSWSCHEDULER
#include "fswspawn.c"
#else
	result = taskSpawn(name, priority, VX_FP_TASK, stackSize, entryPoint,
			(int) arg1, (int) arg2, (int) arg3, (int) arg4,
			(int) arg5, (int) arg6, (int) arg7, (int) arg8,
			(int) arg9, (int) arg10);
#endif
	if (result == ERROR)
	{
		putSysErrmsg("Failed spawning task", name);
	}
	else
	{
		TRACK_BORN(result);
	}

	return result;
}

void	sm_TaskKill(int task, int sigNbr)
{
	oK(kill(task, sigNbr));
}

void	sm_TaskDelete(int task)
{
	if (taskIdVerify(task) != OK)
	{
		writeMemoNote("[?] Can't delete nonexistent task", itoa(task));
		return;
	}

	TRACK_DIED(task);
	oK(taskDelete(task));
}

void	sm_Abort()
{
	oK(tt(taskIdSelf()));
	snooze(2);
	TRACK_DIED(task);
	oK(taskDelete(taskIdSelf()));
}

#endif			/*	End of #ifdef VXWORKS_TASKS		*/

/*	Thread management machinery for bionic and uClibc, both of
 *	which lack pthread_cancel.					*/

#if defined (bionic) || defined (uClibc)

typedef struct
{
	void	*(*function)(void *);
	void	*arg;
} IonPthreadParm;

static void	posixTaskExit(int sig)
{
	pthread_exit(0);
}

static void	sm_ArmPthread()
{
	struct sigaction	actions;

	memset((char *) &actions, 0, sizeof actions);
	sigemptyset(&actions.sa_mask);
	actions.sa_flags = 0;
	actions.sa_handler = posixTaskExit;
	oK(sigaction(SIGUSR2, &actions, NULL));
}

void	sm_EndPthread(pthread_t threadId)
{
	/*	NOTE that this is NOT a faithful implementation of
	 *	pthread_cancel(); there is no support for deferred
	 *	thread cancellation in Bionic (the Android subset
	 *	of Linux).  It's just a code simplification, solely
	 *	for the express, limited purpose of shutting down a
	 *	task immediately, under the highly constrained
	 *	circumstances defined by sm_TaskSpawn, sm_TaskDelete,
	 *	and sm_Abort, below.					*/

	oK(pthread_kill(threadId, SIGUSR2));
}

static void	*posixTaskEntrance(void *arg)
{
	IonPthreadParm	*parm = (IonPthreadParm *) arg;

	sm_ArmPthread();
	return (parm->function)(parm->arg);
}

int	sm_BeginPthread(pthread_t *threadId, const pthread_attr_t *attr,
		void *(*function)(void *), void *arg)
{
	IonPthreadParm	parm;

	parm.function = function;
	parm.arg = arg;
	return pthread_create(threadId, attr, posixTaskEntrance, &parm);
}

#endif	/*	End of #if defined bionic || uClibc			*/

#ifdef POSIX_TASKS

/*	Note: the RTEMS API is UNIX-like except that it omits all SVR4
 *	features.  RTEMS uses POSIX semaphores, and its shared-memory
 *	mechanism is the same as the one we use for VxWorks.  The same
 *	is true of Bionic.  CFS may be either UNIX or VXWORKS, but its
 *	task model is always threads just like RTEMS and bionic.	*/

#include <sys/stat.h>
#include <sched.h>

	/* ---- Task Control services (POSIX) ------------------------- */

#ifndef	MAX_POSIX_TASKS
#define MAX_POSIX_TASKS	50
#endif

typedef struct
{
	int		inUse;		/*	Boolean.		*/
	pthread_t	threadId;
	void		*value;		/*	Task variable value.	*/
} PosixTask;

static void	*_posixTasks(int *taskId, pthread_t *threadId, void **arg)
{
	static PosixTask	tasks[MAX_POSIX_TASKS];
	static int		initialized = 0;/*	Boolean.	*/
	static ResourceLock	tasksLock;
	pthread_t		ownThreadId;
	int			i;
	int			vacancy;
	PosixTask		*task;
	void			*value;

	/*	NOTE: the taskId for a PosixTask is 1 more than
	 *	the index value for that task in the tasks table.
	 *	That is, taskIds range from 1 through MAX_POSIX_TASKS
	 *	and -1 is an invalid task ID signifying "none".		*/

	if (!initialized)
	{
		memset((char *) tasks, 0, sizeof tasks);
		if (initResourceLock(&tasksLock) < 0)
		{
			putErrmsg("Can't initialize POSIX tasks table.", NULL);
			return NULL;
		}

		initialized = 1;
	}

	lockResource(&tasksLock);

	/*	taskId must never be NULL; it is always needed.		*/

	CHKNULL(taskId);

	/*	When *taskId is 0, processing depends on the value
	 *	of threadID.  If threadId is NULL, then the task ID
	 *	of the calling thread (0 if the thread doesn't have
	 *	an assigned task ID) is written into *taskId.
	 *	Otherwise, the thread identified by *threadId is
	 *	added as a new task and the ID of that task (-1
	 *	if the thread could not be assigned a task ID) is
	 *	written into *taskId.  In either case, NULL is
	 *	returned.
	 *
	 *	Otherwise, *taskId must be in the range 1 through
	 *	MAX_POSIX_TASKS inclusive and processing again
	 *	depends on the value of threadId.  If threadId is
	 *	NULL then the indicated task ID is unassigned and
	 *	is available for reassignment to another thread;
	 *	-1 is written into *taskId and NULL is returned.
	 *	Otherwise:
	 *
	 *		The thread ID for the indicated task is
	 *		written into *threadId.
	 *
	 *		If arg is non-NULL, then the task variable
	 *		value for the indicated task is set to *arg.
	 *
	 *		The current value of the indicated task's
	 *		task variable is returned.			*/

	if (*taskId == 0)
	{
		if (threadId == NULL)	/*	Look up own task ID.	*/
		{
			ownThreadId = pthread_self();
			for (i = 0, task = tasks; i < MAX_POSIX_TASKS;
					i++, task++)
			{
				if (task->inUse == 0)
				{
					continue;
				}

				if (pthread_equal(task->threadId, ownThreadId))
				{
					*taskId = i + 1;
					unlockResource(&tasksLock);
					return NULL;
				}
			}

			/*	No task ID for this thread; sub-thread
			 *	of a task.				*/

			unlockResource(&tasksLock);
			return NULL;
		}

		/*	Assigning a task ID to this thread.		*/

		vacancy = -1;
		for (i = 0, task = tasks; i < MAX_POSIX_TASKS; i++, task++)
		{
			if (task->inUse == 0)
			{
				if (vacancy == -1)
				{
					vacancy = i;
				}
			}
			else
			{
				if (pthread_equal(task->threadId, *threadId))
				{
					/*	Already assigned.	*/

					*taskId = i + 1;
					unlockResource(&tasksLock);
					return NULL;
				}
			}
		}

		if (vacancy == -1)
		{
			putErrmsg("Can't start another task.", NULL);
			*taskId = -1;
			unlockResource(&tasksLock);
			return NULL;
		}

		task = tasks + vacancy;
		task->inUse = 1;
		task->threadId = *threadId;
		task->value = NULL;
		*taskId = vacancy + 1;
		unlockResource(&tasksLock);
		return NULL;
	}

	/*	Operating on a previously assigned task ID.		*/

	CHKNULL((*taskId) > 0 && (*taskId) <= MAX_POSIX_TASKS);
	task = tasks + ((*taskId) - 1);
	if (threadId == NULL)	/*	Unassigning this task ID.	*/
	{
		if (task->inUse)
		{
			task->inUse = 0;
		}

		*taskId = -1;
		unlockResource(&tasksLock);
		return NULL;
	}

	/*	Just looking up the thread ID for this task ID and/or
	 *	operating on task variable.				*/

	if (task->inUse == 0)	/*	Invalid task ID.		*/
	{
		*taskId = -1;
		unlockResource(&tasksLock);
		return NULL;
	}

	*threadId = task->threadId;
	if (arg)
	{
		task->value = *arg;
	}

	value = task->value;
	unlockResource(&tasksLock);
	return value;
}

int	sm_TaskIdSelf()
{
	int		taskId = 0;
	pthread_t	threadId;

	oK(_posixTasks(&taskId, NULL, NULL));
	if (taskId > 0)
	{
		return taskId;
	}

	/*	May be a newly spawned task.  Give sm_TaskSpawn
	 *	an opportunity to register the thread as a task.	*/

	sm_TaskYield();
	oK(_posixTasks(&taskId, NULL, NULL));
	if (taskId > 0)
	{
		return taskId;
	}

	/*	This is a subordinate thread of some other task.
	 *	It needs to register itself as a task.			*/

	threadId = pthread_self();
	oK(_posixTasks(&taskId, &threadId, NULL));
	return taskId;
}

int	sm_TaskExists(int taskId)
{
	pthread_t	threadId;

	oK(_posixTasks(&taskId, &threadId, NULL));
	if (taskId < 0)
	{
		return 0;		/*	No such task.		*/
	}

	/*	(Signal 0 in pthread_kill is rejected by RTEMS 4.9.)	*/

	if (pthread_kill(threadId, SIGCONT) == 0)
	{
		return 1;		/*	Thread is running.	*/
	}

	/*	Note: RTEMS 4.9 implementation of pthread_kill does
	 *	not return a valid errno on failure; can't print
	 *	system error message.					*/

	return 0;	/*	No such thread, or some other failure.	*/
}

void	*sm_TaskVar(void **arg)
{
	int		taskId = sm_TaskIdSelf();
	pthread_t	threadId;

	return _posixTasks(&taskId, &threadId, arg);
}

void	sm_TaskSuspend()
{
	pause();
}

void	sm_TaskDelay(int seconds)
{
	sleep(seconds);
}

void	sm_TaskYield()
{
	sched_yield();
}

#ifndef MAX_SPAWNS
#define	MAX_SPAWNS	8
#endif

typedef struct
{
	FUNCPTR	threadMainFunction;
	int	arg1;
	int	arg2;
	int	arg3;
	int	arg4;
	int	arg5;
	int	arg6;
	int	arg7;
	int	arg8;
	int	arg9;
	int	arg10;
} SpawnParms;

static void	*posixDriverThread(void *parm)
{
	SpawnParms	parms;

	/*	Make local copy of spawn parameters.			*/

	memcpy((char *) &parms, parm, sizeof(SpawnParms));

	/*	Clear spawn parameters for use by next sm_TaskSpawn().	*/

	memset((char *) parm, 0, sizeof(SpawnParms));

#if defined (bionic)
	/*	Set up SIGUSR2 handler to enable clean task shutdown.	*/

	sm_ArmPthread();
#endif
	/*	Run main function of thread.				*/

	parms.threadMainFunction(parms.arg1, parms.arg2, parms.arg3,
			parms.arg4, parms.arg5, parms.arg6,
			parms.arg7, parms.arg8, parms.arg9, parms.arg10);
	return NULL;
}

int	sm_TaskSpawn(char *name, char *arg1, char *arg2, char *arg3,
		char *arg4, char *arg5, char *arg6, char *arg7, char *arg8,
		char *arg9, char *arg10, int priority, int stackSize)
{
	char			namebuf[33];
	FUNCPTR			entryPoint;
	static SpawnParms	spawnsArray[MAX_SPAWNS];
	int			i;
	SpawnParms		*parms;
	pthread_attr_t		attr;
	pthread_t		threadId;
	int			taskId;

#ifdef PRIVATE_SYMTAB
	CHKERR(name);
	if ((entryPoint = sm_FindFunction(name, &priority, &stackSize)) == NULL)
	{
		isprintf(namebuf, sizeof namebuf, "_%s", name);
		if ((entryPoint = sm_FindFunction(namebuf, &priority,
				&stackSize)) == NULL)
		{
			putErrmsg("Can't spawn task; function not in \
private symbol table; must be added to mysymtab.c.", name);
			return -1;
		}
	}
#else
	putErrmsg("Can't spawn task; no ION private symbol table.", name);
	return -1;
#endif
	for (i = 0, parms = spawnsArray; i < MAX_SPAWNS; i++, parms++)
	{
		if (parms->threadMainFunction == NULL)
		{
			break;
		}
	}

	if (i == MAX_SPAWNS)
	{
		putErrmsg("Can't spawn task: no parms cleared yet.", NULL);
		return -1;
	}

	parms->threadMainFunction = entryPoint;
	parms->arg1 = (int) arg1;
	parms->arg2 = (int) arg2;
	parms->arg3 = (int) arg3;
	parms->arg4 = (int) arg4;
	parms->arg5 = (int) arg5;
	parms->arg6 = (int) arg6;
	parms->arg7 = (int) arg7;
	parms->arg8 = (int) arg8;
	parms->arg9 = (int) arg9;
	parms->arg10 = (int) arg10;
	sm_ConfigurePthread(&attr, stackSize);
	errno = pthread_create(&threadId, &attr, posixDriverThread,
			(void *) parms);
	if (errno)
	{
		putSysErrmsg("Failed spawning task", name);
		return -1;
	}

	taskId = 0;	/*	Requesting new task ID for thread.	*/
	oK(_posixTasks(&taskId, &threadId, NULL));
	if (taskId < 0)		/*	Too many tasks running.		*/
	{
		if (pthread_kill(threadId, SIGTERM) == 0)
		{
			oK(pthread_end(threadId));
			pthread_join(threadId, NULL);
		}

		return -1;
	}

	TRACK_BORN(taskId);
	return taskId;
}

void	sm_TaskForget(int taskId)
{
	oK(_posixTasks(&taskId, NULL, NULL));
}

void	sm_TaskKill(int taskId, int sigNbr)
{
	pthread_t	threadId;

	oK(_posixTasks(&taskId, &threadId, NULL));
	if (taskId < 0)
	{
		return;		/*	No such task.			*/
	}

	oK(pthread_kill(threadId, sigNbr));
}

void	sm_TaskDelete(int taskId)
{
	pthread_t	threadId;

	oK(_posixTasks(&taskId, &threadId, NULL));
	if (taskId < 0)
	{
		return;		/*	No such task.			*/
	}

	TRACK_DIED(taskId);
	if (pthread_kill(threadId, SIGTERM) == 0)
	{
		oK(pthread_end(threadId));
	}

	oK(_posixTasks(&taskId, NULL, NULL));
}

void	sm_Abort()
{
	int		taskId = sm_TaskIdSelf();
	pthread_t	threadId;

	if (taskId < 0)		/*	Can't register as task.		*/
	{
		/*	Just terminate.					*/

		threadId = pthread_self();
		if (pthread_kill(threadId, SIGTERM) == 0)
		{
			oK(pthread_end(threadId));
		}

		return;
	}

	sm_TaskDelete(taskId);
}

#endif	/*	End of #ifdef POSIX_TASKS				*/

#ifdef MINGW_TASKS

	/* ---- Task Control services (mingw) ----------------------- */

int	sm_TaskIdSelf()
{
	return _getpid();
}

int	sm_TaskExists(int task)
{
	DWORD	processId = task;
	HANDLE	process;
	DWORD	status;
	BOOL	result;

	process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
	if (process == NULL)
	{
		return 0;
	}

	result = GetExitCodeProcess(process, &status);
	CloseHandle(process);
       	if (result == 0 || status != STILL_ACTIVE)
	{
		return 0;		/*	No such process.	*/
	}

	return 1;
}

void	*sm_TaskVar(void **arg)
{
	static void	*value;

	/*	Each Windows process has its own distinct instance
	 *	of each global variable, so all global variables
	 *	are automatically "task variables".			*/

	if (arg != NULL)
	{
		/*	Set value by dereferencing argument.		*/

		value = *arg;
	}

	return value;
}

void	sm_TaskSuspend()
{
	writeMemo("[?] ION for Windows doesn't support sm_TaskSuspend().");
}

void	sm_TaskDelay(int seconds)
{
	Sleep(seconds * 1000);
}

void	sm_TaskYield()
{
	Sleep(0);
}

int	sm_TaskSpawn(char *name, char *arg1, char *arg2, char *arg3,
		char *arg4, char *arg5, char *arg6, char *arg7, char *arg8,
		char *arg9, char *arg10, int priority, int stackSize)
{
	STARTUPINFO		si;
	PROCESS_INFORMATION	pi;
	char			cmdLine[256];

	CHKERR(name);
	ZeroMemory(&si, sizeof si);
	si.cb = sizeof si;
	ZeroMemory(&pi, sizeof pi);
	if (arg1 == NULL) arg1 = "";
	if (arg2 == NULL) arg2 = "";
	if (arg3 == NULL) arg3 = "";
	if (arg4 == NULL) arg4 = "";
	if (arg5 == NULL) arg5 = "";
	if (arg6 == NULL) arg6 = "";
	if (arg7 == NULL) arg7 = "";
	if (arg8 == NULL) arg8 = "";
	if (arg9 == NULL) arg9 = "";
	if (arg10 == NULL) arg10 = "";
	isprintf(cmdLine, sizeof cmdLine,
			"\"%s\" %s %s %s %s %s %s %s %s %s %s",
			name, arg1, arg2, arg3, arg4, arg5,
			arg6, arg7, arg8, arg9, arg10);
	if (CreateProcess(NULL, cmdLine, NULL, NULL, FALSE, 0, NULL, NULL,
			&si, &pi) == 0)
	{
		putSysErrmsg("Can't create process", cmdLine);
		return -1;
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return pi.dwProcessId;
}

void	sm_TaskKill(int task, int sigNbr)
{
	char	eventName[32];
	HANDLE	event;
	BOOL	result;

	if (task <= 1)
	{
		writeMemoNote("[?] Can't delete invalid process ID",
				itoa(task));
		return;
	}

	if (sigNbr != SIGTERM)
	{
		writeMemoNote("[?] ION for Windows only delivers SIGTERM",
				itoa(sigNbr));
		return;
	}

	sprintf(eventName, "%d.sigterm", task);
	event = OpenEvent(EVENT_ALL_ACCESS, FALSE, eventName);
	if (event)
	{
		result = SetEvent(event);
		CloseHandle(event);
		if (result == 0)
		{
			putErrmsg("Can't set SIGTERM event.",
					utoa(GetLastError()));
		}
	}
	else
	{
		putErrmsg("Can't open SIGTERM event.", utoa(GetLastError()));
	}
}

void	sm_TaskDelete(int task)
{
	DWORD	processId = task;
	HANDLE	process;
	BOOL	result;

	sm_TaskKill(task, SIGTERM);
	Sleep(1000);
	process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
	if (process)
	{
		result = TerminateProcess(process, 0);
		CloseHandle(process);
		if (result == 0)
		{
			putErrmsg("Can't terminate process.",
					utoa(GetLastError()));
		}
	}
	else
	{
		putErrmsg("Can't open process.", utoa(GetLastError()));
	}
}

void	sm_Abort()
{
	abort();
}

void	sm_WaitForWakeup(int seconds)
{
	DWORD	millisec;
	char	eventName[32];
	HANDLE	event;

	if (seconds < 0)
	{
		millisec = INFINITE;
	}
	else
	{
		millisec = seconds * 1000;
	}

	sprintf(eventName, "%u.wakeup", (unsigned int) GetCurrentProcessId());
	event = CreateEvent(NULL, FALSE, FALSE, eventName);
	if (event)
	{
		oK(WaitForSingleObject(event, millisec));
		CloseHandle(event);
	}
	else
	{
		putErrmsg("Can't open wakeup event.", utoa(GetLastError()));
	}
}

void	sm_Wakeup(DWORD processId)
{
	char	eventName[32];
	HANDLE	event;
	int	result;

	sprintf(eventName, "%u.wakeup", (unsigned int) processId);
	event = OpenEvent(EVENT_ALL_ACCESS, FALSE, eventName);
	if (event)
	{
		result = SetEvent(event);
		CloseHandle(event);
		if (result == 0)
		{
			putErrmsg("Can't set wakeup event.",
					utoa(GetLastError()));
		}
	}
	else
	{
		putErrmsg("Can't open wakeup event.", utoa(GetLastError()));
	}
}

#endif			/*	End of #ifdef MINGW_TASKS		*/

#ifdef UNIX_TASKS

	/* ---- IPC services access control (Unix) -------------------- */

#include <sys/stat.h>
#include <sys/ipc.h>
#include <sched.h>

	/* ---- Task Control services (Unix) -------------------------- */

int	sm_TaskIdSelf()
{
	return getpid();
}

int	sm_TaskExists(int task)
{
	waitpid(task, NULL, WNOHANG);	/*	In case it's a zombie.	*/
	if (kill(task, 0) < 0)
	{
		return 0;		/*	No such process.	*/
	}

	return 1;
}

void	*sm_TaskVar(void **arg)
{
	static void	*value;

	/*	Each UNIX process has its own distinct instance
	 *	of each global variable, so all global variables
	 *	are automatically "task variables".			*/

	if (arg != NULL)
	{
		/*	Set value by dereferencing argument.		*/

		value = *arg;
	}

	return value;
}

void	sm_TaskSuspend()
{
	pause();
}

void	sm_TaskDelay(int seconds)
{
	sleep(seconds);
}

void	sm_TaskYield()
{
	sched_yield();
}

static void	closeAllFileDescriptors()
{
	struct rlimit	limit;
	int		i;

	oK(getrlimit(RLIMIT_NOFILE, &limit));
	for (i = 3; i < limit.rlim_cur; i++)
	{
		oK(close(i));
	}

	writeMemo("");	/*	Tell logger that log file is closed.	*/
}

int	sm_TaskSpawn(char *name, char *arg1, char *arg2, char *arg3,
		char *arg4, char *arg5, char *arg6, char *arg7, char *arg8,
		char *arg9, char *arg10, int priority, int stackSize)
{
	int	pid;

	CHKERR(name);

	/*	Ignoring SIGCHLD signals causes the parent process
	 *	to ignore the fate of the child process, so the child
	 *	process cannot become a zombie: when it terminates,
	 *	it is removed immediately rather than waiting for
	 *	the parent to wait() on it.				*/

	isignal(SIGCHLD, SIG_IGN);
	switch (pid = fork())
	{
	case -1:		/*	Error.				*/
		putSysErrmsg("Can't fork new process", name);
		return -1;

	case 0:			/*	This is the child process.	*/
		closeAllFileDescriptors();
		execlp(name, name, arg1, arg2, arg3, arg4, arg5, arg6,
				arg7, arg8, arg9, arg10, NULL);

		/*	Can only get to this code if execlp fails.	*/

		putSysErrmsg("Can't execute new process, exiting...", name);
		exit(1);

	default:		/*	This is the parent process.	*/
		TRACK_BORN(pid);
		return pid;
	}
}

void	sm_TaskKill(int task, int sigNbr)
{
	oK(kill(task, sigNbr));
}

void	sm_TaskDelete(int task)
{
	if (task <= 1)
	{
		writeMemoNote("[?] Can't delete invalid process ID",
				itoa(task));
		return;
	}

	TRACK_DIED(task);
	oK(kill(task, SIGTERM));
	oK(waitpid(task, NULL, 0));
}

void	sm_Abort()
{
	TRACK_DIED(getpid());
	abort();
}

#endif	/*	End of #ifdef UNIX_TASKS				*/

/************************ Unique IPC key services *****************************/

#ifdef RTOS_SHM

	/* ----- Unique IPC key system for "task" architecture --------- */

int	sm_GetUniqueKey()
{
	static unsigned long	ipcUniqueKey = 0x80000000;
	int			result;

	takeIpcLock();
	ipcUniqueKey++;
	result = ipcUniqueKey;		/*	Truncates as necessary.	*/
	giveIpcLock();
	return result;
}

sm_SemId	sm_GetTaskSemaphore(int taskId)
{
	return sm_SemCreate(taskId, SM_SEM_FIFO);
}

#else

	/* ---- Unique IPC key system for "process" architecture ------ */

int	sm_GetUniqueKey()
{
	static int	ipcUniqueKey = 0;
	int		result;

	/*	Compose unique key: low-order 16 bits of process ID
		followed by low-order 16 bits of process-specific
		sequence count.						*/

	ipcUniqueKey = (ipcUniqueKey + 1) & 0x0000ffff;
#ifdef mingw
	result = (_getpid() << 16) + ipcUniqueKey;
#else
	result = (getpid() << 16) + ipcUniqueKey;
#endif
	return result;
}

sm_SemId	sm_GetTaskSemaphore(int taskId)
{
	return sm_SemCreate((taskId << 16), SM_SEM_FIFO);
}

#endif	/*	End of #ifdef RTOS_SHM					*/

/******************* platform-independent functions ***************************/

void	sm_ConfigurePthread(pthread_attr_t *attr, size_t stackSize)
{
	struct sched_param	parms;

	CHKVOID(attr);
	oK(pthread_attr_init(attr));
	oK(pthread_attr_setschedpolicy(attr, SCHED_FIFO));
	parms.sched_priority = sched_get_priority_min(SCHED_FIFO);
	oK(pthread_attr_setschedparam(attr, &parms));
	oK(pthread_attr_setdetachstate(attr, PTHREAD_CREATE_JOINABLE));
	if (stackSize > 0)
	{
		oK(pthread_attr_setstacksize(attr, stackSize));
	}
}

int	pseudoshell(char *commandLine)
{
	int	length;
	char	buffer[256];
	char	*cursor;
	int	i;
	char	*argv[11];
	int	argc = 0;
	int	pid;

	if (commandLine == NULL)
	{
		return ERROR;
	}

	length = strlen(commandLine);
	if (length > 255)		/*	Too long to parse.	*/
	{
		putErrmsg("Command length exceeds 255 bytes.", itoa(length));
		return -1;
	}

	istrcpy(buffer, commandLine, sizeof buffer);
	for (cursor = buffer, i = 0; i < 11; i++)
	{
		if (*cursor == '\0')
		{
			argv[i] = NULL;
		}
		else
		{
			findToken(&cursor, &(argv[i]));
			if (argv[i] != NULL)
			{
				argc++;
			}
		}
	}

	/*	Skip over any trailing whitespace.			*/

	while (isspace((int) *cursor))
	{
		cursor++;
	}

	if (*cursor != '\0')		/*	Too many args.	*/
	{
		putErrmsg("More than 11 args in command.", commandLine);
		return -1;
	}
#ifdef ION_LWT
	takeIpcLock();
	if (copyArgs(argc, argv) < 0)
	{
		giveIpcLock();
		putErrmsg("Can't copy args of command.", commandLine);
		return -1;
	}
#endif
	pid = sm_TaskSpawn(argv[0], argv[1], argv[2], argv[3],
			argv[4], argv[5], argv[6], argv[7], argv[8],
			argv[9], argv[10], 0, 0);
#ifdef ION_LWT
	if (pid == -1)
	{
		tagArgBuffers(0);
	}
	else
	{
		tagArgBuffers(pid);
	}

	giveIpcLock();
#endif
	return pid;
}
