#include <src/hierarchicalRegions/node/dtn/sdr/SdrModel.h>

namespace dtnsimhierarchical {

SdrModel::SdrModel(int eid, int totalCapacity) {
	this->eid_ = eid;
	this->totalCapacity_ = totalCapacity;
}

SdrModel::~SdrModel() {
}



void SdrModel::freeSdr() {

	this->perNodeBundleQueue_.clear();
	notify();
}


int SdrModel::getBundlesCountInSdr() {

	int totalBundleCount = 0;

	for (auto const	&queue : perNodeBundleQueue_) {
		totalBundleCount += queue.second.size();

	}

	return totalBundleCount;
}


int SdrModel::getBytesStoredInSdr() {

	int totalBytesCount = 0;

	for (auto const	&queue : perNodeBundleQueue_) {
		std::queue<BundlePacketHIRR*> queueCopy = queue.second;
		while(!queueCopy.empty()) {
			totalBytesCount += queueCopy.front()->getByteLength();
			queueCopy.pop();
		}
	}

	return totalBytesCount;
}

int SdrModel::getBundlesCountInQueue(int eid) {

	return perNodeBundleQueue_.at(eid).size();
}

int SdrModel::getBytesStoredInQueue(int eid) {

	int bytesCount = 0;

	std::queue<BundlePacketHIRR*> queueCopy = perNodeBundleQueue_.at(eid);
	while(!queueCopy.empty()) {
		bytesCount += queueCopy.front()->getByteLength();
		queueCopy.pop();
	}

	return bytesCount;
}

bool SdrModel::isBundleWaiting(int eid) {

	if (perNodeBundleQueue_.count(eid) > 0) {
		if (perNodeBundleQueue_.at(eid).size() > 0) {
			//cout << "dst EID " << eid << " has bundles waiting" << endl;
			return true;
		}
	}

	return false;
}

BundlePacketHIRR* SdrModel::getBundleWaiting(int eid) {

	return perNodeBundleQueue_.at(eid).front();
}


bool SdrModel::enqueueBundle(int eid, BundlePacketHIRR *bundle) {

	int totalBytesCount = this->getBytesStoredInSdr();
	int bundleSize = bundle->getByteLength();

	if (totalBytesCount + bundleSize > totalCapacity_) {
		cout << "Bundle with id " + to_string(bundle->getBundleId())
				+ " is dropped since there is no more space in the queue." << endl;
		delete bundle;
		return false;

	} else {

		if (perNodeBundleQueue_.count(eid) <= 0) {
			queue<BundlePacketHIRR*> placeholder;
			perNodeBundleQueue_.insert(make_pair(eid, placeholder));
		}
		perNodeBundleQueue_.at(eid).push(bundle);
		notify();
		return true;
	}
}

void SdrModel::dequeueBundle(int eid) {

	//cout << "DEQUEUING" << endl;

	perNodeBundleQueue_.at(eid).pop();
	notify();
}


}
