
#ifndef SRC_DISTINCTREGIONS_NODE_DTN_ROUTINGINTERREGIONS_H_
#define SRC_DISTINCTREGIONS_NODE_DTN_ROUTINGINTERREGIONS_H_

#include <src/distinctRegions/node/dtn/routing/IrrRoute.h>
#include <src/distinctRegions/node/dtn/routing/IrrVertex.h>
#include <src/distinctRegions/node/dtn/routing/RegionDatabase.h>
#include <src/distinctRegions/node/dtn/routing/RoutingCgrModelYen.h>
#include <algorithm>
#include <set>


namespace dtnsimdistinct {

class RoutingInterRegions: public RoutingCgrModelYen {


public:
	RoutingInterRegions(int eid, SdrModel *sdr, map<string, ContactPlan> &contactPlans, RegionDatabase *regionDatabase, bool printDebug, bool staticIrr);
	virtual ~RoutingInterRegions();

	// Entry method (reset stats and call cgrForward)
	virtual void routeAndQueueBundle(BundlePacket *bundle, double simTime, int destinationEid);

private:

	RegionDatabase* regionDatabase_;

	int determineNextDestination(BundlePacket *bundle);
	IrrRoute computeInterRegionalRoute(BundlePacket *bundle, set<string> currentNodeRegions, set<string> destinationNodeRegions);

	// Experimental variation of IRR specific to thesis network topology
	bool staticIrr_ = false;
	int determineNextDestinationStatic(BundlePacket *bundle);
	void directEnqueue(BundlePacket *bundle, int nextHopNode);
};
}

#endif /* SRC_DISTINCTREGIONS_NODE_DTN_ROUTINGINTERREGIONS_H_ */
