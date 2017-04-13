/*
 * RoutingCgrModelRev17.cc
 *
 *  Created on: Apr 12, 2017
 *      Author: juanfraire
 */

#include "RoutingCgrModelRev17.h"

RoutingCgrModelRev17::RoutingCgrModelRev17(int eid, int nodeNum, SdrModel * sdr, ContactPlan * contactPlan)
{
	eid_ = eid;
	nodeNum_ = nodeNum;
	sdr_ = sdr;
	contactPlan_ = contactPlan;

	// Initialize routeTable_ size (toNodeNbr=-1 es empty route)
	routeTable_.resize(nodeNum);
	for (int nodeEID = 0; nodeEID < nodeNum_; nodeEID++)
		routeTable_.at(nodeEID).toNodeNbr = EMPTY_ROUTE;
}

RoutingCgrModelRev17::~RoutingCgrModelRev17()
{

}

void RoutingCgrModelRev17::routeAndQueueBundle(BundlePkt * bundle, double simTime)
{
	// Disable cout if degug disabled
	if (printDebug == false)
		cout.setstate(std::ios_base::failbit);

	cout << "node: " << eid_ << " at time: " << simTime << " routing bundle to dst: " << bundle->getDestinationEid() << endl;
	this->cgrForward(bundle, simTime);

	// Re-enable cout if degug disabled
	if (printDebug == false)
		cout.clear();
}

/////////////////////////////////////////////////
// Functions based Ion architecture
/////////////////////////////////////////////////

void RoutingCgrModelRev17::cgrForward(BundlePkt * bundle, double simTime)
{
	// If contact plan was changed, empty route list
	if (contactPlan_->getLastEditTime() > routeTableLastEditTime)
		for (int nodeEID = 0; nodeEID < nodeNum_; nodeEID++)
			routeTable_.at(nodeEID).toNodeNbr = EMPTY_ROUTE;

	routeTableLastEditTime = simTime;

	// Check route table and recalculate if necesary
	for (int nodeEID = 0; nodeEID < nodeNum_; nodeEID++)
	{
		bool needRecalculation = false;

		// Empty route condition
		if (routeTable_.at(nodeEID).toNodeNbr == EMPTY_ROUTE)
			needRecalculation = true;

		// Due route condition
		if (routeTable_.at(nodeEID).toTime < simTime)
			needRecalculation = true;

		// Depleted route condition
		if (routeTable_.at(nodeEID).residualVolume < bundle->getByteLength())
			needRecalculation = true;

		if (needRecalculation)
		{
			CgrRoute route;
			this->findNextBestRoute(nodeEID, bundle->getDestinationEid(), &route, simTime);
			routeTable_.at(nodeEID) = route;
		}
	}

	// Print route
	for (int nodeEID = 0; nodeEID < nodeNum_; nodeEID++)
	{
		CgrRoute route = routeTable_.at(nodeEID);
		if (route.toNodeNbr == NO_ROUTE_FOUND)
			cout << "routeTable[" << nodeEID << "]: No route found"<< endl;
		else if (route.toNodeNbr == EMPTY_ROUTE) // should never happen
			cout << "routeTable[" << nodeEID << "]: Need to recalculate route"<< endl;
		else
			cout << "routeTable[" << nodeEID << "]: toNode: " << route.toNodeNbr << ", frm " << route.fromTime << " to " << route.toTime << ", arrival time: " << route.arrivalTime << ", volume: " << route.residualVolume << "/" << route.maxVolume << endl;
	}

	// TODO: Select route, enqueue, and update volumes

	// if (bundle->getReturnToSender() == false)
}

void RoutingCgrModelRev17::findNextBestRoute(int entryNode, int terminusNode, CgrRoute * route, double simTime)
{
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
	cout << "  suppressing initial contacts: ";
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
			cout << (*it).getId() << ", ";
		}
	}
	cout << endl;

	// Start Dijkstra
	Contact * currentContact = rootContact;
	Contact * finalContact = NULL;
	double earliestFinalArrivalTime = numeric_limits<double>::max();

	cout << "  surfing contact-graph:";
	while (1)
	{
		cout << currentContact->getDestinationEid() << ",";

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
	cout << endl;

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
			// (TODO: this calculation assumes non-overlapped contacts, can be improved)
			if (contact->getVolume() < maxVolume)
				maxVolume = contact->getVolume();

			// Update confidence
			route->confidence *= contact->getConfidence();

			// Store hop
			route->hops.insert(route->hops.begin(), contact);
		}

		route->toNodeNbr = route->hops[0]->getDestinationEid();
		route->fromTime = route->hops[0]->getStart();
		route->toTime = earliestEndTime;
		route->maxVolume = maxVolume;
		route->residualVolume = maxVolume;
	}
	else
	{
		// No route found
		route->toNodeNbr = NO_ROUTE_FOUND;
	}
}