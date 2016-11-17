/*

	rfx.c:	API for managing ION's time-ordered lists of
		contacts and ranges.  Also manages timeline
		of ION events, including alarm timeouts.

	Author:	Scott Burleigh, JPL

	rfx_remove_contact and rfx_remove_range adapted by Greg Menke
	to apply to all objects for specified from/to nodes when
	fromTime is zero.

	Copyright (c) 2007, California Institute of Technology.
	ALL RIGHTS RESERVED.  U.S. Government Sponsorship
	acknowledged.

									*/
#include "rfx.h"
#include "lyst.h"

#ifndef MAX_CONTACT_LOG_LENGTH
#define MAX_CONTACT_LOG_LENGTH	(8)
#endif
#define CONFIDENCE_BASIS	(MAX_CONTACT_LOG_LENGTH * 2.0)

int	rfx_order_ranges(PsmPartition partition, PsmAddress nodeData,
		void *dataBuffer)
{

}

int	rfx_order_contacts(PsmPartition partition, PsmAddress nodeData,
		void *dataBuffer)
{

}


IonNode	*findNode(IonVdb *ionvdb, uvast nodeNbr, PsmAddress *nextElt)
{

}
