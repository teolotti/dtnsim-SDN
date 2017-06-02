#include <dtn/routing/RoutingCgrModelRev17.h>

// This function initializes the routing class:
// To this end, local eid_, the total number of nodes nodeNum_,
// a pointer to local storage sdr_, and a contact plan are set.
RoutingCgrModelRev17::RoutingCgrModelRev17(int eid, int nodeNum, SdrModel * sdr, ContactPlan * localContactPlan,
		ContactPlan * globalContactPlan, string routingType, bool printDebug) {
	// Initialize basic and default variables
	eid_ = eid; 					// local node EID
	nodeNum_ = nodeNum;				// number of neighbors
	sdr_ = sdr;						// local storage
	printDebug_ = printDebug;		// direct debug to cout
	routingType_ = routingType;		// routing type string

	// Check routingType string is correct
	this->checkRoutingTypeString();

	// Set global or local contact plan
	if (routingType_.find("contactPlan:local") != std::string::npos)
		contactPlan_ = localContactPlan;
	else if (routingType_.find("contactPlan:global") != std::string::npos)
		contactPlan_ = globalContactPlan;

	// Initialize routeTable_: it will have nodeNum_ entries, one entry
	// per each possible neighbor node. Each entry will host a routeList
	routeTable_.resize(nodeNum_);
}

RoutingCgrModelRev17::~RoutingCgrModelRev17() {

}

// This function is called every time a new bundle (local or
// in transit) need to be routed.The outcome of the function
// is to enqueue the bundle in the SDR memory which is organized
// by a set of queues addressed by queueIds. In current DtnSim
// version Ids corresponds with the contact Id where the bundle
// is expected to be forwarded. This mimic ION behaviour. Other
// implementations do enqueue bundles on a per neighbour-node basis.
void RoutingCgrModelRev17::routeAndQueueBundle(BundlePkt * bundle, double simTime) {

	// Disable cout if degug disabled
	if (printDebug_ == false)
		cout.setstate(std::ios_base::failbit);

	// Set local time
	simTime_ = simTime;

	// Reset counters (metrics)
	dijkstraCalls = 0;
	dijkstraLoops = 0;
	tableEntriesCreated = 0;
	tableEntriesExplored = 0;

	cout << "TIME: " << simTime_ << "s, NODE: " << eid_ << ", routing bundle to dst: " << bundle->getDestinationEid()
			<< " (" << bundle->getByteLength() << "Bytes)" << endl;

	// If no extensionBlock, run cgr each time a bundle is dispatched
	if (routingType_.find("extensionBlock:off") != std::string::npos)
		this->cgrForward(bundle);

	// If extensionBlock, check header
	if (routingType_.find("extensionBlock:on") != std::string::npos) {
		if (bundle->getCgrRoute().nextHop == EMPTY_ROUTE) {
			// Empty extension block: Calculate, encode a new path and enqueue
			this->cgrForward(bundle);
		} else {
			// Non-empty EB: Use EB Route to route bundle
			CgrRoute ebRoute = bundle->getCgrRoute();

			// Print route and hops
			cout << "extensionBlockRoute: [" << ebRoute.terminusNode << "][" << ebRoute.nextHop << "]: nextHop: "
					<< ebRoute.nextHop << ", frm " << ebRoute.fromTime << " to " << ebRoute.toTime << ", arrival time: "
					<< ebRoute.arrivalTime << ", volume: " << ebRoute.residualVolume << "/" << ebRoute.maxVolume
					<< "Bytes" << endl << "hops: ";
			for (vector<Contact *>::iterator hop = ebRoute.hops.begin(); hop != ebRoute.hops.end(); ++hop)
				cout << "(+" << (*hop)->getStart() << " +" << (*hop)->getEnd() << " " << (*hop)->getSourceEid() << " "
						<< (*hop)->getDestinationEid() << " " << (*hop)->getResidualVolume() << "/"
						<< (*hop)->getVolume() << "Bytes) ";
			cout << endl;

			// Check if encoded path is valid, update local contacts and route hops
			bool ebRouteIsValid = true;
			vector<Contact *> newHops;
			for (vector<Contact *>::iterator hop = ebRoute.hops.begin(); hop != ebRoute.hops.end(); ++hop) {

				// TODO: Fuse the ebRoute information with local contact graph
				// This should provide an improved behavior in between the
				// global and local contact plan extension block configuration
				// But beware that this hop points to a remote contact graph.

				if ((*hop)->getDestinationEid() == eid_)
					// This is the contact that made this bundle arrive here,
					// do not check nothing and discard it from the newHops path
					continue;

				if (routingType_.compare("volumeAware:1stContact") == 0)
					// Only check first contact volume
					if ((*hop)->getSourceEid() == eid_)
						if (contactPlan_->getContactById((*hop)->getId())->getResidualVolume()
								< bundle->getByteLength()) {
							// Not enough residual capacity from local view of the path
							ebRouteIsValid = false;
							break;
						}

				if (routingType_.compare("volumeAware:allContacts") == 0)
					// Check all contacts volume
					if (contactPlan_->getContactById((*hop)->getId())->getResidualVolume() < bundle->getByteLength()) {
						// Not enough residual capacity from local view of the path
						ebRouteIsValid = false;
						break;
					}

				// store in newHops the local contacts
				newHops.push_back(contactPlan_->getContactById((*hop)->getId()));
			}

			if (ebRouteIsValid) {
				// Update necessary route parameters (from/to time and max/residual volume not necessary)
				ebRoute.hops = newHops;
				ebRoute.nextHop = ebRoute.hops[0]->getDestinationEid();

				// Enqueue using this route
				cout << "Using EB Route in header" << endl;
				this->cgrEnqueue(bundle, &ebRoute);
			} else {
				// Discard old extension block, calculate and encode a new path and enqueue
				cout << "EB Route in header not valid, generating a new one" << endl;
				this->cgrForward(bundle);
			}
		}
	}

	// Re-enable cout if debug disabled
	if (printDebug_ == false)
		cout.clear();
}

