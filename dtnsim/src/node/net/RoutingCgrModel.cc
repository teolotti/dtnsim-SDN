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

	cout << "Node " << eid_ << " routeListSize: " << routeList_[terminusNode].size() << " terminusNode: " << terminusNode << endl;

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
		if (bundle->getByteLength() > (*it).maxCapacity)
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
	cout << "Node " << eid_ << " loadRouteList to terminus " << terminusNode << endl;

	// Create rootContact and rootWork
	Contact rootContact(0, 0, 0, eid_, eid_, 0, 1.0);
	Work rootWork;
	rootWork.arrivalTime = simTime;

	// Create route vector in routeList
	vector<CgrRoute> cgrRoute;
	routeList_[terminusNode] = cgrRoute;

	// Create and initialize working area
	vector<Work> cgrWork;
	for (vector<Contact>::iterator it = contactPlan_->getContacts()->begin(); it != contactPlan_->getContacts()->end(); ++it)
	{
		Work work;
		work.contact = &(*it);
		work.arrivalTime = numeric_limits<double>::max();
		work.capacity = 0;
		work.predecessor = 0;
		work.visited = false;
		work.suppressed = false;
		cgrWork.push_back(work);
		//cout << (*it).getId() << " - " << (*it).getSourceEid() << " to " << (*it).getDestinationEid() << " +" << (*it).getStart() << " +" << (*it).getEnd() << endl;
	}

	Work * anchorWork = NULL; // anchorContact in ion
	while (1)
	{
		CgrRoute * route = NULL;
		findNextBestRoute(&rootContact, &rootWork, terminusNode, route);

		// If null, no routes were found
		if (route == NULL)
			break;

		// If anchored search on going and firstContact
		// is not anchor, end the anchor and do not record
		// this route.
		Contact * firstContact = route->hops[0];
		if (anchorWork != NULL)
			if (firstContact != anchorWork->contact)
			{
				// This is endAnchoredSearch() function in ion: it clears the working area
				for (vector<Work>::iterator it = cgrWork.begin(); it != cgrWork.end(); ++it)
				{
					(*it).arrivalTime = numeric_limits<double>::max();
					(*it).predecessor = 0;
					(*it).visited = false;
					// Unsupress all non-local contacts
					if ((*it).contact->getSourceEid() != eid_)
						(*it).suppressed = false;
				}
				// End endAnchoredSearch() function
				anchorWork->suppressed = 1;
				anchorWork = NULL;
				continue;
			}

		// Record route
		routeList_[terminusNode].push_back(*route);

		// Find limiting contact for next iteration
		Contact * limitContact=NULL;
		if (route->toTime == firstContact->getEnd())
			limitContact = firstContact;
		else
		{
			// find the limiting contact in route
			for (vector<Contact *>::iterator it = route->hops.begin(); it != route->hops.end(); ++it)
				if ((*it)->getEnd() == route->toTime)
				{
					limitContact = (*it);
					break;
				}
		}

		// Clear working area and suppress limitingContact
		for (vector<Work>::iterator it = cgrWork.begin(); it != cgrWork.end(); ++it)
		{
			(*it).arrivalTime = numeric_limits<double>::max();
			(*it).predecessor = 0;
			(*it).visited = false;
			// Supress limiting contact in next search
			if ((*it).contact == limitContact)
				(*it).suppressed = true;
		}

	}
}

void RoutingCgrModel::findNextBestRoute(Contact * rootContact, Work * rootWork, int terminusNode, CgrRoute * route)
{

	// Some manual routes to node 4 (based on contacts_Totin.txt):
	if (eid_ == 1)
	{
		CgrRoute route1;
		route1.toNodeNbr = 2;
		route1.fromTime = 0;
		route1.toTime = 10;
		route1.arrivalConfidence = 1.0;
		route1.arrivalTime = 20;
		route1.maxCapacity = 1000;
		route1.hops.push_back(contactPlan_->getContactById(1));
		route1.hops.push_back(contactPlan_->getContactById(2));
		route1.hops.push_back(contactPlan_->getContactById(3));
		route = &route1;
		return;

//		CgrRoute route2;
//		route2.toNodeNbr = 2;
//		route2.fromTime = 40;
//		route2.toTime = 50;
//		route2.arrivalConfidence = 1.0;
//		route2.arrivalTime = 40;
//		route2.maxCapacity = 1000;
//		route2.hops.push_back(contactPlan_->getContactById(4));
//		routeList_[terminusNode].push_back(route2);
	}

	if (eid_ == 2)
	{
		CgrRoute route1;
		route1.toNodeNbr = 3;
		route1.fromTime = 10;
		route1.toTime = 20;
		route1.arrivalConfidence = 1.0;
		route1.arrivalTime = 30;
		route1.maxCapacity = 1000;
		route1.hops.push_back(contactPlan_->getContactById(2));
		route1.hops.push_back(contactPlan_->getContactById(3));
		//routeList_[terminusNode].push_back(route1);
		route = &route1;
		return;
	}

	if (eid_ == 3)
	{
		CgrRoute route1;
		route1.toNodeNbr = 4;
		route1.fromTime = 20;
		route1.toTime = 30;
		route1.arrivalConfidence = 1.0;
		route1.arrivalTime = 20;
		route1.maxCapacity = 1000;
		route1.hops.push_back(contactPlan_->getContactById(3));
		//routeList_[terminusNode].push_back(route1);
		route = &route1;
		return;
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

