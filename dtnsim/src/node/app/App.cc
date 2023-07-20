#include "App.h"

Define_Module (App);

void App::tokenizeInt(const char *input, vector<int> &output) {

	cStringTokenizer tokenizer(input, ",");
	while (tokenizer.hasMoreTokens()) {
		output.push_back(atoi(tokenizer.nextToken()));
	}
}

void App::tokenizeIntTime(const char *input, vector<int> &output) {

	cStringTokenizer tokenizer(input, ",");
	while (tokenizer.hasMoreTokens()) {
		output.push_back(atoi(tokenizer.nextToken()) * 3600);
	}
}

void App::initialize() {

	// Store this node eid
	this->eid_ = this->getParentModule()->getIndex();

	// Configure Traffic Generator
	if (par("enable")) {


		// get the destination for each flow
		tokenizeInt(par("destinationEids"), destinationEidsVec_);

		// get the start time for each flow
		tokenizeIntTime(par("startTimes"), startsVec_);

		distribution_ = par("distribution").stringValue();

		bundleSize_ = par("bundleSize").intValue();

		bundleTtl_ = par("ttl").intValue();


		for (int i = 0; i < destinationEidsVec_.size(); ++i) {

			Flow flow = Flow(
					destinationEidsVec_.at(i),
					startsVec_.at(i),
					distribution_,
					bundleSize_,
					bundleTtl_);

			// scheduleAt() sends an internal message (node to itself)
			// we schedule the traffic generator messages according to some distribution
			vector<pair<TrafficGeneratorMsg*, int>> messages = flow.scheduleTrafficGeneratorMessages();

			for (int j = 0; j < messages.size(); ++j) {
				scheduleAt(messages.at(j).second, messages.at(j).first);
			}
		}
	}

	// Register signals
	appBundleSent = registerSignal("appBundleSent");
	appBundleReceived = registerSignal("appBundleReceived");
	appBundleReceivedHops = registerSignal("appBundleReceivedHops");
	appBundleReceivedDelay = registerSignal("appBundleReceivedDelay");
}

void App::handleMessage(cMessage *msg) {

	if (msg->getKind() == TRAFFIC_TIMER) {

		TrafficGeneratorMsg* trafficGenMsg = check_and_cast<TrafficGeneratorMsg *>(msg);
		BundlePkt* bundle = new BundlePkt("bundle", BUNDLE);
		bundle->setSchedulingPriority(BUNDLE);

		// Bundle properties

		string bundleNameStr = "Src:" + to_string(this->eid_)
				+ ",Dst:" + to_string(trafficGenMsg->getDestinationEid())
				+ ",ID:" + to_string(bundle->getId());
		char bundleName[bundleNameStr.length()+1];
		strcpy(bundleName, bundleNameStr.c_str());
		bundle->setName(bundleName);

		bundle->setBundleId(bundle->getId());
		bundle->setBitLength(trafficGenMsg->getSize() * 8);
		bundle->setByteLength(trafficGenMsg->getSize());

		// Bundle fields (set by source node)
		bundle->setSourceEid(this->eid_);

		// set local destination equal to final destination.
		// the routing algorithm will change this destination to a destination inside the local region
		// if necessary
		bundle->setDestinationEid(trafficGenMsg->getDestinationEid());

		bundle->setReturnToSender(false);
		bundle->setCritical(false);
		bundle->setCustodyTransferRequested(false);
		bundle->setTtl(trafficGenMsg->getTtl());
		bundle->setCreationTimestamp(simTime());
		bundle->setQos(2);
		bundle->setBundleIsCustodyReport(false);

		// Bundle meta-data (set by intermediate nodes)
		bundle->setHopCount(0);
		bundle->setNextHopEid(0);
		bundle->setSenderEid(0);
		bundle->setCustodianEid(this->eid_);
		bundle->getVisitedNodes().clear();
		CgrRoute emptyRoute;
		emptyRoute.nextHop = EMPTY_ROUTE;
		bundle->setCgrRoute(emptyRoute);

		send(bundle, "gateToDtn$o");
		emit(appBundleSent, true);
		delete msg;
		return;

	} else if (msg->getKind() == BUNDLE) {

		BundlePkt* bundle = check_and_cast<BundlePkt *>(msg);
		int destinationEid = bundle->getDestinationEid();

		if (this->eid_ == destinationEid) {
			emit(appBundleReceived, true);
			emit(appBundleReceivedHops, bundle->getHopCount());
			emit(appBundleReceivedDelay, simTime() - bundle->getCreationTimestamp());
			delete msg;
		} else {
			throw cException("Error: message received in wrong destination");
		}
	}
}

App::App() {

}

App::~App() {

}

