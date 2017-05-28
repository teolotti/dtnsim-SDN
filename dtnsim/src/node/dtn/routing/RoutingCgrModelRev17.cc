#include <dtn/routing/RoutingCgrModelRev17.h>

// This function initializes the routing class:
// To this end, local eid_, the total number of nodes nodeNum_,
// a pointer to local storage sdr_, and a contact plan are set.
RoutingCgrModelRev17::RoutingCgrModelRev17(int eid, int nodeNum, SdrModel * sdr, ContactPlan * localContactPlan, ContactPlan * globalContactPlan, string routingType, bool printDebug)
{
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

	for (int n1 = 0; n1 < nodeNum_; n1++)
		routeTable_.at(n1).resize(nodeNum_);

	this->clearRouteTable();
}

RoutingCgrModelRev17::~RoutingCgrModelRev17()
{

}

// This function is called every time a new bundle (local or
// in transit) need to be routed.The outcome of the function
// is to enqueue the bundle in the SDR memory which is organized
// by a set of queues addressed by queueIds. In current DtnSim
// version Ids corresponds with the contact Id where the bundle
// is expected to be forwarded. This mimic ION behaviour. Other
// implementations do enqueue bundles on a per neighbour-node basis.
void RoutingCgrModelRev17::routeAndQueueBundle(BundlePkt * bundle, double simTime)
{
	// Disable cout if degug disabled
	if (printDebug_ == false)
		cout.setstate(std::ios_base::failbit);

	// Reset counters (metrics)
	dijkstraCalls = 0;
	dijkstraLoops = 0;
	tableEntriesExplored = 0;

	cout << "TIME: " << simTime << "s, NODE: " << eid_ << ", routing bundle to dst: " << bundle->getDestinationEid() << " (" << bundle->getByteLength() << "Bytes)" << endl;

	// If no extensionBlock, run cgr each time a bundle is dispatched
	if (routingType_.find("extensionBlock:off") != std::string::npos)
		this->cgrForward(bundle, simTime);

	// If extensionBlock, check header
	if (routingType_.compare("extensionBlock:on") == 0)
	{
		if (bundle->getCgrRoute().nextHop == EMPTY_ROUTE)
		{
			// Empty extension block: Calculate, encode a new path and enqueue
			this->cgrForward(bundle, simTime);
		}
		else
		{
			// Non-empty EB: Use EB Route to route bundle
			CgrRoute ebRoute = bundle->getCgrRoute();

			// Print route and hops
			cout << "extensionBlockRoute: [" << ebRoute.terminusNode << "][" << ebRoute.nextHop << "]: nextHop: " << ebRoute.nextHop << ", frm " << ebRoute.fromTime << " to " << ebRoute.toTime << ", arrival time: " << ebRoute.arrivalTime << ", volume: " << ebRoute.residualVolume << "/"
					<< ebRoute.maxVolume << "Bytes" << endl << "hops: ";
			for (vector<Contact *>::iterator hop = ebRoute.hops.begin(); hop != ebRoute.hops.end(); ++hop)
				cout << "(+" << (*hop)->getStart() << " +" << (*hop)->getEnd() << " " << (*hop)->getSourceEid() << " " << (*hop)->getDestinationEid() << " " << (*hop)->getResidualVolume() << "/" << (*hop)->getVolume() << "Bytes) ";
			cout << endl;

			// Check if encoded path is valid, update local contacts and route hops
			bool ebRouteIsValid = true;
			vector<Contact *> newHops;
			for (vector<Contact *>::iterator hop = ebRoute.hops.begin(); hop != ebRoute.hops.end(); ++hop)
			{
				// TODO: Fuse the ebRoute information with local contact graph
				// But beware that this hop points to a remote contact graph.

				if ((*hop)->getDestinationEid() == eid_)
					// This is the contact that made this bundle arrive here,
					// do not check nothing and discard it from the newHops path
					continue;

				if (routingType_.compare("volumeAware:1stContact") == 0)
					// Only check first contact volume
					if ((*hop)->getSourceEid() == eid_)
						if (contactPlan_->getContactById((*hop)->getId())->getResidualVolume() < bundle->getByteLength())
						{
							// Not enough residual capacity from local view of the path
							ebRouteIsValid = false;
							break;
						}

				if (routingType_.compare("volumeAware:allContacts") == 0)
					// Check all contacts volume
					if (contactPlan_->getContactById((*hop)->getId())->getResidualVolume() < bundle->getByteLength())
					{
						// Not enough residual capacity from local view of the path
						ebRouteIsValid = false;
						break;
					}

				// store in newHops the local contacts
				newHops.push_back(contactPlan_->getContactById((*hop)->getId()));
			}

			if (ebRouteIsValid)
			{
				// Update necesary route parameters (from/to time and max/residual volume not necesary)
				ebRoute.hops = newHops;
				ebRoute.nextHop = ebRoute.hops[0]->getDestinationEid();

				// Enqueue using this route
				cout << "Using EB Route in header" << endl;
				this->cgrEnqueue(bundle, &ebRoute);
			}
			else
			{
				// Discard old extension block, calculate and encode a new path and enqueue
				cout << "EB Route in header not valid, generating a new one" << endl;
				this->cgrForward(bundle, simTime);
			}
		}
	}

	// Re-enable cout if degug disabled
	if (printDebug_ == false)
		cout.clear();
}

