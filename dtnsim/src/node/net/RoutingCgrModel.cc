/*
 * RoutingCgr.cpp
 *
 *  Created on: Nov 8, 2016
 *      Author: juanfraire
 */

#include "RoutingCgrModel.h"

RoutingCgrModel::RoutingCgrModel()
{
}

RoutingCgrModel::~RoutingCgrModel()
{
}

void RoutingCgrModel::setLocalNode(int eid)
{
	eid_ = eid;
}

void RoutingCgrModel::setSdr(SdrModel * sdr)
{
	sdr_ = sdr;
}

void RoutingCgrModel::setContactPlan(ContactPlan * contactPlan)
{
	contactPlan_ = contactPlan;
}

void RoutingCgrModel::routeBundle(BundlePkt * bundle, double simTime)
{
	if (!printDebug) // disable cout if degug disabled
		cout.setstate(std::ios_base::failbit);

//	if (eid_==10 && bundle->getSourceEid()==8 && bundle->getDestinationEid()==48)
//		cout.clear();

	// Call cgrForward from ion (route and forwarding)
	cgrForward(bundle, simTime);

	if (!printDebug)
		cout.clear();

}

/////////////////////////////////////////////////
// Ion Cgr Functions based in libcgr.c (v 3.5.0):
/////////////////////////////////////////////////

void RoutingCgrModel::cgrForward(BundlePkt * bundle, double simTime)
{
	cout << endl << "simTime: " << simTime << "s, Node " << eid_ << ", calling cgrForward (src: " << bundle->getSourceEid() << ", dst:" << bundle->getDestinationEid() << ", created:" << bundle->getCreationTimestamp().dbl() << "s, id:" << bundle->getId() << ")" << endl;

	// If contact plan was changed, discard route list
	if (contactPlan_->getLastEditTime() > routeListLastEditTime)
		routeList_.clear();

	vector<ProximateNode> proximateNodes;
	vector<int> excludedNodes;

	// Insert sender node to excludedNodes to avoid loops
	if (bundle->getReturnToSender() == false)
		excludedNodes.push_back(bundle->getSenderEid());

	// Insert other nodes into excludedNodes (embargoed nodes)
	// Not happening in the DtnSim

	// Populate proximateNodes
	cout << "  calling identifyProximateNodes: " << endl;
	identifyProximateNodes(bundle, simTime, excludedNodes, &proximateNodes);

	// TODO: send critical bundle to all proximateNodes
	if (bundle->getCritical())
	{
		cout << "***Critical bundle forwarding not implemented yet!***" << endl;
		exit(1);
	}

	cout << "  proximateNodesSize: " << proximateNodes.size() << ":" << endl;

	ProximateNode * selectedNeighbor = NULL;
	for (vector<ProximateNode>::iterator it = proximateNodes.begin(); it != proximateNodes.end(); ++it)
	{
		cout << "    ProxNode " << (*it).neighborNodeNbr << " (cId:" << (*it).contactId << ") arrivalConf:" << (*it).arrivalConfidence << " arrivalT:" << (*it).arrivalTime << " hopCnt:" << (*it).hopCount << " forfT:" << (*it).forfeitTime << ":";

		// If the confidence improvement is less than the minimal, continue
		if (bundle->getDlvConfidence() > 0.0 && bundle->getDlvConfidence() < 1.0)
		{
			float newDlvConfidence = 1.0 - (1.0 - bundle->getDlvConfidence()) * (1.0 - it->arrivalConfidence);
			float confidenceImprovement = (newDlvConfidence / bundle->getDlvConfidence()) - 1.0;
			if (confidenceImprovement < MIN_CONFIDENCE_IMPROVEMENT)
			{
				cout << " Not chosen, not enough confidence improvement" << endl;
				continue;
			}
		}

		// Select Neighbor by arrival confidence, arrival time, hop count, and node number
		// (Confidence criteria to be removed after 3.5.0)
		if (selectedNeighbor == NULL)
		{
			cout << " Chosen, first suitable neighbor" << endl;
			selectedNeighbor = &(*it);
		}

		else if (it->arrivalConfidence > selectedNeighbor->arrivalConfidence)
		{
			cout << " Chosen, best arrival confidence" << endl;
			selectedNeighbor = &(*it);
		}
		else if (it->arrivalConfidence < selectedNeighbor->arrivalConfidence)
		{
			cout << " Not Chosen, bad arrival confidence" << endl;
			continue;
		}
		else if (it->arrivalTime < selectedNeighbor->arrivalTime)
		{
			cout << " Chosen, best arrival time" << endl;
			selectedNeighbor = &(*it);
		}
		else if (it->arrivalTime > selectedNeighbor->arrivalTime)
		{
			cout << " Not Chosen, bad arrival time" << endl;
			continue;
		}
		else if (it->hopCount < selectedNeighbor->hopCount)
		{
			cout << " Chosen, best hop count" << endl;
			selectedNeighbor = &(*it);
		}
		else if (it->hopCount > selectedNeighbor->hopCount)
		{
			cout << " Not Chosen, bad hop count" << endl;
			continue;
		}
		else if (it->neighborNodeNbr < selectedNeighbor->neighborNodeNbr)
		{
			cout << " Chosen, best node number" << endl;
			selectedNeighbor = &(*it);
		}
	}

	if (selectedNeighbor != NULL)
	{
		// enqueueToNeighbor() function in ion
		cout << "  enqueueing to chosen Neighbor" << endl;
		enqueueToNeighbor(bundle, selectedNeighbor);

		// TODO: manageOverbooking() function
		// Only necesary for bundles with priority > bulk.
		return;
	}

	// if the expected confidence level is reached, done
	if (bundle->getDlvConfidence() >= MIN_NET_DELIVERY_CONFIDENCE)
	{
		cout << "  delivery confidence reached, end cgrForward" << endl;
		// TODO: delete bundle if not forwarded!
		return;
	}

	// if no routes to destination, done
	// TODO: shouldnt send to limbo?
	if (routeList_[bundle->getDestinationEid()].size() == 0)
	{
		// cout << "  delivery confidence not reached but no routes to dst, end cgrForward" << endl;
		//return;
	}

	if (selectedNeighbor != NULL)
	{ // if bundle was enqueued (bundle ->ductXmitElt)
	  // TODO: clone bundle and send it for routing (enqueueToLimbo in ion)
	  // Here we should clone and call cgrForward again (while), Im a genious :)
		cout << "  delivery confidence not reached, clone and repeat forward" << endl;
	}
	else
	{
		cout << "  no chosen neighbor and delivery confidence not reached, enqueueing to limbo" << endl;
		enqueueToLimbo(bundle);
		return;
	}
}

