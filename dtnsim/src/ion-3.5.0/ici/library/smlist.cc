/*
 *	smlist.c:	shared-memory linked list management.
 *			Adapted from SDR linked-list management,
 *			which in turn was lifted from Jeff
 *			Biesiadecki's list library.
 *
 *	Copyright (c) 2001, California Institute of Technology.
 *	ALL RIGHTS RESERVED.  U.S. Government Sponsorship
 *	acknowledged.
 *
 *	Author: Scott Burleigh, JPL
 *
 *	Modification History:
 *	Date	  Who	What
 *	04-26-03  SCB	One mutex per list.
 *	12-11-00  SCB	Initial implementation.
 */

#include "platform.h"
#include "smlist.h"

/* define a list */
typedef struct
{
	PsmAddress	userData;
	PsmAddress	first;	/*	first element in the list	*/
	PsmAddress	last;	/*	last element in the list	*/
	unsigned long	length;	/*	number of elements in the list	*/
	sm_SemId	lock;	/*	mutex for list			*/
} SmList;

/* define an element of a list */
typedef struct
{
	PsmAddress	list;	/* address of list that element is in	*/
	PsmAddress	prev;	/* address of previous element in list	*/
	PsmAddress	next;	/* address of next element in list	*/
	PsmAddress	data;	/* address of data for this element	*/
} SmListElt;


PsmAddress	Sm_list_create(const char *fileName, int lineNbr,
			PsmPartition partition)
{

}

int	Sm_list_destroy(const char *fileName, int lineNbr,
		PsmPartition partition, PsmAddress list,
		SmListDeleteFn deleteFn, void *arg)
{

}

PsmAddress	sm_list_user_data(PsmPartition partition, PsmAddress list)
{

}

int	sm_list_user_data_set(PsmPartition partition, PsmAddress list,
			PsmAddress data)
{

}

int	sm_list_length(PsmPartition partition, PsmAddress list)
{

}

PsmAddress	Sm_list_insert_first(const char *fileName, int lineNbr,
			PsmPartition partition, PsmAddress list,
			PsmAddress data)
{

}

PsmAddress	Sm_list_insert_last(const char *fileName, int lineNbr,
			PsmPartition partition, PsmAddress list,
			PsmAddress data)
{

}

PsmAddress	Sm_list_insert_before(const char *fileName, int lineNbr,
			PsmPartition partition, PsmAddress oldElt,
			PsmAddress data)
{

}


PsmAddress	Sm_list_insert_after(const char *fileName, int lineNbr,
			PsmPartition partition, PsmAddress oldElt,
			PsmAddress data)
{

}

PsmAddress	Sm_list_insert(const char *fileName, int lineNbr,
			PsmPartition partition, PsmAddress list,
			PsmAddress data, SmListCompareFn compare, void *argData)
{

}

int	Sm_list_delete(const char *fileName, int lineNbr,
		PsmPartition partition, PsmAddress elt, SmListDeleteFn deleteFn,
		void *arg)
{

}

PsmAddress	sm_list_first(PsmPartition partition, PsmAddress list)
{

}

PsmAddress	sm_list_last(PsmPartition partition, PsmAddress list)
{

}

PsmAddress	sm_list_next(PsmPartition partition, PsmAddress elt)
{

}

PsmAddress	sm_list_prev(PsmPartition partition, PsmAddress elt)
{

}

PsmAddress	sm_list_search(PsmPartition partition, PsmAddress fromElt,
			SmListCompareFn compare, void *arg)
{

}

PsmAddress	sm_list_list(PsmPartition partition, PsmAddress elt)
{

}

PsmAddress	sm_list_data(PsmPartition partition, PsmAddress elt)
{

}

PsmAddress	sm_list_data_set(PsmPartition partition, PsmAddress elt,
			PsmAddress new_)
{

}
