#include "Central.h"

Define_Module (dtnsimdistinct::Central);

namespace dtnsimdistinct {

Central::Central() {
}

Central::~Central() {
}

void Central::initialize() {

	// parse backbone contact plan (obsolete)
	cout << "parsing backbone contact plan" << endl;
	parseBackboneContactPlanFile(par("backboneContactPlan"));

	// parse contact plan for each region
	cout << "parsing region contact plans" << endl;
	// at "A", we will save the entire contact plan (for contact scheduling reasons)
	ContactPlan temporaryContactPlan;
	regionContactPlans_["A"] = temporaryContactPlan;
	parseRegionContactPlansFile(par("regionContactPlan"));

	/*
	// print out all regions recorded
	for (auto & entry : regionContactPlans_) {
		cout << "KEY: " << entry.first << endl;
	}
	*/

	/*
	// print out all contacts assigned to a specific region, i.e. "C"
	for (auto & contact : *regionContactPlans_["C"].getContacts()) {
		cout << "FROM: " << contact.getSourceEid() << " TO: " << contact.getDestinationEid() << endl;
	}
	*/

	// parse region database (if provided)
	string regionDatabaseFileName = par("regionDatabase");
	if (!regionDatabaseFileName.empty()) {
		cout << "parsing region database" << endl;
		parseRegionDatabaseFile(regionDatabaseFileName);
	}

	/*
	// print all vents noted for each node cited in region database
	for (auto & entry : regionDatabase_.getAllVents()) {
		cout << "Node EID: " << entry.first << endl;
		for (auto & vent : entry.second) {
			cout << "Vents: " << vent.nodeEid << endl;
		}
 	}
 	*/

	// processing backbone contact plan in global network (obsolete)
	cout << "initializing backbone contact plan in global passageway(s), i.e. backbone nodes" << endl;
	backboneNodeNumber_ = this->getParentModule()->par("backboneNodeNumber");

	for (int i = 0; i < backboneNodeNumber_; ++i) {

		Dtn *dtn = check_and_cast<Dtn *>(this->getParentModule()->getSubmodule("backboneNode", i)->getSubmodule("dtn"));
		dtn->setBackboneContactPlan(backboneContactPlan_);

		Com *com = check_and_cast<Com *>(this->getParentModule()->getSubmodule("backboneNode", i)->getSubmodule("com"));
		com->setBackboneContactPlan(backboneContactPlan_);
	}

	// processing backbone and region contact plans inside regions (obsolete)
	cout << "initializing backbone and region contact plans in region passageway(s)" << endl;
	passagewayNodeNumber_ = this->getParentModule()->par("passagewayNodeNumber");

	for (int i = 0; i < passagewayNodeNumber_; ++i) {

		// get the nodes region
		string region = this->getParentModule()->getSubmodule("passagewayNode", i)->par("region");

		Dtn *dtn = check_and_cast<Dtn *>(this->getParentModule()->getSubmodule("passagewayNode", i)->getSubmodule("dtn"));
		dtn->setBackboneContactPlan(backboneContactPlan_);
		dtn->setRegionContactPlan(regionContactPlans_[region]);

		Com *com = check_and_cast<Com *>(this->getParentModule()->getSubmodule("passagewayNode", i)->getSubmodule("com"));
		com->setBackboneContactPlan(backboneContactPlan_);
		com->setRegionContactPlan(regionContactPlans_[region]);
	}

	// processing region contact plans inside regions
	cout << "initializing region contact plans in region node(s)" << endl;
	regionNodeNumber_ = this->getParentModule()->par("regionNodeNumber");

	for (int i = 0; i < regionNodeNumber_; ++i) {

		// get the nodes region
		string region = this->getParentModule()->getSubmodule("regionNode", i)->par("region");

		Dtn *dtn = check_and_cast<Dtn *>(this->getParentModule()->getSubmodule("regionNode", i)->getSubmodule("dtn"));
		dtn->setRegionContactPlan(regionContactPlans_[region]);
		dtn->addRegionContactPlan(region, regionContactPlans_[region]);

		if (!regionDatabaseFileName.empty()) {
			dtn->setRegionDatabase(regionDatabase_);
			int nodeEid = this->getParentModule()->getSubmodule("regionNode", i)->par("eid").intValue();
			set<string> regions = regionDatabase_.getRegions(nodeEid);
			for (auto & actualRegion : regions) {
				dtn->addRegionContactPlan(actualRegion, regionContactPlans_[actualRegion]);
			}
		}

		Com *com = check_and_cast<Com *>(this->getParentModule()->getSubmodule("regionNode", i)->getSubmodule("com"));
		com->setRegionContactPlan(regionContactPlans_[region]);
	}

}

void Central::parseBackboneContactPlanFile(string fileName) {

	ifstream file;
	file.open(fileName.c_str());

	if (!file.is_open()) {
		throw cException(("Error: wrong path to contacts file " + string(fileName)).c_str());
	}

	string line, word;

	// iterate through lines of csv
	while (getline(file, line)) {


		if (line.at(0) == 'C') { // skip first line
			continue;
		}

		// iterate through words of individual csv row
		stringstream s(line);
		vector<string> row;
		while (getline(s, word, ',')) {

			row.push_back(word);
		}

		backboneContactPlan_.addContact(row);
	}


	if (cin.bad()) {
		// IO error
	} else if (!cin.eof()) {
		// format error (not possible with getline but possible with operator>>)
	} else {
		// format error (not possible with getline but possible with operator>>)
		// or end of file (can't make the difference)
	}

	file.close();
}

void Central::parseRegionContactPlansFile(string fileName) {

	ifstream file;
	file.open(fileName.c_str());

	if (!file.is_open()) {
		throw cException(("Error: wrong path to contacts file " + string(fileName)).c_str());
	}

	string line, word;

	// iterate through lines of csv
	while (getline(file, line)) {


		if (line.at(0) == 'C') { // skip first line
			continue;
		}

		// iterate through words of individual csv row
		stringstream s(line);
		vector<string> row;
		while (getline(s, word, ',')) {

			row.push_back(word);
		}

		regionContactPlans_["A"].addContact(row);

		string region;
		if (row.size()>=13) {
			region = row.at(12);
		} else {
			continue;
		}

		if (regionContactPlans_.count(region) > 0) {
			ContactPlan contactPlan = regionContactPlans_.at(region);
			contactPlan.addContact(row);
			regionContactPlans_[region] = contactPlan;
		} else {
			ContactPlan temporaryContactPlan;
			temporaryContactPlan.addContact(row);
			regionContactPlans_[region] = temporaryContactPlan;
		}

	}


	if (cin.bad()) {
		// IO error
	} else if (!cin.eof()) {
		// format error (not possible with getline but possible with operator>>)
	} else {
		// format error (not possible with getline but possible with operator>>)
		// or end of file (can't make the difference)
	}

	file.close();
}

void Central::parseRegionDatabaseFile(string fileName) {

	ifstream file;
	file.open(fileName.c_str());

	if (!file.is_open()) {
		throw cException(("Error: wrong path to region database file " + string(fileName)).c_str());
	}

	string line, word;
	while (getline(file, line)) {

		if (line.at(0) == '#') {
			continue; // skip comment
		}

		// iterate through words of individual row
		stringstream s(line);
		vector<string> row;
		while (getline(s, word, ' ')) {
			row.push_back(word);
		}

		regionDatabase_.addNode(row);
	}

	if (cin.bad()) {
		// IO error
	} else if (!cin.eof()) {
		// format error (not possible with getline but possible with operator>>)
	} else {
		// format error (not possible with getline but possible with operator>>)
		// or end of file (can't make the difference)
	}

	regionDatabase_.populateVents();
	file.close();
}

}