void RoutingCgrModel::identifyProximateNodes(BundlePkt * bundle, double simTime, vector<int> excludedNodes, vector<ProximateNode> * proximateNodes)
{
	int terminusNode = bundle->getDestinationEid();

	// If routes are empty for this node, load route list
	if (routeList_[terminusNode].empty() == true)
	{
		cout << "    routeList to Node:" << terminusNode << " empty, calling loadRouteList" << endl;
		loadRouteList(terminusNode, simTime);
		routeListLastEditTime = simTime;
	}

	cout << "    routeList to Node:" << terminusNode << " size:" << routeList_[terminusNode].size() << endl;

	for (vector<CgrRoute>::iterator it = routeList_[terminusNode].begin(); it != routeList_[terminusNode].end(); ++it)
	{
		cout << "      route through node:" << (*it).toNodeNbr << ", arrivalConf:" << (*it).arrivalConfidence << ", arrivalT:" << (*it).arrivalTime << ", txWin:(" << (*it).fromTime << "-" << (*it).toTime << "), maxCap:" << (*it).maxCapacity << "bits:";

		if ((*it).toTime <= simTime)
		{
			cout << " ignoring, route due, recompute route for contact (not implemented yet!)" << endl;
			recomputeRouteForContact();
			// TODO: a new route should be looked and the
			// for loop might need to be restarted if found
			// Now we just ignore the old route (pesimistic)
			continue;
		}

		// If arrival time is after deadline, ignore route
		if ((*it).arrivalTime > bundle->getTtl().dbl())
		{
			cout << " ignoring, does not satisfies bundle deadline" << endl;
			continue;
		}

		// When a contact happen or is in the contact
		// plan, ion set its confidence to 1.0. Otherwise,
		// it is an opportunistic contact.
		if ((*it).hops[0]->getConfidence() != 1.0)
		{
			cout << " ignoring, first hop has confidence different than 1" << endl;
			continue;
		}

		// If Im the final destination and the next hop,
		// do not route through myself. Not sure when would
		// this happen.
		if ((*it).toNodeNbr == eid_)
			if (bundle->getDestinationEid() == (*it).toNodeNbr)
			{
				cout << " ignoring, first hop and destination is this node" << endl;
				continue;
			}

		// If bundle does not fit in route, ignore.
		// With proactive fragmentation, this should not stay.
		if (bundle->getBitLength() > (*it).maxCapacity)
		{
			cout << " ignoring, maxCapacity cannot accomodate bundle" << endl;
			continue;
		}

		// If next hop is in excluded nodes, ignore.
		vector<int>::iterator itExl = find(excludedNodes.begin(), excludedNodes.end(), (*it).toNodeNbr);
		if (itExl != excludedNodes.end())
		{
			cout << " ignoring, next hop is in excludedNodes" << endl;
			continue;
		}

		// If we got to this point, the route might be
		// considered. However, some final tests must be
		// donde before evaluating the node for the proxNodes.
		tryRoute(bundle, &(*it), proximateNodes);
	}
}

