#include "Central.h"

Define_Module (dtnsimhierarchical::Central);
namespace dtnsimhierarchical {

Central::Central() {
}

Central::~Central() {
}

void Central::initialize() {

	// parse contact plan for each region
	cout << "parsing contact plans for each region" << endl;
	parseContactPlansFile(par("contactPlan"));

	// initialize contact plan(s) for each node
	cout << "initializing contact plans in passageways" << endl;
	passagewayNodeNumber_ = this->getParentModule()->par("passagewayNodeNumber");

	for (int i = 0; i < passagewayNodeNumber_; ++i) {

		// passageways can belong to two regions (home and outer)
		string homeRegion = this->getParentModule()->getSubmodule("passagewayNode", i)->par("homeRegion");
		string outerRegion = this->getParentModule()->getSubmodule("passagewayNode", i)->par("outerRegion");

		Dtn *dtn = check_and_cast<Dtn *>(this->getParentModule()->getSubmodule("passagewayNode", i)->getSubmodule("dtn"));
		dtn->setHomeContactPlan(contactPlans_[homeRegion]);
		dtn->setOuterContactPlan(contactPlans_[outerRegion]);

		Com *com = check_and_cast<Com *>(this->getParentModule()->getSubmodule("passagewayNode", i)->getSubmodule("com"));
		com->setHomeContactPlan(contactPlans_[homeRegion]);
		com->setOuterContactPlan(contactPlans_[outerRegion]);
	}

	cout << "initializing contact plan in region nodes" << endl;
	regionNodeNumber_ = this->getParentModule()->par("regionNodeNumber");

	for (int i = 0; i < regionNodeNumber_; ++i) {

		// get the node's region
		string homeRegion = this->getParentModule()->getSubmodule("regionNode", i)->par("homeRegion");

		Dtn *dtn = check_and_cast<Dtn *>(this->getParentModule()->getSubmodule("regionNode", i)->getSubmodule("dtn"));
		dtn->setHomeContactPlan(contactPlans_[homeRegion]);

		Com *com = check_and_cast<Com *>(this->getParentModule()->getSubmodule("regionNode", i)->getSubmodule("com"));
		com->setHomeContactPlan(contactPlans_[homeRegion]);
	}
}

void Central::parseContactPlansFile(string fileName) {

	string a;
	string contact;
	string region;
	double start = 0.0;
	double end = 0.0;
	int sourceEid;
	int destinationEid;
	double dataRate = 0.0;

	string fileLine = "#";
	ifstream file;

	file.open(fileName.c_str());

	if (!file.is_open()) {
		throw cException(("Error: wrong path to contacts file " + string(fileName)).c_str());
	}

	ContactPlan temporaryContactPlan;
	while (getline(file, fileLine)) {

		if ((fileLine.empty()) || (fileLine.at(0) == '#')) {
			continue;

		} else if (fileLine.size() == 1 && isalpha(fileLine.at(0))) {

			if (!region.empty() && !temporaryContactPlan.getContacts()->empty()) {

				contactPlans_[region] = temporaryContactPlan;

			}

			region = fileLine.at(0);
			temporaryContactPlan = ContactPlan();
			continue;

		} else if (fileLine.find("passageway") != string::npos) {
			string passageway;
			int eid;
			stringstream stringLine(fileLine);
			stringLine >> a >> passageway >> eid;
			if ((a.compare("a") == 0) && (passageway.compare("passageway") == 0)) {
				temporaryContactPlan.addPassageway(eid);
			}
			continue;
		}

		stringstream stringLine(fileLine);
		stringLine >> a >> contact >> start >> end >> sourceEid >> destinationEid >> dataRate;

		if ((a.compare("a") == 0) && (contact.compare("contact") == 0)) {

			temporaryContactPlan.addContact(contactId_, start, end, sourceEid, destinationEid, dataRate, region, region);
			contactId_++;

		} else {
			cout << "dtnsim error: unknown contact plan command type: a " << fileLine << endl;
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

	// EOF -> add last contact plan
	if (!region.empty() && !temporaryContactPlan.getContacts()->empty()) {

		contactPlans_[region] = temporaryContactPlan;

	}

	file.close();
}


}
