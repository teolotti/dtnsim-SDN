#include <src/distinctRegions/node/dtn/routing/RoutingCgrModelYen.h>

namespace dtnsimdistinct {

RoutingCgrModelYen::RoutingCgrModelYen(int eid, SdrModel *sdr, map<string, ContactPlan> &contactPlans, bool printDebug) : RoutingDeterministic(eid, sdr, contactPlans) {
	printDebug_ = printDebug;
}

RoutingCgrModelYen::~RoutingCgrModelYen() {
}

void RoutingCgrModelYen::routeAndQueueBundle(BundlePacket *bundle, double simTime, int destinationEid) {

	if (!printDebug_) // disable cout if degug disabled
		cout.setstate(std::ios_base::failbit);

	// Reset counters
	dijkstraCalls = 0;
	dijkstraLoops = 0;
	nodesOneDijkstra = 0;
	edgesOneDijkstra = 0;

	tableEntriesCreated = 0;
	tableEntriesExplored = 0;
	routeTableSize = 0;

	routeSearchStarts = 0;
	yenIterations = 0;

	// Call cgrForward from ion (route and forwarding)
	cgrForward(bundle, simTime, destinationEid);

	if (!printDebug_)
		cout.clear();
}

void RoutingCgrModelYen::cgrForward(BundlePacket *bundle, double simTime, int destinationEid) {

	currentContactPlan_ = &contactPlans_.at(currentRegion_);

	cout << "TIME: " << simTime << "s, at NODE: " << eid_
			<< ", routing bundle to Dst: " << destinationEid << endl;

	// Remove all terminated routes that have been previously computed
	pruneRouteList(simTime, destinationEid);

	// Collect statistic (sum of number of routes saved for all destinations)
	for (auto & list : routeLists_) {
		routeTableSize += list.second.size();
	}

	// Candidate route(s) for current bundle
	vector<CgrRoute> candidateRoutes;
	identifyCandidateRoutes(bundle, simTime, destinationEid, &candidateRoutes);

	// Select most suitable route out of candidates
	CgrRoute* selectedRoute = NULL;
	for (auto & route : candidateRoutes) {

		if (selectedRoute == NULL) {
			selectedRoute = &route;
		}

		else if (route.forwardingWork.pat < selectedRoute->forwardingWork.pat) {
			selectedRoute = &route;
		}

		else if (route.forwardingWork.pat > selectedRoute->forwardingWork.pat) {
			continue;
		}

		else if (route.hops.size() < selectedRoute->hops.size()) {
			selectedRoute = &route;
		}

		else if (route.hops.size() > selectedRoute->hops.size()) {
			continue;
		}

		else if (route.toTime > selectedRoute->toTime) {
			selectedRoute = &route;
		}

		else if (route.toTime < selectedRoute->toTime) {
			continue;
		}
	}

	if (selectedRoute != NULL) {
		bpEnqueue(bundle, selectedRoute);

	} else {
		cout << "Dropping Bundle - no route found" << endl;
		bundleDropped = true;
		delete bundle;
	}
}

void RoutingCgrModelYen::pruneRouteList(double simTime, int destinationEid) {

	vector<CgrRoute>::iterator it = routeLists_[destinationEid].begin();
	while (it != routeLists_[destinationEid].end()) {
		if (it->toTime < simTime) {
			it = routeLists_[destinationEid].erase(it);
		} else {
			++it;
		}
	}
}

// note: this is not quite Yen's CGR - the treatment of potential routes differs slightly
void RoutingCgrModelYen::identifyCandidateRoutes(BundlePacket *bundle, double simTime, int destinationEid, vector<CgrRoute> *candidateRoutes) {

	// Test, if any of the previously calculated routes are suitable for this bundle
	for (auto & route : routeLists_[destinationEid]) {
 		if (isCandidateRoute(bundle, &route, simTime)) {
			candidateRoutes->push_back(route);
		}
	}

	if (!candidateRoutes->empty()) {
		return;
	}

	routeSearchStarts++;

	// If none of the previously calculated routes are suitable, use Yen
	// to compute the next best route and add it to candidate list if suitable

	// Create rootContact
	Contact rootContact(0, 0, numeric_limits<double>::max(), eid_, eid_, 0, 0);
	rootContact.routeSearchWork.arrivalTime = simTime;

	// Clear working area of each contact
	currentContactPlan_->clearAllRouteSearchWorkingArea();
	currentContactPlan_->clearAllRouteManagementWorkingArea();

	// Determine best path and record it
	CgrRoute bestRoute;
	findNextBestRoute(&rootContact, destinationEid, &bestRoute);
	if (bestRoute.nextHopNode == -1) {
		return;
	} else {
		routeLists_[destinationEid].push_back(bestRoute);
		tableEntriesCreated++;
		if (isCandidateRoute(bundle, &bestRoute, simTime)) {
			candidateRoutes->push_back(bestRoute);
			return;
		}
	}

	bestRoute.hops.insert(bestRoute.hops.begin(), &rootContact);
	vector<CgrRoute> potentialRoutes;
	potentialRoutes.push_back(bestRoute);

	int k = 0;
	while (candidateRoutes->empty() && k < 500) { //TODO make cap for K adjustable

		cout << "STARTING NEW YEN ITERATION" << endl;
		yenIterations++;

		// Iterate through all contacts of most recently added route
		CgrRoute* mostRecentlyAddedRoute = &potentialRoutes.back();
		vector<Contact*> hops = mostRecentlyAddedRoute->hops;
		for (int spurIndex = 0; spurIndex < hops.size()-1; ++spurIndex) {

			Contact* spurContact = hops[spurIndex];

			// Create a root path for all spur contacts
			CgrRoute rootPath;
			for (size_t hopIndex = 0; hopIndex < spurIndex+1; ++hopIndex) {
				Contact* hop = hops[hopIndex];
				rootPath.hops.push_back(hop);
			}
			rootPath.refreshMetrics();

			// Clear working area of each contact
			currentContactPlan_->clearAllRouteSearchWorkingArea();
			currentContactPlan_->clearAllRouteManagementWorkingArea();

			// Suppress all hops of root path (except spur contact at end)
			for (auto & rootHop : rootPath.hops) {
				if (rootHop->getId() != spurContact->getId()) {
					rootHop->routeManagementWork.suppressed = true;
				}
			}

			// Suppress all edges from spurContact to neighbors
			// considered in previous routes with same rootPath
			for (auto & route : potentialRoutes) {
				bool isRootPathContained = true;
				vector<Contact *> hops = route.hops;
				for (size_t index = 0; index < rootPath.hops.size(); ++index) {
					Contact* rootHop = rootPath.hops[index];
					Contact* hop = hops[index];
					if (rootHop->getId() != hop->getId()) {
						isRootPathContained = false;
						break;
					}
				}
				if (isRootPathContained) {
					Contact* nextContact = hops[rootPath.hops.size()];
					if (!count(spurContact->routeManagementWork.suppressedNextContacts.begin(), spurContact->routeManagementWork.suppressedNextContacts.end(), nextContact->getId())) {
						spurContact->routeManagementWork.suppressedNextContacts.push_back(nextContact->getId());
					}
				}
			}

			spurContact->clearRouteSearchWorkingArea();
			spurContact->routeSearchWork.arrivalTime = rootPath.arrivalTime;
			for (auto & rootHop : rootPath.hops) {
				spurContact->routeSearchWork.visitedNodes.push_back(rootHop->getDestinationEid());
			}

			CgrRoute spurPath;
			findNextBestRoute(spurContact, destinationEid, &spurPath);
			if (spurPath.nextHopNode == -1) {
				continue; // try next spur contact
			} else {

				for(vector<Contact*>::reverse_iterator rootHop = rootPath.hops.rbegin(); rootHop != rootPath.hops.rend(); ++rootHop) {
					spurPath.hops.insert(spurPath.hops.begin(), *rootHop);
				}
				spurPath.refreshMetrics();
				spurPath.rootPathLength = rootPath.hops.size();
				potentialRoutes.push_back(spurPath);

				CgrRoute potentialCandidate;
				for(vector<Contact*>::reverse_iterator hop = spurPath.hops.rbegin(); hop != spurPath.hops.rend(); ++hop) {
					potentialCandidate.hops.insert(potentialCandidate.hops.begin(), *hop);
				}
				potentialCandidate.hops.erase(potentialCandidate.hops.begin());
				potentialCandidate.refreshMetrics();
				routeLists_[destinationEid].push_back(potentialCandidate);
				tableEntriesCreated++;
				if (isCandidateRoute(bundle, &potentialCandidate, simTime)) {
					candidateRoutes->push_back(potentialCandidate);
				}
			}
		} // loop through spur contacts of most recently added route
		k++;
	} // while candidateRoutes.empty() loop
}

bool RoutingCgrModelYen::isCandidateRoute(BundlePacket *bundle, CgrRoute* potentialRoute, double simTime) {

	// Every time we check if a route is a candidate, we explore a route table entry
	tableEntriesExplored++;

	// Ignore the route if the bundle is projected to arrive after its expired
	if (potentialRoute->arrivalTime > bundle->getTtl().dbl()) {
		return false;
	}

	// Ignore the route if it loops back to the original sender
	if (bundle->getSenderEid() == potentialRoute->nextHopNode) {
		return false;
	}

	// Earliest Transmission Opportunity Check
	double adjustedStartTime = max(simTime, potentialRoute->fromTime);
	double applicablePriorContactVolume = 0;
	double applicableBacklogVolume = sdr_->getBytesStoredInQueue(potentialRoute->nextHopNode) * 1.03;
	for (vector<Contact>::iterator contact = currentContactPlan_->getContacts()->begin(); contact != currentContactPlan_->getContacts()->end(); ++contact) {
		if ((contact->getSourceEid() == eid_) && (contact->getDestinationEid() == potentialRoute->nextHopNode)) {
			if ((contact->getEnd() > simTime) && (contact->getStart() < potentialRoute->fromTime)) {
				applicablePriorContactVolume += (contact->getEnd() - max(simTime, contact->getStart())) * contact->getDataRate();
			}
		}
	}

	double backlogTime = max(0.0, applicableBacklogVolume-applicablePriorContactVolume) / potentialRoute->hops.front()->getDataRate();
	double eto = adjustedStartTime + backlogTime;
	potentialRoute->forwardingWork.eto = eto;
	if (eto>potentialRoute->hops.front()->getEnd()) {
		return false;
	}

	// Projected Arrival Time Check
	potentialRoute->hops.front()->forwardingWork.fbtx = eto;
	for (size_t i = 1; i<potentialRoute->hops.size(); ++i) {
		Contact* hop = potentialRoute->hops[i];
		hop->forwardingWork.fbtx = max(hop->getStart(), potentialRoute->hops[i-1]->forwardingWork.lbrx);
		hop->forwardingWork.lbtx = hop->forwardingWork.fbtx + (bundle->getByteLength()*1.03)/hop->getDataRate();
		hop->forwardingWork.lbrx = hop->forwardingWork.lbtx + hop->getRange();
	}
	double pat = potentialRoute->hops.back()->forwardingWork.lbrx;
	potentialRoute->forwardingWork.pat = pat;
	if (pat > bundle->getTtl().dbl()) {
		return false;
	}

	// Effective Volume Limit Check
	double minEvl = numeric_limits<double>::max();
	for (size_t i = 0; i<potentialRoute->hops.size(); ++i) {
		double minEndTime = numeric_limits<double>::max();
		for (size_t j = i; j<potentialRoute->hops.size(); ++j){
			if (potentialRoute->hops[j]->getEnd() < minEndTime) {
				minEndTime = potentialRoute->hops[j]->getEnd();
			}
		}
		double effectiveStopTime = min(potentialRoute->hops[i]->getEnd(), minEndTime);
		double effectiveDuration = effectiveStopTime - potentialRoute->hops[i]->forwardingWork.fbtx;
		potentialRoute->hops[i]->forwardingWork.evl = min(effectiveDuration*potentialRoute->hops[i]->getDataRate(), potentialRoute->hops[i]->getRemainingCapacity());
		if (potentialRoute->hops[i]->forwardingWork.evl < minEvl) {
			minEvl = potentialRoute->hops[i]->forwardingWork.evl;
		}
	}
	potentialRoute->forwardingWork.evl = minEvl;
	if (minEvl <= 0) {
		return false;
	}

	return true;
}

void RoutingCgrModelYen::findNextBestRoute(Contact *rootContact, int destinationEid, CgrRoute *route) {

	// increment counter
	dijkstraCalls++;

	// Clear route search working area of all contacts except the passed root contact
	// (Yen's algorithm depends on the working area of passed root contact)
	currentContactPlan_->clearAllRouteSearchWorkingArea(rootContact->getId());

	// source and destination notational vertices
	nodesOneDijkstra += 2;

	Contact* currentContact = rootContact;
	Contact* finalContact = NULL;
	double earliestFinalArrivalTime = numeric_limits<double>::max();

	// Add root contact's receiving node to its visited nodes (if it's not there already)
	if (!count(rootContact->routeSearchWork.visitedNodes.begin(), rootContact->routeSearchWork.visitedNodes.end(), rootContact->getDestinationEid())) {
		rootContact->routeSearchWork.visitedNodes.push_back(rootContact->getDestinationEid());
	}

	while (true) {

		// increment counter
		dijkstraLoops++;

		vector<Contact*> currentNeighbors = currentContactPlan_->getContactsBySrc(currentContact->getDestinationEid());
		nodesOneDijkstra += currentNeighbors.size();
		edgesOneDijkstra += currentNeighbors.size();

		for (auto & neighbor : currentNeighbors) {

			// Yen's modification:
			if (count(currentContact->routeManagementWork.suppressedNextContacts.begin(), currentContact->routeManagementWork.suppressedNextContacts.end(), neighbor->getId())) {
				continue;
			}

			if (neighbor->routeManagementWork.suppressed) {
				continue;
			}

			if (neighbor->routeSearchWork.visited) {
				continue;
			}

			if (count(currentContact->routeSearchWork.visitedNodes.begin(), currentContact->routeSearchWork.visitedNodes.end(), neighbor->getDestinationEid())) {
				continue;
			}

			if (neighbor->getEnd() <= currentContact->routeSearchWork.arrivalTime) {
				continue;
			}

			//if ((currentContact->getSourceEid() == neighbor->getDestinationEid()) & (currentContact->getDestinationEid() == neighbor->getDestinationEid())) {
			//	continue;
			//}

			// Contact is considered

			double arrivalTime;
			if (neighbor->getStart() < currentContact->routeSearchWork.arrivalTime) {
				arrivalTime = currentContact->routeSearchWork.arrivalTime;
			} else {
				arrivalTime = neighbor->getStart();
			}
			arrivalTime += neighbor->getRange();

			// Update the cost of this contact
			if (arrivalTime < neighbor->routeSearchWork.arrivalTime) {

				neighbor->routeSearchWork.arrivalTime = arrivalTime;
				neighbor->routeSearchWork.predecessor = currentContact;
				neighbor->routeSearchWork.visitedNodes = currentContact->routeSearchWork.visitedNodes;
				neighbor->routeSearchWork.visitedNodes.push_back(neighbor->getDestinationEid());

				if (neighbor->getDestinationEid() == destinationEid) {

					if ((neighbor->routeSearchWork.arrivalTime) < earliestFinalArrivalTime) {
						earliestFinalArrivalTime = neighbor->routeSearchWork.arrivalTime;
						finalContact = neighbor;
					}
				}
			}
		} // end for (currentNeighbors)
		currentContact->routeSearchWork.visited = true;


		// Select next (best) contact to move to in next iteration
		Contact* nextContact = NULL;
		double earliestArrivalTime = numeric_limits<double>::max();
		for (vector<Contact>::iterator contact = currentContactPlan_->getContacts()->begin(); contact != currentContactPlan_->getContacts()->end(); ++contact) {

			// Do not evaluate suppressed or visited contacts
			if (contact->routeManagementWork.suppressed || contact->routeSearchWork.visited) {
				continue;
			}

			// If the arrival time is worse than the best found so far, ignore
			if ((contact->routeSearchWork.arrivalTime) > earliestFinalArrivalTime) {
				continue;
			}

			// Then this might be the best candidate contact
			if ((contact->routeSearchWork.arrivalTime) < earliestArrivalTime) {
				nextContact = &*contact;
				earliestArrivalTime = contact->routeSearchWork.arrivalTime;
			}
		}

		if (nextContact == NULL) {
			break; // No next contact found, exit search
		}

		// Update next contact and go with next iteration
		currentContact = nextContact;
	}

	// If we got a final contact to destination
	// then it is the best route
	if (finalContact != NULL) {

		// Go through all contacts in the path
		for (Contact* contact = finalContact; contact != rootContact; contact = contact->routeSearchWork.predecessor) {
			route->hops.insert(route->hops.begin(), contact);
		}

		route->refreshMetrics();
	}
}

void RoutingCgrModelYen::bpEnqueue(BundlePacket *bundle, CgrRoute *selectedRoute) {

	int nextHop = selectedRoute->nextHopNode;
	bundle->setNextHopEid(nextHop);
	bool enqueued = sdr_->enqueueBundle(nextHop, bundle, selectedRoute->hops);

	if (bundle->getFirstHopEid() == 0) {
		bundle->setFirstHopEid(nextHop);
	}

	if (enqueued) {

		// Decrease C.MAV of every hop
		for (auto & hop : selectedRoute->hops) {
			hop->setRemainingCapacity(hop->getRemainingCapacity()-(bundle->getByteLength() *1.03));
		}

	} else {
		cout << "Dropping Bundle failed enqueue" << endl;
		bundleDropped = true;
		delete bundle;
	}
}

//////////////////////
// Stats recollection
//////////////////////

double RoutingCgrModelYen::getDijkstraCalls() {
	return dijkstraCalls;
}

double RoutingCgrModelYen::getDijkstraLoops() {
	return dijkstraLoops;
}

double RoutingCgrModelYen::getRouteTableEntriesCreated() {
	return tableEntriesCreated;
}

double RoutingCgrModelYen::getRouteTableEntriesExplored() {
	return tableEntriesExplored;
}

double RoutingCgrModelYen::getEdgesOneDijkstra() const {
	return edgesOneDijkstra;
}

double RoutingCgrModelYen::getNodesOneDijkstra() const {
	return nodesOneDijkstra;
}

double RoutingCgrModelYen::getOneDijkstraComplexity() const {
	return ((edgesOneDijkstra + nodesOneDijkstra) * log(nodesOneDijkstra));
}

bool RoutingCgrModelYen::getBundleDropped() {
	return bundleDropped;
}

double RoutingCgrModelYen::getRouteTableSize() {
	return routeTableSize;
}

int RoutingCgrModelYen::getYenIterations() {
	return yenIterations;
}

int RoutingCgrModelYen::getRouteSearchStarts() {
	return routeSearchStarts;
}

}
