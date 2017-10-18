#ifndef _DTN_H_
#define _DTN_H_

#include <dtn/ContactPlan.h>
#include <dtn/routing/Routing.h>
#include <dtn/routing/RoutingCgrIon350.h>
#include <dtn/routing/RoutingCgrModel350.h>
#include <dtn/routing/RoutingCgrModel350_3.h>
#include <dtn/routing/RoutingCgrModelRev17.h>
#include <dtn/routing/RoutingCgrModelYen.h>
#include <dtn/routing/RoutingDirect.h>
#include <dtn/routing/RoutingEpidemic.h>
#include <dtn/routing/RoutingSprayAndWait.h>
#include <dtn/routing/RoutingCgrModel350_Proactive.h>
#include <dtn/routing/RoutingCgrModel350_Probabilistic.h>
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
#include "Config.h"
#include "RouterUtils.h"
#include "utils/TopologyUtils.h"
#include "utils/RouterUtils.h"
#include "utils/ContactPlanUtils.h"
#include "utils/Observer.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace omnetpp;
using namespace std;



class Dtn: public cSimpleModule, public Observer
{
public:
	Dtn();
	virtual ~Dtn();

	virtual void setOnFault(bool onFault);
	virtual void refreshForwarding();
	ContactPlan * getContactPlanPointer();
	virtual void setContactPlan(ContactPlan &contactPlan);
	virtual void setContactTopology(ContactPlan &contactTopology);
	virtual Routing * getRouting();

	virtual void update(void);

protected:
	virtual void initialize(int stage);
	virtual int numInitStages() const;
	virtual void handleMessage(cMessage *msg);
	virtual void finish();

	virtual void dispatchBundle(BundlePkt *bundle);

private:

	int eid_;
	bool onFault = false;
	bool simpleCustodyModel_;

	// Pointer to grahics module
	Graphics *graphicsModule;

	// Forwarding threads
	map<int, ForwardingMsgStart *> forwardingMsgs_;

	// Routing and storage
	Routing * routing;

	// Contact Plan to feed CGR
	// and get transmission rates
	ContactPlan contactPlan_;

	// Contact Topology to schedule Contacts
	// and get transmission rates
	ContactPlan contactTopology_;

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
};

#endif /* DTN_H_ */

