/*
 *	sdrmgt.c:	simple data recorder database space
 *			management library.
 *
 *	Copyright (c) 2001-2007, California Institute of Technology.
 *	ALL RIGHTS RESERVED.  U.S. Government Sponsorship
 *	acknowledged.
 *
 *	Author: Scott Burleigh, JPL
 *
 *	This library implements management of some portion of a
 *	spacecraft data recording resource that is to be configured
 *	and accessed as a fixed-size pool of notionally non-volatile
 *	memory with a flat address space.
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
#include "sdrmgt.h"

typedef struct
{
	Address		from;	/*	1st byte of object		*/
	Address		to;	/*	1st byte beyond scope of object	*/
} ObjectExtent;

/*		--	SDR space management stuff.	--		*/

#if SPACE_ORDER == 3
#define	SMALL_IN_USE	(0xffffffffffffff00)
#define	SMALL_NEXT_ADDR (0x00ffffffffffffff)
#define	LARGE_IN_USE	(0xffffffffffffffff)
#else
#define	SMALL_IN_USE	(0xffffff00)
#define	SMALL_NEXT_ADDR (0x00ffffff)
#define	LARGE_IN_USE	(0xffffffff)
#endif

#define	SMALL_BLOCK_OHD	(WORD_SIZE)
#define	SMALL_BLK_LIMIT	(SMALL_SIZES * WORD_SIZE)
#define SMALL_MAX_ADDR	(LARGE1 << (8 * (WORD_SIZE - 1)))

void	sdr_stage(Sdr sdrv, char *into, Object from, long length)
{

}
