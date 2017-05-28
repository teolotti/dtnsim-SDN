#ifndef _DTN_H_
#define _DTN_H_

#include <dtn/ContactPlan.h>
#include <dtn/routing/Routing.h>
#include <dtn/routing/RoutingCgrIon350.h>
#include <dtn/routing/RoutingCgrModel350.h>
#include <dtn/routing/RoutingCgrModelRev17.h>
#include <dtn/routing/RoutingCgrModelYen.h>
#include <dtn/routing/RoutingDirect.h>
#include <dtn/SdrModel.h>
#include <cstdio>
#include <string>
#include <omnetpp.h>
#include <fstream>
#include <sstream>
#include <map>
#include <queue>

#include "MsgTypes.h"
#include "dtnsim_m.h"

#include "Graphics.h"
#include "Routing.h"
#include "Ion.h"
#include "Config.h"
#include "RouterUtils.h"
#include "utils/TopologyUtils.h"
#include "utils/RouterUtils.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace omnetpp;
using namespace std;



class Dtn: public cSimpleModule
{
public:
	Dtn();
	virtual ~Dtn();

	virtual void setOnFault(bool onFault);
	virtual void refreshForwarding();
	ContactPlan * getContactPlanPointer();

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
	simsignal_t dtnBundleSentToCom;
	simsignal_t dtnBundleSentToApp;
	simsignal_t dtnBundleSentToAppHopCount;
	simsignal_t dtnBundleSentToAppRevisitedHops;
	simsignal_t dtnBundleReceivedFromCom;
	simsignal_t dtnBundleReceivedFromApp;
	simsignal_t dtnBundleReRouted;
	simsignal_t sdrBundleStored;
	simsignal_t sdrBytesStored;
	simsignal_t routeCgrDijkstraCalls;
	simsignal_t routeCgrDijkstraLoops;
	simsignal_t routeCgrRouteTableEntriesExplored;
};

#endif /* DTN_H_ */

