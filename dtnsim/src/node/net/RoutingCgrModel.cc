/*
 * RoutingCgr.cpp
 *
 *  Created on: Nov 8, 2016
 *      Author: juanfraire
 */

#include "RoutingCgrModel.h"

RoutingCgrModel::RoutingCgrModel()
{
	// TODO Auto-generated constructor stub

}

RoutingCgrModel::~RoutingCgrModel()
{
	// TODO Auto-generated destructor stub
}

void RoutingCgrModel::setLocalNode(int eid)
{
	eid_ = eid;
}

void RoutingCgrModel::setQueue(map<int, queue<Bundle *> > * bundlesQueue)
{
	bundlesQueue_ = bundlesQueue;
}

void RoutingCgrModel::setContactPlan(ContactPlan * contactPlan)
{
	contactPlan_ = contactPlan;
}

void RoutingCgrModel::routeBundle(Bundle * bundle, double simTime)
{
	// Call cgrForward from ion (route and forwarding)
	cgrForward(bundle, simTime);
}

/////////////////////////////////////////////////
// Ion Cgr Functions based in libcgr.c (v 3.5.0):
/////////////////////////////////////////////////

void RoutingCgrModel::cgrForward(Bundle * bundle, double simTime)
{

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
	identifyProximateNodes(bundle, simTime, excludedNodes, &proximateNodes);

	//cout << "Node " << eid_ << " proximateNodesSize: " << proximateNodes.size() << endl;

	// TODO: send critical bundle to all proximateNodes
	if (bundle->getCritical())
	{
		cout << "Critical bundle forwarding not implemented yet!" << endl;
		exit(1);
	}

	ProximateNode * selectedNeighbor = NULL;
	for (vector<ProximateNode>::iterator it = proximateNodes.begin(); it != proximateNodes.end(); ++it)
	{
		// If the confidence improvement is less than the minimal, continue
		if (bundle->getDlvConfidence() > 0.0 && bundle->getDlvConfidence() < 1.0)
		{
			float newDlvConfidence = 1.0 - (1.0 - bundle->getDlvConfidence()) * (1.0 - it->arrivalConfidence);
			float confidenceImprovement = (newDlvConfidence / bundle->getDlvConfidence()) - 1.0;
			if (confidenceImprovement < MIN_CONFIDENCE_IMPROVEMENT)
				continue;
		}

		// Select Neighbor by arrival confidence, arrival time, hop count, and node number
		if (selectedNeighbor == NULL)
			selectedNeighbor = &(*it);
		// TODO: confidence criteria to be removed
		else if (it->arrivalConfidence > selectedNeighbor->arrivalConfidence)
			selectedNeighbor = &(*it);
		else if (it->arrivalConfidence < selectedNeighbor->arrivalConfidence)
			continue;
		else if (it->arrivalTime < selectedNeighbor->arrivalTime)
			selectedNeighbor = &(*it);
		else if (it->arrivalTime > selectedNeighbor->arrivalTime)
			continue;
		else if (it->hopCount < selectedNeighbor->hopCount)
			selectedNeighbor = &(*it);
		else if (it->hopCount > selectedNeighbor->hopCount)
			continue;
		else if (it->neighborNodeNbr < selectedNeighbor->neighborNodeNbr)
			selectedNeighbor = &(*it);
	}

	if (selectedNeighbor != NULL)
	{
		// enqueueToNeighbor() function in ion
		enqueueToNeighbor(bundle, selectedNeighbor);

		// TODO: manageOverbooking() function
		// Only necesary for bundles with priority > bulk.
	}

	// if the expected confidence level is reached, done
	if (bundle->getDlvConfidence() >= MIN_NET_DELIVERY_CONFIDENCE)
		return;

	// if no routes to destination, done
	if (routeList_[bundle->getDestinationEid()].size() == 0)
		return;

	if (selectedNeighbor != NULL)
	{ // if bundle was enqueued (bundle ->ductXmitElt)
	  // TODO: clone bundle and send it for routing (enqueueToLimbo in ion)
	  // Here we should clone and call cgrForward again (while), Im a genious :)
	}
	else
	{
		enqueueToLimbo(bundle);
	}
}