/////////////////////////////////////////////////
// Functions based Ion architecture
/////////////////////////////////////////////////

// This function tries to find the best path for the current bundle.
// Initially it checks if the route table is up-to-date and update it
// if it is outdated (by running a new Dijkstra search for the
// corresponding neighbor).
void RoutingCgrModelRev17::cgrForward(BundlePkt * bundle, double simTime)
{
	// If contact plan was changed, empty route list
	if (contactPlan_->getLastEditTime() > routeTableLastEditTime)
		this->clearRouteTable();

	routeTableLastEditTime = simTime;

	int terminusNode = bundle->getDestinationEid();

	// Check route list for terminus node and recalculate if necesary depending on
	// the routeListType configured in the routingType string
	if (routingType_.find("routeListType:allPaths-yen") != std::string::npos)
	{
		cout << "dtnsim error: routeListType:allPaths-yen not implemented yet" << endl;
		exit(1);
	}
	if (routingType_.find("routeListType:allPaths-initial+anchor") != std::string::npos)
	{
		cout << "dtnsim error: routeListType:allPaths-initial+anchor not implemented yet" << endl;
		exit(1);
	}
	if (routingType_.find("routeListType:allPaths-firstEnding") != std::string::npos)
	{
		cout << "dtnsim error: routeListType:allPaths-firstEnding not implemented yet" << endl;
		exit(1);

	}
	if (routingType_.find("routeListType:allPaths-firstDepleted") != std::string::npos)
	{
		cout << "dtnsim error: routeListType:allPaths-firstDepleted not implemented yet" << endl;
		exit(1);
	}
	if (routingType_.find("routeListType:oneBestPath") != std::string::npos)
	{
		cout << "dtnsim error: routeListType:oneBestPath not implemented yet" << endl;
		exit(1);
	}
	if (routingType_.find("routeListType:perNeighborBestPath") != std::string::npos)
		for (int nodeEID = 1; nodeEID < nodeNum_; nodeEID++)
		{
			// NO_ROUTE_FOUND does not trigger a recalculation
			if (routeTable_.at(terminusNode).at(nodeEID).nextHop == NO_ROUTE_FOUND)
				continue;

			bool needRecalculation = false;

			// Empty route condition
			if (routeTable_.at(terminusNode).at(nodeEID).nextHop == EMPTY_ROUTE)
				needRecalculation = true;

			// Due route condition
			if (routeTable_.at(terminusNode).at(nodeEID).toTime < simTime)
				needRecalculation = true;

			// Depleted route condition
			if (routeTable_.at(terminusNode).at(nodeEID).residualVolume < bundle->getByteLength())
			{
				// Make sure that the capacity-limiting contact is marked as depleted
				vector<Contact *>::iterator hop;
				for (hop = routeTable_.at(terminusNode).at(nodeEID).hops.begin(); hop != routeTable_.at(terminusNode).at(nodeEID).hops.end(); ++hop)
					if (routeTable_.at(terminusNode).at(nodeEID).residualVolume == (*hop)->getResidualVolume())
						(*hop)->setResidualVolume(0);

				needRecalculation = true;
			}

			if (needRecalculation)
			{
				CgrRoute route;
				this->findNextBestRoute(nodeEID, terminusNode, &route, simTime);
				routeTable_.at(terminusNode).at(nodeEID) = route;
			}

			tableEntriesExplored++;
		}

	// Print route table for this terminus
	this->printRouteTable(terminusNode);

	// Select best route
	vector<CgrRoute>::iterator bestRoute;
	if (bundle->getReturnToSender() == true)
	{
		// consider all routes in table
		bestRoute = min_element(routeTable_.at(terminusNode).begin(), routeTable_.at(terminusNode).end(), this->compareRoutes);
	}
	else
	{
		// Do not consider route back to sender (achieved bytemporaly set arrivalTime to infinite)
		// This variant disables the forwarding back to sender node. This a feature currently
		// operating in ION which mitigates the formation routing loops. However, it is well-known
		// this does not completly avoid the issue as the route loop can still be formed by a
		// third node that can reach the sender node instead. This situations are generally provoked
		// by congestion and are very hard to solve via in-band protocols.
		// See paper http://onlinelibrary.wiley.com/doi/10.1002/sat.1210/abstract for a more general discussion.
		double arrivalTime = routeTable_.at(terminusNode).at(bundle->getSenderEid()).arrivalTime;
		routeTable_.at(terminusNode).at(bundle->getSenderEid()).arrivalTime = numeric_limits<double>::max();
		bestRoute = min_element(routeTable_.at(terminusNode).begin(), routeTable_.at(terminusNode).end(), this->compareRoutes);
		routeTable_.at(terminusNode).at(bundle->getSenderEid()).arrivalTime = arrivalTime;
	}

	// Enqueue bundle to route and update volumes
	this->cgrEnqueue(bundle, &(*bestRoute));
}

