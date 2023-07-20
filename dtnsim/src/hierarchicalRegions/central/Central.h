#ifndef SRC_HIERARCHICALREGIONS_CENTRAL_CENTRAL_H_
#define SRC_HIERARCHICALREGIONS_CENTRAL_CENTRAL_H_

#include <omnetpp.h>
#include <string>
#include <iostream>
#include <map>
#include <cctype>

#include <src/hierarchicalRegions/node/dtn/contacts/ContactPlan.h>
#include <src/hierarchicalRegions/node/dtn/Dtn.h>
#include <src/hierarchicalRegions/node/com/Com.h>

using namespace omnetpp;
using namespace std;

namespace dtnsimhierarchical {
class Central: public cSimpleModule {

public:
	Central();
	virtual ~Central();

protected:
	virtual void initialize();

private:
	// ensures globally unique contact IDs
	int contactId_ = 0;

	// a map of region names and their respective CP
	map<string, ContactPlan> contactPlans_;
	void parseContactPlansFile(string fileName);

	int passagewayNodeNumber_;
	int regionNodeNumber_;

};
}

#endif /* SRC_HIERARCHICALREGIONS_CENTRAL_CENTRAL_H_ */
