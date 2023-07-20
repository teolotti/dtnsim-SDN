#ifndef SRC_DISTINCTREGIONS_NODE_DTN_ROUTING_REGIONDATABASE_H_
#define SRC_DISTINCTREGIONS_NODE_DTN_ROUTING_REGIONDATABASE_H_

#include <src/distinctRegions/node/dtn/routing/Vent.h>
#include <omnetpp.h>
#include <list>
#include <map>
#include <fstream>
#include <string>
#include <limits>

using namespace std;
using namespace omnetpp;

namespace dtnsimdistinct {

class RegionDatabase {

public:

	RegionDatabase();
	virtual ~RegionDatabase();

	// Region database population functions
	// Node: node EID | region ID
	void addNode(vector<string> row);
	void populateVents();

	// get regions where nodeEid is a member (either as rx or tx)
	set<string> getRegions(int nodeEid);
	map<int, set<Vent>> &getAllVents();

	// debug function
	//void printRegionDatabase();

private:

	// Node EID -> all regions it is part of
	map<int, set<string>> nodeRegions_;

	// Node EID -> all vents it represents
	map<int, set<Vent>> nodesVents_;

};
}

#endif /* SRC_DISTINCTREGIONS_NODE_DTN_ROUTING_REGIONDATABASE_H_ */
