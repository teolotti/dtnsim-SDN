#ifndef _NET_H_
#define _NET_H_

#include <cstdio>
#include <string>
#include <omnetpp.h>
#include <fstream>
#include <sstream>
#include <map>
#include <queue>

#include "MsgTypes.h"
#include "dtnsim_m.h"

#include "ContactPlan.h"
#include "Graphics.h"
#include "SdrModel.h"

#include "Routing.h"
#include "Ion.h"
#include "RoutingDirect.h"
#include "RoutingCgrIon350.h"
#include "RoutingCgrModelYen.h"
#include "RoutingCgrModel350.h"
#include "RoutingCgrModelRev17.h"

#include "Config.h"
#include "RouterUtils.h"
#include "utils/TopologyUtils.h"
#include "utils/RouterUtils.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace omnetpp;
using namespace std;



class Net: public cSimpleModule
{
public:
	Net();
	virtual ~Net();

	virtual void setOnFault(bool onFault);
	virtual void refreshForwarding();

protected:
	virtual void initialize(int stage);
	virtual int numInitStages() const;
	virtual void handleMessage(cMessage *msg);
	virtual void finish();

	virtual void dispatchBundle(BundlePkt *bundle);

private:

	int eid_;
	bool onFault = false;

	// Pointer to grahics module
	Graphics *graphicsModule;

	// Forwarding threads
	map<int, ForwardingMsg *> forwardingMsgs_;

	// Routing and storage
	Routing * routing;
	ContactPlan contactPlan_;
	SdrModel sdr_;

	// BundlesMap
	bool saveBundleMap_;
	ofstream bundleMap_;

	// Signals
	simsignal_t netBundleSentToMac;
	simsignal_t netBundleSentToApp;
	simsignal_t netBundleSentToAppHopCount;
	simsignal_t netBundleSentToAppRevisitedHops;
	simsignal_t netBundleReceivedFromMac;
	simsignal_t netBundleReceivedFromApp;
	simsignal_t netBundleReRouted;
	simsignal_t sdrBundleStored;
	simsignal_t sdrBytesStored;
	simsignal_t routeCgrDijkstraCalls;
	simsignal_t routeCgrDijkstraLoops;
	simsignal_t routeCgrRouteTableEntriesExplored;
};

#endif /* NET_H_ */

