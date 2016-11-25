/*
 * RoutingCgrIon350.cpp
 *
 *  Created on: Nov 8, 2016
 *      Author: juanfraire
 */

#include "RoutingCgrIon350.h"

RoutingCgrIon350::RoutingCgrIon350()
{
	// TODO Auto-generated constructor stub

}

RoutingCgrIon350::~RoutingCgrIon350()
{
	// TODO Auto-generated destructor stub
}

void RoutingCgrIon350::setLocalNode(int eid){
	eid_ = eid;
}

void RoutingCgrIon350::setSdr(SdrModel * sdr){
	sdr_ = sdr;
}

void RoutingCgrIon350::setContactPlan(ContactPlan * contactPlan){
	contactPlan_ = contactPlan;
}

void RoutingCgrIon350::routeBundle(Bundle * bundle, double simTime)
{
	int contactId=0; // contact 0 is the limbo


	// Search for the target contact to send the bundle
	// TODO: Implement necesary calls to CGR in ION 3.5.0

	// Enqueue the bundle
	bundle->setNextHopEid(contactPlan_->getContactById(contactId)->getDestinationEid());
	sdr_->enqueueBundleToContact(bundle, contactId);

}
