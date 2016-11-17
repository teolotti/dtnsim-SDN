/*
 *	smrbt.c:	shared memory red-black tree management library.
 *
 *	Author: Scott Burleigh, JPL
 *
 *	Modification History:
 *	Date	  Who	What
 *	11-17-11  SCB	Adapted from Julienne Walker tutorial, public
 *			domain code.
 *	(http://eternallyconfuzzled.com/tuts/datastructures/jsw_tut_rbtree.aspx)
 *
 *	Copyright (c) 2011, California Institute of Technology.
 *	ALL RIGHTS RESERVED.  U.S. Government Sponsorship
 *	acknowledged.
 */

#include "platform.h"
#include "smrbt.h"

/*		Private definitions of shared-memory rbt structures.	*/

#define	LEFT		0
#define	RIGHT		1

typedef struct
{
	PsmAddress	userData;
	PsmAddress	root;		/*	root node of the rbt	*/
	unsigned long   length;		/*	number of nodes in rbt	*/
	sm_SemId	lock;		/*	mutex for tree		*/
} SmRbt;

typedef struct
{
	PsmAddress	rbt;		/*	rbt that node is in	*/
	PsmAddress	parent;		/*	parent node in tree	*/
	PsmAddress	child[2];	/*	child nodes in tree	*/
	PsmAddress	data;		/*	data for this node	*/
	int		isRed;		/*	Boolean; if 0, is Black	*/
} SmRbtNode;

/*	*	*	Rbt management functions	*	*	*/

PsmAddress	sm_rbt_first(PsmPartition partition, PsmAddress rbt)
{

}

PsmAddress	sm_rbt_last(PsmPartition partition, PsmAddress rbt)
{

}

static PsmAddress	traverseRbt(PsmPartition partition,
				PsmAddress fromNode, int direction)
{

}

PsmAddress	Sm_rbt_traverse(PsmPartition partition, PsmAddress fromNode,
			int direction)
{

}

PsmAddress	sm_rbt_search(PsmPartition partition, PsmAddress rbt,
			SmRbtCompareFn compare, void *dataBuffer,
			PsmAddress *successor)
{

}

PsmAddress	sm_rbt_rbt(PsmPartition partition, PsmAddress node)
{

}

PsmAddress	sm_rbt_data(PsmPartition partition, PsmAddress node)
{

}


