
#ifndef SRC_NODE_DTN_ROUTINGDIRECT_H_
#define SRC_NODE_DTN_ROUTINGDIRECT_H_

#include <dtn/routing/Routing.h>
#include <dtn/SdrModel.h>

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

#endif /* SRC_NODE_DTN_ROUTINGDIRECT_H_ */
