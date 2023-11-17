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



RoutingSDN::RoutingSDN(int eid, SdrModel * sdr, ContactPlan * contactPlan, int nodeNum, string routingType, Controller* instance) : RoutingDeterministic(eid, sdr, contactPlan), controllerPtr(instance){

	nodeNum_ = nodeNum;

	routingType_ = routingType;

	controllerPtr->addRoutingSDNInstance(this);
}

void RoutingSDN::routeAndQueueBundle(BundlePkt* bundle, double simTime){

	vector<int> route = getRoute(bundle);  //values in the vector are ID of Contacts

	if(route.empty()){

		route = controllerPtr->buildRoute(bundle, simTime, routingType_);

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

vector<int> RoutingSDN::getRoute(BundlePkt* bundle){

	vector<int> emptyRoute;

    vector<pair<BundlePkt*, vector<int>>>::iterator it = find_if(routes.begin(), routes.end(),
            [bundle](const std::pair<BundlePkt*, std::vector<int>>& element) {
                return element.first == bundle;
            });


	if (it != routes.end())
		return it->second;
	else
		return emptyRoute;

}

RoutingSDN::~RoutingSDN()
{
	// TODO Auto-generated destructor stub
}

