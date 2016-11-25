/*
 * Routing.h
 *
 *  Created on: Nov 8, 2016
 *      Author: juanfraire
 */

#ifndef SRC_NODE_NET_ROUTING_H_
#define SRC_NODE_NET_ROUTING_H_

#include <map>
#include <queue>
#include <limits>
#include <algorithm>
#include "ContactPlan.h"
#include "dtnsim_m.h"
#include "SdrModel.h"

using namespace omnetpp;
using namespace std;

class Routing
{
public:
	Routing()
	{
	}
	virtual ~Routing()
	{
	}
	// These are pure virtual methods
	virtual void setLocalNode(int eid) = 0;
	virtual void setSdr(SdrModel * sdr) = 0;
	virtual void setContactPlan(ContactPlan * contactPlan) = 0;
	virtual void routeBundle(Bundle *bundle, double simTime) = 0;
};



#endif /* SRC_NODE_NET_ROUTING_H_ */
