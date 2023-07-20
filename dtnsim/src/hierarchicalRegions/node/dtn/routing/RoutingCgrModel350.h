#ifndef SRC_HIERARCHICALREGIONS_NODE_DTN_ROUTINGCGRMODEL_H_
#define SRC_HIERARCHICALREGIONS_NODE_DTN_ROUTINGCGRMODEL_H_

#include <src/hierarchicalRegions/node/dtn/routing/CgrRoute.h>
#include <src/hierarchicalRegions/node/dtn/routing/RoutingDeterministic.h>
#include <algorithm>


namespace dtnsimhierarchical {

class RoutingCgrModel350: public RoutingDeterministic {


public:
	RoutingCgrModel350(int eid, SdrModel *sdr, ContactPlan *contactPlan, bool printDebug);
	virtual ~RoutingCgrModel350();

	virtual void routeAndQueueBundle(BundlePacketHIRR *bundle, double simTime, int terminusNode);

	// stats recollection
	double getDijkstraCalls();
	double getDijkstraLoops();
	double getRouteTableEntriesExplored();
	double getEdgesOneDijkstra() const;
	double getNodesOneDijkstra() const;
	double getOneDijkstraComplexity() const;

private:

	double dijkstraCalls;
	double dijkstraLoops;
	double tableEntriesExplored;
	double nodesOneDijkstra;
	double edgesOneDijkstra;

	bool printDebug_ = false;

	// list of candidate routes for a given destination EID
	map<int, vector<CgrRoute> > routeLists_;
	void loadRouteList(int terminusNode, double simTime);
	void printRouteLists();
	void findNextBestRoute(Contact *rootContact, int terminusNode, CgrRoute *route);

	typedef struct {
		int neighborNodeNbr;
		int contactId; // This is not, in ION
		double forfeitTime;
		double arrivalTime;
		unsigned int hopCount; // hops from dest. node.
		CgrRoute* route; // pointer to route so we can decrement capacities
	} ProximateNode;

	typedef struct {
		Contact* contact;
		Contact* predecessor;	// predecessor Contact
		double arrivalTime;
		double capacity; 	// in Bytes
		bool visited;
		bool suppressed;
	} Work;

	// called by routeAndQueueBundle (resets all counters)
	void cgrForward(BundlePacketHIRR *bundle, double simTime, int terminusNode);
	void identifyProximateNodes(BundlePacketHIRR *bundle, double simTime, int terminusNode, vector<int> excludedNodes, vector<ProximateNode> *proximateNodes);

	void tryRoute(BundlePacketHIRR *bundle, CgrRoute *route, vector<ProximateNode> *proximateNodes);
	void recomputeRouteForContact();
	void enqueueToNeighbor(BundlePacketHIRR *bundle, ProximateNode *selectedNeighbor);
	void enqueueToLimbo(BundlePacketHIRR *bundle);
	void bpEnqueue(BundlePacketHIRR *bundle, ProximateNode *selectedNeighbor);
};
}

#endif /* SRC_HIERARCHICALREGIONS_NODE_DTN_ROUTINGCGRMODEL_H_ */
