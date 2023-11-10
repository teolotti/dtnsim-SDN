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

	vector<int> route = controllerPtr->getRoute(bundle);  //values in the vector are ID of Contacts

	if(route.empty()){

		route = controllerPtr->buildRoute(bundle, simTime);

	}



	int targetEid = this->eid_;

	auto nextContactIDit = std::find_if(route.begin(), route.end(),
	    [this, targetEid](const int& x) {
			int source = contactPlan_->getContactById(x)->getSourceEid();
	        return source == targetEid;
	    });//Search for the next contact for the bundle, contact whose SourceId is this Node's Id

	int destID = bundle->getDestinationEid();

	Contact* nextContact = contactPlan_->getContactById(*nextContactIDit);

	if((destID != this->eid_) && (nextContactIDit != route.end())){
		bundle->setNextHopEid(nextContact->getDestinationEid());		//serve il contact
		sdr_->enqueueBundleToContact(bundle, *nextContactIDit);
	}
}

RoutingSDN::~RoutingSDN()
{
	// TODO Auto-generated destructor stub
}

