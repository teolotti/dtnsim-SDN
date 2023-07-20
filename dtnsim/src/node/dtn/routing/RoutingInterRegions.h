
#ifndef SRC_NODE_DTN_ROUTINGINTERREGIONS_H_
#define SRC_NODE_DTN_ROUTINGINTERREGIONS_H_

#include <src/node/dtn/routing/IrrRoute.h>
#include <src/node/dtn/RegionDatabase.h>
#include "src/dtnsim_m.h"
#include <src/node/dtn/routing/RoutingInterRegionsGraph.h>
#include <limits>
#include <string>


using namespace std;

class RoutingInterRegions {

public:
	RoutingInterRegions(int eid, RegionDatabase * regionDatabase, string interRegionsRoutingType, bool printDebug);
	virtual ~RoutingInterRegions();

	// compute inter regions routing
	IrrRoute computeInterRegionsRouting(BundlePkt * bundle, set < string > thisNodeRegionIds, set < string > destinationRegionIds);
	RegionDatabase* getRegionDatabase();
	double getDijkstraCalls() const;
	double getEdgesOneDijkstra() const;
	double getNodesOneDijkstra() const;
	double getOneDijkstraComplexity() const;

private:

	IrrRoute  getShortestPath(BundlePkt * bundle, set < string > thisNodeRegionIds, set < string > destinationRegionIds);
	IrrRoute  getShortestPathBetweenRegions(BundlePkt * bundle, string sourceRegionId, string destinationRegionId);
	double computeDistanceBetweenVertices(InterRegionsVertex v1, InterRegionsVertex v2);
	set<pair<string, string> > getCombinations(set<string> s1, set<string> s2);

	int eid_;
	RegionDatabase *regionDatabase_;
	string interRegionsRoutingType_;
	bool printDebug_ = false;

	int dijkstraCalls_;
	int nodesOneDijkstra;
	int edgesOneDijkstra;

};

#endif /* SRC_NODE_DTN_ROUTINGINTERREGIONS_H_ */
