/*
	dtn2fw.c:	scheme-specific forwarder for the "dtn2"
			scheme.

	Author: Scott Burleigh, JPL

	Copyright (c) 2006, California Institute of Technology.
	ALL RIGHTS RESERVED.  U.S. Government Sponsorship
	acknowledged.
									*/
#include "dtn2fw.h"

static sm_SemId	_dtn2fwSemaphore(sm_SemId *newValue)
{
	uaddr		temp;
	void		*value;
	sm_SemId	sem;

	if (newValue)			/*	Add task variable.	*/
	{
		temp = *newValue;
		value = (void *) temp;
		value = sm_TaskVar(&value);
	}
	else				/*	Retrieve task variable.	*/
	{
		value = sm_TaskVar(NULL);
	}

	temp = (uaddr) value;
	sem = temp;
	return sem;
}

static void	shutDown()	/*	Commands forwarder termination.	*/
{
	isignal(SIGTERM, (void(*)(int)) shutDown);
	sm_SemEnd(_dtn2fwSemaphore(NULL));
}

static int	parseDtn2Nss(char *nss, char *nodeName, char *demux)
{
	int	nssLength;
	char	*startOfNodeName;
	char	*endOfNodeName;
	char	*startOfDemux;

	nssLength = strlen(nss);
	if (nssLength < 3 || strncmp(nss, "//", 2) != 0)
	{
		return 0;		/*	Wrong NSS format.	*/
	}

	startOfNodeName = nss;
	endOfNodeName = strchr(startOfNodeName + 2, '/');
	if (endOfNodeName == NULL)	/*	No delimiter, no demux.	*/
	{
		if (nssLength >= SDRSTRING_BUFSZ)
		{
			return 0;	/*	Too long.		*/
		}

		istrcpy(nodeName, startOfNodeName, SDRSTRING_BUFSZ);
		*demux = '\0';
		return 1;		/*	Fully parsed.		*/
	}

	if ((endOfNodeName - startOfNodeName) >= SDRSTRING_BUFSZ)
	{
		return 0;		/*	Too long.		*/
	}

	*endOfNodeName = '\0';		/*	Temporary.		*/
	istrcpy(nodeName, startOfNodeName, SDRSTRING_BUFSZ);
	*endOfNodeName = '/';		/*	Restore original.	*/
	startOfDemux = endOfNodeName + 1;
	if (strlen(startOfDemux) >= SDRSTRING_BUFSZ)
	{
		return 0;
	}

	istrcpy(demux, startOfDemux, SDRSTRING_BUFSZ);
	return 1;
}

static int	enqueueBundle(Bundle *bundle, Object bundleObj)
{
	Sdr		sdr = getIonsdr();
	Object		elt;
	char		eidString[SDRSTRING_BUFSZ];
	MetaEid		metaEid;
	VScheme		*vscheme;
	PsmAddress	vschemeElt;
	char		nodeName[SDRSTRING_BUFSZ];
	char		demux[SDRSTRING_BUFSZ];
	int		result;
	FwdDirective	directive;

	elt = sdr_list_first(sdr, bundle->stations);
	if (elt == 0)
	{
		putErrmsg("Forwarding error; stations stack is empty.", NULL);
		return -1;
	}

	sdr_string_read(sdr, eidString, sdr_list_data(sdr, elt));
	if (parseEidString(eidString, &metaEid, &vscheme, &vschemeElt) == 0)
	{
		putErrmsg("Can't parse node EID string.", eidString);
		return bpAbandon(bundleObj, bundle, BP_REASON_NO_ROUTE);
	}

	if (strcmp(vscheme->name, "dtn") != 0)
	{
		putErrmsg("Forwarding error; EID scheme wrong for dtn2fw.",
				vscheme->name);
		return -1;
	}

	result = parseDtn2Nss(metaEid.nss, nodeName, demux);
	restoreEidString(&metaEid);
	if (result == 0)
	{
		putErrmsg("Invalid nss in EID string, cannot forward.",
				eidString);
		return bpAbandon(bundleObj, bundle, BP_REASON_NO_ROUTE);
	}

	if (dtn2_lookupDirective(nodeName, demux, bundle, &directive) == 0)
	{
		putErrmsg("Can't find forwarding directive for EID.",
				eidString);
		return bpAbandon(bundleObj, bundle, BP_REASON_NO_ROUTE);
	}

	if (directive.action == xmit)
	{
		if (bpEnqueue(&directive, bundle, bundleObj, eidString) < 0)
		{
			putErrmsg("Can't enqueue bundle.", NULL);
			return -1;
		}

		if (bundle->ductXmitElt)
		{
			/*	Enqueued.				*/

			return bpAccept(bundleObj, bundle);
		}
		else
		{
			return bpAbandon(bundleObj, bundle, BP_REASON_NO_ROUTE);
		}
	}

	/*	Can't transmit to indicated next node directly, must
	 *	forward through some other node.			*/

	sdr_write(sdr, bundleObj, (char *) &bundle, sizeof(Bundle));
	sdr_string_read(sdr, eidString, directive.eid);
	return forwardBundle(bundleObj, bundle, eidString);
}

