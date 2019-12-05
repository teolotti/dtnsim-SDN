#include <src/node/dtn/routing/RoutingCgrCentralized.h>

RoutingCgrCentralized::RoutingCgrCentralized(int eid, int neighborsNum, SdrModel *sdr, ContactPlan *localContactPlan,
        string routingType)
    : RoutingDeterministic(eid, sdr, localContactPlan)
{
    routingType_ = routingType;
    neighborsNum_ = neighborsNum;
    routeTable_.resize(neighborsNum);
}

RoutingCgrCentralized::~RoutingCgrCentralized()
{
}

// TODO: refactor route capacity check, used both in routeAndQueueBundle() and cgrForward().

void RoutingCgrCentralized::routeAndQueueBundle(BundlePkt *bundle, double simTime) {

    // If no extensionBlock, find the best route for this bundle
    if (routingType_.find("extensionBlock:off") != std::string::npos) {
        this->cgrForward(bundle);

    } else {
        // Non-empty EB: Use EB Route to route bundle
        CgrRoute ebRoute = bundle->getCgrRoute();
        // Check if encoded path is valid, update local contacts and route hops
        bool ebRouteIsValid = true;

        // Reason 1) If this bundle first contact is not a contact to this node, something
        // weird happened (might be a bundle re-route). Declare ebRoute invalid.
        if (ebRoute.hops.at(0)->getDestinationEid() != eid_) {
            ebRouteIsValid = false;
        }

        // Reason 2) If the remaining volume of local contact plan cannot
        // accommodate the encoded path, declare ebRout invalid.
        vector<Contact *> newHops;
        for (vector<Contact *>::iterator hop = ebRoute.hops.begin(); hop != ebRoute.hops.end(); ++hop) {
            if ((*hop)->getDestinationEid() == eid_) {
                // This is the contact that made this bundle arrive here,
                // do not check anything and discard it from the newHops path
                continue;
            }

            if (routingType_.find("volumeAware:1stContact") != std::string::npos) {
                // Only check first contact volume
                if ((*hop)->getSourceEid() == eid_) {
                    if (contactPlan_->getContactById((*hop)->getId())->getResidualVolume()
                            < bundle->getByteLength()) {
                        // Not enough residual capacity from local view of the path
                        ebRouteIsValid = false;
                        // TODO: this break could be out of this if-statement
                        break;
                    }
                }
            }

            if (routingType_.find("volumeAware:allContacts") != std::string::npos) {
                // Check all contacts volume
                if (contactPlan_->getContactById((*hop)->getId())->getResidualVolume()
                        < bundle->getByteLength()) {
                    // Not enough residual capacity from local view of the path
                    ebRouteIsValid = false;
                    break;
                }
            }

            newHops.push_back(contactPlan_->getContactById((*hop)->getId()));
        }

        if (ebRouteIsValid) {
            // Update necessary route parameters (from/to time and max/residual volume not necessary)
            ebRoute.hops = newHops;
            ebRoute.nextHop = ebRoute.hops[0]->getDestinationEid();

            // Enqueue using this route
            cout << "Using EB Route in header. Next hop: " << ebRoute.nextHop << endl;
            this->cgrEnqueue(bundle, &ebRoute);
        } else {
            // Discard old extension block, calculate and encode a new path and enqueue
            cout << "EB Route in header not valid, generating a new one" << endl;
            this->cgrForward(bundle);
        }
    }
}

void RoutingCgrCentralized::cgrForward(BundlePkt *bundle) {
    // Since we are running centralized CGR, all possible routes are computed
    // on earth and distributed to the corresponding nodes. So routing in a node is
    // just filtering the routes that cannot deliver the bundle and select the best
    // among the ones that can.

    int terminusNode = bundle->getDestinationEid();

    // Filter routes
    for (unsigned int r = 0; r < routeTable_.at(terminusNode).size(); r++) {

        routeTable_.at(terminusNode).at(r).filtered = false;

        // TODO: modify contact.getResidualVolume() to include current time in the formula.
        // criteria 1) filter route: capacity is depleted
        if (routeTable_.at(terminusNode).at(r).residualVolume < bundle->getByteLength()) {
            routeTable_.at(terminusNode).at(r).filtered = true;
            cout << "setting filtered true due to capacity to route next hop = "
                    << routeTable_.at(terminusNode).at(r).nextHop << endl;
        }

        // criteria 2) filter route: due time is passed
        if (routeTable_.at(terminusNode).at(r).toTime <= simTime().dbl()) {
            routeTable_.at(terminusNode).at(r).filtered = true;
            cout << "setting filtered true due to time to route next hop = "
                    << routeTable_.at(terminusNode).at(r).nextHop << endl;
        }

        // Filter those that go back to sender if such
        // type of forwarding is forbidden as per .ini file
        if (bundle->getReturnToSender() == false) {
            if (routeTable_.at(terminusNode).at(r).nextHop == bundle->getSenderEid()) {
                routeTable_.at(terminusNode).at(r).filtered = true;
                cout << "setting filtered true due to not-return-to-sender to route next hop = "
                        << routeTable_.at(terminusNode).at(r).nextHop << endl;
            }
        }
    }


    if (!routeTable_.at(terminusNode).empty()) {

        // Select best route
        vector<CgrRoute>::iterator bestRoute;
        bestRoute = min_element(routeTable_.at(terminusNode).begin(), routeTable_.at(terminusNode).end(),
                this->compareRoutes);

        // TODO: add table entries explored metrics

        // if route is plausible (i.e. such route exists and is not filtered), enqueue bundle
        if (bestRoute->nextHop != NO_ROUTE_FOUND && !bestRoute->filtered) {
            // Enqueue bundle to route and update volumes
            this->cgrEnqueue(bundle, &(*bestRoute));
            return;
        }
    }

    // If the route is not plausible, enqueue to limbo
    bundle->setNextHopEid(NO_ROUTE_FOUND);
    sdr_->enqueueBundleToContact(bundle, 0);

    cout << "*BestRoute not found (enqueing to limbo)" << endl;
}

