/*
 * RoutingCgrModelRev17.h
 *
 *  Created on: Apr 12, 2017
 *      Author: juanfraire
 */

#ifndef SRC_NODE_NET_ROUTING_ROUTINGCGRMODELREV17_H_
#define SRC_NODE_NET_ROUTING_ROUTINGCGRMODELREV17_H_

#include "Routing.h"

class RoutingCgrModelRev17: public Routing
{
public:
	RoutingCgrModelRev17(int eid, SdrModel * sdr, ContactPlan * contactPlan);
	virtual ~RoutingCgrModelRev17();
	virtual void routeAndQueueBundle(BundlePkt *bundle, double simTime);

private:
	int eid_;
	SdrModel * sdr_;
	ContactPlan * contactPlan_;
};

#endif /* SRC_NODE_NET_ROUTING_ROUTINGCGRMODELREV17_H_ */