void RoutingCgrModel::tryRoute(BundlePkt * bundle, CgrRoute * route, vector<ProximateNode> * proximateNodes)
{

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
	if (route->hops[0]->getResidualCapacity() <= bundle->getBitLength())
	{
		cout << " residual capacity of first contact in route depleted" << endl;
		return;
	}

	// Last, we go through proximateNodes to add the route
	for (vector<ProximateNode>::iterator it = (*proximateNodes).begin(); it != (*proximateNodes).end(); ++it)
	{
		if ((*it).neighborNodeNbr == route->toNodeNbr)
		{
			// The next-hop is already among proximateNodes.
			// Test if we should update this node metrics.
			// Confidence criteria to be removed in post 3.5.0
			if (route->arrivalConfidence > (*it).arrivalConfidence)
			{
				cout << " good route, replace node " << route->toNodeNbr << " in proxNodes" << endl;
				(*it).arrivalConfidence = route->arrivalConfidence;
				(*it).arrivalTime = route->arrivalTime;
				(*it).hopCount = route->hops.size();
				(*it).forfeitTime = route->toTime;
				(*it).contactId = route->hops[0]->getId();
				(*it).route = route;
			}
			else if (route->arrivalConfidence < (*it).arrivalConfidence)
			{
				cout << " route through node " << route->toNodeNbr << " in proxNodes has better arrival confidence" << endl;
				return;
			}
			else if (route->arrivalTime < (*it).arrivalTime)
			{
				cout << " good route, replace node " << route->toNodeNbr << " in proxNodes" << endl;
				(*it).arrivalTime = route->arrivalTime;
				(*it).hopCount = route->hops.size();
				(*it).forfeitTime = route->toTime;
				(*it).contactId = route->hops[0]->getId();
				(*it).route = route;
			}
			else if (route->arrivalTime > (*it).arrivalTime)
			{
				cout << " route through node " << route->toNodeNbr << " in proxNodes has better arrival time" << endl;
				return;
			}
			else if (route->hops.size() < (*it).hopCount)
			{
				cout << " good route, replace node " << route->toNodeNbr << " in proxNodes" << endl;
				(*it).hopCount = route->hops.size();
				(*it).forfeitTime = route->toTime;
				(*it).contactId = route->hops[0]->getId();
				(*it).route = route;
			}
			else if (route->hops.size() > (*it).hopCount)
			{
				cout << " route through node " << route->toNodeNbr << " in proxNodes has better hop count" << endl;
				return;
			}
			else if (route->toNodeNbr < (*it).neighborNodeNbr)
			{
				cout << " good route, replace node " << route->toNodeNbr << " in proxNodes" << endl;
				(*it).forfeitTime = route->toTime;
				(*it).contactId = route->hops[0]->getId();
				(*it).route = route;
			}
			cout << " route through node " << route->toNodeNbr << " in proxNodes has same metrics" << endl;
			return;
		}
	}

	// If we got to this point, the node is not in
	// proximateNodes list. So we create and add one.
	cout << " good route, add node " << route->toNodeNbr << " to proxNodes" << endl;
	ProximateNode node;
	node.neighborNodeNbr = route->toNodeNbr;
	node.arrivalConfidence = route->arrivalConfidence;
	node.arrivalTime = route->arrivalTime;
	node.contactId = route->hops[0]->getId();
	node.forfeitTime = route->toTime;
	node.hopCount = route->hops.size();
	node.route = route;
	proximateNodes->push_back(node);
}

