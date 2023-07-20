#include <src/hierarchicalRegions/node/dtn/routing/RoutingDeterministic.h>

namespace dtnsimhierarchical {



RoutingDeterministic::RoutingDeterministic(int eid, SdrModel *sdr, ContactPlan *contactPlan) : Routing(eid, sdr) {
	contactPlan_ = contactPlan;
}

RoutingDeterministic::~RoutingDeterministic() {
}

void RoutingDeterministic::msgToOtherArrive(BundlePacketHIRR *bundle, double simTime, int terminusNode) {
	// Route and enqueue bundle
	routeAndQueueBundle(bundle, simTime, terminusNode);
}

bool RoutingDeterministic::msgToMeArrive(BundlePacketHIRR *bundle) {
	return true;
}

void RoutingDeterministic::contactStart(Contact *c) {

}

void RoutingDeterministic::successfulBundleForwarded(long bundleId, Contact *c, bool sentToDestination) {

}

void RoutingDeterministic::refreshForwarding(Contact *c) {

}

void RoutingDeterministic::contactEnd(Contact *c) {
	//TODO
}
}

