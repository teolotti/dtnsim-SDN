#ifndef SRC_DISTINCTREGIONS_NODE_DTN_DTN_H_
#define SRC_DISTINCTREGIONS_NODE_DTN_DTN_H_

#include <omnetpp.h>

#include <src/distinctRegions/node/dtn/contacts/ContactPlan.h>
#include <src/distinctRegions/node/dtn/routing/RegionDatabase.h>
#include <src/distinctRegions/node/graphics/Graphics.h>
#include <src/distinctRegions/node/dtn/sdr/SdrModel.h>
#include <src/distinctRegions/node/dtn/routing/Routing.h>
#include <src/distinctRegions/node/dtn/routing/RoutingCgrModelYen.h>
#include <src/distinctRegions/node/dtn/routing/RoutingInterRegions.h>

#include "src/distinctRegions/node/MsgTypes.h"
#include "src/distinctRegions/RegionsNetwork_m.h"

#include <chrono>

using namespace omnetpp;
using namespace std::chrono;


namespace dtnsimdistinct {
class Dtn: public cSimpleModule, public Observer {

public:
	Dtn();
	virtual ~Dtn();

	virtual void setRegionContactPlan(ContactPlan &regionContactPlan);
	virtual void setBackboneContactPlan(ContactPlan &backboneContactPlan);
	virtual void setRegionDatabase(RegionDatabase &regionDatabase);
	virtual void addRegionContactPlan(string region, ContactPlan &ContactPlan);

protected:
	virtual void initialize(int stage);
	virtual int numInitStages() const;
	virtual void handleMessage(cMessage *msg);
	virtual void finish();

	void scheduleContacts(vector<Contact*> contacts, short priorityStart, short priorityEnd, string messageNameStart, string messageNameEnd);
	virtual void dispatchBundle(BundlePacket *bundle);
	virtual void refreshForwarding();
	virtual void update();

private:

	Graphics* graphicsModule_;

	int eid_;
	string region_;
	string type_;
	ContactPlan regionContactPlan_;
	ContactPlan backboneContactPlan_;
	RegionDatabase regionDatabase_;

	// contact plans of all regions this node is part of
	// If no IRR -> contactPlans_ has one element for region "A" equal to regionContactPlan_
	// If IRR -> regionContactPlan_ is all contact plans combined
	map<string, ContactPlan> contactPlans_;

	vector<int> regionalPassageways_; // list of PW EIDs known to region nodes (obsolete)
	virtual void identifyRegionalPassageways();

	map<string, vector<int>> globalPassageways_; // list of region->EIDs known to PW and BB nodes (obsolete)
	virtual void identifyGlobalPassageways();

	SdrModel* sdr_;
	Routing* intraRouting_; // routing inside the region
	Routing* interRouting_; // for now, only ever intraRouting_ is used, distiction obsolete, remnant from distinct regions
	map<int, StartForwardingMsg *> forwardingMsgs_;

	simsignal_t routeExecutionTimeUs;
	simsignal_t sdrBundleStored;
	simsignal_t sdrBytesStored;
	simsignal_t routeCgrRouteTableEntriesCreated;
	simsignal_t routeCgrRouteTableEntriesExplored;
	simsignal_t bundleDropped;
	simsignal_t bundleReceivedFromCom;
	simsignal_t routeComplexity;
	simsignal_t routeTableSize;
	simsignal_t yenIterations;
	simsignal_t routeSearchCalls;
	simsignal_t routeSearchStarts;
	simsignal_t firstHop;

};
}

#endif /* SRC_DISTINCTREGIONS_NODE_DTN_DTN_H_ */
