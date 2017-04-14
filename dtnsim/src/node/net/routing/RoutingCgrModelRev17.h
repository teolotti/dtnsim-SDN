/*
 * RoutingCgrModelRev17.h
 *
 *  Created on: Apr 12, 2017
 *      Author: juanfraire
 */

#ifndef SRC_NODE_NET_ROUTING_ROUTINGCGRMODELREV17_H_
#define SRC_NODE_NET_ROUTING_ROUTINGCGRMODELREV17_H_

#include "Routing.h"

#define	MAX_SPEED_MPH	(150000)
#define NO_ROUTE_FOUND (-1)
#define EMPTY_ROUTE (-2)

class RoutingCgrModelRev17: public Routing
{
public:
	RoutingCgrModelRev17(int eid, int nodeNum, SdrModel * sdr, ContactPlan * contactPlan, bool printDebug);
	virtual ~RoutingCgrModelRev17();
	virtual void routeAndQueueBundle(BundlePkt *bundle, double simTime);

	bool printDebug_ = true;

private:

	int eid_;
	int nodeNum_;
	SdrModel * sdr_;
	ContactPlan * contactPlan_;

	typedef struct
	{
		int nextHop; 				// Entry node
		double fromTime; 			// Init time
		double toTime;	 			// Due time (earliest contact end time among all)
		float confidence;
		double arrivalTime;
		double maxVolume; 			// In Bytes
		double residualVolume;		// In Bytes
		vector<Contact *> hops;	 	// Contact list
	} CgrRoute;

	// Route Table: one table per destination, one entry per neighbour node
	vector<vector<CgrRoute>> routeTable_;
	double routeTableLastEditTime = -1;

	typedef struct
	{
		Contact * contact;			// The owner contact of this Work
		Contact * predecessor;		// Predecessor Contact
		vector<int> visitedNodes;	// Dijkstra exploration: list of visited nodes
		double arrivalTime;			// Dijkstra exploration: best arrival time so far
		bool visited;				// Dijkstra exploration: visited
		bool suppressed;			// Dijkstra exploration: suppressed
	} Work;

	void cgrForward(BundlePkt * bundle, double simTime);
	void findNextBestRoute(int entryNode, int terminusNode, CgrRoute * route, double simTime);

	void clearRouteTable();
	void printRouteTable(int terminusNode);
	static bool compareRoutes(CgrRoute i, CgrRoute j);
	void printContactPlan();
};

#endif /* SRC_NODE_NET_ROUTING_ROUTINGCGRMODELREV17_H_ */
