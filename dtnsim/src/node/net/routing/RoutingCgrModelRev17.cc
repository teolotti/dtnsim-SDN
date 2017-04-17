/*
 * RoutingCgrModelRev17.cc
 *
 *  Created on: Apr 12, 2017
 *      Author: juanfraire
 */

#include "RoutingCgrModelRev17.h"

RoutingCgrModelRev17::RoutingCgrModelRev17(int eid, int nodeNum, SdrModel * sdr, ContactPlan * contactPlan, string routingType, bool printDebug)
{
	eid_ = eid;
	nodeNum_ = nodeNum;
	sdr_ = sdr;
	contactPlan_ = contactPlan;
	printDebug_ = printDebug;

	routingType_ = routingType;
	if (routingType_.compare("none") == 0)
	{
		routingType_ = "volumeAware:allContacts";
	}

	// "volumeAware:allContacts"
	// "volumeAware:1stContact"
	// "volumeAware:extensionBlock"

	cout << routingType_ << endl;

	// Initialize routeTable_
	routeTable_.resize(nodeNum_);
	for (int n1 = 0; n1 < nodeNum_; n1++)
		routeTable_.at(n1).resize(nodeNum_);
	this->clearRouteTable();
}

RoutingCgrModelRev17::~RoutingCgrModelRev17()
{

}

void RoutingCgrModelRev17::routeAndQueueBundle(BundlePkt * bundle, double simTime)
{
	// Disable cout if degug disabled
	if (printDebug_ == false)
		cout.setstate(std::ios_base::failbit);

	cout << "TIME: " << simTime << "s, NODE: " << eid_ << ", routing bundle to dst: " << bundle->getDestinationEid() << " (" << bundle->getByteLength() << "Bytes)" << endl;

	this->cgrForward(bundle, simTime);

	// Re-enable cout if degug disabled
	if (printDebug_ == false)
		cout.clear();
}

/////////////////////////////////////////////////
// Functions based Ion architecture
/////////////////////////////////////////////////

void RoutingCgrModelRev17::cgrForward(BundlePkt * bundle, double simTime)
{
	// If contact plan was changed, empty route list
	if (contactPlan_->getLastEditTime() > routeTableLastEditTime)
		this->clearRouteTable();

	routeTableLastEditTime = simTime;

	int terminusNode = bundle->getDestinationEid();

	// Check route table and recalculate if necesary
	for (int nodeEID = 1; nodeEID < nodeNum_; nodeEID++)
	{
		bool needRecalculation = false;

		// Empty route condition (NO_ROUTE_FOUND does not trigger a recalculation)
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
		// do not consider route back to sender (temporaly set arrivalTime to infinite)
		double arrivalTime = routeTable_.at(terminusNode).at(bundle->getSenderEid()).arrivalTime;
		routeTable_.at(terminusNode).at(bundle->getSenderEid()).arrivalTime = numeric_limits<double>::max();
		bestRoute = min_element(routeTable_.at(terminusNode).begin(), routeTable_.at(terminusNode).end(), this->compareRoutes);
		routeTable_.at(terminusNode).at(bundle->getSenderEid()).arrivalTime = arrivalTime;
	}

	// Enqueue bundle to route and update volumes
	if ((*bestRoute).nextHop != NO_ROUTE_FOUND)
	{
		bundle->setNextHopEid((*bestRoute).nextHop);
		sdr_->enqueueBundleToContact(bundle, (*bestRoute).hops.at(0)->getId());

		cout << "*Best: routeTable[" << terminusNode << "][" << (*bestRoute).nextHop << "]: nextHop: " << (*bestRoute).nextHop << ", frm " << (*bestRoute).fromTime << " to " << (*bestRoute).toTime << ", arrival time: " << (*bestRoute).arrivalTime << ", volume: " << (*bestRoute).residualVolume << "/"
				<< (*bestRoute).maxVolume << "Bytes" << endl;

		// Update residualVolume: this route hops
		for (vector<Contact *>::iterator hop = (*bestRoute).hops.begin(); hop != (*bestRoute).hops.end(); ++hop)
			(*hop)->setResidualVolume((*hop)->getResidualVolume() - bundle->getByteLength());

		//this->printContactPlan();

		// Update residualVolume: all routes that uses these hops
		if (routingType_.compare("volumeAware:allContacts") == 0 || routingType_.compare("volumeAware:extensionBlock") == 0)
			for (int n1 = 1; n1 < nodeNum_; n1++)
				for (int n2 = 1; n2 < nodeNum_; n2++)
					for (vector<Contact *>::iterator hop1 = routeTable_.at(n1).at(n2).hops.begin(); hop1 != routeTable_.at(n1).at(n2).hops.end(); ++hop1)
						for (vector<Contact *>::iterator hop2 = (*bestRoute).hops.begin(); hop2 != (*bestRoute).hops.end(); ++hop2)
							if ((*hop1)->getId() == (*hop2)->getId())
								// Does the reduction of this contact volume requires a route volume update?
								if (routeTable_.at(n1).at(n2).residualVolume > (*hop1)->getResidualVolume())
								{
									routeTable_.at(n1).at(n2).residualVolume = (*hop1)->getResidualVolume();
									cout << "*Rvol: routeTable[" << n1 << "][" << n2 << "]: updated to " << (*hop1)->getResidualVolume() << "Bytes" << endl;
								}

		// Update residualVolume: 1st contact
		if (routingType_.compare("volumeAware:1stContact") == 0)
		{
			(*bestRoute).residualVolume = (*bestRoute).hops[0]->getResidualVolume();
		}

	}
	else
	{
		// Enqueue to Limbo
		bundle->setNextHopEid((*bestRoute).nextHop);
		sdr_->enqueueBundleToContact(bundle, 0);

		cout << "*BestRoute not found (enqueing to limbo)" << endl;
	}
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
			// (TODO: this calculation assumes non-overlapped contacts, can be improved)
			if (contact->getVolume() < maxVolume)
				maxVolume = contact->getVolume();

			// Update confidence
			route->confidence *= contact->getConfidence();

			// Store hop
			route->hops.insert(route->hops.begin(), contact);
		}

		route->nextHop = route->hops[0]->getDestinationEid();
		route->fromTime = route->hops[0]->getStart();
		route->toTime = earliestEndTime;
		route->maxVolume = maxVolume;
		route->residualVolume = maxVolume;
	}
	else
	{
		// No route found
		route->nextHop = NO_ROUTE_FOUND;
		route->arrivalTime = numeric_limits<double>::max(); // never chosen as best route
	}
}

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
			cout << "routeTable[" << terminusNode << "][" << nodeEID << "]: nextHop: " << route.nextHop << ", frm " << route.fromTime << " to " << route.toTime << ", arrival time: " << route.arrivalTime << ", volume: " << route.residualVolume << "/" << route.maxVolume << "Bytes" << endl;
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

void RoutingCgrModelRev17::printContactPlan()
{
	vector<Contact>::iterator it;
	for (it = contactPlan_->getContacts()->begin(); it != contactPlan_->getContacts()->end(); ++it)
		cout << "a contact +" << (*it).getStart() << " +" << (*it).getEnd() << " " << (*it).getSourceEid() << " " << (*it).getDestinationEid() << " " << (*it).getResidualVolume() << "/" << (*it).getVolume() << endl;
}
