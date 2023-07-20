#include "Flow.h"

namespace dtnsimdistinct {


Flow::Flow(string destinationEid, int start, string distribution, int size, int ttl) {

	this->destinationEid_ = stoi(destinationEid.substr(1, destinationEid.size()-1));
	this->destinationRegion_ = destinationEid.substr(0, 1);
	this->distribution_ = distribution;
	this->startTime_ = start;
	this->bundleSize_ = size;
	this->ttl_ = ttl;

}

vector<pair<TrafficGenMsg*, int>> Flow::scheduleTrafficGeneratorMessages() {

	vector<double> startTimes;
	int numberOfBundles = 0;
	double endTime = 3600.0 + startTime_;

	if (distribution_.compare("uniform") == 0) {

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


	} else if (distribution_.compare("exp") == 0) {

		std::exponential_distribution<double> dist(1.0);
		std::default_random_engine generator;

		numberOfBundles = 3600 / 10;
		//startTimes.push_back(startTime_);

		for (int i = 0; i < numberOfBundles; ++i) {
			double sendTime;
			do {
				sendTime = startTime_ + dist(generator);
			} while (sendTime > endTime);

			startTimes.push_back(sendTime);
		}

	} else if (distribution_.compare("invexp") == 0) {

		std::gamma_distribution<double> dist(2, 5.0);
		std::default_random_engine generator;

		numberOfBundles = 3600 / 10;
		//startTimes.push_back(startTime_);

		for (int i = 0; i < numberOfBundles; ++i) {
			double sendTime;
			do {
				sendTime = startTime_ + dist(generator);
			} while (sendTime > endTime);

			startTimes.push_back(sendTime);
		}

	} else if (distribution_.compare("normal") == 0) {

		std::normal_distribution<double> dist(1800.0, 600.0);
		std::default_random_engine generator;

		numberOfBundles = 3600 / 10;
		//startTimes.push_back(startTime_);

		for (int i = 0; i < numberOfBundles; ++i) {
			double sendTime;
			do {
				sendTime = max((double)startTime_, startTime_ + dist(generator)-1800.0);
			} while (sendTime > endTime);

			startTimes.push_back(sendTime);
		}

	} else {
		//some default
	}

	char destination[destinationRegion_.length() + 1];
	strcpy(destination, destinationRegion_.c_str());

	vector<pair<TrafficGenMsg*, int>> trafficGenMessages;

	for (int i = 0; i < numberOfBundles; ++i) {

		TrafficGenMsg *trafficGenMsg = new TrafficGenMsg("trafficGenMsg");
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

