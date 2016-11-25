/*
 * RoutingCgrIon350.h
 *
 *  Created on: Nov 8, 2016
 *      Author: juanfraire
 */

#ifndef SRC_NODE_NET_ROUTINGCGRION350_H_
#define SRC_NODE_NET_ROUTINGCGRION350_H_

#include "Routing.h"
#include "SdrModel.h"

class RoutingCgrIon350 : public Routing
{
public:
	RoutingCgrIon350();
	virtual ~RoutingCgrIon350();
	virtual void setLocalNode(int eid);
	virtual void setSdr(SdrModel * sdr);
	virtual void setContactPlan(ContactPlan * contactPlan);
	virtual void routeBundle(Bundle *bundle, double simTime);
private:
	int eid_;
	SdrModel * sdr_;
	ContactPlan * contactPlan_;
};


#endif /* SRC_NODE_NET_ROUTINGCGRION350_H_ */