/////////////////////////////////////////////////
// Functions based Ion architecture
/////////////////////////////////////////////////

// This function tries to find the best path for the current bundle.
// Initially it checks if the route table is up-to-date and update it
// if it is out of date (by running a new Dijkstra search for the
// corresponding neighbor).
void RoutingCgrModelRev17::cgrForward(BundlePkt * bundle) {
	// If contact plan was changed, empty route list
	if (contactPlan_->getLastEditTime() > routeTableLastEditTime)
		this->clearRouteTable();
	routeTableLastEditTime = simTime_;

	int terminusNode = bundle->getDestinationEid();

	// Check route list for terminus node and recalculate if necesary depending on
	// the routeListType configured in the routingType string
	if (routingType_.find("routeListType:allPaths-yen") != std::string::npos) {
		//////////////////////////////////////////////////
		// allPaths-yen: All paths are calculated at once
		// when the contact plan changes. The route list
		// is completed using yen algorithm.
		//////////////////////////////////////////////////

		cout << "dtnsim error: routeListType:allPaths-yen not implemented yet" << endl;
		exit(1);

		// If this is the route list to this terminus is empty,
		// populate it with all paths to the destination. This
		// should only happen once per contact plan update.
		if (routeTable_.at(terminusNode).empty()) {

			vector<int> suppressedContactIds;

			// Determine the shortest path and add it to routeList
			CgrRoute route;
			this->findNextBestRoute(suppressedContactIds, terminusNode, &route);

			// Work on-going here...

		}
	}
	if (routingType_.find("routeListType:allPaths-initial+anchor") != std::string::npos) {
		//////////////////////////////////////////////////
		// allPaths-initial+anchor: All paths are calculated
		// at once when the contact plan changes. The route
		// list is completed using remove initial + anchor
		// algorithm currently used in ION 3.6.0.
		//////////////////////////////////////////////////

		// If this is the route list to this terminus is empty,
		// populate it with all paths to the destination. This
		// should only happen once per contact plan update.
		if (routeTable_.at(terminusNode).empty()) {

			vector<int> suppressedContactIds;
			Contact * anchorContact = NULL;

			while (1) {
				CgrRoute route;
				this->findNextBestRoute(suppressedContactIds, terminusNode, &route);

				// If no more routes were found, stop search loop
				if (route.nextHop == NO_ROUTE_FOUND)
					break;

				Contact * firstContact = route.hops.at(0);

				// If anchor search on-going
				if (anchorContact != NULL)
					// And the first contact is no longer anchor
					if (firstContact != anchorContact) {
						// End anchor search: [endAnchoredSearch() function in ion]

						cout << "-anchored search ends in contactId: " << anchorContact->getId() << " (src:"
								<< anchorContact->getSourceEid() << "-dst:" << anchorContact->getDestinationEid()
								<< ", " << anchorContact->getStart() << "s to " << anchorContact->getEnd() << "s)"
								<< endl;

						// Unsupress all remote contacts (suppressed local contacts dont change)
						for (vector<int>::iterator it = suppressedContactIds.begin(); it != suppressedContactIds.end();)
							if ((contactPlan_->getContactById((*it)))->getSourceEid() != eid_)
								it = suppressedContactIds.erase(it);
							else
								++it;

						// Suppress anchorContact from further searches.
						suppressedContactIds.push_back(anchorContact->getId());
						anchorContact = NULL;

						// Ignore the latest route and continue with next iteration
						continue;
					}

				// Add new valid route to route table
				routeTable_.at(terminusNode).push_back(route);
				cout << "-new route found through node:" << route.nextHop << ", arrivalConf:" << route.confidence
						<< ", arrivalT:" << route.arrivalTime << ", txWin:(" << route.fromTime << "-" << route.toTime
						<< "), maxCap:" << route.maxVolume << "Bytes:" << endl;

				// Find limiting contact
				Contact * limitContact = NULL;
				if (route.toTime == firstContact->getEnd())
					// Generally it is the first
					limitContact = firstContact;
				else {
					// If not, start new anchor search on firstContact
					anchorContact = firstContact;

					cout << "-anchored search starts in contactId: " << anchorContact->getId() << " (src:"
							<< anchorContact->getSourceEid() << "-dst:" << anchorContact->getDestinationEid() << ", "
							<< anchorContact->getStart() << "s-" << anchorContact->getEnd() << "s)" << endl;

					// Find the limiting contact in route
					for (vector<Contact *>::iterator it = route.hops.begin(); it != route.hops.end(); ++it)
						if ((*it)->getEnd() == route.toTime) {
							limitContact = (*it);
							break;
						}
				}

				// Supress limiting contact
				suppressedContactIds.push_back(limitContact->getId());

				cout << "-suppressing limiting contactId: " << limitContact->getId() << " (src:"
						<< limitContact->getSourceEid() << "-dst:" << limitContact->getDestinationEid() << ", "
						<< limitContact->getStart() << "s-" << limitContact->getEnd() << "s)" << endl;

				tableEntriesCreated++;
			}
		}
	}
	if (routingType_.find("routeListType:allPaths-firstEnding") != std::string::npos) {
		//////////////////////////////////////////////////
		// allPaths-firstEnding: All paths are calculated at once
		// when the contact plan changes. The route list
		// is completed removing the first ending contact.
		//////////////////////////////////////////////////

		// If this is the route list to this terminus is empty,
		// populate it with all paths to the destination. This
		// should only happen once per contact plan update.
		if (routeTable_.at(terminusNode).empty()) {

			vector<int> suppressedContactIds;

			while (1) {
				CgrRoute route;
				this->findNextBestRoute(suppressedContactIds, terminusNode, &route);

				// If no more routes were found, stop search loop
				if (route.nextHop == NO_ROUTE_FOUND)
					break;

				// Add new valid route to route table
				routeTable_.at(terminusNode).push_back(route);

				// Suppress the first ending contact of the last route found
				double earliestEndingTime = numeric_limits<double>::max();
				int earliestEndingContactId;
				vector<Contact *>::iterator hop;
				for (hop = route.hops.begin(); hop != route.hops.end(); ++hop)
					if ((*hop)->getEnd() < earliestEndingTime) {
						earliestEndingTime = (*hop)->getEnd();
						earliestEndingContactId = (*hop)->getId();
					}
				suppressedContactIds.push_back(earliestEndingContactId);

				tableEntriesCreated++;
			}
		}
	}
	if (routingType_.find("routeListType:allPaths-firstDepleted") != std::string::npos) {
		//////////////////////////////////////////////////
		// allPaths-firstDepleted: All paths are calculated at once
		// when the contact plan changes. The route list
		// is completed removing the first depleted contact.
		//////////////////////////////////////////////////

		// If this is the route list to this terminus is empty,
		// populate it with all paths to the destination. This
		// should only happen once per contact plan update.
		if (routeTable_.at(terminusNode).empty()) {

			vector<int> suppressedContactIds;

			while (1) {
				CgrRoute route;
				this->findNextBestRoute(suppressedContactIds, terminusNode, &route);

				// If no more routes were found, stop search loop
				if (route.nextHop == NO_ROUTE_FOUND)
					break;

				// Add new valid route to route table
				routeTable_.at(terminusNode).push_back(route);

				// Suppress the least capacitated contact of the last route found
				double leastVolume = numeric_limits<double>::max();
				int leastVolumeContactId;
				vector<Contact *>::iterator hop;
				for (hop = route.hops.begin(); hop != route.hops.end(); ++hop)
					if ((*hop)->getVolume() < leastVolume) {
						leastVolume = (*hop)->getVolume();
						leastVolumeContactId = (*hop)->getId();
					}
				suppressedContactIds.push_back(leastVolumeContactId);

				tableEntriesCreated++;
			}
		}

	}
	if (routingType_.find("routeListType:oneBestPath") != std::string::npos) {
		//////////////////////////////////////////////////
		// allPaths-oneBestPath: Only a best path is
		// calculated for a given destination. The entry
		// is updated by the ending or depletion of a contact in the path
		//////////////////////////////////////////////////

		// If this is the route list to this terminus is empty,
		// create a single entry place to hold the route
		if (routeTable_.at(terminusNode).empty()) {
			routeTable_.at(terminusNode).resize(2);
			CgrRoute route;
			route.nextHop = EMPTY_ROUTE;
			route.arrivalTime = numeric_limits<double>::max(); // never chosen as best route
			routeTable_.at(terminusNode).at(0) = route;
			routeTable_.at(terminusNode).at(1) = route;
		}

		// Explore list and recalculate if necesary
		bool needRecalculation = false;

		// Empty route condition
		if (routeTable_.at(terminusNode).at(0).nextHop == EMPTY_ROUTE)
			needRecalculation = true;

		// Due route condition
		if (routeTable_.at(terminusNode).at(0).toTime < simTime_)
			needRecalculation = true;

		// Depleted route condition
		if (routeTable_.at(terminusNode).at(0).residualVolume < bundle->getByteLength()) {
			// Make sure that the capacity-limiting contact is marked as depleted
			vector<Contact *>::iterator hop;
			for (hop = routeTable_.at(terminusNode).at(0).hops.begin();
					hop != routeTable_.at(terminusNode).at(0).hops.end(); ++hop)
				if (routeTable_.at(terminusNode).at(0).residualVolume == (*hop)->getResidualVolume())
					(*hop)->setResidualVolume(0);

			needRecalculation = true;
		}

		if (needRecalculation) {
			vector<int> suppressedContactIds; // no suppressed contacts here
			CgrRoute route;
			this->findNextBestRoute(suppressedContactIds, terminusNode, &route);
			routeTable_.at(terminusNode).at(0) = route;

			// Also create a secondary route thorugh a different entry node
			// in order to have an alternative when return to sender is forbidden
			// through this entry node. set not found if not primary route found.

			if (route.nextHop != NO_ROUTE_FOUND) {
				// Suppress all contacts which connect this node with the entry node of
				// the route found. All other neighbors should be considered
				for (vector<Contact>::iterator it = contactPlan_->getContacts()->begin();
						it != contactPlan_->getContacts()->end(); ++it)
					if ((*it).getSourceEid() == eid_ && (*it).getDestinationEid() == route.nextHop)
						suppressedContactIds.push_back((*it).getId());
				this->findNextBestRoute(suppressedContactIds, terminusNode, &route);
				routeTable_.at(terminusNode).at(1) = route;
			} else {
				routeTable_.at(terminusNode).at(1).nextHop = NO_ROUTE_FOUND;
			}

			tableEntriesCreated++;
		}
	}
	if (routingType_.find("routeListType:perNeighborBestPath") != std::string::npos) {
		//////////////////////////////////////////////////
		// allPaths-perNeighborBestPath: A best path per
		// neighbour is calculated for a given destination. The entry
		// is updated by the ending or depletion of a contact in the paths
		//////////////////////////////////////////////////

		// If this is the route list to this terminus is empty,
		// create list with nodeNum_ EMPTY_ROUTE entries
		if (routeTable_.at(terminusNode).empty()) {
			routeTable_.at(terminusNode).resize(nodeNum_);
			for (int r = 0; r < nodeNum_; r++) {
				CgrRoute route;
				route.nextHop = EMPTY_ROUTE;
				route.arrivalTime = numeric_limits<double>::max(); // never chosen as best route
				routeTable_.at(terminusNode).at(r) = route;
			}
		}

		// Explore list and recalculate if necessary
		for (int r = 0; r < nodeNum_; r++) {
			// NO_ROUTE_FOUND does not trigger a recalculation
			if (routeTable_.at(terminusNode).at(r).nextHop == NO_ROUTE_FOUND)
				continue;

			bool needRecalculation = false;

			// Empty route condition
			if (routeTable_.at(terminusNode).at(r).nextHop == EMPTY_ROUTE)
				needRecalculation = true;

			// Due route condition
			if (routeTable_.at(terminusNode).at(r).toTime < simTime_)
				needRecalculation = true;

			// Depleted route condition
			if (routeTable_.at(terminusNode).at(r).residualVolume < bundle->getByteLength()) {
				// Make sure that the capacity-limiting contact is marked as depleted
				vector<Contact *>::iterator hop;
				for (hop = routeTable_.at(terminusNode).at(r).hops.begin();
						hop != routeTable_.at(terminusNode).at(r).hops.end(); ++hop)
					if (routeTable_.at(terminusNode).at(r).residualVolume == (*hop)->getResidualVolume())
						(*hop)->setResidualVolume(0);

				needRecalculation = true;
			}

			if (needRecalculation) {

				// Suppress all contacts which connect his node with other nodes
				// different than the entry node of this route table entry (r)
				vector<int> suppressedContactIds;
				for (vector<Contact>::iterator it = contactPlan_->getContacts()->begin();
						it != contactPlan_->getContacts()->end(); ++it)
					if ((*it).getSourceEid() == eid_ && (*it).getDestinationEid() != r)
						suppressedContactIds.push_back((*it).getId());

				CgrRoute route;
				this->findNextBestRoute(suppressedContactIds, terminusNode, &route);
				routeTable_.at(terminusNode).at(r) = route;

				tableEntriesCreated++;
			}
		}
	}

	// At this point routeTable must be updated
	// no matter which routingType was chosen

	// Print route table for this terminus
	this->printRouteTable(terminusNode);

	// Filter routes
	for (unsigned int r = 0; r < routeTable_.at(terminusNode).size(); r++) {

		routeTable_.at(terminusNode).at(r).filtered = false;

		// Filter those that should not be considered in
		// the next best route determination calculation.
		// Should have no effect when using dynamically updated
		// route tables (perNeighborBestPath and oneBestPath),
		// but on allPaths routing types.

		// criteria 1) filter route: capacity is depleted
		if (routeTable_.at(terminusNode).at(r).residualVolume < bundle->getByteLength()) {
			routeTable_.at(terminusNode).at(r).filtered = true;
		}
		// criteria 2) filter route: due time is passed
		if (routeTable_.at(terminusNode).at(r).toTime <= simTime_) {
			routeTable_.at(terminusNode).at(r).filtered = true;
		}

		// Filter those that goes back to sender if such
		// type of forwarding is forbidden as per .ini file
		if (bundle->getReturnToSender() == false)
			if (routeTable_.at(terminusNode).at(r).nextHop == bundle->getSenderEid())
				routeTable_.at(terminusNode).at(r).filtered = true;
	}

	if (!routeTable_.at(terminusNode).empty()) {
		// Select best route
		vector<CgrRoute>::iterator bestRoute;
		bestRoute = min_element(routeTable_.at(terminusNode).begin(), routeTable_.at(terminusNode).end(),
				this->compareRoutes);

		// Save tableEntriesExplored metric. Notice that
		// explored also includes filtered routes (i.e., pessimistic)
		tableEntriesExplored = routeTable_.at(terminusNode).size();

		// Enqueue bundle to route and update volumes
		this->cgrEnqueue(bundle, &(*bestRoute));
	} else {
		// Enqueue to limbo
		bundle->setNextHopEid(NO_ROUTE_FOUND);
		sdr_->enqueueBundleToContact(bundle, 0);

		cout << "*BestRoute not found (enqueing to limbo)" << endl;
	}
}

