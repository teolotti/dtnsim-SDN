#include "App.h"

Define_Module (dtnsimhierarchical::App);

namespace dtnsimhierarchical {

void App::tokenizeString(const char *input, vector<string> &output) {

	cStringTokenizer tokenizer(input, ",");
	while (tokenizer.hasMoreTokens()) {
		output.push_back(tokenizer.nextToken());
	}
}

void App::tokenizeInt(const char *input, vector<int> &output) {

	cStringTokenizer tokenizer(input, ",");
	while (tokenizer.hasMoreTokens()) {
		output.push_back(atoi(tokenizer.nextToken()));
	}
}

void App::initialize() {

	// Store this node's EID and region
	this->eid_ = this->getParentModule()->par("eid");
	this->homeRegion_ = this->getParentModule()->par("homeRegion").stringValue();
	this->outerRegion_ = this->getParentModule()->par("outerRegion").stringValue();

	// Configure Traffic Generator if it is enabled for this specific node
	if (par("enable")) {

		// get the number of flows starting from this node
		int numberOfFlows = par("numberOfFlows");

		// get the destination for each flow
		tokenizeString(par("destinationEids"), destinationEidsVec_);

		// get the distribution for each flow
		tokenizeString(par("distributions"), distributionsVec_);

		// get the number of bundles per flow
		tokenizeInt(par("numbersOfBundles"), numbersVec_);

		// get the start time for each flow
		tokenizeInt(par("startTimes"), startsVec_);

		// get the size of every bundle for each flow
		tokenizeInt(par("bundleSizes"), sizesVec_);

		// get the TTL of every bundle for each flow
		tokenizeInt(par("ttls"), ttlsVec_);


		// TODO check if vectors all have same size


		for (int i = 0; i < numberOfFlows; ++i) {

			Flow flow = Flow(
					destinationEidsVec_.at(i),
					distributionsVec_.at(i),
					numbersVec_.at(i),
					startsVec_.at(i),
					sizesVec_.at(i),
					ttlsVec_.at(i));

			// scheduleAt() sends an internal message (node to itself)
			// we schedule the traffic generator messages according to some distribution
			vector<pair<TrafficGenMsgHIRR*, int>> messages = flow.scheduleTrafficGeneratorMessages();

			for (int j = 0; j < numbersVec_.at(i); ++j) {
				scheduleAt(messages.at(j).second, messages.at(j).first);
			}
		}
	}

	// Register signals
	appBundleSent_ = registerSignal("appBundleSent");
	appBundleReceived_ = registerSignal("appBundleReceived");
	appBundleReceivedDelay_ = registerSignal("appBundleReceivedDelay");
}

void App::handleMessage(cMessage *msg) {

	// TODO

	// generate bundle according to parameters in traffic generation message
	// generated bundle is relayed down to the DTN layer
	if (msg->getKind() == TRAFFIC_TIMER) {

		cout << "APP of node " << eid_ << " is generating a bundle." << endl;

		TrafficGenMsgHIRR* trafficGenMsg = check_and_cast<TrafficGenMsgHIRR *>(msg);

		BundlePacketHIRR* bundle = new BundlePacketHIRR("bundle", BUNDLE);
		bundle->setSchedulingPriority(BUNDLE);

		// Bundle properties
		bundle->setBundleId(bundle->getId());

		string bundleNameStr = "Src:" + to_string(this->eid_)
				+ ",Dst:" + trafficGenMsg->getDestinationRegion() + to_string(trafficGenMsg->getDestinationEid())
				+ ",ID:" + to_string(bundle->getId());
		char bundleName[bundleNameStr.length()+1]; //TODO change
		strcpy(bundleName, bundleNameStr.c_str());
		bundle->setName(bundleName);

		bundle->setBitLength(trafficGenMsg->getSize() * 8);
		bundle->setByteLength(trafficGenMsg->getSize());

		bundle->setSourceEid(this->eid_);

		bundle->setDestinationEid(trafficGenMsg->getDestinationEid());

		bundle->setCreationTimestamp(simTime());
		bundle->setTtl(trafficGenMsg->getTtl());

		bundle->setType(0);
		bundle->setPriorPassageway(-1);


		// send newly created bundle to node's DTN layer
		send(bundle, "gateToDtn$o");
		emit(appBundleSent_, true);
		delete msg;

	// accept bundle if it is meant for this node, else delete bundle
	} else if (msg->getKind() == BUNDLE) {

		BundlePacketHIRR* bundle = check_and_cast<BundlePacketHIRR *>(msg);

		if (this->eid_ == bundle->getDestinationEid()) {

			// this node is the intended receiver (final destination, bundle arrived at application layer)
			cout << "APP of node " << eid_ << " successfully received bundle " << bundle->getName() << endl;

			emit(appBundleReceived_, true);
			emit(appBundleReceivedDelay_, simTime() - bundle->getCreationTimestamp());
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