// This function enqueues the bundle in the best found path.
// To this end, it updates contacts volume depending on the volume-awareness
// type configured for the routing routine.
void RoutingCgrModelRev17::cgrEnqueue(BundlePkt * bundle, CgrRoute *bestRoute)
{
	if (bestRoute->nextHop != NO_ROUTE_FOUND)
	{
		cout << "*Best: routeTable[" << bestRoute->terminusNode << "][" << bestRoute->nextHop << "]: nextHop: " << bestRoute->nextHop << ", frm " << bestRoute->fromTime << " to " << bestRoute->toTime << ", arrival time: " << bestRoute->arrivalTime << ", volume: " << bestRoute->residualVolume << "/"
				<< bestRoute->maxVolume << "Bytes" << endl;

		// Update residualVolume: all contact
		if (routingType_.find("volumeAware:allContacts") != std::string::npos)
		{
			// Update residualVolume of all this route hops
			for (vector<Contact *>::iterator hop = bestRoute->hops.begin(); hop != bestRoute->hops.end(); ++hop)
				(*hop)->setResidualVolume((*hop)->getResidualVolume() - bundle->getByteLength());

			// Update residualVolume of all routes that uses the updated hops
			for (int n1 = 1; n1 < nodeNum_; n1++)
				for (int n2 = 1; n2 < nodeNum_; n2++) //TODO: explore routeList range, this only works for routeListType:perNeighborBestPath
					for (vector<Contact *>::iterator hop1 = routeTable_.at(n1).at(n2).hops.begin(); hop1 != routeTable_.at(n1).at(n2).hops.end(); ++hop1)
						for (vector<Contact *>::iterator hop2 = bestRoute->hops.begin(); hop2 != bestRoute->hops.end(); ++hop2)
							if ((*hop1)->getId() == (*hop2)->getId())
								// Does the reduction of this contact volume requires a route volume update?
								if (routeTable_.at(n1).at(n2).residualVolume > (*hop1)->getResidualVolume())
								{
									routeTable_.at(n1).at(n2).residualVolume = (*hop1)->getResidualVolume();
									cout << "*Rvol: routeTable[" << n1 << "][" << n2 << "]: updated to " << (*hop1)->getResidualVolume() << "Bytes (all contacts)" << endl;
								}
		}

		// Update residualVolume: 1st contact
		if (routingType_.find("volumeAware:1stContact") != std::string::npos)
		{
			// Update residualVolume of the first hop
			bestRoute->hops[0]->setResidualVolume(bestRoute->hops[0]->getResidualVolume() - bundle->getByteLength());

			// Update residualVolume of the used route (TODO: shall we also check other routes?)
			bestRoute->residualVolume = bestRoute->hops[0]->getResidualVolume();

			cout << "*Rvol: routeTable[" << bestRoute->terminusNode << "][" << bestRoute->nextHop << "]: updated to " << bestRoute->hops[0]->getResidualVolume() << "Bytes (1st contact)" << endl;
		}

		// Save CgrRoute in header
		if (routingType_.find("extensionBlock:on") != std::string::npos)
			bundle->setCgrRoute(*bestRoute);

		// Enqueue bundle
		bundle->setNextHopEid(bestRoute->nextHop);
		sdr_->enqueueBundleToContact(bundle, bestRoute->hops.at(0)->getId());
	}
	else
	{
		// Enqueue to Limbo
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
// valid and related to the current Dijstra search is stored.
void RoutingCgrModelRev17::findNextBestRoute(int entryNode, int terminusNode, CgrRoute * route, double simTime)
{
	// increment counter
	dijkstraCalls++;

	// Create rootContact and its corresponding rootWork
	// id=0, start=0, end=inf, src=me, dst=me, rate=0, conf=1
	Contact * rootContact = new Contact(0, 0, numeric_limits<double>::max(), eid_, eid_, 0, 1.0);
	Work rootWork;
	rootWork.contact = rootContact;
	rootWork.arrivalTime = simTime;
	rootContact->work = &rootWork;

	// Create and initialize working area in each contact
	// Supress next hop contacts though nodes different than entryNode
	vector<Contact>::iterator it;
	//cout << "  suppressing initial contacts: ";
	for (it = contactPlan_->getContacts()->begin(); it != contactPlan_->getContacts()->end(); ++it)
	{
		(*it).work = new Work;
		((Work *) (*it).work)->contact = &(*it);
		((Work *) (*it).work)->visitedNodes.clear();
		((Work *) (*it).work)->arrivalTime = numeric_limits<double>::max();
		((Work *) (*it).work)->predecessor = 0;
		((Work *) (*it).work)->visited = false;
		((Work *) (*it).work)->suppressed = false;

		if ((*it).getSourceEid() == eid_ && (*it).getDestinationEid() != entryNode)
		{
			((Work *) (*it).work)->suppressed = true;
			//cout << (*it).getId() << ", ";
		}
	}
	//cout << endl;

	// Start Dijkstra
	Contact * currentContact = rootContact;
	Contact * finalContact = NULL;
	double earliestFinalArrivalTime = numeric_limits<double>::max();

	//cout << "  surfing contact-graph:";
	while (1)
	{
		// increment counter
		dijkstraLoops++;

		//cout << currentContact->getDestinationEid() << ",";

		// Get local neighbor set and evaluate them
		vector<Contact> currentNeighbors = contactPlan_->getContactsBySrc(currentContact->getDestinationEid());
		for (vector<Contact>::iterator it = currentNeighbors.begin(); it != currentNeighbors.end(); ++it)
		{
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

			// Get owlt (TODO: get it from contact plan)
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
			if (arrivalTime < ((Work *) (*it).work)->arrivalTime)
			{
				((Work *) (*it).work)->arrivalTime = arrivalTime;
				((Work *) (*it).work)->predecessor = currentContact;
				((Work *) (*it).work)->visitedNodes = ((Work *) (currentContact->work))->visitedNodes;
				((Work *) (*it).work)->visitedNodes.push_back((*it).getDestinationEid());

				// Mark if destination reached
				if ((*it).getDestinationEid() == terminusNode)
					if (((Work *) (*it).work)->arrivalTime < earliestFinalArrivalTime)
					{
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
		for (vector<Contact>::iterator it = contactPlan_->getContacts()->begin(); it != contactPlan_->getContacts()->end(); ++it)
		{
			// Do not evaluate suppressed or visited contacts
			if (((Work *) (*it).work)->suppressed || ((Work *) (*it).work)->visited)
				continue;

			// If the arrival time is worst than the best found so far, ignore
			if (((Work *) (*it).work)->arrivalTime > earliestFinalArrivalTime)
				continue;

			// Then this might be the best candidate contact
			if (((Work *) (*it).work)->arrivalTime < earliestArrivalTime)
			{
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
	if (finalContact != NULL)
	{
		route->arrivalTime = earliestFinalArrivalTime;
		route->confidence = 1.0;

		double earliestEndTime = numeric_limits<double>::max();
		double maxVolume = numeric_limits<double>::max();

		// Go through all contacts in the path
		for (Contact * contact = finalContact; contact != rootContact; contact = ((Work *) (*contact).work)->predecessor)
		{
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
	}
	else
	{
		// No route found
		route->terminusNode = NO_ROUTE_FOUND;
		route->nextHop = NO_ROUTE_FOUND;
		route->arrivalTime = numeric_limits<double>::max(); // never chosen as best route
	}
}

/////////////////////////////////////////////////
// Auxiliary Functions
/////////////////////////////////////////////////

void RoutingCgrModelRev17::clearRouteTable()
{
	for (int n1 = 0; n1 < nodeNum_; n1++)
		for (int n2 = 0; n2 < nodeNum_; n2++)
		{
			CgrRoute route;
			route.nextHop = EMPTY_ROUTE;
			route.arrivalTime = numeric_limits<double>::max(); // never chosen as best route
			routeTable_.at(n1).at(n2) = route;
		}
}

void RoutingCgrModelRev17::printRouteTable(int terminusNode)
{
	// Print route table for this destination
	for (int nodeEID = 1; nodeEID < nodeNum_; nodeEID++)
	{
		CgrRoute route = routeTable_.at(terminusNode).at(nodeEID);
		if (route.nextHop == NO_ROUTE_FOUND)
			cout << "routeTable[" << terminusNode << "][" << nodeEID << "]: No route found" << endl;
		else if (route.nextHop == EMPTY_ROUTE) // should never happen
			cout << "routeTable[" << terminusNode << "][" << nodeEID << "]: Need to recalculate route" << endl;
		else
		{
			cout << "routeTable[" << terminusNode << "][" << nodeEID << "]: nextHop: " << route.nextHop << ", frm " << route.fromTime << " to " << route.toTime << ", arrival time: " << route.arrivalTime << ", volume: " << route.residualVolume << "/" << route.maxVolume << "Bytes: ";

			// print route:
			for (vector<Contact *>::iterator ith = route.hops.begin(); ith != route.hops.end(); ++ith)
				cout << "(+" << (*ith)->getStart() << "+" << (*ith)->getEnd() << " " << (*ith)->getSourceEid() << " " << (*ith)->getDestinationEid() << ")";
			cout << endl;
		}
	}
}

bool RoutingCgrModelRev17::compareRoutes(CgrRoute i, CgrRoute j)
{
	// Returns true if first argument is minor (i.e., better)
	if (i.arrivalTime < j.arrivalTime)
		return true;
	else if (i.arrivalTime > j.arrivalTime)
		return false;
	else
	{
		// equal arrivalTime, evaluate residual volume
		if (i.residualVolume > j.residualVolume)
			return true;
		else if (i.residualVolume < j.residualVolume)
			return false;
		else
		{
			// equal residual volume, evaluate hop count
			if (i.hops.size() < j.hops.size())
				return true;
			else
				return false;
		}
	}
}

// Verify the routingType string contains all necesary parameters
void RoutingCgrModelRev17::checkRoutingTypeString()
{
	// If no routing type set, set a default one
	if (routingType_.compare("none") == 0)
	{
		routingType_ = "contactPlan:local,routeListType:perNeighborBestPath,volumeAware:allContacts,extensionBlock:off";
		cout << "NODE: " << eid_ <<", DEFAULT rouingType string: " << routingType_ << endl;
	}

	// Check contactPlan
	if (routingType_.find("contactPlan:local") == std::string::npos && routingType_.find("contactPlan:global") == std::string::npos)
	{
		cout << "dtnsim error: unknown or missing contactPlan type in routingType string: " << routingType_ << endl;
		exit(1);
	}

	// Check routeListType
	if (routingType_.find("routeListType:allPaths-yen") == std::string::npos && routingType_.find("routeListType:allPaths-initial+anchor") == std::string::npos && routingType_.find("routeListType:allPaths-firstEnding") == std::string::npos
			&& routingType_.find("routeListType:allPaths-firstDepleted") == std::string::npos && routingType_.find("routeListType:oneBestPath") == std::string::npos && routingType_.find("routeListType:perNeighborBestPath") == std::string::npos)
	{
		cout << "dtnsim error: unknown or missing routeListType type in routingType string: " << routingType_ << endl;
		exit(1);
	}

	// Check volumeAware
	if (routingType_.find("volumeAware:off") == std::string::npos && routingType_.find("volumeAware:1stContact") == std::string::npos && routingType_.find("volumeAware:allContacts") == std::string::npos)
	{
		cout << "dtnsim error: unknown or missing volumeAware type in routingType string: " << routingType_ << endl;
		exit(1);
	}

	// Check extensionBlock
	if (routingType_.find("extensionBlock:on") == std::string::npos && routingType_.find("extensionBlock:off") == std::string::npos)
	{
		cout << "dtnsim error: unknown or missing extensionBlock type in routingType string: " << routingType_ << endl;
		exit(1);
	}

	cout << "NODE: " << eid_ << ", rouingType string: " << routingType_ << endl;
}

//////////////////////
// Stats recollection
//////////////////////

int RoutingCgrModelRev17::getDijkstraCalls()
{
	return dijkstraCalls;
}

int RoutingCgrModelRev17::getDijkstraLoops()
{
	return dijkstraLoops;
}

int RoutingCgrModelRev17::getRouteTableEntriesExplored()
{
	return tableEntriesExplored;
}
