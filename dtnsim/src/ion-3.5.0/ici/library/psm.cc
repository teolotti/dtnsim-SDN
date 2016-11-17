/*
  	psm.c:	personal space management library implementation.
  
  	Originally designed for managing a spacecraft solid-state recorder
  	that takes the form of a fixed-size pool of static RAM with a flat
  	address space.
									*/
/*	Copyright (c) 1997, California Institute of Technology.		*/
/*	ALL RIGHTS RESERVED.  U.S. Government Sponsorship		*/
/*	acknowledged.							*/
/*									*/
/*	Author: Scott Burleigh, Jet Propulsion Laboratory		*/
/*									*/

#ifndef NO_PSM_TRACE
#define	PSM_TRACE
#endif

#include "psm.h"
#include "smlist.h"

#define	SMALL_BLOCK_OHD	(WORD_SIZE)
#define	SMALL_BLK_LIMIT	(SMALL_SIZES * WORD_SIZE)

#if SPACE_ORDER ==3	/* 64-bit machine	*/
#define	SMALL_IN_USE	((PsmAddress) 0xffffffffffffff00)
#define	BLK_IN_USE	((PsmAddress) 0xffffffffffffffff)
#else
#define	SMALL_IN_USE	((PsmAddress) 0xffffff00)
#define	BLK_IN_USE	((PsmAddress) 0xffffffff)
#endif

/*
 * The overhead on a small block is WORD_SIZE bytes.  When the block is
 * free, these bytes contain a pointer to the next free block, which must be
 * an integral multiple of WORD_SIZE.  When the block is in use, the low-order
 * byte is set to the size of the block's user data (expressed as an integer
 * 1 through SMALL_SIZES: the total block size minus overhead, divided by
 * WORD_SIZE) and the three high-order bytes are all set to 0xff.
 *
 * NOTE: since any address in excess of 0xffffff00 will be interpreted as
 * indicating that the block is in use, the maximum size of the small pool
 * is 0xffffff00; the address of any free block at a higher address would
 * be unrecognizable as such.
 */
struct small_ohd
{
	PsmAddress	next;
};

/*
 * The overhead on a large block is in two parts that MUST BE THE SAME SIZE,
 * a leading overhead area and a trailing overhead area.  The first word of
 * the leading overhead contains the size of the block's user data.
 * The first word of the trailing overhead contains a pointer to
 * the start of the leading overhead.  When the block is in use, the second
 * word of the leading overhead and the second word of the trailing overhead
 * both contain BLK_IN_USE; when the block is free, the second word of the
 * leading overhead points to the next free block and the second word of
 * the trailing overhead points to the preceding free block.
 *
 * To ensure correct alignment of the trailing overhead area, the user
 * data area in a large block must be an integral multiple of the leading
 * and trailing overhead areas' size.
 */

struct big_ohd1				/*	Leading overhead.	*/
{
	u_int		userDataSize;	/*	in bytes, not words	*/
	PsmAddress	next;
};

struct big_ohd2				/*	Trailing overhead.	*/
{
	PsmAddress	start;
	PsmAddress	prev;
};

#define SMALL(x)	((struct small_ohd *)(((char *) map) + x))
#define BIG1(x)		((struct big_ohd1 *)(((char *) map) + x))
#define BIG2(x)		((struct big_ohd2 *)(((char *) map) + x))
#define PTR(x)		(((char *) map) + x)

#define	LG_OHD_SIZE	(1 << LARGE_ORDER1)   /* double word	*/
#define	LARGE_BLOCK_OHD	(2 * LG_OHD_SIZE)
#define	MIN_LARGE_BLOCK	(3 * LG_OHD_SIZE)
#define	LARGE_BLK_LIMIT	(LARGE1 << LARGE_ORDERn)

#define	INITIALIZED	(0x99999999)
#define	MANAGED		(0xbbbbbbbb)

