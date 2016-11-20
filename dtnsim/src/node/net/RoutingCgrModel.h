/*
 * RoutingCgr.h
 *
 *  Created on: Nov 8, 2016
 *      Author: juanfraire
 */

#ifndef SRC_NODE_NET_ROUTINGCGRMODEL_H_
#define SRC_NODE_NET_ROUTINGCGRMODEL_H_

#include "Routing.h"

class RoutingCgrModel: public Routing
{
public:
	RoutingCgrModel();
	virtual ~RoutingCgrModel();
	virtual void setLocalNode(int eid);
	virtual void setQueue(map<int, queue<Bundle *> > * bundlesQueue);
	virtual void setContactPlan(ContactPlan * contactPlan);
	virtual void routeBundle(Bundle *bundle, double simTime);
private:
	int eid_;
	map<int, queue<Bundle *> > * bundlesQueue_;
	ContactPlan * contactPlan_;

	/////////////////////////////////////////////////
	// Ion Cgr Functions based in libcgr.c (v 3.5.0):
	/////////////////////////////////////////////////
#define	MIN_CONFIDENCE_IMPROVEMENT	(.05)
#define MIN_NET_DELIVERY_CONFIDENCE	(.80)
#define MAX_XMIT_COPIES (20)

	typedef struct
	{
		int neighborNodeNbr;
		int contactId; // This is not, in ION
		double forfeitTime;
		double arrivalTime;
		float arrivalConfidence;
		unsigned int hopCount; // hops from dest. node.
		//Scalar	overbooked; 	//Bytes needing reforward.
		//Scalar	protected; 		//Bytes not overbooked.
	} ProximateNode;

	typedef struct
	{
		int toNodeNbr; 	//Initial-hop neighbor.
		double fromTime; 	// init tx time
		double toTime;	 	// Time at which route shuts down: earliest contact end time among all
		float arrivalConfidence;
		double arrivalTime;
		double maxCapacity; // in Bytes (?)
		vector<Contact *> hops; // list: IonCXref addr
	} CgrRoute;

	typedef struct
	{
		Contact * contact;
		int predecessor;	// contactId of predecessor
		double arrivalTime;
		double capacity; 	// in Bytes (?)
		bool visited;
		bool suppressed;
	} Work;

	map<int, vector<CgrRoute> > routeList_;
	double routeListLastEditTime = -1;

	void cgrForward(Bundle * bundle, double simTime);
	void identifyProximateNodes(Bundle * bundle, double simTime, vector<int> excludedNodes, vector<ProximateNode> * proximateNodes);
	void loadRouteList(int terminusNode, double simTime);

	void findNextBestRoute(Contact * rootContact, int terminusNode, CgrRoute * route);
	void tryRoute(Bundle * bundle, CgrRoute * route, vector<ProximateNode> * proximateNodes);
	void recomputeRouteForContact();
	void enqueueToNeighbor(Bundle * bundle, ProximateNode * selectedNeighbor);
	void enqueueToLimbo(Bundle * bundle);
	void bpEnqueue(Bundle * bundle, ProximateNode * selectedNeighbor);
};

#endif /* SRC_NODE_NET_ROUTINGCGRMODEL_H_ */
