#include <src/central/Central.h>

Define_Module (dtnsim::Central);

namespace dtnsim {

Central::Central() {
}

Central::~Central() {
}

void Central::initialize() {

	nodesNumber_ = this->getParentModule()->par("nodesNumber");

	// Initialize region plan (if network is divided into regions)
	// the same for all nodes
	cout << "initializing regionDatabase" << endl;
	string regionsFile = par("regionsFile");

	if (!regionsFile.empty()) {
		regionDatabase_.parseRegionDatabaseFile(regionsFile);
		regionDatabase_.printRegionDatabase();

		// parse region specific contact plans and region names
		cout << "initializing one contact plan per region" << endl;

		const char *contactsFileChar = par("contactsFile");
		cStringTokenizer contactsFileTokenizer(contactsFileChar, ",");

		const char *contactsFileRegionsChar = par("contactsFileRegions");
		cStringTokenizer contactsFileRegionsTokenizer(contactsFileRegionsChar, ",");

		int lastContactId = 1;
		while (contactsFileTokenizer.hasMoreTokens() && contactsFileRegionsTokenizer.hasMoreTokens()) {

			string regionStr = string(contactsFileRegionsTokenizer.nextToken());
			regions_.push_back(regionStr);

			// Initialize Contact Plan
			ContactPlan contactPlan;
			string cpFile = string(contactsFileTokenizer.nextToken());
			contactPlan.parseContactPlanFile(cpFile, lastContactId);
			contactPlans_.push_back(contactPlan);
			contactTopologies_.push_back(contactPlan);

			lastContactId = contactPlan.getLastContactId() + 1;
		}

		// input files are not structured correctly
		if(regions_.size() != contactPlans_.size()) {
			string str1 = string("Error in Central Module: ") + string("Missmatch between contactsFile and contactsFileRegions");
			throw cException((str1).c_str());
		}

	} else {

		// Initialize unique global contact plan for all nodes
		ContactPlan contactPlan;
		contactPlan.parseContactPlanFile(par("contactsFile"));

		contactPlans_.push_back(contactPlan);

		// Initialize topology
		ContactPlan contactTopology;
		contactTopology.parseContactPlanFile(par("contactsFile"));
		contactTopologies_.push_back(contactTopology);
		cout << "Parsed CP" << endl;
	}

	// schedule dummy event to make time pass until
	// last potential contact. This is mandatory in order for nodes
	// finish() method to be called and collect some statistics when
	// none contact is scheduled.
	if (!contactTopologies_.at(0).getContacts()->empty()) {

		double topologyEndTime = contactTopologies_.at(0).getContacts()->back().getEnd();
		ContactMsg *contactMsgEnd = new ContactMsg("contactEnd", CONTACT_END_TIMER);
		contactMsgEnd->setSchedulingPriority(CONTACT_END_TIMER);
		scheduleAt(topologyEndTime, contactMsgEnd);
	}

	// emit contactsNumber statistic
	contactsNumber = registerSignal("contactsNumber");
	emit(contactsNumber, contactPlans_.at(0).getContacts()->size());

	bool faultsAware = this->par("faultsAware");

	if (par("enableAvailableRoutesCalculation")) {

		int totalRoutesVar = 0;
		set < pair<int, int> > nodePairsRoutes1;
		totalRoutesVar = this->computeTotalRoutesNumber(contactPlans_.at(0), nodesNumber_, nodePairsRoutes1);

		// emit totalRoutes statistic
		totalRoutes = registerSignal("totalRoutes");
		emit(totalRoutes, totalRoutesVar);
	}

	int deleteNContacts = this->par("deleteNContacts");
	if (deleteNContacts > 0) {
		// delete N contacts
		vector<int> contactIdsToDelete;

		if (par("useCentrality")) {
			contactIdsToDelete = getCentralityContactIds(deleteNContacts, nodesNumber_);
		} else {
			contactIdsToDelete = getRandomContactIds(deleteNContacts);
		}

		deleteContacts(contactIdsToDelete, faultsAware);
	}

	// setting modified contact plan and contact topology each node
	for (int i = 0; i <= nodesNumber_; i++) {

		Dtn *dtn = check_and_cast<Dtn *>(this->getParentModule()->getSubmodule("node", i)->getSubmodule("dtn"));
		Com *com = check_and_cast<Com *>(this->getParentModule()->getSubmodule("node", i)->getSubmodule("com"));

		if(regionsFile.empty()) {

			map<string, ContactPlan> cps;
			cps[""] = contactPlans_.at(0);
			dtn->setContactPlans(cps);
			dtn->setContactTopology(contactTopologies_.at(0));
			com->setContactTopology(contactTopologies_.at(0));
			cout << "Set CPs in modules" << endl;

		} else {

			// each node receives a map<regionID, contactPlan> with all
			// the contact plans where this node is a member
			dtn->setRegionDatabase(regionDatabase_);
			set<string> regionsOfNodei = regionDatabase_.getRegions(i);
			map<string, ContactPlan> cps;
			ContactPlan contactTopology;
			for(auto it = regionsOfNodei.begin(); it != regionsOfNodei.end(); ++it) {

				string regionOfNodei = *it;

				for(unsigned int i = 0; i<regions_.size(); i++) {

					if(regions_.at(i) == regionOfNodei) {
						cps[regions_.at(i)] = contactPlans_.at(i);
						contactTopology.addContactPlan(contactPlans_.at(i));
					}
				}
			}

			dtn->setContactPlans(cps);
			dtn->setContactTopology(contactTopology);
			com->setContactTopology(contactTopology);

//			cout<<"central printing contact Topology"<<endl;
//			vector<Contact> *cnts = contactTopology.getContacts();
//			for(unsigned int i = 0; i<cnts->size(); i++)
//			{
//				cout<<"id = "<<cnts->at(i).getId()<<endl;
//			}
//			exit(0);
		}
	}

	if (par("enableAvailableRoutesCalculation")) {

		int availableRoutesVar = 0;
		set < pair<int, int> > nodePairsRoutes2;
		availableRoutesVar = this->computeTotalRoutesNumber(contactPlans_.at(0), nodesNumber_, nodePairsRoutes2);

		// emit availableRoutes statistic
		availableRoutes = registerSignal("availableRoutes");
		emit(availableRoutes, availableRoutesVar);

		// emit pairsOfNodesWithAtLeastOneRoute statistic
		pairsOfNodesWithAtLeastOneRoute = registerSignal("pairsOfNodesWithAtLeastOneRoute");
		int pairsOfNodesWithAtLeastOneRouteVar = nodePairsRoutes2.size();
		emit(pairsOfNodesWithAtLeastOneRoute, pairsOfNodesWithAtLeastOneRouteVar);
	}
}

void Central::finish() {

	if (nodesNumber_ >= 1) {

		if (this->par("saveTopology")) {
			this->saveTopology();
		}

		if (this->par("saveFlows")) {
			this->saveFlows();
		}

		if (this->par("saveLpFlows")) {
			this->saveLpFlows();
		}
	}
}

void Central::handleMessage(cMessage * msg) {
	// delete dummy msg
	delete msg;
}


void Central::saveTopology() {
#ifdef USE_BOOST_LIBRARIES
	map<double, TopologyGraph> topology = topologyUtils::computeTopology(&this->contactTopology_, nodesNumber_);
	topologyUtils::saveGraphs(&topology, "results/topology.dot");
#endif
}

void Central::saveFlows() {
#ifdef USE_BOOST_LIBRARIES
	this->computeFlowIds();
	vector < string > dotColors = routerUtils::getDotColors();
	map<double, RouterGraph> flows = routerUtils::computeFlows(&this->contactTopology_, nodesNumber_, "results");
	routerUtils::saveGraphs(&this->contactTopology_, &flows, dotColors, flowIds_, "results/flows.dot");
#endif
}

void Central::saveLpFlows() {
#ifdef USE_BOOST_LIBRARIES
#ifdef USE_CPLEX_LIBRARY
	this->computeFlowIds();
	vector < string > dotColors = routerUtils::getDotColors();

	// traffic[k][k1][k2] generated in state k by commodity k1-k2
	map<int, map<int, map<int, double> > > traffic = getTraffics();

	Lp lp(&this->contactTopology_, nodesNumber_, traffic);

	lp.exportModel("results/lpModel");
	bool solved = lp.solve();

	if (solved)
	{
		map<double, RouterGraph> flows = lpUtils::computeFlows(&this->contactTopology_, nodesNumber_, &lp);
		routerUtils::saveGraphs(&this->contactTopology_, &flows, dotColors, flowIds_, "results/lpFlows.dot");
	}

#endif
#endif
}



double Central::getState(double trafficStart) {

	// compute state times
	vector<double> stateTimes;
	stateTimes.push_back(0);

	vector<Contact> *contacts = contactTopologies_.at(0).getContacts();
	vector<Contact>::iterator it1 = contacts->begin();
	vector<Contact>::iterator it2 = contacts->end();
	for (; it1 != it2; ++it1) {
		Contact contact = *it1;

		stateTimes.push_back(contact.getStart());
		stateTimes.push_back(contact.getEnd());

		std::sort(stateTimes.begin(), stateTimes.end());
		stateTimes.erase(std::unique(stateTimes.begin(), stateTimes.end()), stateTimes.end());
	}

	// get state of traffic generation
	double state = 0;
	vector<double>::iterator iit1 = stateTimes.begin();
	vector<double>::iterator iit2 = stateTimes.end();
	for (; iit1 != iit2; ++iit1) {
		double stateStart = *iit1;
		double stateEnd = stateTimes.back();

		++iit1;
		if (iit1 != iit2) {
			stateEnd = *iit1;
		}
		--iit1;

		if (trafficStart >= stateStart && trafficStart < stateEnd) {
			state = stateStart;
		}
	}

	return state;
}

void Central::deleteContacts(vector<int> contactsToDelete, bool faultsAware) {

	for (size_t i = 0; i < contactsToDelete.size(); i++) {
		int contactId = contactsToDelete.at(i);

		contactTopologies_.at(0).deleteContactById(contactId);

		// if faultsAware we delete contacts also from contactPlan
		// so CGR will take better decisions
		if (faultsAware) {
			contactPlans_.at(0).deleteContactById(contactId);
		}
	}
}

vector<int> Central::getRandomContactIds(int nContacts) {
	vector<int> contactIds;

	// get working copy of contactPlan_
	ContactPlan workCP(contactPlans_.at(0));

	int contactsDeleted = 0;
	while (contactsDeleted < nContacts) {
		vector<Contact> *contacts = workCP.getContacts();

		if (contacts->size() == 0) {
			break;
		}

		int randomPosition = intuniform(0, contacts->size() - 1);
		contactIds.push_back(contacts->at(randomPosition).getId());

		contacts->erase(contacts->begin() + randomPosition);

		contactsDeleted++;
	}

	return contactIds;
}

vector<int> Central::getCentralityContactIds(int nContacts, int nodesNumber) {

	//cout<<"delete central contacts"<<endl;

	vector<int> contactIds;

	// get working copy of contactPlan_
	ContactPlan workCP(contactPlans_.at(0));

	// contact id -> centrality
	map<int, double> centralityMap;

	int contactsDeleted = 0;
	while (contactsDeleted < nContacts) {

		vector<Contact> *contacts = workCP.getContacts();
		if (contacts->size() == 0) {
			break;
		}

		for (size_t i = 0; i < contacts->size(); i++) {
			centralityMap[contacts->at(i).getId()] = 0;
		}

		for (int i = 1; i <= nodesNumber; i++) {
			SdrModel sdr;
			int eid = i;
			sdr.setEid(eid);
			sdr.setNodesNumber(nodesNumber);
			sdr.setContactPlan(&workCP);
			bool printDebug = false;

			map<string, ContactPlan> workCPs;
			workCPs[""] = workCP;
			Routing *routing = new RoutingCgrModel350(eid, &sdr, &workCPs, NULL, printDebug);

			for (int j = 1; j <= nodesNumber; j++) {
				if (i != j) {
					//cout<<"!!! source = "<<i<<endl;
					//cout<<"!!! destination = "<<j<<endl;

					BundlePkt* bundle = new BundlePkt("bundle", BUNDLE);
					bundle->setSchedulingPriority(BUNDLE);
					bundle->setBitLength(0);
					bundle->setByteLength(0);
					bundle->setSourceEid(i);
					bundle->setDestinationEid(j);
					bundle->setReturnToSender(true);
					bundle->setCritical(false);
					bundle->setTtl(1000000);
					bundle->setCreationTimestamp(simTime());
					bundle->setHopCount(0);
					bundle->setNextHopEid(0);
					bundle->setSenderEid(0);
					bundle->getVisitedNodes().clear();
					CgrRoute emptyRoute;
					emptyRoute.nextHop = EMPTY_ROUTE;
					bundle->setCgrRoute(emptyRoute);

					vector<CgrRoute> routes = check_and_cast<RoutingCgrModel350 *>(routing)->getCgrRoutes(bundle, simTime().dbl());
					vector<CgrRoute> shortestPaths;

					time_t bestArrivalTime = 10000000;
					for (size_t k = 0; k < routes.size(); k++) {
						CgrRoute route = routes.at(k);
						if (route.arrivalTime == bestArrivalTime) {
							shortestPaths.push_back(route);
						} else if (route.arrivalTime < bestArrivalTime) {
							shortestPaths.clear();
							shortestPaths.push_back(route);
							bestArrivalTime = route.arrivalTime;
						}
					}
					//cout<<"routes size = "<<routes.size()<<endl;
					//cout<<"sp size = "<<shortestPaths.size()<<endl;

					set<int> affectedContacts = this->getAffectedContacts(shortestPaths);
					set<int>::iterator cit1 = affectedContacts.begin();
					set<int>::iterator cit2 = affectedContacts.end();
					for (; cit1 != cit2; ++cit1) {
						int cId = *cit1;
						int routesThroughContact = computeNumberOfRoutesThroughContact(cId, shortestPaths);
						double centrality = (double) routesThroughContact / (double) shortestPaths.size();
						centralityMap[cId] += centrality;
					}

					delete bundle;
				}
			}

			delete routing;
		}

		map<int, double>::iterator x = max_element(centralityMap.begin(), centralityMap.end(), [](const pair<int, double>& p1, const pair<int, double>& p2)
		{	return p1.second < p2.second;});

		// choose random contact among all max centrality contacts
		vector < pair<int, double> > allMaxElements;
		map<int, double>::iterator ii1 = centralityMap.begin();
		map<int, double>::iterator ii2 = centralityMap.end();
		for (; ii1 != ii2; ++ii1) {
			if (ii1->second == (*x).second) {
				allMaxElements.push_back(make_pair(ii1->first, ii1->second));
			}
		}

		int randomPosition = intuniform(0, allMaxElements.size() - 1);

//		cout<<"centralityMap = "<<endl;
//	    map<int, double>::iterator i1 = centralityMap.begin();
//	    map<int, double>::iterator i2 = centralityMap.end();
//	    for(; i1 != i2; ++i1)
//	    {
//	    	cout<<i1->first<<" "<<i1->second<<endl;
//	    }

		int contactToDelete = allMaxElements.at(randomPosition).first;
		contactIds.push_back(contactToDelete);
		workCP.deleteContactById(contactToDelete);

//		cout<<"##### contact deleted = "<<contactIds.back()<<endl;
//		cout<<endl;

		contactsDeleted++;
		centralityMap.clear();
	}

	return contactIds;
}

/// @brief compute total routes from all to all nodes
int Central::computeTotalRoutesNumber(ContactPlan &contactPlan, int nodesNumber, set<pair<int, int> > &nodePairsRoutes) {
	int totalRoutesNumber = 0;

	// get working copy of contactPlan_
	ContactPlan workCP(contactPlans_.at(0));

	vector<Contact> *contacts = workCP.getContacts();
	if (contacts->size() == 0) {
		return 0;
	}

	for (int i = 1; i <= nodesNumber; i++) {
		SdrModel sdr;
		int eid = i;
		sdr.setEid(eid);
		sdr.setNodesNumber(nodesNumber);
		sdr.setContactPlan(&workCP);
		bool printDebug = false;

		map<string, ContactPlan> workCPs;
		workCPs[""] = workCP;
		Routing *routing = new RoutingCgrModel350(eid, &sdr, &workCPs, NULL, printDebug);

		for (int j = 1; j <= nodesNumber; j++) {
			if (i != j) {
				//cout<<"!!! source = "<<i<<endl;
				//cout<<"!!! destination = "<<j<<endl;

				BundlePkt* bundle = new BundlePkt("bundle", BUNDLE);
				bundle->setSchedulingPriority(BUNDLE);
				bundle->setBitLength(0);
				bundle->setByteLength(0);
				bundle->setSourceEid(i);
				bundle->setDestinationEid(j);
				bundle->setReturnToSender(true);
				bundle->setCritical(false);
				bundle->setTtl(1000000);
				bundle->setCreationTimestamp(simTime());
				bundle->setHopCount(0);
				bundle->setNextHopEid(0);
				bundle->setSenderEid(0);
				bundle->getVisitedNodes().clear();
				CgrRoute emptyRoute;
				emptyRoute.nextHop = EMPTY_ROUTE;
				bundle->setCgrRoute(emptyRoute);

				vector<CgrRoute> routes = check_and_cast<RoutingCgrModel350 *>(routing)->getCgrRoutes(bundle, simTime().dbl());

				if (!routes.empty()) {
					nodePairsRoutes.insert(make_pair(i, j));
				}

				totalRoutesNumber += routes.size();

				delete bundle;
			}
		}

		delete routing;
	}

	return totalRoutesNumber;
}

int Central::computeNumberOfRoutesThroughContact(int contactId, vector<CgrRoute> shortestPaths) {
	int numberOfRoutesThroughContact = 0;
	for (size_t r = 0; r < shortestPaths.size(); r++) {
		CgrRoute route = shortestPaths.at(r);
		vector<Contact *> hops = route.hops;
		for (size_t h = 0; h < hops.size(); h++) {
			Contact *contact = hops.at(h);
			if (contact->getId() == contactId) {
				numberOfRoutesThroughContact++;
			}
		}
	}

	return numberOfRoutesThroughContact;
}

set<int> Central::getAffectedContacts(vector<CgrRoute> shortestPaths) {
	set<int> affectedContacts;
	for (size_t r = 0; r < shortestPaths.size(); r++) {
		CgrRoute route = shortestPaths.at(r);
		vector<Contact *> hops = route.hops;
		for (size_t h = 0; h < hops.size(); h++) {
			affectedContacts.insert(hops.at(h)->getId());
		}
	}

	return affectedContacts;
}

}