// This function enqueues the bundle in the best found path.
// To this end, it updates contacts volume depending on the volume-awareness
// type configured for the routing routine.
void RoutingCgrModelRev17::cgrEnqueue(BundlePkt * bundle, CgrRoute *bestRoute) {

	if (bestRoute->nextHop != NO_ROUTE_FOUND && !bestRoute->filtered) {

		cout << "*Best: routeTable[" << bestRoute->terminusNode << "][ ]: nextHop: " << bestRoute->nextHop << ", frm "
				<< bestRoute->fromTime << " to " << bestRoute->toTime << ", arrival time: " << bestRoute->arrivalTime
				<< ", volume: " << bestRoute->residualVolume << "/" << bestRoute->maxVolume << "Bytes" << endl;

		//////////////////////////////////////////////////
		// Update residualVolume: all contact
		//////////////////////////////////////////////////
		if (routingType_.find("volumeAware:allContacts") != std::string::npos) {
			// Update residualVolume of all this route hops
			for (vector<Contact *>::iterator hop = bestRoute->hops.begin(); hop != bestRoute->hops.end(); ++hop)
				(*hop)->setResidualVolume((*hop)->getResidualVolume() - bundle->getByteLength());

			// Update residualVolume of all routes that uses the updated hops
			for (int n = 1; n < nodeNum_; n++)
				if (!routeTable_.at(n).empty())
					for (unsigned int r = 0; r < routeTable_.at(n).size(); r++)
						for (vector<Contact *>::iterator hop1 = routeTable_.at(n).at(r).hops.begin();
								hop1 != routeTable_.at(n).at(r).hops.end(); ++hop1)
							for (vector<Contact *>::iterator hop2 = bestRoute->hops.begin();
									hop2 != bestRoute->hops.end(); ++hop2)
								if ((*hop1)->getId() == (*hop2)->getId())
									// Does the reduction of this contact volume requires a route volume update?
									if (routeTable_.at(n).at(r).residualVolume > (*hop1)->getResidualVolume()) {
										routeTable_.at(n).at(r).residualVolume = (*hop1)->getResidualVolume();
										cout << "*Rvol: routeTable[" << n << "][" << r << "]: updated to "
												<< (*hop1)->getResidualVolume() << "Bytes (all contacts)" << endl;
									}
		}

		//////////////////////////////////////////////////
		// Update residualVolume: 1st contact
		//////////////////////////////////////////////////
		if (routingType_.find("volumeAware:1stContact") != std::string::npos) {
			// Update residualVolume of the first hop
			bestRoute->hops[0]->setResidualVolume(bestRoute->hops[0]->getResidualVolume() - bundle->getByteLength());

			// Update residualVolume of all routes that uses the updated hops
			for (int n = 1; n < nodeNum_; n++)
				if (!routeTable_.at(n).empty())
					for (unsigned int r = 0; r < routeTable_.at(n).size(); r++)
						for (vector<Contact *>::iterator hop1 = routeTable_.at(n).at(r).hops.begin();
								hop1 != routeTable_.at(n).at(r).hops.end(); ++hop1)
							for (vector<Contact *>::iterator hop2 = bestRoute->hops.begin();
									hop2 != bestRoute->hops.end(); ++hop2)
								if ((*hop1)->getId() == (*hop2)->getId())
									// Does the reduction of this contact volume requires a route volume update?
									if (routeTable_.at(n).at(r).residualVolume > (*hop1)->getResidualVolume()) {
										routeTable_.at(n).at(r).residualVolume = (*hop1)->getResidualVolume();
										cout << "*Rvol: routeTable[" << n << "][" << r << "]: updated to "
												<< (*hop1)->getResidualVolume() << "Bytes (all contacts)" << endl;
									}
		}

		//////////////////////////////////////////////////
		// Update residualVolume: off -> do nothing
		//////////////////////////////////////////////////

		// Save CgrRoute in header
		if (routingType_.find("extensionBlock:on") != std::string::npos)
			bundle->setCgrRoute(*bestRoute);

		// Enqueue bundle
		bundle->setNextHopEid(bestRoute->nextHop);
		sdr_->enqueueBundleToContact(bundle, bestRoute->hops.at(0)->getId());
	} else {
		// Enqueue to limbo
		bundle->setNextHopEid(bestRoute->nextHop);
		sdr_->enqueueBundleToContact(bundle, 0);

		cout << "*BestRoute not found (enqueing to limbo)" << endl;
	}
}

