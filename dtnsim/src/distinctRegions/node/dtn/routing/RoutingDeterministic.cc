#include <src/distinctRegions/node/dtn/routing/RoutingDeterministic.h>

namespace dtnsimdistinct {



RoutingDeterministic::RoutingDeterministic(int eid, SdrModel *sdr, map<string, ContactPlan> &contactPlans) : Routing(eid, sdr) {
	contactPlans_ = contactPlans;
}

RoutingDeterministic::~RoutingDeterministic() {
}

void RoutingDeterministic::msgToOtherArrive(BundlePacket *bundle, double simTime, int terminusNode) {
	// Route and enqueue bundle
	routeAndQueueBundle(bundle, simTime, terminusNode);
}

bool RoutingDeterministic::msgToMeArrive(BundlePacket *bundle) {
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

