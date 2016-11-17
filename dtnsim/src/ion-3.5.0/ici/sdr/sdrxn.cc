/*
 *	sdrxn.c:	simple data recorder transaction system library.
 *
 *	Copyright (c) 2001-2007, California Institute of Technology.
 *	ALL RIGHTS RESERVED.  U.S. Government Sponsorship
 *	acknowledged.
 *
 *	Author: Scott Burleigh, JPL
 *
 *	This library implements the transaction mechanism underlying the
 *	Simple Data Recorder system.
 *
 *	Modification History:
 *	Date	  Who	What
 *	4-3-96	  APS	Abstracted IPC services and task control.
 *	5-1-96	  APS	Ported to sparc-sunos4.
 *	12-20-00  SCB	Revised for sparc-sunos5.
 *	6-8-07    SCB	Divided sdr.c library into separable components.
 */

#include "sdrP.h"
#include "lyst.h"
#include "sdrxn.h"


int	sdr_begin_xn(Sdr sdrv)
{

}

void	sdr_exit_xn(Sdr sdrv)
{

}

void	sdr_read(Sdr sdrv, char *into, Address from, long length)
{

}

void	Sdr_write(const char *file, int line, Sdr sdrv, Address into,
		char *from, long length)
{

}
