#ifndef SRC_DISTINCTREGIONS_NODE_APP_FLOW_H_
#define SRC_DISTINCTREGIONS_NODE_APP_FLOW_H_

#include <stdio.h>
#include <string>
#include <omnetpp.h>
#include <random>

#include <src/distinctRegions/RegionsNetwork_m.h>
#include <src/distinctRegions/node/MsgTypes.h>

using namespace omnetpp;
using namespace std;

namespace dtnsimdistinct {
class Flow {

public:
	Flow(string destinationEid, int start, string distribution, int size, int ttl);
	virtual ~Flow();
	
	vector<pair<TrafficGenMsg*, int>> scheduleTrafficGeneratorMessages();

private:
	int destinationEid_;
	string destinationRegion_;
	string distribution_;
	int startTime_;
	int bundleSize_;
	int ttl_;
};

}
#endif /* SRC_DISTINCTREGIONS_NODE_APP_FLOW_H_ */