// This function is the Dijkstra search over the contact-graph.
// It is based on current implementation in ION but adds a few corrections
// such as visited nodes list to avoid topological loops. From the implementation
// perspective it needs severe improvements as it currently overutilizes pointer
// operations which render the code very difficult to read and to debug.
// In general, each contact has a work pointer where temporal information only
// valid and related to the current Dijkstra search is stored.
void RoutingCgrModelRev17::findNextBestRoute(vector<int> suppressedContactIds, int terminusNode, CgrRoute * route) {
	// increment metrics counter
	dijkstraCalls++;

	// Create rootContact and its corresponding rootWork
	// id=0, start=0, end=inf, src=me, dst=me, rate=0, conf=1
	Contact * rootContact = new Contact(0, 0, numeric_limits<double>::max(), eid_, eid_, 0, 1.0);
	Work rootWork;
	rootWork.contact = rootContact;
	rootWork.arrivalTime = simTime_;
	rootContact->work = &rootWork;

	// Create and initialize working area in each contact.
	for (vector<Contact>::iterator it = contactPlan_->getContacts()->begin(); it != contactPlan_->getContacts()->end();
			++it) {
		(*it).work = new Work;
		((Work *) (*it).work)->contact = &(*it);
		((Work *) (*it).work)->visitedNodes.clear();
		((Work *) (*it).work)->arrivalTime = numeric_limits<double>::max();
		((Work *) (*it).work)->predecessor = 0;
		((Work *) (*it).work)->visited = false;

		// Suppress contacts as indicated in the suppressed list
		if (find(suppressedContactIds.begin(), suppressedContactIds.end(), (*it).getId()) != suppressedContactIds.end())
			((Work *) (*it).work)->suppressed = true;
		else
			((Work *) (*it).work)->suppressed = false;
	}

	// Start Dijkstra
	Contact * currentContact = rootContact;
	Contact * finalContact = NULL;
	double earliestFinalArrivalTime = numeric_limits<double>::max();

	//cout << "  surfing contact-graph:";
	while (1) {
		// increment counter
		dijkstraLoops++;

		//cout << currentContact->getDestinationEid() << ",";

		// Get local neighbor set and evaluate them
		vector<Contact> currentNeighbors = contactPlan_->getContactsBySrc(currentContact->getDestinationEid());
		for (vector<Contact>::iterator it = currentNeighbors.begin(); it != currentNeighbors.end(); ++it) {
			// If this contact is suppressed/visited, ignore it.
			if (((Work *) (*it).work)->suppressed || ((Work *) (*it).work)->visited)
				continue;

			// If this contact is finished, ignore it.
			if ((*it).getEnd() <= ((Work *) (currentContact->work))->arrivalTime)
				continue;

			// If the residual volume is 0, ignore it.
			if ((*it).getResidualVolume() == 0)
				continue;

			// If this contact leads to visited node, ignore it.
			vector<int> * v = &((Work *) (currentContact->work))->visitedNodes;
			if (std::find(v->begin(), v->end(), (*it).getDestinationEid()) != v->end())
				continue;

			// Get owlt (one way light time). If none found, ignore contact
//			double owlt = contactPlan_->getRangeBySrcDst((*it).getSourceEid(), (*it).getDestinationEid());
//			if(owlt==-1)
//				continue;
			double owlt = 0;
			double owltMargin = ((MAX_SPEED_MPH / 3600) * owlt) / 186282;
			owlt += owltMargin;

			// Calculate the cost for this contact (Arrival Time)
			double arrivalTime;
			if ((*it).getStart() < ((Work *) (currentContact->work))->arrivalTime)
				arrivalTime = ((Work *) (currentContact->work))->arrivalTime;
			else
				arrivalTime = (*it).getStart();
			arrivalTime += owlt;

			// Update the cost if better or equal
			if (arrivalTime < ((Work *) (*it).work)->arrivalTime) {
				((Work *) (*it).work)->arrivalTime = arrivalTime;
				((Work *) (*it).work)->predecessor = currentContact;
				((Work *) (*it).work)->visitedNodes = ((Work *) (currentContact->work))->visitedNodes;
				((Work *) (*it).work)->visitedNodes.push_back((*it).getDestinationEid());

				// Mark if destination reached
				if ((*it).getDestinationEid() == terminusNode)
					if (((Work *) (*it).work)->arrivalTime < earliestFinalArrivalTime) {
						earliestFinalArrivalTime = ((Work *) (*it).work)->arrivalTime;
						finalContact = contactPlan_->getContactById((*it).getId());
					}
			}
		}

		// End exploring next hop contact, mark current as visited
		((Work *) (currentContact->work))->visited = true;

		// Select next (best) contact to move to in next iteration
		Contact * nextContact = NULL;
		double earliestArrivalTime = numeric_limits<double>::max();
		for (vector<Contact>::iterator it = contactPlan_->getContacts()->begin();
				it != contactPlan_->getContacts()->end(); ++it) {
			// Do not evaluate suppressed or visited contacts
			if (((Work *) (*it).work)->suppressed || ((Work *) (*it).work)->visited)
				continue;

			// If the arrival time is worst than the best found so far, ignore
			if (((Work *) (*it).work)->arrivalTime > earliestFinalArrivalTime)
				continue;

			// Then this might be the best candidate contact
			if (((Work *) (*it).work)->arrivalTime < earliestArrivalTime) {
				nextContact = &(*it);
				earliestArrivalTime = ((Work *) (*it).work)->arrivalTime;
			}
		}
		if (nextContact == NULL)
			break; // No next contact found, exit search (while(1))

		// Update next contact and go with next itartion
		currentContact = nextContact;
	}

	// End contact graph exploration
	//cout << endl;

	// If we got a final contact to destination
	// then it is the best route and we need to
	// recover the data from the work area
	if (finalContact != NULL) {
		route->arrivalTime = earliestFinalArrivalTime;
		route->confidence = 1.0;

		double earliestEndTime = numeric_limits<double>::max();
		double maxVolume = numeric_limits<double>::max();

		// Go through all contacts in the path
		for (Contact * contact = finalContact; contact != rootContact; contact =
				((Work *) (*contact).work)->predecessor) {
			// Get earliest end time
			if (contact->getEnd() < earliestEndTime)
				earliestEndTime = contact->getEnd();

			// Get the minimal capacity
			// (TODO: this calculation assumes non-overlapped contacts, can be made more accurate)
			if (contact->getVolume() < maxVolume)
				maxVolume = contact->getVolume();

			// Update confidence
			route->confidence *= contact->getConfidence();

			// Store hop
			route->hops.insert(route->hops.begin(), contact);
		}

		route->terminusNode = terminusNode;
		route->nextHop = route->hops[0]->getDestinationEid();
		route->fromTime = route->hops[0]->getStart();
		route->toTime = earliestEndTime;
		route->maxVolume = maxVolume;
		route->residualVolume = maxVolume;
	} else {
		// No route found
		route->terminusNode = NO_ROUTE_FOUND;
		route->nextHop = NO_ROUTE_FOUND;
		route->arrivalTime = numeric_limits<double>::max();			// never chosen as best route
	}

	// Delete working area in each contact.
	for (vector<Contact>::iterator it = contactPlan_->getContacts()->begin(); it != contactPlan_->getContacts()->end();
			++it) {
		delete ((Work *) (*it).work);
	}
}

