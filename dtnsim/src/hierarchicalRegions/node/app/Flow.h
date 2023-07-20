#ifndef SRC_HIERARCHICALREGIONS_NODE_APP_FLOW_H_
#define SRC_HIERARCHICALREGIONS_NODE_APP_FLOW_H_

#include <stdio.h>
#include <string>
#include <omnetpp.h>

#include <src/hierarchicalRegions/HierarchicalRegionsNetwork_m.h>
#include <src/hierarchicalRegions/node/MsgTypes.h>

using namespace omnetpp;
using namespace std;

namespace dtnsimhierarchical {
class Flow {

public:
	Flow(string destinationEid, string distribution, int numberOfBundles, int start, int size, int ttl);
	virtual ~Flow();
	
	vector<pair<TrafficGenMsgHIRR*, int>> scheduleTrafficGeneratorMessages();

private:
	int destinationEid_;
	string destinationRegion_;
	string distribution_;
	int numberOfBundles_;
	int startTime_;
	int bundleSize_;
	int ttl_;
};

}
#endif /* SRC_HIERARCHICALREGIONS_NODE_APP_FLOW_H_ */
