#ifndef _DTN_H_
#define _DTN_H_

#include <src/node/dtn/ContactPlan.h>
#include <src/node/dtn/CustodyModel.h>
#include <src/node/dtn/routing/Routing.h>
#include <src/node/dtn/routing/RoutingDeterministic.h>
#include <src/node/dtn/routing/RoutingCgrModel350.h>
#include <src/node/dtn/routing/RoutingCgrModelRev17.h>
#include <src/node/dtn/routing/RoutingCgrModelYen.h>
#include <src/node/dtn/routing/RoutingInterRegions.h>
#include <src/node/dtn/routing/RoutingDirect.h>
#include <src/node/dtn/routing/RoutingEpidemic.h>
#include <src/node/dtn/routing/RoutingSprayAndWait.h>
#include <src/node/dtn/routing/RoutingCgrModel350_Proactive.h>
#include <src/node/dtn/routing/RoutingCgrModel350_Probabilistic.h>
#include <src/node/dtn/SdrModel.h>
#include <cstdio>
#include <string>
#include <omnetpp.h>
#include <src/node/dtn/RegionDatabase.h>
#include <fstream>
#include <sstream>
#include <map>
#include <queue>
#include <string>

#include "src/node/MsgTypes.h"
#include "src/dtnsim_m.h"

#include "src/node/graphics/Graphics.h"
#include "src/node/dtn/routing/Routing.h"
#include "src/utils/ContactPlanUtils.h"
#include "src/utils/Observer.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <chrono>
#include "routing/RoutingCgrModel350_Hops.h"

using namespace omnetpp;
using namespace std;
using namespace std::chrono;

class Dtn: public cSimpleModule, public Observer {

public:
	Dtn();
	virtual ~Dtn();

	virtual void setOnFault(bool onFault);
	virtual void refreshForwarding();
	ContactPlan * getContactPlanPointer();
	virtual void setContactTopology(ContactPlan &contactTopology);
	virtual void setRegionDatabase(RegionDatabase &regionDatabase);
	virtual Routing * getRouting();
	virtual RegionDatabase * getRegionDatabase();
	void setContactPlans(map<string, ContactPlan> contactPlans);

	virtual void update(void);

protected:
	virtual void initialize(int stage);
	virtual int numInitStages() const;
	virtual void handleMessage(cMessage *msg);
	virtual void finish();

	virtual void dispatchBundle(BundlePkt *bundle);

private:

	virtual set<string> getRegionsInCommon(set<string> la, set<string> lb);

	int eid_;
	bool onFault = false;

	// Pointer to grahics module
	Graphics *graphicsModule;

	// Forwarding threads
	map<int, ForwardingMsgStart *> forwardingMsgs_;

	// Routing and storage
	Routing * routing;
	RoutingInterRegions *routingInterRegions_;

	// Contact Plan per region to feed CGR
	// and get transmission rates
	map<string, ContactPlan> contactPlans_;

	// Contact Topology to schedule Contacts
	// and get transmission rates
	ContactPlan contactTopology_;

	// Region Database to feed IIR
	RegionDatabase regionDatabase_;

	string interRegionsRoutingType_;

	CustodyModel custodyModel_;

	double custodyTimeout_;

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
	simsignal_t routeCgrRouteTableEntriesCreated;
	simsignal_t routeCgrRouteTableEntriesExplored;
	simsignal_t routeDijkstraCallsXnodes;
	simsignal_t routeDijkstraCallsXcomplexity;
	simsignal_t routeExecutionTimeUs;

	simsignal_t routeTableSize;
};

#endif /* DTN_H_ */