/////////////////////////////////////////////////
// Auxiliary Functions
/////////////////////////////////////////////////

// This function set all route list to size 0
// (like the initial condition of the class)
void RoutingCgrModelRev17::clearRouteTable() {

	for (int n = 0; n < nodeNum_; n++) {
		routeTable_.at(n).clear();
	}
}

void RoutingCgrModelRev17::printRouteTable(int terminusNode) {
	// Print route table for this destination
	for (unsigned int r = 0; r < routeTable_.at(terminusNode).size(); r++) {
		CgrRoute route = routeTable_.at(terminusNode).at(r);
		if (route.nextHop == NO_ROUTE_FOUND)
			cout << "routeTable[" << terminusNode << "][" << r << "]: No route found" << endl;
		else if (route.nextHop == EMPTY_ROUTE) // should never happen
			cout << "routeTable[" << terminusNode << "][" << r << "]: Need to recalculate route" << endl;
		else {
			cout << "routeTable[" << terminusNode << "][" << r << "]: nextHop: " << route.nextHop << ", frm "
					<< route.fromTime << " to " << route.toTime << ", arrival time: " << route.arrivalTime
					<< ", volume: " << route.residualVolume << "/" << route.maxVolume << "Bytes: ";

			// print route:
			for (vector<Contact *>::iterator ith = route.hops.begin(); ith != route.hops.end(); ++ith)
				cout << "(+" << (*ith)->getStart() << "+" << (*ith)->getEnd() << " " << (*ith)->getSourceEid() << " "
						<< (*ith)->getDestinationEid() << ")";
			cout << endl;
		}
	}
}

