#include "App.h"

Define_Module (dtnsimdistinct::App);

namespace dtnsimdistinct {

void App::tokenizeString(const char *input, vector<string> &output) {

	cStringTokenizer tokenizer(input, ",");
	while (tokenizer.hasMoreTokens()) {
		output.push_back(tokenizer.nextToken());
	}
}

void App::tokenizeInt(const char *input, vector<int> &output) {

	cStringTokenizer tokenizer(input, ",");
	while (tokenizer.hasMoreTokens()) {
		output.push_back(atoi(tokenizer.nextToken()) * 3600);
	}
}

void App::initialize() {

	// Store this node's EID and region
	this->eid_ = this->getParentModule()->par("eid");
	this->region_ = this->getParentModule()->par("region").stringValue();

	// Configure Traffic Generator if it is enabled for this specific node
	if (par("enable")) {

		// get the destination for each flow
		tokenizeString(par("destinationEids"), destinationEidsVec_);

		// get the start time for each flow
		tokenizeInt(par("startTimes"), startsVec_);

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
			vector<pair<TrafficGenMsg*, int>> messages = flow.scheduleTrafficGeneratorMessages();

			for (int j = 0; j < messages.size(); ++j) {
				scheduleAt(messages.at(j).second, messages.at(j).first);
			}
		}
	}

	// Register signals
	appBundleSent_ = registerSignal("appBundleSent");
	appBundleReceived_ = registerSignal("appBundleReceived");
	appBundleReceivedDelay_ = registerSignal("appBundleReceivedDelay");
	appBundleReceivedHops_ = registerSignal("appBundleReceivedHops");
	appBundleReceivedFirstHop_ = registerSignal("appBundleReceivedFirstHop");
}

void App::handleMessage(cMessage *msg) {


	// generate bundle according to parameters in traffic generation message
	// generated bundle is relayed down to the DTN layer
	if (msg->getKind() == TRAFFIC_TIMER) {

		//cout << "APP of node " << eid_ << " is generating a bundle." << endl;

		TrafficGenMsg* trafficGenMsg = check_and_cast<TrafficGenMsg *>(msg);

		BundlePacket* bundle = new BundlePacket("bundle", BUNDLE);
		bundle->setSchedulingPriority(BUNDLE);

		// Bundle properties
		bundle->setBundleId(bundle->getId());

		string bundleNameStr = "Src:" + this->region_ + to_string(this->eid_)
				+ ",Dst:" + trafficGenMsg->getDestinationRegion() + to_string(trafficGenMsg->getDestinationEid())
				+ ",ID:" + to_string(bundle->getId());
		char bundleName[bundleNameStr.length()+1]; //TODO change
		strcpy(bundleName, bundleNameStr.c_str());
		bundle->setName(bundleName);

		bundle->setBitLength(trafficGenMsg->getSize() * 8);
		bundle->setByteLength(trafficGenMsg->getSize());

		bundle->setSourceEid(this->eid_);
		char source[this->region_.length() + 1];
		strcpy(source, this->region_.c_str());
		bundle->setSourceRegion(source);

		bundle->setDestinationEid(trafficGenMsg->getDestinationEid());
		bundle->setDestinationRegion(trafficGenMsg->getDestinationRegion());

		bundle->setCreationTimestamp(simTime());
		bundle->setTtl(trafficGenMsg->getTtl());
		bundle->setHopCount(0);
		bundle->setFirstHopEid(0);

		// send newly created bundle to node's DTN layer
		send(bundle, "gateToDtn$o");
		emit(appBundleSent_, true);
		delete msg;

	// accept bundle if it is meant for this node, else delete bundle
	} else if (msg->getKind() == BUNDLE) {

		BundlePacket* bundle = check_and_cast<BundlePacket *>(msg);

		if (this->eid_ == bundle->getDestinationEid() && this->region_.compare(bundle->getDestinationRegion()) == 0) {

			// this node is the intended receiver (final destination, bundle arrived at application layer)
			//cout << "APP of node " << eid_ << " successfully received bundle " << bundle->getName() << endl;

			emit(appBundleReceived_, true);
			emit(appBundleReceivedDelay_, simTime() - bundle->getCreationTimestamp());
			emit(appBundleReceivedHops_, bundle->getHopCount());
			emit(appBundleReceivedFirstHop_, bundle->getFirstHopEid());
			delete msg;

		} else {
			cout << "Error: bundle " << bundle->getBundleId() << " received in wrong destination" << endl;
			delete msg;
		}

	}

}


App::App() {

}

App::~App() {

}


}
