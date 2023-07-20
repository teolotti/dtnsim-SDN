#include <src/distinctRegions/node/dtn/sdr/SdrModel.h>

namespace dtnsimdistinct {

SdrModel::SdrModel(int eid, int totalCapacity) {
	this->eid_ = eid;
	this->totalCapacity_ = totalCapacity;
}

SdrModel::~SdrModel() {
}

void SdrModel::freeSdr() {

	// TODO delete bundle references!
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
		std::queue<pair<BundlePacket*, vector<Contact*>>> queueCopy = queue.second;
		while(!queueCopy.empty()) {
			totalBytesCount += queueCopy.front().first->getByteLength();
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

	if (perNodeBundleQueue_.count(eid) <= 0) {
		return bytesCount;
	}
	std::queue<pair<BundlePacket*, vector<Contact*>>> queueCopy = perNodeBundleQueue_.at(eid);
	while(!queueCopy.empty()) {
		bytesCount += queueCopy.front().first->getByteLength();
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

BundlePacket* SdrModel::getBundleWaiting(int eid) {

	return perNodeBundleQueue_.at(eid).front().first;
}


bool SdrModel::enqueueBundle(int eid, BundlePacket *bundle, vector<Contact*> hops) {

	int totalBytesCount = this->getBytesStoredInSdr();
	int bundleSize = bundle->getByteLength();

	/*
	if (totalBytesCount + bundleSize > totalCapacity_) {
		cout << "Bundle with id " + to_string(bundle->getBundleId())
				+ " is dropped since there is no more space in the queue." << endl;
		delete bundle;
		return false;

	} else {

		if (perNodeBundleQueue_.count(eid) <= 0) {
			queue<BundlePacket*> placeholder;
			perNodeBundleQueue_.insert(make_pair(eid, placeholder));
		}
		perNodeBundleQueue_.at(eid).push(bundle);
		notify();
		return true;
	}
	*/

	if (perNodeBundleQueue_.count(eid) <= 0) {
		queue<pair<BundlePacket*, vector<Contact*>>> placeholder;
		perNodeBundleQueue_.insert(make_pair(eid, placeholder));
	}
	perNodeBundleQueue_.at(eid).push(make_pair(bundle, hops));
	notify();
	return true;
}

void SdrModel::dequeueBundle(int eid) {

	//cout << "DEQUEUING" << endl;

	BundlePacket* bundle = perNodeBundleQueue_.at(eid).front().first;
	vector<Contact*> hops = perNodeBundleQueue_.at(eid).front().second;
	for (auto & hop : hops) {
		hop->setRemainingCapacity(hop->getRemainingCapacity() + bundle->getByteLength());
	}
	perNodeBundleQueue_.at(eid).pop();
	notify();
}


}
