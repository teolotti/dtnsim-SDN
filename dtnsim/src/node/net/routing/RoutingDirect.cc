/*
 * RoutingDirect.cpp
 *
 *  Created on: Nov 8, 2016
 *      Author: juanfraire
 */

#include "RoutingDirect.h"

RoutingDirect::RoutingDirect(int eid, SdrModel * sdr, ContactPlan * contactPlan)
{
	eid_ = eid;
	sdr_ = sdr;
	contactPlan_ = contactPlan;
}

RoutingDirect::~RoutingDirect()
{

}

void RoutingDirect::routeAndQueueBundle(BundlePkt * bundle, double simTime)
{
	int contactId=0; // contact 0 is the limbo

	// Search for the target contact to send the bundle
	int neighborEid = bundle->getDestinationEid();
	double earliestStart = numeric_limits<double>::max();
	vector<Contact> contacts = contactPlan_->getContactsBySrcDst(eid_,neighborEid);
	for(size_t i = 0; i<contacts.size(); i++){
		if((contacts.at(i).getEnd()>simTime)&&(contacts.at(i).getStart()<earliestStart))
			contactId=contacts.at(i).getId();
	}

	// Enqueue the bundle
	bundle->setNextHopEid(contactPlan_->getContactById(contactId)->getDestinationEid());
	sdr_->enqueueBundleToContact(bundle, contactId);
}


