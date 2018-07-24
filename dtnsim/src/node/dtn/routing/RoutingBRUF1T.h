
#ifndef SRC_NODE_DTN_ROUTINGBRUF1T_H_
#define SRC_NODE_DTN_ROUTINGBRUF1T_H_

#include <dtn/routing/RoutingDeterministic.h>
#include <dtn/SdrModel.h>


class RoutingBRUF1T: public RoutingDeterministic
{
public:
	RoutingBRUF1T(int eid, SdrModel * sdr, ContactPlan * contactPlan, string path_to_routing);
	virtual ~RoutingBRUF1T();
	virtual void routeAndQueueBundle(BundlePkt *bundle, double simTime);

private:
	//<target_node_id, time up to the routing decision is valid, outgoing contact_id>
	std::map<int, std::map<int,int>> routing_decisions_;

};

#endif /* SRC_NODE_DTN_ROUTINGDIRECT_H_ */
