#ifndef SRC_HIERARCHICALREGIONS_NODE_DTN_ROUTING_ROUTINGDETERMINISTIC_H_
#define SRC_HIERARCHICALREGIONS_NODE_DTN_ROUTING_ROUTINGDETERMINISTIC_H_

#include <src/hierarchicalRegions/node/dtn/routing/Routing.h>

namespace dtnsimhierarchical {
class RoutingDeterministic: public Routing {

public:

	RoutingDeterministic(int eid, SdrModel *sdr, ContactPlan *contactPlan);
	virtual ~RoutingDeterministic();

	virtual void msgToOtherArrive(BundlePacketHIRR *bundle, double simTime, int terminusNode); //simply calls routeAndQueueBundle

	virtual bool msgToMeArrive(BundlePacketHIRR *bundle);

	virtual void contactStart(Contact *c);

	virtual void contactEnd(Contact *c);

	virtual void successfulBundleForwarded(long bundleId, Contact *contact,  bool sentToDestination);

	virtual void  refreshForwarding(Contact *c);

	// This is a pure virtual method (all deterministic routing must at least
	// implement this function)
	virtual void routeAndQueueBundle(BundlePacketHIRR *bundle, double simTime, int terminusNode) = 0;

protected:

	ContactPlan* contactPlan_;

};
}

#endif /* SRC_HIERARCHICALREGIONS_NODE_DTN_ROUTING_ROUTINGDETERMINISTIC_H_ */
