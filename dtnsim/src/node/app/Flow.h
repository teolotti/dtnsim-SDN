#ifndef SRC_NODE_APP_FLOW_H_
#define SRC_NODE_APP_FLOW_H_

#include <stdio.h>
#include <string>
#include <omnetpp.h>

#include <src/dtnsim_m.h>
#include <src/node/MsgTypes.h>

using namespace omnetpp;
using namespace std;


class Flow {

public:
	Flow(int destinationEid, int start, string distribution, int size, int ttl);
	virtual ~Flow();
	
	vector<pair<TrafficGeneratorMsg*, int>> scheduleTrafficGeneratorMessages();

private:
	int destinationEid_;
	string distribution_;
	int startTime_;
	int bundleSize_;
	int ttl_;
};


#endif /* SRC_NODE_APP_FLOW_H_ */