// This functions is used to determine the best route out of a route list.
// Must returns true if first argument is better (i.e., minor)
bool RoutingCgrModelRev17::compareRoutes(CgrRoute i, CgrRoute j) {

	// If one is filtered, the other is the best
	if (i.filtered && !j.filtered)
		return false;
	if (!i.filtered && j.filtered)
		return true;

	// If both are not filtered, then compare criteria,
	// If both are filtered, return any of them.

	// criteria 1) lowest arrival time
	if (i.arrivalTime < j.arrivalTime)
		return true;
	else if (i.arrivalTime > j.arrivalTime)
		return false;
	else {
		// if equal, criteria 2) lowest hop count
		if (i.hops.size() < j.hops.size())
			return true;
		else if (i.hops.size() > j.hops.size())
			return false;
		else {
			// if equal, criteria 3) larger residual volume
			if (i.residualVolume > j.residualVolume)
				return true;
			else if (i.residualVolume < j.residualVolume)
				return false;
			else {
				// if equal, first is better.
				return true;
			}
		}
	}
}

// Verify the routingType string contains all necessary parameters
void RoutingCgrModelRev17::checkRoutingTypeString() {
	// If no routing type set, set a default one
	if (routingType_.compare("none") == 0) {
		routingType_ = "contactPlan:local,routeListType:perNeighborBestPath,volumeAware:allContacts,extensionBlock:off";
		cout << "NODE: " << eid_ << ", DEFAULT rouingType string: " << routingType_ << endl;
	}

	// Check contactPlan
	if (routingType_.find("contactPlan:local") == std::string::npos
			&& routingType_.find("contactPlan:global") == std::string::npos) {
		cout << "dtnsim error: unknown or missing contactPlan type in routingType string: " << routingType_ << endl;
		exit(1);
	}

	// Check routeListType
	if (routingType_.find("routeListType:allPaths-yen") == std::string::npos
			&& routingType_.find("routeListType:allPaths-initial+anchor") == std::string::npos
			&& routingType_.find("routeListType:allPaths-firstEnding") == std::string::npos
			&& routingType_.find("routeListType:allPaths-firstDepleted") == std::string::npos
			&& routingType_.find("routeListType:oneBestPath") == std::string::npos
			&& routingType_.find("routeListType:perNeighborBestPath") == std::string::npos) {
		cout << "dtnsim error: unknown or missing routeListType type in routingType string: " << routingType_ << endl;
		exit(1);
	}

	// Check volumeAware
	if (routingType_.find("volumeAware:off") == std::string::npos
			&& routingType_.find("volumeAware:1stContact") == std::string::npos
			&& routingType_.find("volumeAware:allContacts") == std::string::npos) {
		cout << "dtnsim error: unknown or missing volumeAware type in routingType string: " << routingType_ << endl;
		exit(1);
	}

	// Check extensionBlock
	if (routingType_.find("extensionBlock:on") == std::string::npos
			&& routingType_.find("extensionBlock:off") == std::string::npos) {
		cout << "dtnsim error: unknown or missing extensionBlock type in routingType string: " << routingType_ << endl;
		exit(1);
	}

	//cout << "NODE: " << eid_ << ", rouingType string: " << routingType_ << endl;
}

//////////////////////
// Stats recollection
//////////////////////

int RoutingCgrModelRev17::getDijkstraCalls() {
	return dijkstraCalls;
}

int RoutingCgrModelRev17::getDijkstraLoops() {
	return dijkstraLoops;
}

int RoutingCgrModelRev17::getRouteTableEntriesCreated() {
	return tableEntriesCreated;
}

int RoutingCgrModelRev17::getRouteTableEntriesExplored() {
	return tableEntriesExplored;
}
