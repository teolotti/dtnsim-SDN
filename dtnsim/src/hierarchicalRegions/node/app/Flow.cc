#include "Flow.h"

namespace dtnsimhierarchical {


Flow::Flow(string destinationEid, string distribution,
		int numberOfBundles, int start, int size, int ttl) {

	this->destinationEid_ = stoi(destinationEid.substr(1, destinationEid.size()-1));
	this->destinationRegion_ = destinationEid.substr(0, 1);
	this->distribution_ = distribution;
	this->numberOfBundles_ = numberOfBundles;
	this->startTime_ = start;
	this->bundleSize_ = size;
	this->ttl_ = ttl;

}

// TODO also depends on node gate throughput... needs more work later, end time?
// interval in seconds

vector<pair<TrafficGenMsgHIRR*, int>> Flow::scheduleTrafficGeneratorMessages() {

	vector<int> startTimes;

	if (distribution_.compare("uniform1") == 0) {

		int interval = 1; // 1 second between each bundle

		startTimes.push_back(startTime_);
		for (int i = 1; i < numberOfBundles_; ++i) {
			startTimes.push_back(startTimes.at(i-1) + interval);
		}

	} else if (distribution_.compare("uniform2") == 0) {

		int interval = 5; // 5 seconds between each bundle

		startTimes.push_back(startTime_);
		for (int i = 1; i < numberOfBundles_; ++i) {
			startTimes.push_back(startTimes.at(i-1) + interval);
		}

	} else if (distribution_.compare("random") == 0 ) {

	} else if (distribution_.compare("bursty") == 0 ) {

	} else if (distribution_.compare("exponential") == 0 ) {

	} else if (distribution_.compare("immediate") == 0 ) {

		// send all bundles of this flow at the same time
		fill(startTimes.begin(), startTimes.end(), startTime_);

	} else {
		//some default
	}

	char destination[destinationRegion_.length() + 1];
	strcpy(destination, destinationRegion_.c_str());

	vector<pair<TrafficGenMsgHIRR*, int>> trafficGenMessages;

	for (int i = 0; i < numberOfBundles_; ++i) {

		TrafficGenMsgHIRR *trafficGenMsg = new TrafficGenMsgHIRR("trafficGenMsg");
		trafficGenMsg->setSchedulingPriority(TRAFFIC_TIMER);
		trafficGenMsg->setKind(TRAFFIC_TIMER);
		trafficGenMsg->setDestinationEid(destinationEid_);
		trafficGenMsg->setDestinationRegion(destination);
		trafficGenMsg->setBundleNumber(i); // TODO flow number as well?
		trafficGenMsg->setSize(bundleSize_);
		trafficGenMsg->setTtl(ttl_);

		trafficGenMessages.push_back(make_pair(trafficGenMsg, startTimes.at(i)));
	}

	return trafficGenMessages;

}


Flow::~Flow() {
}
}

