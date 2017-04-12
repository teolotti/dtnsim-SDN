/*
 * RoutingDirect.h
 *
 *  Created on: Nov 8, 2016
 *      Author: juanfraire
 */

#ifndef SRC_NODE_NET_ROUTINGDIRECT_H_
#define SRC_NODE_NET_ROUTINGDIRECT_H_

#include "Routing.h"
#include "SdrModel.h"

class RoutingDirect : public Routing
{
public:
	RoutingDirect(int eid, SdrModel * sdr, ContactPlan * contactPlan);
	virtual ~RoutingDirect();
	virtual void routeAndQueueBundle(BundlePkt *bundle, double simTime);
private:
	int eid_;
	SdrModel * sdr_;
	ContactPlan * contactPlan_;
};

#endif /* SRC_NODE_NET_ROUTINGDIRECT_H_ */
