/*
 * RoutingDirect.h
 *
 *  Created on: Nov 8, 2016
 *      Author: juanfraire
 */

#ifndef SRC_NODE_NET_ROUTINGDIRECT_H_
#define SRC_NODE_NET_ROUTINGDIRECT_H_

#include "Routing.h"

class RoutingDirect : public Routing
{
public:
	RoutingDirect();
	virtual ~RoutingDirect();
	virtual void setLocalNode(int eid);
	virtual void setQueue(map<int, queue<Bundle *> > * bundlesQueue);
	virtual void setContactPlan(ContactPlan * contactPlan);
	virtual void routeBundle(Bundle *bundle, double simTime);
private:
	int eid_;
	map<int, queue<Bundle *> > * bundlesQueue_;
	ContactPlan * contactPlan_;
};

#endif /* SRC_NODE_NET_ROUTINGDIRECT_H_ */
