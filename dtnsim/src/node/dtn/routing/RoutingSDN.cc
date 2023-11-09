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
	vector<int> route = controllerPtr->getRoute(bundle);

	if(route.empty()){
		route = controllerPtr->buildRoute(bundle, simTime);
	}

	std::vector<int>::iterator current = find(route.begin(), route.end(), this->eid_);
	if(bundle->getDestinationEid()!=this->eid_ && current != route.end()){
		++current;
		sdr_->enqueueBundleToContact(bundle, *current);
	}
}

RoutingSDN::~RoutingSDN()
{
	// TODO Auto-generated destructor stub
}