void RoutingCgrModel::identifyProximateNodes(Bundle * bundle, double simTime, vector<int> excludedNodes, vector<ProximateNode> * proximateNodes)
{
	int terminusNode = bundle->getDestinationEid();

	// If routes are empty for this node, load route list
	if (routeList_[terminusNode].empty() == true)
	{
		loadRouteList(terminusNode, simTime);
		routeListLastEditTime = simTime;
	}

	//cout << "Node " << eid_ << " routeListSize: " << routeList_[terminusNode].size() << " terminusNode: " << terminusNode << endl;

	for (vector<CgrRoute>::iterator it = routeList_[terminusNode].begin(); it != routeList_[terminusNode].end(); ++it)
	{
		if ((*it).toTime < simTime)
		{
			recomputeRouteForContact();
			// TODO: a new route should be looked and the
			// for loop might need to be restarted if found
			// Now we just ignore the old route (pesimistic)
			continue;
		}

		// If arrival time is after deadline, ignore route
		if ((*it).arrivalTime > bundle->getTtl().dbl())
			continue;

		// When a contact happen or is in the contact
		// plan, ion set its confidence to 1.0. Otherwise,
		// it is an opportunistic contact.
		if ((*it).hops[0]->getConfidence() != 1.0)
			continue;

		// If Im the final destination and the next hop,
		// do not route through myself. Not sure when would
		// this happen.
		if ((*it).toNodeNbr == eid_)
			if (bundle->getDestinationEid() == (*it).toNodeNbr)
				continue;

		// If bundle does not fit in route, ignore.
		// TODO: But with proactive fragmentation, this should not stay.
		if (bundle->getBitLength() > (*it).maxCapacity)
			continue;

		// If next hop is in excluded nodes, ignore.
		vector<int>::iterator itExl = find(excludedNodes.begin(), excludedNodes.end(), (*it).toNodeNbr);
		if (itExl != excludedNodes.end())
			continue;

		// If we got to this point, the route might be
		// considered. However, some final tests must be
		// donde before evaluating the node for the proxNodes.
		tryRoute(bundle, &(*it), proximateNodes);
	}
}