void RoutingCgrModel::loadRouteList(int terminusNode, double simTime)
{
	// Create rootContact and its corresponding rootWork
	Contact rootContact(0, 0, 0, eid_, eid_, 0, 1.0);
	Work rootWork;
	rootWork.contact = &rootContact;
	rootWork.arrivalTime = simTime;
	rootContact.work = &rootWork;

	// Create route vector in routeList
	vector<CgrRoute> cgrRoute;
	routeList_[terminusNode] = cgrRoute;

	// Create and initialize working area in each contact
	for (vector<Contact>::iterator it = contactPlan_->getContacts()->begin(); it != contactPlan_->getContacts()->end(); ++it)
	{
		(*it).work = new Work;
		((Work *) (*it).work)->contact = &(*it);
		((Work *) (*it).work)->arrivalTime = numeric_limits<double>::max();
		((Work *) (*it).work)->capacity = 0;
		((Work *) (*it).work)->predecessor = 0;
		((Work *) (*it).work)->visited = false;
		((Work *) (*it).work)->suppressed = false;
	}

	Contact * anchorContact = NULL;
	while (1)
	{
		// Find the next best route
		CgrRoute route;
		route.toNodeNbr = 0;
		findNextBestRoute(&rootContact, terminusNode, &route);

		// If toNodeNbr still 0, no routes were found
		// End search
		if (route.toNodeNbr == 0)
		{
			cout << "      no more routes found" << endl;
			break;
		}

		// If anchored search on going and firstContact
		// is not anchor, end the anchor and do not record
		// this route.
		Contact * firstContact = route.hops[0];
		if (anchorContact != NULL)
			if (firstContact != anchorContact)
			{
				cout << "        ending anchored search in contactId: " << anchorContact->getId() << " (src:" << anchorContact->getSourceEid() << "-dst:" << anchorContact->getDestinationEid() << ", " << anchorContact->getStart() << "s to " << anchorContact->getEnd() << "s)" << endl;
				// This is endAnchoredSearch() function in ion: it clears the working area
				for (vector<Contact>::iterator it = contactPlan_->getContacts()->begin(); it != contactPlan_->getContacts()->end(); ++it)
				{
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

		// Record route
		cout << "      new route found through node:" << route.toNodeNbr << ", arrivalConf:" << route.arrivalConfidence << ", arrivalT:" << route.arrivalTime << ", txWin:(" << route.fromTime << "-" << route.toTime << "), maxCap:" << route.maxCapacity << "bits:" << endl;
		routeList_[terminusNode].push_back(route);

		// Find limiting contact for next iteration
		// (earliest ending contact in path, generally the first)
		Contact * limitContact = NULL;
		if (route.toTime == firstContact->getEnd())
		{
			limitContact = firstContact;
		}
		else
		{
			// Start new anchor search. Anchoring only
			// happens in the first hop.. not good!
			anchorContact = firstContact;
			cout << "        starting anchored search in contactId: " << anchorContact->getId() << " (src:" << anchorContact->getSourceEid() << "-dst:" << anchorContact->getDestinationEid() << ", " << anchorContact->getStart() << "s-" << anchorContact->getEnd() << "s)" << endl;
			// find the limiting contact in route
			for (vector<Contact *>::iterator it = route.hops.begin(); it != route.hops.end(); ++it)
				if ((*it)->getEnd() == route.toTime)
				{
					limitContact = (*it);
					break;
				}
		}

		// Supress limiting contact in next search
		cout << "        supressing limiting contactId: " << limitContact->getId() << " (src:" << limitContact->getSourceEid() << "-dst:" << limitContact->getDestinationEid() << ", " << limitContact->getStart() << "s-" << limitContact->getEnd() << "s)" << endl;
		((Work *) limitContact->work)->suppressed = true;

		// Clear working area
		for (vector<Contact>::iterator it = contactPlan_->getContacts()->begin(); it != contactPlan_->getContacts()->end(); ++it)
		{
			((Work *) (*it).work)->arrivalTime = numeric_limits<double>::max();
			((Work *) (*it).work)->predecessor = 0;
			((Work *) (*it).work)->visited = false;
		}
	}

	// Free memory allocated for work
	for (vector<Contact>::iterator it = contactPlan_->getContacts()->begin(); it != contactPlan_->getContacts()->end(); ++it)
		delete ((Work *) (*it).work);

}

void RoutingCgrModel::findNextBestRoute(Contact * rootContact, int terminusNode, CgrRoute * route)
{
	// If toNodeNbr remains equal to 0, it means no
	// route was found by this function. In ion this
	// is signaled by a null psm address.

	// This is the computeDistanceToTerminus() in ion:
	Contact * currentContact = rootContact;
	Contact * finalContact = NULL;
	double earliestFinalArrivalTime = numeric_limits<double>::max();
	float highestConfidence = 0.0;

	cout << "        surfing contact-graph:";

	while (1)
	{
		// Go thorugh all next hop neighbors in the
		// contact plan (all contacts which source
		// node is the currentWork destination node)

		cout << currentContact->getDestinationEid() << ",";
		vector<Contact> currentNeighbors = contactPlan_->getContactsBySrc(currentContact->getDestinationEid());
		for (vector<Contact>::iterator it = currentNeighbors.begin(); it != currentNeighbors.end(); ++it)
		{
			// First, check if contact needs to be considered
			// in ion an if (contact->fromNode > arg.fromNode)
			// is necesary due to the red-black tree stuff. Not here :)

			// This contact is finished, ignore it.
			if ((*it).getEnd() <= ((Work *) (currentContact->work))->arrivalTime)
				continue;

			// This contact is suppressed/visited, ignore it.
			if (((Work *) (*it).work)->suppressed || ((Work *) (*it).work)->visited)
				continue;

			// Check if this contact has a range associated and
			// obtain One Way Light Time (owlt)
			// in ion: if (getApplicableRange(contact, &owlt) < 0) continue;
			// TODO: we need to get this from contact plan.
			double owlt = 0;
			double owltMargin = ((MAX_SPEED_MPH / 3600) * owlt) / 186282;
			owlt += owltMargin;

			// Calculate capacity
			// TODO: This capacity calculation should be then
			// updated based on the start of the effective
			// usage of the contact
			if (((Work *) (*it).work)->capacity == 0)
				((Work *) (*it).work)->capacity = (*it).getDataRate() * (*it).getDuration();

			// Calculate the cost for this contact (Arrival Time)
			double arrivalTime;
			if ((*it).getStart() < ((Work *) (currentContact->work))->arrivalTime)
				arrivalTime = ((Work *) (currentContact->work))->arrivalTime;
			else
				arrivalTime = (*it).getStart();
			arrivalTime += owlt;

			// Update the cost of this contact
			if (arrivalTime < ((Work *) (*it).work)->arrivalTime)
			{
				((Work *) (*it).work)->arrivalTime = arrivalTime;
				((Work *) (*it).work)->predecessor = currentContact;

				// If this contact reaches the terminus node
				// consider it as final contact
				if ((*it).getDestinationEid() == terminusNode)
				{

					// Confidence criteria to be removed in post 3.5.0
					if ((*it).getConfidence() > highestConfidence || (((*it).getConfidence() == highestConfidence) && ((Work *) (*it).work)->arrivalTime < earliestFinalArrivalTime))
					{
						highestConfidence = (*it).getConfidence();
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
			break; // No next contact found, exit search

		// Update next contact and go with next itartion
		currentContact = nextContact;
	} // end while (1)

	cout << endl;

	// If we got a final contact to destination
	// then it is the best route and we need to
	// translate the info from the work area to
	// the CgrRoute route pointer.
	if (finalContact != NULL)
	{
		route->arrivalTime = earliestFinalArrivalTime;
		route->arrivalConfidence = 1.0;

		double earliestEndTime = numeric_limits<double>::max();
		double maxCapacity = numeric_limits<double>::max();

		// Go through all contacts in the path
		for (Contact * contact = finalContact; contact != rootContact; contact = ((Work *) (*contact).work)->predecessor)
		{
			// Get earliest end time
			if (contact->getEnd() < earliestEndTime)
				earliestEndTime = contact->getEnd();

			// Get the minimal capacity
			if (((Work *) (*contact).work)->capacity < maxCapacity)
				maxCapacity = ((Work *) (*contact).work)->capacity;

			// Update confidence
			route->arrivalConfidence *= contact->getConfidence();

			// Store hop:
			//route->hops.push_back(contact);
			route->hops.insert(route->hops.begin(), contact);
		}

		route->toNodeNbr = route->hops[0]->getDestinationEid();
		route->fromTime = route->hops[0]->getStart();
		route->toTime = earliestEndTime;
		route->maxCapacity = maxCapacity;
	}
}

void RoutingCgrModel::recomputeRouteForContact()
{
	//cout << "***RecomputeRouteForContact not implemented yet!, ignoring route***" << endl;
}

void RoutingCgrModel::enqueueToNeighbor(BundlePkt * bundle, ProximateNode * selectedNeighbor)
{

	if (bundle->getXmitCopiesCount() == MAX_XMIT_COPIES)
		return;
	bundle->setXmitCopiesCount(bundle->getXmitCopiesCount() + 1);

	float newDlvConfidence = 1.0 - (1.0 - bundle->getDlvConfidence()) * (1.0 - selectedNeighbor->arrivalConfidence);
	bundle->setDlvConfidence(newDlvConfidence);

	// In ion, bpEnqueue goes directly
	// to SDR duct (bpEnqueue() in libbpP.c)
	// For us this goes to the neighbor queue.
	bpEnqueue(bundle, selectedNeighbor);
}

void RoutingCgrModel::enqueueToLimbo(BundlePkt * bundle)
{
	ProximateNode limboNode;
	limboNode.contactId = 0;
	limboNode.neighborNodeNbr = 0;

	// In ion, enqueueToLimbo goes directly
	// to SDR limbo (enqueueToLimbo() in libbpP.c)
	// For us this goes to queue 0.
	bpEnqueue(bundle, &limboNode);
}

void RoutingCgrModel::bpEnqueue(BundlePkt * bundle, ProximateNode * selectedNeighbor)
{
	bundle->setNextHopEid(selectedNeighbor->neighborNodeNbr);
	sdr_->enqueueBundleToContact(bundle, selectedNeighbor->contactId);

	if (selectedNeighbor->contactId != 0)
	{
		// Decrease first contact capacity:
		selectedNeighbor->route->hops[0]->setResidualCapacity(selectedNeighbor->route->hops[0]->getResidualCapacity() - bundle->getBitLength());

		// Decrease route capacity:
		// It seems this does not happen in ION. In fact, the
		// queiung process considers the local outbound capacity, which
		// is analogous to consider the first contact capacity. However,
		// there is chance to also consider remote contact capacity as
		// in PA-CGR. Furthermore, the combined effect of routeList
		// and PA-CGR need to be investigated because an update from
		// one route might impact other routes that uses the same contacts.
		selectedNeighbor->route->maxCapacity -= bundle->getBitLength();

		EV << "Node " << eid_ << ": bundle to node " << bundle->getDestinationEid() << " enqueued in queueId: " << selectedNeighbor->contactId << " (next hop: " << selectedNeighbor->neighborNodeNbr << ")" << endl;
	} else {
		EV << "Node " << eid_ << ": bundle to node " << bundle->getDestinationEid() << " enqueued to limbo!" << endl;
	}
}

