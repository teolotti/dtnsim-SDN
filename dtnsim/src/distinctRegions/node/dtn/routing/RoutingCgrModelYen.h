#ifndef SRC_DISTINCTREGIONS_NODE_DTN_ROUTINGCGRMODELYEN_H_
#define SRC_DISTINCTREGIONS_NODE_DTN_ROUTINGCGRMODELYEN_H_

#include <src/distinctRegions/node/dtn/routing/CgrRoute.h>
#include <src/distinctRegions/node/dtn/routing/RoutingDeterministic.h>
#include <algorithm>


namespace dtnsimdistinct {

class RoutingCgrModelYen: public RoutingDeterministic {


public:
	RoutingCgrModelYen(int eid, SdrModel *sdr, map<string, ContactPlan> &contactPlans, bool printDebug);
	virtual ~RoutingCgrModelYen();

	// Entry method (reset stats and call cgrForward)
	virtual void routeAndQueueBundle(BundlePacket *bundle, double simTime, int destinationEid);

	void cgrForward(BundlePacket *bundle, double simTime, int destinationEid);

	// stats recollection
	double getDijkstraCalls();
	double getDijkstraLoops();
	double getRouteTableEntriesCreated();
	double getRouteTableEntriesExplored();
	double getEdgesOneDijkstra() const;
	double getNodesOneDijkstra() const;
	double getOneDijkstraComplexity() const;
	bool getBundleDropped();
	double getRouteTableSize();
	int getRouteSearchStarts();
	int getYenIterations();

	bool printDebug_ = false;

	// Statistics
	double dijkstraCalls;
	double dijkstraLoops;
	double tableEntriesCreated;
	double tableEntriesExplored;
	double nodesOneDijkstra;
	double edgesOneDijkstra;
	bool bundleDropped = false;
	double routeTableSize;
	int routeSearchStarts;
	int yenIterations;

	string currentRegion_ = "A";
	ContactPlan* currentContactPlan_;

private:

	// list of computed, non-terminated routes for a given destination EID
	map<int, vector<CgrRoute> > routeLists_;
	void pruneRouteList(double simTime, int destinationEid);

	void identifyCandidateRoutes(BundlePacket *bundle, double simTime, int destinationEid, vector<CgrRoute> *candidateRoutes);
	bool isCandidateRoute(BundlePacket *bundle, CgrRoute* potentialRoute, double simTime);
	void findNextBestRoute(Contact *rootContact, int destinationEid, CgrRoute *route);
	void bpEnqueue(BundlePacket *bundle, CgrRoute *selectedRoute);

};
}

#endif /* SRC_DISTINCTREGIONS_NODE_DTN_ROUTINGCGRMODELYEN_H_ */
