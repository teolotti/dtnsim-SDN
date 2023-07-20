#include <src/hierarchicalRegions/node/dtn/routing/RoutingCgrModel350.h>

namespace dtnsimhierarchical {



RoutingCgrModel350::RoutingCgrModel350(int eid, SdrModel *sdr, ContactPlan *contactPlan, bool printDebug): RoutingDeterministic(eid, sdr, contactPlan) {

	printDebug_ = printDebug;

	//for (auto & potentialTerminusNode : potentialDestinationEids_) {
	//	loadRouteList(potentialTerminusNode, simTime().dbl());
	//}

	//printRouteLists();

}

RoutingCgrModel350::~RoutingCgrModel350() {
}


void RoutingCgrModel350::loadRouteList(int terminusNode, double simTime) {

	// Create rootContact and its corresponding rootWork
	Contact rootContact(0, 0, 0, eid_, eid_, 0, "Z", "Z"); // srcEID = dstEID = this EID, placeholder regions
	Work rootWork;
	rootWork.contact = &rootContact;
	rootWork.arrivalTime = simTime;
	rootContact.work = &rootWork;

	// Create route vector in routeList
	vector<CgrRoute> cgrRouteVector;
	routeLists_[terminusNode] = cgrRouteVector;

	// Create and initialize working area in each contact
	for (vector<Contact>::iterator it = contactPlan_->getContacts()->begin(); it != contactPlan_->getContacts()->end(); ++it) {
		(*it).work = new Work;
		((Work *) (*it).work)->contact = &(*it);
		((Work *) (*it).work)->arrivalTime = numeric_limits<double>::max();
		((Work *) (*it).work)->capacity = 0;
		((Work *) (*it).work)->predecessor = 0;
		((Work *) (*it).work)->visited = false;
		((Work *) (*it).work)->suppressed = false;
	}


	Contact* anchorContact = NULL;
	while (true) {

		// Find the next best route
		CgrRoute route;
		route.nextHop = -1;
		findNextBestRoute(&rootContact, terminusNode, &route);

		// if next hop is still -1, no routes were found -> end search
		if (route.nextHop == -1) {
			break;
		}

		// If anchored search on going and firstContact
		// is not anchor, end the anchor and do not record
		// this route.
		Contact * firstContact = route.hops[0];
		if (anchorContact != NULL) {
			if (firstContact != anchorContact) {

				// This is endAnchoredSearch() function in ion: it clears the working area
				for (vector<Contact>::iterator it = contactPlan_->getContacts()->begin(); it != contactPlan_->getContacts()->end(); ++it) {
					((Work *) (*it).work)->arrivalTime = numeric_limits<double>::max();
					((Work *) (*it).work)->predecessor = NULL;
					((Work *) (*it).work)->visited = false;
					// Unsupress all non-local contacts
					// (local contacts must keep their value).
					if ((*it).getSourceEid() != eid_)
						((Work *) (*it).work)->suppressed = false;
				}
				// End endAnchoredSearch() function
				((Work *) anchorContact->work)->suppressed = 1;
				anchorContact = NULL;
				continue;
			}
		}

		// Record route
		routeLists_[terminusNode].push_back(route);

		// Find limiting contact for next iteration
		// (earliest ending contact in path, generally the first)
		Contact * limitContact = NULL;
		if (route.toTime == firstContact->getEnd()) {
			limitContact = firstContact;

		} else {
			// Start new anchor search. Anchoring only
			// happens in the first hop.. not good!
			anchorContact = firstContact;
			// find the limiting contact in route
			for (vector<Contact *>::iterator it = route.hops.begin(); it != route.hops.end(); ++it) {

				if ((*it)->getEnd() == route.toTime) {
					limitContact = (*it);
					break;
				}
			}
		}

		// Suppress limiting contact in next search
		((Work *) limitContact->work)->suppressed = true;

		// Clear working area
		for (vector<Contact>::iterator it = contactPlan_->getContacts()->begin(); it != contactPlan_->getContacts()->end(); ++it) {
			((Work *) (*it).work)->arrivalTime = numeric_limits<double>::max();
			((Work *) (*it).work)->predecessor = 0;
			((Work *) (*it).work)->visited = false;
		}
	}

	// Free memory allocated for work
	for (vector<Contact>::iterator it = contactPlan_->getContacts()->begin(); it != contactPlan_->getContacts()->end(); ++it) {
		delete ((Work *) (*it).work);
	}
}

void RoutingCgrModel350::printRouteLists() {

	cout << endl;
	cout << "Printing route lists for source node with EID: " << eid_ << endl;

	for (map<int, vector<CgrRoute>>::iterator tableEntry = routeLists_.begin(); tableEntry != routeLists_.end(); ++tableEntry) {

		cout << "Destination " << tableEntry->first << ":" << endl;

		int i = 0;
		for (vector<CgrRoute>::iterator cgrRoute = tableEntry->second.begin(); cgrRoute != tableEntry->second.end(); ++cgrRoute) {

			cout << "Route " << i
					<< " -> terminus node: " << cgrRoute->terminusNode
					<< "; next hop: " << cgrRoute->nextHop
					<< "; tx window: (" << cgrRoute->fromTime << "," << cgrRoute->toTime << ")"
					<< "; arrival time: " << cgrRoute->arrivalTime
					<< "; max volume: " << cgrRoute->maxVolume
					<< "; hops: ["; // contact IDs

			for (vector<Contact*>::iterator hop = cgrRoute->hops.begin(); hop != cgrRoute->hops.end(); ++hop) {
				cout << (*hop)->getId() << ", ";
			}
			cout << "]" << endl;
			i++;
		}
	}

	cout << "END" << endl;
}

void RoutingCgrModel350::findNextBestRoute(Contact *rootContact, int terminusNode, CgrRoute *route) {

	// increment counter
	dijkstraCalls++;

	// If toNodeNbr remains equal to -1, it means no
	// route was found by this function. In ion this
	// is signaled by a null psm address.

	// This is the computeDistanceToTerminus() in ion:
	Contact* currentContact = rootContact;

	// source and destination notational vertices
	nodesOneDijkstra += 2;

	Contact* finalContact = NULL;
	double earliestFinalArrivalTime = numeric_limits<double>::max();
	float highestConfidence = 0.0;

	while (true) {

		// increment counter
		dijkstraLoops++;

		// Go through all next hop neighbors in the
		// contact plan (all contacts which source
		// node is the currentWork destination node)
		vector<Contact> currentNeighbors = contactPlan_->getContactsBySrc(currentContact->getDestinationEid());

		// for each neighbor CGR is considering a new node in a conceptual graph
		nodesOneDijkstra += currentNeighbors.size();

		// for each neighbor CGR is considering a new edge in a conceptual graph
		// when the contact is already finished or visited we consider and edge with
		// cost infinity
		edgesOneDijkstra += currentNeighbors.size();

		for (vector<Contact>::iterator it = currentNeighbors.begin(); it != currentNeighbors.end(); ++it) {

			// First, check if contact needs to be considered
			// in ion an if (contact->fromNode > arg.fromNode)
			// is necessary due to the red-black tree stuff. Not here :)

			// This contact is finished, ignore it.
			if ((*it).getEnd() <= ((Work *) (currentContact->work))->arrivalTime) {
				continue;
			}

			// This contact is suppressed/visited, ignore it.
			if (((Work *) (*it).work)->suppressed || ((Work *) (*it).work)->visited) {
				continue;
			}

			// Get owlt (one way light time). If none found, ignore contact
			double owlt = 1; // TODO
					//contactPlan_->getRangeBySrcDst((*it).getSourceEid(), (*it).getDestinationEid());
			if (owlt == -1) {
				//cout << "warning, range not available for nodes " << (*it).getSourceEid() << "-" << (*it).getDestinationEid() << ", assuming range=0" << endl;
				owlt = 0;
			}
			//double owltMargin = ((MAX_SPEED_MPH / 3600) * owlt) / 186282;
			//owlt += owltMargin;

			// Calculate capacity
			// TODO: This capacity calculation should be then
			// updated based on the start of the effective
			// usage of the contact
			if (((Work *) (*it).work)->capacity == 0) {
				((Work *) (*it).work)->capacity = (*it).getDataRate() * (*it).getDuration();
			}

			// Calculate the cost for this contact (Arrival Time)
			double arrivalTime;
			if ((*it).getStart() < ((Work *) (currentContact->work))->arrivalTime) {
				arrivalTime = ((Work *) (currentContact->work))->arrivalTime;
			} else {
				arrivalTime = (*it).getStart();
			}
			arrivalTime += owlt;

			// Update the cost of this contact
			if (arrivalTime < ((Work *) (*it).work)->arrivalTime) {

				((Work *) (*it).work)->arrivalTime = arrivalTime;
				((Work *) (*it).work)->predecessor = currentContact;

				// If this contact reaches the terminus node
				// consider it as final contact
				if ((*it).getDestinationEid() == terminusNode) {

					if ((((Work *) (*it).work)->arrivalTime < earliestFinalArrivalTime)) {
						earliestFinalArrivalTime = ((Work *) (*it).work)->arrivalTime;
						// Warning: we need to point finalContact to
						// the real contact in contactPlan. This iteration
						// goes over a copy of the original contact plan
						// returned by getContactsBySrc().
						finalContact = contactPlan_->getContactById((*it).getId());
					}
				}
			}
		} // end for (currentNeighbors)

		((Work *) (currentContact->work))->visited = true;

		// Select next (best) contact to move to in next iteration
		Contact * nextContact = NULL;
		double earliestArrivalTime = numeric_limits<double>::max();
		for (vector<Contact>::iterator it = contactPlan_->getContacts()->begin(); it != contactPlan_->getContacts()->end(); ++it) {

			// Do not evaluate suppressed or visited contacts
			if (((Work *) (*it).work)->suppressed || ((Work *) (*it).work)->visited) {
				continue;
			}

			// If the arrival time is worst than the best found so far, ignore
			if (((Work *) (*it).work)->arrivalTime > earliestFinalArrivalTime) {
				continue;
			}

			// Then this might be the best candidate contact
			if (((Work *) (*it).work)->arrivalTime < earliestArrivalTime) {
				nextContact = &(*it);
				earliestArrivalTime = ((Work *) (*it).work)->arrivalTime;
			}
		}

		if (nextContact == NULL) {
			break; // No next contact found, exit search
		}

		// Update next contact and go with next itartion
		currentContact = nextContact;
	}

	// If we got a final contact to destination
	// then it is the best route and we need to
	// translate the info from the work area to
	// the CgrRoute route pointer.
	if (finalContact != NULL) {

		route->arrivalTime = earliestFinalArrivalTime;
		route->confidence = 1.0;

		double earliestEndTime = numeric_limits<double>::max();
		double maxCapacity = numeric_limits<double>::max();

		// Go through all contacts in the path
		for (Contact * contact = finalContact; contact != rootContact; contact = ((Work *) (*contact).work)->predecessor) {

			// Get earliest end time
			if (contact->getEnd() < earliestEndTime) {
				earliestEndTime = contact->getEnd();
			}

			// Get the minimal capacity
			if (((Work *) (*contact).work)->capacity < maxCapacity) {
				maxCapacity = ((Work *) (*contact).work)->capacity;
			}

			// Update confidence
			route->confidence *= contact->getConfidence();

			// Store hop:
			route->hops.insert(route->hops.begin(), contact);
		}

		route->nextHop = route->hops[0]->getDestinationEid();
		route->fromTime = route->hops[0]->getStart();
		route->toTime = earliestEndTime;
		route->maxVolume = maxCapacity;
		route->terminusNode = terminusNode;
	}
}

void RoutingCgrModel350::routeAndQueueBundle(BundlePacketHIRR *bundle, double simTime, int terminusNode) {

	if (!printDebug_) // disable cout if degug disabled
		cout.setstate(std::ios_base::failbit);

	// Reset counters
	dijkstraCalls = 0;
	dijkstraLoops = 0;
	tableEntriesExplored = 0;
	nodesOneDijkstra = 0;
	edgesOneDijkstra = 0;

	// Call cgrForward from ion (route and forwarding)
	cgrForward(bundle, simTime, terminusNode);

	if (!printDebug_)
		cout.clear();
}


void RoutingCgrModel350::cgrForward(BundlePacketHIRR *bundle, double simTime, int terminusNode) {

	//cout << "calling cgrForward" << endl;
	cout << "TIME: " << simTime << "s, NODE: " << eid_ << ", routing bundle to dst: " << terminusNode << " (" << bundle->getByteLength() << "Bytes)." << endl;

	vector<ProximateNode> proximateNodes;
	vector<int> excludedNodes;

	// Insert sender node to excludedNodes to avoid loops
	excludedNodes.push_back(bundle->getSenderEid());

	// Populate proximateNodes
	identifyProximateNodes(bundle, simTime, terminusNode, excludedNodes, &proximateNodes);

	// TODO: send critical bundle to all proximateNodes
	//if (bundle->getCritical())

	// Select Neighbor by arrival time, hop count, and node number
	ProximateNode* selectedNeighbor = NULL;
	for (vector<ProximateNode>::iterator it = proximateNodes.begin(); it != proximateNodes.end(); ++it) {

		// Select first suitable neighbor to be able to make comparisons (see below)
		if (selectedNeighbor == NULL) {
			//cout << " Chosen, first suitable neighbor" << endl;
			selectedNeighbor = &(*it);
		}

		else if (it->arrivalTime < selectedNeighbor->arrivalTime) {
			//cout << " Chosen, best arrival time" << endl;
			selectedNeighbor = &(*it);
		}

		else if (it->arrivalTime > selectedNeighbor->arrivalTime) {
			//cout << " Not Chosen, bad arrival time" << endl;
			continue;
		}

		else if (it->hopCount < selectedNeighbor->hopCount) {
			//cout << " Chosen, best hop count" << endl;
			selectedNeighbor = &(*it);
		}

		else if (it->hopCount > selectedNeighbor->hopCount) {
			//cout << " Not Chosen, bad hop count" << endl;
			continue;
		}

		else if (it->neighborNodeNbr < selectedNeighbor->neighborNodeNbr) {
			//cout << " Chosen, best node number" << endl;
			selectedNeighbor = &(*it);
		}
	}

	if (selectedNeighbor != NULL) {

		//cout << "CALLING ENQUEUE TO NEIGHBOR" << endl;
		enqueueToNeighbor(bundle, selectedNeighbor); //TODO
		return;
	} //TODO check if route list for this terminus node empty?

	else {
		cout << "  no chosen neighbor, enqueuing to limbo" << endl;
		//enqueueToLimbo(bundle); //TODO
		return;
	}
}

void RoutingCgrModel350::identifyProximateNodes(BundlePacketHIRR *bundle, double simTime, int terminusNode, vector<int> excludedNodes, vector<ProximateNode> *proximateNodes) {

	// If routes are empty for this node, load route list
	if (routeLists_[terminusNode].empty() == true) {
		loadRouteList(terminusNode, simTime);
	}

	vector<CgrRoute>::iterator it = routeLists_[terminusNode].begin();
	while(it != routeLists_[terminusNode].end()) {

		tableEntriesExplored++;

		//cout << "*route through node:" << (*it).nextHop
		//		<< ", arrivalConf:" << (*it).confidence
		//		<< ", arrivalT:" << (*it).arrivalTime
		//		<< ", txWin:(" << (*it).fromTime << "-" << (*it).toTime
		//		<< "), maxCap:" << (*it).maxVolume << "Bytes:"
		//		<< endl;

		// print route
		//for (vector<Contact *>::iterator ith = (*it).hops.begin(); ith != (*it).hops.end(); ++ith) {
		//	cout << "(+" << (*ith)->getStart() << " +" << (*ith)->getEnd() << " " << (*ith)->getSourceEid() << " " << (*ith)->getDestinationEid() << ")";
		//}
		//cout << endl;


		// cgrRoute.toTime = earliest end time of any of its contacts -> i.e. some contact part of the
		// route has already ended, we need to load the route list again and iterate through the new one
		// any proximate nodes that have been computed so far are cleared
		if ((*it).toTime <= simTime) {

			// clean list
			routeLists_[terminusNode].clear();
			loadRouteList(terminusNode, simTime);
			it = routeLists_[terminusNode].begin();

			// clean proximate nodes
			proximateNodes->clear();

			continue;
		}

		// If arrival time is after deadline, ignore route
		if ((*it).arrivalTime > bundle->getTtl().dbl()) {
			++it;
			continue;
		}

		// Route cannot accommodate bundle, even if there are no other bundles already on the way
		// TODO pro-active fragmentation should handle this case
		if (bundle->getByteLength() > (*it).maxVolume) {
			++it;
			continue;
		}

		// The first contact does not have enough capacity for the bundle
		if (bundle->getByteLength() > (*it).hops[0]->getResidualVolume()) {
			++it;
			continue;
		}

		// The sending node of the next hop is in the excluded nodes list for current node
		vector<int>::iterator itExl = find(excludedNodes.begin(), excludedNodes.end(), (*it).nextHop);
		if (itExl != excludedNodes.end()) {
			++it;
			continue;
		}

		// If we got to this point, the route might be
		// considered. However, some final tests must be
		// done before evaluating the node for the proxNodes.
		tryRoute(bundle, &(*it), proximateNodes);

		++it;
	}
}

void RoutingCgrModel350::tryRoute(BundlePacketHIRR *bundle, CgrRoute *route, vector<ProximateNode> *proximateNodes) {

	// First, ion test if outduct is blocked,
	// we do not considered blocked outducts here

	// Then, ion test the do-not-fragment flag.
	// if set, and outduct frame size is not enough,
	// return. We do not a frame limit in dtnsim.

	// Thirdly, ion computeArrivalTime() to determine
	// the impact of the outbound queue in the arrival
	// time. We coud do this here also (TODO).
	// We imitate this behaviour by measuring the
	// residual capacity of the first contact.
	if (route->hops[0]->getResidualVolume() < bundle->getByteLength()) {
		//cout << " residual capacity of first contact in route depleted" << endl;
		return;
	}

	// Last, we go through proximateNodes to add the route
	for (vector<ProximateNode>::iterator it = (*proximateNodes).begin(); it != (*proximateNodes).end(); ++it) {

		if ((*it).neighborNodeNbr == route->nextHop) {

			// The next-hop is already among proximateNodes.
			// Test if we should update this node metrics.
			if (route->arrivalTime < (*it).arrivalTime) {
				//cout << " good route, replace node " << route->nextHop << " in proxNodes" << endl;
				(*it).arrivalTime = route->arrivalTime;
				(*it).hopCount = route->hops.size();
				(*it).forfeitTime = route->toTime;
				(*it).contactId = route->hops[0]->getId();
				(*it).route = route;
			}

			else if (route->arrivalTime > (*it).arrivalTime) {
				//cout << " route through node " << route->nextHop << " in proxNodes has better arrival time" << endl;
				return;
			}

			else if (route->hops.size() < (*it).hopCount) {
				//cout << " good route, replace node " << route->nextHop << " in proxNodes" << endl;
				(*it).hopCount = route->hops.size();
				(*it).forfeitTime = route->toTime;
				(*it).contactId = route->hops[0]->getId();
				(*it).route = route;
			}

			else if (route->hops.size() > (*it).hopCount) {
				//cout << " route through node " << route->nextHop << " in proxNodes has better hop count" << endl;
				return;
			}

			else if (route->nextHop < (*it).neighborNodeNbr) {
				//cout << " good route, replace node " << route->nextHop << " in proxNodes" << endl;
				(*it).forfeitTime = route->toTime;
				(*it).contactId = route->hops[0]->getId();
				(*it).route = route;
			}
			//cout << " route through node " << route->nextHop << " in proxNodes has same metrics" << endl;
			return;
		}
	}

	// If we got to this point, the node is not in
	// proximateNodes list. So we create and add one.
	// cout << " good route, add node " << route->toNodeNbr << " to proxNodes" << endl;
	ProximateNode node;
	node.neighborNodeNbr = route->nextHop;
	node.arrivalTime = route->arrivalTime;
	node.contactId = route->hops[0]->getId();
	node.forfeitTime = route->toTime;
	node.hopCount = route->hops.size();
	node.route = route;
	proximateNodes->push_back(node);
}


void RoutingCgrModel350::recomputeRouteForContact()
{
	//cout << "***RecomputeRouteForContact not implemented yet!, ignoring route***" << endl;
}

void RoutingCgrModel350::enqueueToNeighbor(BundlePacketHIRR *bundle, ProximateNode *selectedNeighbor) {

	bpEnqueue(bundle, selectedNeighbor);
}

void RoutingCgrModel350::enqueueToLimbo(BundlePacketHIRR * bundle) {
	ProximateNode limboNode;
	limboNode.contactId = 0;
	limboNode.neighborNodeNbr = 0;

	// In ion, enqueueToLimbo goes directly
	// to SDR limbo (enqueueToLimbo() in libbpP.c)
	// For us this goes to queue 0.
	bpEnqueue(bundle, &limboNode);
}

void RoutingCgrModel350::bpEnqueue(BundlePacketHIRR * bundle, ProximateNode * selectedNeighbor) {

	bundle->setNextHopEid(selectedNeighbor->neighborNodeNbr);
	//bundle->setNextHopRegion("A"); // TODO
	bool enqueued = sdr_->enqueueBundle(selectedNeighbor->neighborNodeNbr, bundle);

	if (enqueued) {

		if (selectedNeighbor->contactId != 0) {

			// Decrease first contact capacity:
			selectedNeighbor->route->hops[0]->setResidualVolume(selectedNeighbor->route->hops[0]->getResidualVolume() - bundle->getByteLength());

			//cout << "RVol: routeTable[" << terminusNode << "][" << selectedNeighbor->neighborNodeNbr << "]: new resCap: (cId:" << selectedNeighbor->contactId << ", resCap:"
			//		<< contactPlan_->getContactById(selectedNeighbor->contactId)->getResidualVolume() << "Bytes)" << endl;

			// Decrease route capacity:
			// It seems this does not happen in ION. In fact, the
			// queiung process considers the local outbound capacity, which
			// is analogous to consider the first contact capacity. However,
			// there is chance to also consider remote contact capacity as
			// in PA-CGR. Furthermore, the combined effect of routeList
			// and PA-CGR need to be investigated because an update from
			// one route might impact other routes that uses the same contacts.

			// todo ver
			selectedNeighbor->route->maxVolume -= bundle->getByteLength();

			//EV << "Node " << eid_ << ": bundle to node " << terminusNode << " enqueued in queueId: " << selectedNeighbor->contactId << " (next hop: " << selectedNeighbor->neighborNodeNbr << ")" << endl;
		}

		else {
			//EV << "Node " << eid_ << ": bundle to node " << terminusNode << " enqueued to limbo!" << endl;
		}
	}
}


//////////////////////
// Stats recollection
//////////////////////

double RoutingCgrModel350::getDijkstraCalls() {
	return dijkstraCalls;
}

double RoutingCgrModel350::getDijkstraLoops() {
	return dijkstraLoops;
}

double RoutingCgrModel350::getRouteTableEntriesExplored() {
	return tableEntriesExplored;
}

double RoutingCgrModel350::getEdgesOneDijkstra() const {
	return edgesOneDijkstra;
}

double RoutingCgrModel350::getNodesOneDijkstra() const {
	return nodesOneDijkstra;
}

double RoutingCgrModel350::getOneDijkstraComplexity() const {
	return (edgesOneDijkstra + nodesOneDijkstra * log(nodesOneDijkstra));
}

}
