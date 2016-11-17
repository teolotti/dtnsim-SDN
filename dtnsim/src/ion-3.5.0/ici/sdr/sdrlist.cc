/*
 *	sdrlist.c:	spacecraft data recorder list management
 *			library.
 *
 *	Copyright (c) 2001-2007, California Institute of Technology.
 *	ALL RIGHTS RESERVED.  U.S. Government Sponsorship
 *	acknowledged.
 *
 *	Author: Scott Burleigh, JPL
 *
 *	This library implements the Simple Data Recorder system's
 *	self-delimiting strings.
 *
 *	Modification History:
 *	Date	  Who	What
 *	4-3-96	  APS	Abstracted IPC services and task control.
 *	5-1-96	  APS	Ported to sparc-sunos4.
 *	12-20-00  SCB	Revised for sparc-sunos5.
 *	6-8-07    SCB	Divided sdr.c library into separable components.
 */

#include "sdrP.h"
#include "sdrlist.h"

/*		Private definitions of SDR list structures.		*/

typedef struct
{
	Address		userData;
	Object		first;	/*	first element in the list	*/
	Object		last;	/*	last element in the list	*/
	unsigned long   length;	/*	number of elements in the list	*/
} SdrList;

typedef struct
{
	Object		list;	/*	list that this element is in	*/
	Object		prev;	/*	previous element in list	*/
	Object		next;	/*	next element in list		*/
	Object		data;	/*	data for this element		*/
} SdrListElt;

Address	sdr_list_data(Sdr sdrv, Object elt)
{

}

Object	sdr_list_next(Sdr sdrv, Object elt)
{

}

Object	sdr_list_first(Sdr sdrv, Object list)
{

}

Object	sdr_list_prev(Sdr sdrv, Object elt)
{

}

Object	sdr_list_last(Sdr sdrv, Object list)
{

}

