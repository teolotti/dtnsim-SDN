
#ifndef SRC_NODE_DTN_ROUTINGIRUCOPN2_H_
#define SRC_NODE_DTN_ROUTINGIRUCOPN2_H_

#include <dtn/routing/RoutingDeterministic.h>
#include <dtn/SdrModel.h>
#include "json.hpp"
#include <dtn/Dtn.h>

using json = nlohmann::json;

class RoutingDecision{
	public:
		int copies_;
		vector<int> contact_ids_;

		RoutingDecision(){}
		~RoutingDecision(){}
};

class RoutingIRUCoPn2 : public RoutingDeterministic
{

public:
	RoutingIRUCoPn2(int eid, SdrModel * sdr, ContactPlan * contactPlan,  cModule * dtn, int max_copies, double probability_of_failure, int ts_duration, int numOfNodes, string pathPrefix, string pathPosfix);
	virtual ~RoutingIRUCoPn2();

	virtual void msgToOtherArrive(BundlePkt * bundle, double simTime);
	virtual bool msgToMeArrive(BundlePkt * bundle);
	virtual void contactStart(Contact *c);
	virtual void contactEnd(Contact *c);
	virtual void routeAndQueueBundle(BundlePkt *bundle, double simTime);
	virtual bool isDeliveredBundle(long bundleId);
	virtual void successfulBundleForwarded(long bundleId, Contact * contact,  bool sentToDestination);

private:
	json bruf_function;
	double contact_failure_probability;
	int ts_duration;
	int max_copies;
	map<long, list<RoutingDecision>> bag; //BundleId -> Contacts to send each copy
	list<int> currentContacts;
	list<int> finishedContacts;

	// The bundles this node has received as the final recipient or sent to final destination
	list<long> deliveredBundles_;
	cModule * dtn_;

	virtual void queueBundles();
	virtual void removeBundleFromBag(long bundleId);
	virtual void reportErrorAndExit(string method, string msg);
	virtual void routeBundle(BundlePkt * bundle, int copies, vector<int> route);

};

#endif /* SRC_NODE_DTN_ROUTINGIRUCOPN_H_ */