void RoutingCgrCentralized::cgrEnqueue(BundlePkt *bundle, CgrRoute *bestRoute) {
    cout << "*Best: routeTable[" << bestRoute->terminusNode << "][ ]: nextHop: " << bestRoute->nextHop << ", frm "
            << bestRoute->fromTime << " to " << bestRoute->toTime << ", arrival time: " << bestRoute->arrivalTime
            << ", volume: " << bestRoute->residualVolume << "/" << bestRoute->maxVolume << "Bytes" << endl;

    // Update route's residual volume
    for (vector<Contact *>::iterator hop = bestRoute->hops.begin(); hop != bestRoute->hops.end(); ++hop) {
        (*hop)->setResidualVolume((*hop)->getResidualVolume() - bundle->getByteLength());

        // This should never happen. We'ĺl temporarily leave
        // this exit code 1 here to detect potential issues
        // with the volume booking algorithms
        if ((*hop)->getResidualVolume() < 0)
            exit(1);
    }

    // TODO: check whether this can be optimized. Probably just iterate
    // over all routes and get the minimum residual volume among all contacts.

    // Update residualVolume of all routes that uses the updated hops (including those
    // routes that leads to other destinations). This is a very expensive routine
    // that scales with large routes tables that need to happen in forwarding time.
    for (int n = 1; n < neighborsNum_; n++)
        for (unsigned int r = 0; r < routeTable_.at(n).size(); r++)
            for (vector<Contact *>::iterator hop1 = routeTable_.at(n).at(r).hops.begin();
                    hop1 != routeTable_.at(n).at(r).hops.end(); ++hop1)
                for (vector<Contact *>::iterator hop2 = bestRoute->hops.begin();
                        hop2 != bestRoute->hops.end(); ++hop2)
                    if ((*hop1)->getId() == (*hop2)->getId())
                        // Does the reduction of this contact volume requires a route volume update?
                        if (routeTable_.at(n).at(r).residualVolume > (*hop1)->getResidualVolume()) {
                            routeTable_.at(n).at(r).residualVolume = (*hop1)->getResidualVolume();
                            cout << "*Rvol: routeTable[" << n << "][" << r << "]: updated to "
                                    << (*hop1)->getResidualVolume() << "Bytes (all contacts)" << endl;
                        }

    // Save CgrRoute in header
    if (routingType_.find("extensionBlock:on") != std::string::npos)
        bundle->setCgrRoute(*bestRoute);

    // Enqueue bundle
    cout << "queuing bundle in contact " << bestRoute->hops.at(0)->getId() << endl;

    bundle->setNextHopEid(bestRoute->nextHop);
    sdr_->enqueueBundleToContact(bundle, bestRoute->hops.at(0)->getId());
}

// This functions is used to determine the best route out of a route list.
// Must returns true if first argument is better (i.e., minor)
bool RoutingCgrCentralized::compareRoutes(CgrRoute i, CgrRoute j) {

    // If one is filtered, the other is the best
    if (i.filtered && !j.filtered)
        return false;
    if (!i.filtered && j.filtered)
        return true;

    // If both are not filtered, then compare criteria,
    // If both are filtered, return any of them.

    // criteria 1) lowest arrival time
    if (i.arrivalTime < j.arrivalTime)
        return true;
    else if (i.arrivalTime > j.arrivalTime)
        return false;
    else {
        // if equal, criteria 2) lowest hop count
        if (i.hops.size() < j.hops.size())
            return true;
        else if (i.hops.size() > j.hops.size())
            return false;
        else {
            // if equal, criteria 3) larger residual volume
            if (i.residualVolume > j.residualVolume)
                return true;
            else if (i.residualVolume < j.residualVolume)
                return false;
            else {
                // if equal, first is better.
                return true;
            }
        }
    }
}
