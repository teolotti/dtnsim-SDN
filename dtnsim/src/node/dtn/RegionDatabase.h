#ifndef REGIONDATABASE_H_
#define REGIONDATABASE_H_

#include <omnetpp.h>
#include <list>
#include <map>
#include <fstream>
#include <string>
#include <limits>

using namespace std;
using namespace omnetpp;

class RegionDatabase {

public:

	RegionDatabase();
	virtual ~RegionDatabase();
	RegionDatabase(RegionDatabase &regionDatabase);

	// parse regions file and fill nodesRegion structure
	void parseRegionDatabaseFile(string fileName);
	void readLine(const string & s);

	// Region plan population functions
	void addNode(int nodeEid, string regionId, bool rx, bool tx);
	void addVent(int portalNodeEid, string regionId1, string regionId2);

	// debug function
	void printRegionDatabase();

	void setRegionsFile(string regionsFile);

	int getNodesRxRegionSize();
	int getNodesTxRegionSize();

	set<string> getRxRegionIds(int nodeEid);
	set<string> getTxRegionIds(int nodeEid);

	map<int, set<string> > &getNodesRxRegions();
	map<int, set<string> > &getNodesTxRegions();

	// get regions where nodeEid is a member (either as rx or tx)
	set<string> getRegions(int nodeEid);

	map<int, set<pair<string, string> > > &getNodesVents();

	int getVentsNumber();

private:

	void populateVents();
	set<pair<string, string> > getCombinations(set<string> s1, set<string> s2);

	// regions where a node can receive bundles
	// nodeEid -> set<rxRegionId>
	map<int, set<string> > nodesRxRegions_;

	// regions where a node can transmit bundles
	// nodeEid -> set<txRegionId>
	map<int, set<string> > nodesTxRegions_;

	// set of vents associated to a portal node
	// nodeEid -> set<pair<string,string> >
	map<int, set<pair<string, string> > > nodesVents_;

	string regionsFile_;

};

#endif /* REGIONPLAN_H_ */
