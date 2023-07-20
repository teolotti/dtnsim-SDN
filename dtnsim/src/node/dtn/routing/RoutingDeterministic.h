/*
 * RoutingDeterministic.h
 *
 *  Created on: Jul 18, 2017
 *      Author: FRaverta
 *
 * According to Zhang's taxonomy which classifies routing protocols based on the availability
 * of knowledge about the network topology, We have two different kind of them:
 *
 * 	1) Deterministic:
 *	 					In this class, the schemes assume that information
 *						about the network topology is known a priori. The sub-categories in
 *						this class include tree-based, space and time based, and
 *						modification-based schemes.
 *
 * 	2) Stochastic:
 * 						In this class, the schemes assume that no prior information about the
 * 						node movement or network topology is available and thus, path selections
 * 						are based simply on heuristics, e.g., forwarding to neighbor nodes
 * 						(like epidemic routing), or based on the mobility pattern of nodes, or
 * 						link forwarding probability based.
 *
 * This class provides a implementation by default for inherited Routing's methods that result
 * suitable for the major deterministic schemes implemented here. All routing methods which inherited from this
 * class must implement routeAndQueueBundle method since routing policies are routing scheme dependents.
 */

#ifndef SRC_NODE_DTN_ROUTING_ROUTINGDETERMINISTIC_H_
#define SRC_NODE_DTN_ROUTING_ROUTINGDETERMINISTIC_H_

#include <src/node/dtn/routing/Routing.h>
#include <src/node/dtn/routing/RoutingInterRegions.h>

class RoutingDeterministic: public Routing
{
public:
	RoutingDeterministic(int eid, SdrModel * sdr, map<string, ContactPlan> *contactPlans, RoutingInterRegions *routingInterRegions);
	RoutingDeterministic(int eid, SdrModel * sdr, ContactPlan *contactPlan, RoutingInterRegions *routingInterRegions);

	virtual ~RoutingDeterministic();

	virtual void msgToOtherArrive(BundlePkt * bundle, double simTime, int terminusNode);

	virtual bool msgToMeArrive(BundlePkt * bundle);

	virtual void contactStart(Contact *c);

	virtual void contactEnd(Contact *c);

	virtual void successfulBundleForwarded(long bundleId, Contact * contact,  bool sentToDestination);

	virtual void  refreshForwarding(Contact * c);

	// This is a pure virtual method (all deterministic routing must at least
	// implement this function)
	virtual void routeAndQueueBundle(BundlePkt *bundle, double simTime, int terminusNode) = 0;

	virtual bool haveElementsInCommon(set<string> la, set<string> lb);

	virtual void setCurrentRegionId(string currentRegionId)
	{
		this->currentRegionId_ = currentRegionId;
		this->contactPlan_ = &(contactPlans_->operator[](currentRegionId));
	}

	virtual string getCurrentRegionId()
	{
		return this->currentRegionId_;
	}

	virtual ContactPlan *getContactPlan()
	{
		return contactPlan_;
	}

protected:

	// we use this variable so that CGR consider only contacts from a specific region
	string currentRegionId_;

	// this contact plan pointer is changed with the method setCurrentRegion of routing
	ContactPlan *contactPlan_;

	map<string, ContactPlan> *contactPlans_;

	RoutingInterRegions *routingInterRegions_;
};

#endif /* SRC_NODE_DTN_ROUTING_ROUTINGDETERMINISTIC_H_ */
