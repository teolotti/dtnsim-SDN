
#ifndef SRC_NODE_NET_ROUTING_H_
#define SRC_NODE_NET_ROUTING_H_

#include <dtn/ContactPlan.h>
#include <dtn/SdrModel.h>
#include <map>
#include <queue>
#include <limits>
#include <algorithm>
#include "dtnsim_m.h"

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

	// This is a pure virtual method (all routing must at least
	// implement this function)
	virtual void routeAndQueueBundle(BundlePkt *bundle, double simTime) = 0;
};

#endif /* SRC_NODE_NET_ROUTING_H_ */
