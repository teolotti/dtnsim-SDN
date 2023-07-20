/*
 * RoutingDeterministic.cpp
 *
 *  Created on: Jul 18, 2017
 *      Author: FRaverta
 */

#include <src/node/dtn/routing/RoutingDeterministic.h>

RoutingDeterministic::RoutingDeterministic(int eid, SdrModel * sdr, map<string, ContactPlan> *contactPlans, RoutingInterRegions *routingInterRegions) :
		Routing(eid, sdr)
{
	contactPlan_ = &contactPlans->operator[]("");
	contactPlans_ = contactPlans;
	routingInterRegions_ = routingInterRegions;
}

RoutingDeterministic::RoutingDeterministic(int eid, SdrModel * sdr, ContactPlan *contactPlan, RoutingInterRegions *routingInterRegions) :
		Routing(eid, sdr)
{
	contactPlan_ = contactPlan;
	contactPlans_->operator[]("") = *contactPlan;
	routingInterRegions_ = routingInterRegions;
}

RoutingDeterministic::~RoutingDeterministic()
{
}

void RoutingDeterministic::msgToOtherArrive(BundlePkt * bundle, double simTime, int terminusNode)
{
	// Route and enqueue bundle
	routeAndQueueBundle(bundle, simTime, terminusNode);
}

bool RoutingDeterministic::msgToMeArrive(BundlePkt * bundle)
{
	return true;
}

void RoutingDeterministic::contactStart(Contact *c)
{

}

void RoutingDeterministic::successfulBundleForwarded(long bundleId, Contact * c, bool sentToDestination)
{

}

void RoutingDeterministic::refreshForwarding(Contact * c)
{

}

void RoutingDeterministic::contactEnd(Contact *c)
{
	while (sdr_->isBundleForContact(c->getId()))
	{
		BundlePkt* bundle = sdr_->getNextBundleForContact(c->getId());
		sdr_->popNextBundleForContact(c->getId());

		// if IRR is applied, this variable is changed for a passageNodeID
		int terminusNode = bundle->getDestinationEid();
		string currentRegionId;
		if (routingInterRegions_ != NULL)
		{
			// get the region/regions where this node can transmit the bundle to
			set < string > thisNodeRegionIds = routingInterRegions_->getRegionDatabase()->getTxRegionIds(this->eid_);

			// get the region/regions where this bundle can be received
			set < string > destinationRegionIds = routingInterRegions_->getRegionDatabase()->getRxRegionIds(bundle->getDestinationEid());

			//cout << "calling to inter region routing" << endl;

			// call inter region routing
			IrrRoute irrRoute = routingInterRegions_->computeInterRegionsRouting(bundle, thisNodeRegionIds, destinationRegionIds);

			// get localDestinationEid from irrRoute
			list < pair<int, pair<string, string> > > route = irrRoute.route;
			for (auto it1 = thisNodeRegionIds.begin(); it1 != thisNodeRegionIds.end(); ++it1)
			{
				string sourceRegion = *it1;

				for (auto it2 = route.begin(); it2 != route.end(); ++it2)
				{
					if (it2->second.first == sourceRegion)
					{
						int passagewayNodeEid = it2->first;
						if (passagewayNodeEid == 0)
						{
							terminusNode = bundle->getDestinationEid();
							currentRegionId = it2->second.first;
							break;
						}
						else if (passagewayNodeEid != this->eid_)
						{
							terminusNode = passagewayNodeEid;
							currentRegionId = it2->second.first;
							break;
						}
						else
						{
							continue;
						}
					}
				}
			}

			// before calling to intra region routing, we set the currentRegionId
			this->setCurrentRegionId(currentRegionId);
		}

		//emit(dtnBundleReRouted, true);
		routeAndQueueBundle(bundle, simTime().dbl(), terminusNode);
	}
}

bool RoutingDeterministic::haveElementsInCommon(set<string> la, set<string> lb)
{
	for (auto it1 = la.begin(); it1 != la.end(); ++it1)
	{
		if (lb.find(*it1) != lb.end())
		{
			return true;
		}
	}

	return false;
}