void RoutingCgrModel::tryRoute(Bundle * bundle, CgrRoute * route, vector<ProximateNode> * proximateNodes)
{

	// First, ion test if outduct is blocked,
	// we do not considered blocked outducts here

	// Then, ion test the do-not-fragment flag.
	// if set, and outduct frame size is not enough,
	// return. We do not a frame limit in dtnsim.

	// Thirdly, ion computeArrivalTime() to determine
	// the impact of the outbound queue in the arrival
	// time. We coud do this here also (TODO).

	// Last, we go through proximateNodes to add the route
	for (vector<ProximateNode>::iterator it = (*proximateNodes).begin(); it != (*proximateNodes).end(); ++it)
	{
		if ((*it).neighborNodeNbr == route->toNodeNbr)
		{
			// The next-hop is already among proximateNodes.
			// Test if we should update this node metrics.
			// TODO: confidence criteria to be removed
			if (route->arrivalConfidence > (*it).arrivalConfidence)
			{
				(*it).arrivalConfidence = route->arrivalConfidence;
				(*it).arrivalTime = route->arrivalTime;
				(*it).hopCount = route->hops.size();
				(*it).forfeitTime = route->toTime;
				(*it).contactId = route->hops[0]->getId();
			}
			else if (route->arrivalConfidence < (*it).arrivalConfidence)
				return;
			else if (route->arrivalTime < (*it).arrivalTime)
			{
				(*it).arrivalTime = route->arrivalTime;
				(*it).hopCount = route->hops.size();
				(*it).forfeitTime = route->toTime;
				(*it).contactId = route->hops[0]->getId();
			}
			else if (route->arrivalTime > (*it).arrivalTime)
				return;
			else if (route->hops.size() < (*it).hopCount)
			{
				(*it).hopCount = route->hops.size();
				(*it).forfeitTime = route->toTime;
				(*it).contactId = route->hops[0]->getId();
			}
			else if (route->hops.size() > (*it).hopCount)
				return;
			else if (route->toNodeNbr < (*it).neighborNodeNbr)
			{
				(*it).forfeitTime = route->toTime;
				(*it).contactId = route->hops[0]->getId();
			}
			return;
		}
	}

	// If we got to this point, the node is not in
	// proximateNodes list. So we create and add one.
	ProximateNode node;
	node.neighborNodeNbr = route->toNodeNbr;
	node.arrivalConfidence = route->arrivalConfidence;
	node.arrivalTime = route->arrivalTime;
	node.contactId = route->hops[0]->getId();
	node.forfeitTime = route->toTime;
	node.hopCount = route->hops.size();
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
		//cout << (*it).getId() << " - " << (*it).getSourceEid() << " to " << (*it).getDestinationEid() << " +" << (*it).getStart() << " +" << (*it).getEnd() << endl;
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
			break;

		//cout << "Node " << eid_ << " new route: arrivalTime:" << route.arrivalTime << " fromTime:" << route.fromTime << " toTime: " << route.toTime << " hops:" << route.hops.size() << ", first hop Id:" << route.hops[0]->getId() << " (endTime:" << route.hops[0]->getEnd() << ")" << endl;

		// If anchored search on going and firstContact
		// is not anchor, end the anchor and do not record
		// this route.
		Contact * firstContact = route.hops[0];
		if (anchorContact != NULL)
			if (firstContact != anchorContact)
			{
				//cout << "Node " << eid_ << " ending anchored search in contactId: " << anchorContact->getId() << endl;
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
			//cout << "Node " << eid_ << " starting anchored search in contactId: " << anchorContact->getId() << endl;
			// find the limiting contact in route
			for (vector<Contact *>::iterator it = route.hops.begin(); it != route.hops.end(); ++it)
				if ((*it)->getEnd() == route.toTime)
				{
					limitContact = (*it);
					break;
				}
		}

		// Supress limiting contact in next search
		((Work *) limitContact->work)->suppressed = true;
		//cout << "Node " << eid_ << " suppresing contactId:" << limitContact->getId() << endl;

		// Clear working area and suppress limitingContact
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

	while (1)
	{
		// Go thorugh all next hop neighbors in the
		// contact plan (all contacts which source
		// node is the currentWork destination node)
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
					// TODO: confidence criteria to be removed
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
	cout << "Node " << eid_ << " recomputeRouteForContact not implemented, ignoring route" << endl;
}

void RoutingCgrModel::enqueueToNeighbor(Bundle * bundle, ProximateNode * selectedNeighbor)
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

void RoutingCgrModel::enqueueToLimbo(Bundle * bundle)
{

	ProximateNode limboNode;
	limboNode.contactId = 0;
	limboNode.neighborNodeNbr = 0;

	// In ion, enqueueToLimbo goes directly
	// to SDR limbo (enqueueToLimbo() in libbpP.c)
	// For us this goes to queue 0.
	bpEnqueue(bundle, &limboNode);
}

void RoutingCgrModel::bpEnqueue(Bundle * bundle, ProximateNode * selectedNeighbor)
{

	bundle->setNextHopEid(selectedNeighbor->neighborNodeNbr);

	// TODO: we are forwarding by contact, need to add by node sometime?
	map<int, queue<Bundle *> >::iterator it = bundlesQueue_->find(selectedNeighbor->contactId);
	if (it != bundlesQueue_->end())
	{
		it->second.push(bundle);
	}
	else
	{
		queue<Bundle *> q;
		q.push(bundle);
		(*bundlesQueue_)[selectedNeighbor->contactId] = q;
	}

	EV << "Node " << eid_ << ": bundle to node " << bundle->getDestinationEid() << " enqueued in queueId: " << selectedNeighbor->contactId << " (next hop: " << selectedNeighbor->neighborNodeNbr << ")" << endl;
}

