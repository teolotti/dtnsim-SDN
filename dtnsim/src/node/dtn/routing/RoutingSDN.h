/*
 * RoutingSDN.h
 *
 *  Created on: Nov 9, 2023
 *      Author: matteo
 */

#ifndef SRC_NODE_DTN_ROUTING_ROUTINGSDN_ROUTINGSDN_H_
#define SRC_NODE_DTN_ROUTING_ROUTINGSDN_ROUTINGSDN_H_

#include <src/node/dtn/routing/Routing.h>
#include <src/node/dtn/routing/RoutingDeterministic.h>
#include <src/node/dtn/routing/routingSDN/Controller.h>

class Controller;


class RoutingSDN: public RoutingDeterministic
{
private:
	static Controller* controllerPtr;

	int nodeNum_;

	string routingType_;
public:
	RoutingSDN(int eid, SdrModel * sdr, ContactPlan * contactPlan, int nodeNum, string routingType);
	virtual ~RoutingSDN();

	void routeAndQueueBundle(BundlePkt *bundle, double simTime);
};

#endif /* SRC_NODE_DTN_ROUTING_ROUTINGSDN_ROUTINGSDN_H_ */
