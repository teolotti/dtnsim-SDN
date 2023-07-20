#include "Flow.h"




Flow::Flow(int destinationEid, int start, string distribution, int size, int ttl) {

	this->destinationEid_ = destinationEid;
	this->distribution_ = distribution;
	this->startTime_ = start;
	this->bundleSize_ = size;
	this->ttl_ = ttl;

}

vector<pair<TrafficGeneratorMsg*, int>> Flow::scheduleTrafficGeneratorMessages() {

	vector<int> startTimes;
	int numberOfBundles = 0;

	if (distribution_.compare("dist0") == 0) {

		int interval = 10; // 10 seconds between each bundle
		numberOfBundles = 3600 / interval;

		startTimes.push_back(startTime_);
		for (int i = 1; i < numberOfBundles; ++i) {
			startTimes.push_back(startTimes.at(i-1) + interval);
		}

	} else if (distribution_.compare("dist1") == 0) {

		int interval = 60; // 1 minute between each bundle
		numberOfBundles = 3600 / interval;

		startTimes.push_back(startTime_);
		for (int i = 1; i < numberOfBundles; ++i) {
			startTimes.push_back(startTimes.at(i-1) + interval);
		}

	} else if (distribution_.compare("dist2") == 0) {

		int interval = 300; // 5 minutes between each bundle
		numberOfBundles = 3600 / interval;

		startTimes.push_back(startTime_);
		for (int i = 1; i < numberOfBundles; ++i) {
			startTimes.push_back(startTimes.at(i-1) + interval);
		}

	} else if (distribution_.compare("dist3") == 0) {

		int interval = 900; // 15 minutes between each bundle
		numberOfBundles = 3600 / interval;

		startTimes.push_back(startTime_);
		for (int i = 1; i < numberOfBundles; ++i) {
			startTimes.push_back(startTimes.at(i-1) + interval);
		}

	} else if (distribution_.compare("dist4") == 0) {

		int interval = 1800; // 30 minutes between each bundle
		numberOfBundles = 3600 / interval;

		startTimes.push_back(startTime_);
		for (int i = 1; i < numberOfBundles; ++i) {
			startTimes.push_back(startTimes.at(i-1) + interval);
		}

	} else {
		//some default
	}

	vector<pair<TrafficGeneratorMsg*, int>> trafficGenMessages;

	for (int i = 0; i < numberOfBundles; ++i) {

		TrafficGeneratorMsg *trafficGeneratorMsg = new TrafficGeneratorMsg("TrafficGeneratorMsg");

		trafficGeneratorMsg->setSchedulingPriority(TRAFFIC_TIMER);
		trafficGeneratorMsg->setKind(TRAFFIC_TIMER);
		trafficGeneratorMsg->setDestinationEid(destinationEid_);
		trafficGeneratorMsg->setBundlesNumber(i); // TODO flow number as well?
		trafficGeneratorMsg->setSize(bundleSize_);
		trafficGeneratorMsg->setTtl(ttl_);

		trafficGenMessages.push_back(make_pair(trafficGeneratorMsg, startTimes.at(i)));
	}

	return trafficGenMessages;

}


Flow::~Flow() {
}


