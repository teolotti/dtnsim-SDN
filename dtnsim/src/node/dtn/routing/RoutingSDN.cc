/*
 * RoutingSDN.cpp
 *
 *  Created on: Nov 9, 2023
 *      Author: matteo
 */

#include "RoutingSDN.h"
#include <algorithm>
#include <iterator>

using namespace std;

Controller* RoutingSDN::controllerPtr = nullptr;

RoutingSDN::RoutingSDN(int eid, SdrModel * sdr, ContactPlan * contactPlan, int nodeNum) : RoutingDeterministic(eid, sdr, contactPlan){

	nodeNum_ = nodeNum;

	controllerPtr = controllerPtr->getInstance(contactPlan, nodeNum);
}

void RoutingSDN::routeAndQueueBundle(BundlePkt* bundle, double simTime){

	vector<int> route = controllerPtr->getRoute(bundle);  //values in the vector are ID of Nodes

	if(route.empty()){

		route = controllerPtr->buildRoute(bundle, simTime);

	}

	std::vector<int>::iterator nextIDit = find(route.begin(), route.end(), this->eid_); //nextIDit represents iterator to next Node ID
	int destID = bundle->getDestinationEid();
	if((destID != this->eid_) && (nextIDit != route.end())){
		++nextIDit;
		bundle->setNextHopEid(nextIDit);
		//serve il contact
		sdr_->enqueueBundleToContact(bundle, *contactIDit);
	}
}

RoutingSDN::~RoutingSDN()
{
	// TODO Auto-generated destructor stub
}

