#ifndef SRC_DISTINCTREGIONS_NODE_DTN_ROUTING_ROUTINGDETERMINISTIC_H_
#define SRC_DISTINCTREGIONS_NODE_DTN_ROUTING_ROUTINGDETERMINISTIC_H_

#include <src/distinctRegions/node/dtn/routing/Routing.h>

namespace dtnsimdistinct {
class RoutingDeterministic: public Routing {

public:

	RoutingDeterministic(int eid, SdrModel *sdr, map<string, ContactPlan> &contactPlans);
	virtual ~RoutingDeterministic();

	virtual void msgToOtherArrive(BundlePacket *bundle, double simTime, int terminusNode); //simply calls routeAndQueueBundle

	virtual bool msgToMeArrive(BundlePacket *bundle);

	virtual void contactStart(Contact *c);

	virtual void contactEnd(Contact *c);

	virtual void successfulBundleForwarded(long bundleId, Contact *contact,  bool sentToDestination);

	virtual void  refreshForwarding(Contact *c);

	// This is a pure virtual method (all deterministic routing must at least
	// implement this function)
	virtual void routeAndQueueBundle(BundlePacket *bundle, double simTime, int terminusNode) = 0;

protected:

	map<string, ContactPlan> contactPlans_;

};
}

#endif /* SRC_DISTINCTREGIONS_NODE_DTN_ROUTING_ROUTINGDETERMINISTIC_H_ */
