/*
 *	libbpP.c:	functions enabling the implementation of
 *			ION-BP bundle forwarders.
 *
 *	Copyright (c) 2006, California Institute of Technology.
 *	ALL RIGHTS RESERVED.  U.S. Government Sponsorship
 *	acknowledged.
 *
 *	Author: Scott Burleigh, JPL
 *
 *	Modification History:
 *	Date	  Who	What
 *	06-17-03  SCB	Original development.
 *	05-30-04  SCB	Revision per version 3 of Bundle Protocol spec.
 *	01-09-06  SCB	Revision per version 4 of Bundle Protocol spec.
 *	12-29-06  SCB	Revision per version 5 of Bundle Protocol spec.
 *	07-31-07  SCB	Revision per version 6 of Bundle Protocol spec.
 */

#include "bpP.h"

#define MAX_STARVATION		10
#define NOMINAL_BYTES_PER_SEC	(256 * 1024)
#define NOMINAL_PRIMARY_BLKSIZE	29
#define	ION_DEFAULT_XMIT_RATE	125000000

#define	BASE_BUNDLE_OVERHEAD	(sizeof(Bundle))

#ifndef BUNDLES_HASH_KEY_LEN
#define	BUNDLES_HASH_KEY_LEN	64
#endif

#define	BUNDLES_HASH_KEY_BUFLEN	(BUNDLES_HASH_KEY_LEN << 1)

#ifndef BUNDLES_HASH_ENTRIES
#define	BUNDLES_HASH_ENTRIES	10000
#endif

#ifndef BUNDLES_HASH_SEARCH_LEN
#define	BUNDLES_HASH_SEARCH_LEN	20
#endif

void	computePriorClaims(ClProtocol *protocol, Outduct *duct, Bundle *bundle,
		Scalar *priorClaims, Scalar *totalBacklog)
{

}

int	computeECCC(int bundleSize, ClProtocol *protocol)
{

}

int	guessBundleSize(Bundle *bundle)
{

}

Object	insertBpTimelineEvent(BpEvent *newEvent)
{

}

int	bpEnqueue(FwdDirective *directive, Bundle *bundle, Object bundleObj,
		char *proxNodeEid)
{

}

void	removeBundleFromQueue(Bundle *bundle, Object bundleObj,
		ClProtocol *protocol, Object outductObj, Outduct *outduct)
{

}

int	bpReforwardBundle(Object bundleAddr)
{

}

int	bpClone(Bundle *oldBundle, Bundle *newBundle, Object *newBundleObj,
		unsigned int offset, unsigned int length)
{

}

int	enqueueToLimbo(Bundle *bundle, Object bundleObj)
{

}




