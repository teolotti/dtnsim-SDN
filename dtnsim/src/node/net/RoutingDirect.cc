/*
 * RoutingDirect.cpp
 *
 *  Created on: Nov 8, 2016
 *      Author: juanfraire
 */

#include "RoutingDirect.h"

RoutingDirect::RoutingDirect()
{
	// TODO Auto-generated destructor stub
}

RoutingDirect::~RoutingDirect()
{
	// TODO Auto-generated destructor stub
}

void RoutingDirect::setLocalNode(int eid){
	eid_ = eid;
}

void RoutingDirect::setQueue(map<int, queue<Bundle *> > * bundlesQueue){
	bundlesQueue_ = bundlesQueue;
}

void RoutingDirect::setContactPlan(ContactPlan * contactPlan){
	contactPlan_ = contactPlan;
}

void RoutingDirect::routeBundle(Bundle * bundle, double simTime)
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
	map<int, queue<Bundle *> >::iterator it = bundlesQueue_->find(contactId);
	if (it != bundlesQueue_->end())
	{
		it->second.push(bundle);
	}
	else
	{
		queue<Bundle *> q;
		q.push(bundle);
		(*bundlesQueue_)[contactId] = q;
	}
}


