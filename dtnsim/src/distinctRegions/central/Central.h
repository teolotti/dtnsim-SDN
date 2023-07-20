/*
 * The central module is responsible for parsing contact plans
 * and distributing them to the corresponding nodes in the network.
 * If a region database file is supplied, the central module
 * also populates and distributes the region database to every node.
 * In case of enforced regions (i.e. LRK method), all contacts are
 * assigned to a notational region "A", while also splitting contacts
 * into divided regions accordingly.
 *
 * Every contact is assigned a globally unique ID.
 */

/*
 * TODO:
 * - allow for updating CPs / uploading new CPs
 * - remove all backbone functionality (from discarded distinct regions idea)
 * - remove differentiation between region and pw nodes (again, discarded idea, now all nodes are region nodes)
 */
#ifndef SRC_DISTINCTREGIONS_CENTRAL_CENTRAL_H_
#define SRC_DISTINCTREGIONS_CENTRAL_CENTRAL_H_

#include <omnetpp.h>
#include <string>
#include <iostream>
#include <map>
#include <cctype>

#include <src/distinctRegions/node/dtn/contacts/ContactPlan.h>
#include <src/distinctRegions/node/dtn/routing/RegionDatabase.h>
#include <src/distinctRegions/node/dtn/Dtn.h>
#include <src/distinctRegions/node/com/Com.h>

using namespace omnetpp;
using namespace std;

namespace dtnsimdistinct {

class Central: public cSimpleModule {

public:
	Central();
	virtual ~Central();

protected:
	virtual void initialize();

private:

	int backboneNodeNumber_;
	int passagewayNodeNumber_;
	int regionNodeNumber_;

	// global backbone CP
	ContactPlan backboneContactPlan_;
	void parseBackboneContactPlanFile(string fileName);

	// a map of region names and their respective CP
	map<string, ContactPlan> regionContactPlans_;
	void parseRegionContactPlansFile(string fileName);

	// region database (IRR)
	RegionDatabase regionDatabase_;
	void parseRegionDatabaseFile(string fileName);

};
}

#endif /* SRC_DISTINCTREGIONS_CENTRAL_CENTRAL_H_ */
