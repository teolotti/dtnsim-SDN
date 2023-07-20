#ifndef SRC_HIERARCHICALREGIONS_NODE_DTN_DTN_H_
#define SRC_HIERARCHICALREGIONS_NODE_DTN_DTN_H_

#include <omnetpp.h>
#include <algorithm>
#include <list>

#include <src/hierarchicalRegions/node/dtn/contacts/ContactPlan.h>
#include <src/hierarchicalRegions/node/graphics/Graphics.h>
#include <src/hierarchicalRegions/node/dtn/sdr/SdrModel.h>
#include <src/hierarchicalRegions/node/dtn/routing/Routing.h>
#include <src/hierarchicalRegions/node/dtn/routing/RoutingCgrModel350.h>

#include "src/hierarchicalRegions/node/MsgTypes.h"
#include "src/hierarchicalRegions/HierarchicalRegionsNetwork_m.h"


using namespace omnetpp;
using namespace std;

namespace dtnsimhierarchical {
class Dtn: public cSimpleModule, public Observer {

public:
	Dtn();
	virtual ~Dtn();

	virtual void setHomeContactPlan(ContactPlan &contactPlan);
	virtual void setOuterContactPlan(ContactPlan &contactPlan);

protected:
	virtual void initialize(int stage);
	virtual int numInitStages() const;
	virtual void handleMessage(cMessage *msg);
	virtual void finish();

	void scheduleContacts(vector<Contact> contacts);
	virtual void dispatchBundle(BundlePacketHIRR *bundle);
	virtual void refreshForwarding();
	virtual void update();

	virtual void handleBlacklistBundle(BundlePacketHIRR *bundle);

private:

	Graphics* graphicsModule_;

	int eid_;
	string homeRegion_;
	string outerRegion_;
	string type_;
	ContactPlan homeContactPlan_;
	ContactPlan outerContactPlan_;
	ContactPlan routingContactPlan_;

	// each node has a blacklist, i.e. for a destination dEid,
	// a list of PW EIDs that are not on the path to dEid
	map<int, vector<int>> blacklist_;


	// all passageways cited in the accessible contact plan(s)
	vector<int> homePeers_;
	vector<int> outerPeers_;
	virtual void identifyPeers();

	int findProvenance(int pwEid); // TODO

	SdrModel* sdr_;
	Routing* routing_;
	map<int, StartForwardingMsgHIRR *> forwardingMsgs_;

};
}

#endif /* SRC_HIERARCHICALREGIONS_NODE_DTN_DTN_H_ */
