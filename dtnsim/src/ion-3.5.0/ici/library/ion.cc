/*
 *	ion.c:	functions common to multiple protocols in the ION stack.
 *
 *	Copyright (c) 2007, California Institute of Technology.
 *	ALL RIGHTS RESERVED.  U.S. Government Sponsorship acknowledged.
 *
 *	Author: Scott Burleigh, JPL
 *
 */

#include "ion.h"
#include "smlist.h"
#include "rfx.h"
#include "time.h"

/*	*	*	Memory access	 *	*	*	*	*/


void	*allocFromIonMemory(const char *fileName, int lineNbr, size_t length)
{

}

void	releaseToIonMemory(const char *fileName, int lineNbr, void *block)
{

}

/*	*	*	Initialization	* 	*	*	*	*/

Sdr	getIonsdr()
{

}

PsmPartition	getIonwm()
{

}

int	getIonMemoryMgr()
{

}

IonVdb	*getIonVdb()
{

}

uvast	getOwnNodeNbr()
{

}

time_t	getUTCTime()
{

}

