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

void RoutingCgrIon350::setQueue(map<int, queue<Bundle *> > * bundlesQueue){
	bundlesQueue_ = bundlesQueue;
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
