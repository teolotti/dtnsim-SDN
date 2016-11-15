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
	  //TODO: clone bundle and send it for routing (enqueueToLimbo in ion, but no sure here)
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

		cout << (*it).toNodeNbr << endl;

	}

	// map<int, vector<CgrRoute *> > routeList_;
	// double routeListLastEditTime=-1;

//	To test some proximate ProximateNode:
//	ProximateNode node1;
//	node1.arrivalConfidence = 0.5;
//	node1.arrivalTime = 100;
//	node1.contactId = 10;
//	node1.forfeitTime = 10;
//	node1.hopCount = 5;
//	node1.neighborNodeNbr = 2;
//	proximateNodes->push_back(node1);
//
//	ProximateNode node2;
//	node2.arrivalConfidence = 0.6;
//	node2.arrivalTime = 100;
//	node2.contactId = 7;
//	node2.forfeitTime = 10;
//	node2.hopCount = 3;
//	node2.neighborNodeNbr = 4;
//	proximateNodes->push_back(node2);

}

void RoutingCgrModel::loadRouteList(int terminusNode, double simTime)
{
	cout << "Node " << eid_ << " loadRouteList to terminus " << terminusNode << endl;

	vector<CgrRoute> cgrRoute;
	routeList_[terminusNode] = cgrRoute;

	// To test some CgrRoute:
	CgrRoute route1;
	route1.toNodeNbr = terminusNode;
	route1.fromTime = 0;
	route1.toTime = 5;
	route1.arrivalConfidence = 1.0;
	route1.arrivalTime = 10;
	route1.maxCapacity = 1000;
	route1.hops.push_back(contactPlan_->getContactById(1));
	route1.hops.push_back(contactPlan_->getContactById(3));
	route1.hops.push_back(contactPlan_->getContactById(5));
	routeList_[terminusNode].push_back(route1);

	CgrRoute route2;
	route2.toNodeNbr = terminusNode;
	route2.fromTime = 15;
	route2.toTime = 20;
	route2.arrivalConfidence = 1.0;
	route2.arrivalTime = 15;
	route2.maxCapacity = 1000;
	route2.hops.push_back(contactPlan_->getContactById(7));
	routeList_[terminusNode].push_back(route2);
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