typedef struct			/*	Global view in shared memory.	*/
{
	PsmAddress	directory;
	u_int		status;
	sm_SemId	semaphore;
	int		ownerTask;	/*	Last took the semaphore.*/
	pthread_t	ownerThread;	/*	Last took the semaphore.*/
	int		depth;		/*	Count of ungiven takes.	*/
	int		desperate;
	u_long		partitionSize;
	char		name[32];
	int		traceKey;	/*	For sptrace.		*/
	long		traceSize;	/*	0 = trace disabled.	*/
	PsmAddress	startOfSmallPool;
	PsmAddress	endOfSmallPool;
	PsmAddress	firstSmallFree[SMALL_SIZES];
	PsmAddress	startOfLargePool;
	PsmAddress	endOfLargePool;
	PsmAddress	firstLargeFree[LARGE_ORDERS];
	u_long		unassignedSpace;
} PartitionMap;

typedef struct
{
	char		name[33];
	PsmAddress	address;
} PsmCatlgEntry;

static char	*_outOfSpaceMsg()
{

}

static char	*_badBlockSizeMsg()
{

}

static char	*_noTraceMsg()
{

}


/*	*	Non-platform-specific implementation	*	*	*/

static void	lockPartition(PartitionMap *map)
{

}

static void	unlockPartition(PartitionMap *map)
{

}

static void	discard(PsmPartition partition)
{

}

int	psm_manage(char *start, u_long length, char *name, PsmPartition *psmp,
		PsmMgtOutcome *outcome)
{

}

char	*psm_name(PsmPartition partition)
{

}

char	*psm_space(PsmPartition partition)
{

}

void	psm_unmanage(PsmPartition partition)
{

}

void	psm_erase(PsmPartition partition)
{

}

void    *psp(PsmPartition partition, PsmAddress address)
{

}

PsmAddress	psa(PsmPartition partition, void *pointer)
{

}

void	psm_panic(PsmPartition partition)
{

}

void	psm_relax(PsmPartition partition)
{

}

int	psm_set_root(PsmPartition partition, PsmAddress root)
{

}

void	psm_erase_root(PsmPartition partition)
{

}

PsmAddress	psm_get_root(PsmPartition partition)
{

}

int	Psm_add_catlg(const char *file, int line, PsmPartition partition)
{

}

int	Psm_catlg(const char *file, int line, PsmPartition partition,
		char *name, PsmAddress address)
{

}

int	Psm_uncatlg(const char *file, int line, PsmPartition partition,
		char *name)
{

}

int	psm_locate(PsmPartition partition, char *name, PsmAddress *address,
		PsmAddress *entryElt)
{

}

static void	removeFromBucket(PartitionMap *map, int bucket,
			struct big_ohd1 *blk, struct big_ohd2 *trailer)
{

}

static int	computeBucket(u_int userDataSize)
{

}

static void	insertFreeBlock(PartitionMap *map, struct big_ohd1 *blk,
			struct big_ohd2 *trailer, PsmAddress block)
{

}

static void	freeLarge(PartitionMap *map, PsmAddress block)
{

}

static int	traceInProgress(PsmPartition partition)
{

}

static void	traceAlloc(const char *file, int line, PsmPartition partition,
			PsmAddress address, int size)
{

}

static void	traceFree(const char *file, int line, PsmPartition partition,
		PsmAddress address)
{

}

static void	traceMemo(const char *file, int line, PsmPartition partition,
			PsmAddress address, char *msg)
{

}

void	Psm_free(const char *file, int line, PsmPartition partition,
		PsmAddress address)
{

}

static PsmAddress	mallocLarge(PartitionMap *map, register u_int nbytes)
{

}

PsmAddress	Psm_malloc(const char *file, int line, PsmPartition partition,
			register u_long nbytes)
{

}

PsmAddress	Psm_zalloc(const char *file, int line, PsmPartition partition,
			register u_long nbytes)
{

}

void	psm_usage(PsmPartition partition, PsmUsageSummary *usage)
{

}

void	psm_report(PsmUsageSummary *usage)
{

}

int	psm_start_trace(PsmPartition partition, long shmSize, char *shm)
{

}

void	psm_print_trace(PsmPartition partition, int verbose)
{

}

void	psm_clear_trace(PsmPartition partition)
{

}

void	psm_stop_trace(PsmPartition partition)
{

}
