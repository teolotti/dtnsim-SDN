/*
 * RoutingCgrModelRev17.cc
 *
 *  Created on: Apr 12, 2017
 *      Author: juanfraire
 */

#include "RoutingCgrModelRev17.h"

RoutingCgrModelRev17::RoutingCgrModelRev17(int eid, SdrModel * sdr, ContactPlan * contactPlan)
{
	eid_ = eid;
	sdr_ = sdr;
	contactPlan_ = contactPlan;
}

RoutingCgrModelRev17::~RoutingCgrModelRev17()
{

}

void RoutingCgrModelRev17::routeAndQueueBundle(BundlePkt * bundle, double simTime)
{


}
