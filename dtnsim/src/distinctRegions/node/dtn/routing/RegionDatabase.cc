#include <src/distinctRegions/node/dtn/routing/RegionDatabase.h>

namespace dtnsimdistinct {

RegionDatabase::~RegionDatabase() {
}

RegionDatabase::RegionDatabase() {
}

void RegionDatabase::addNode(vector<string> row) {

	int nodeEid = stoi(row.at(0));
	string region = row.at(1);

	if (nodeRegions_.count(nodeEid) <= 0) {
		set<string> temp;
		nodeRegions_.insert(make_pair(nodeEid, temp));
	}
	nodeRegions_[nodeEid].insert(region);
}


void RegionDatabase::populateVents() {

	for (auto & entry : nodeRegions_) {
		int nodeEid = entry.first;

		for (auto & fromRegion : entry.second) {

			for (auto & toRegion : entry.second) {
				if (fromRegion != toRegion) {

					if (nodesVents_.count(nodeEid) <= 0) {
						set<Vent> temp;
						nodesVents_.insert(make_pair(nodeEid, temp));
					}
					Vent vent(nodeEid, fromRegion, toRegion);
					nodesVents_[nodeEid].insert(vent);
				}
			}
		}
	}
}


set<string> RegionDatabase::getRegions(int nodeEid) {

	set<string> regions;

	for (auto & region : nodeRegions_[nodeEid]) {
		regions.insert(region);
	}

	return regions;
}


map<int, set<Vent> > &RegionDatabase::getAllVents() {
	return nodesVents_;
}

}
