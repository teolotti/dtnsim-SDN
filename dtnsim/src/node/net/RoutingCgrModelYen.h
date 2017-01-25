/*
 * RoutingCgrModelYen.h
 *
 *  Created on: Jan 24, 2017
 *      Author: juanfraire
 */

#ifndef SRC_NODE_NET_ROUTINGCGRMODELYEN_H_
#define SRC_NODE_NET_ROUTINGCGRMODELYEN_H_

#include "Routing.h"
#include "SdrModel.h"

class RoutingCgrModelYen: public Routing
{
public:
	RoutingCgrModelYen();
	virtual ~RoutingCgrModelYen();
	virtual void setLocalNode(int eid);
	virtual void setSdr(SdrModel * sdr);
	virtual void setContactPlan(ContactPlan * contactPlan);
	virtual void routeBundle(Bundle *bundle, double simTime);
private:
	bool printDebug = true;
	int eid_;
	SdrModel * sdr_;
	ContactPlan * contactPlan_;

	/////////////////////////////////////////////////
	// Ion Cgr Functions based in libcgr.c (v 3.5.0):
	/////////////////////////////////////////////////
#define	MIN_CONFIDENCE_IMPROVEMENT	(.05)
#define MIN_NET_DELIVERY_CONFIDENCE	(.80)
#define MAX_XMIT_COPIES (20)
#define	MAX_SPEED_MPH	(150000)

	typedef struct
	{
		int toNodeNbr; 	//Initial-hop neighbor.
		double fromTime; 	// init tx time
		double toTime;	 	// Time at which route shuts down: earliest contact end time among all
		float arrivalConfidence;
		double arrivalTime;
		double maxCapacity; // in Bits
		vector<Contact *> hops; // list: IonCXref addr
	} CgrRoute;

	typedef struct
	{
		int neighborNodeNbr;
		int contactId; // This is not, in ION
		double forfeitTime;
		double arrivalTime;
		float arrivalConfidence;
		unsigned int hopCount; // hops from dest. node.
		CgrRoute * route; // pointer to route so we can decrement capacities
		//Scalar	overbooked; 	//Bytes needing reforward.
		//Scalar	protected; 		//Bytes not overbooked.
	} ProximateNode;

	typedef struct
	{
		Contact * contact;
		Contact * predecessor;	// predecessor Contact
		double arrivalTime;
		double capacity; 	// in Bytes (?)
		bool visited;
		bool suppressed;
		vector<Contact *> suppressedNextContact;
	} Work;

	map<int, vector<CgrRoute> > routeList_;
	double routeListLastEditTime = -1;

	void cgrForward(Bundle * bundle, double simTime);
	void identifyProximateNodes(Bundle * bundle, double simTime, vector<int> excludedNodes, vector<ProximateNode> * proximateNodes);
	void loadRouteList(int terminusNode, double simTime);
	void loadRouteListYen(int terminusNode, double simTime);

	void findNextBestRoute(Contact * rootContact, int terminusNode, CgrRoute * route);
	void tryRoute(Bundle * bundle, CgrRoute * route, vector<ProximateNode> * proximateNodes);
	void recomputeRouteForContact();
	void enqueueToNeighbor(Bundle * bundle, ProximateNode * selectedNeighbor);
	void enqueueToLimbo(Bundle * bundle);
	void bpEnqueue(Bundle * bundle, ProximateNode * selectedNeighbor);
};

#endif /* SRC_NODE_NET_ROUTINGCGRMODELYEN_H_ */
