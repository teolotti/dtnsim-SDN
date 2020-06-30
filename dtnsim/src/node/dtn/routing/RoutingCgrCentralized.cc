#include <src/node/dtn/routing/RoutingCgrCentralized.h>

RoutingCgrCentralized::RoutingCgrCentralized(int eid, int neighborsNum, SdrModel *sdr, ContactPlan *localContactPlan,
        bool printDebug, string routingType, int maxRouteHops, int maxRoutesWithSameDst,
        double bfsIntervalTime, int bfsIntervalNum)
    : RoutingDeterministic(eid, sdr, localContactPlan)
{
    simTime_ = 0.0;
    routingType_ = routingType;
    neighborsNum_ = neighborsNum;
    routeTable_.resize(neighborsNum + 1);
    maxRouteHops_ = maxRouteHops;
    maxRoutesWithSameDst_ = maxRoutesWithSameDst;
    computedRoutes_ = 0;
    printDebug_ = printDebug;
    bfsIntervalTime_ = bfsIntervalTime;
    bfsIntervalNum_ = bfsIntervalNum;

    double clock_start = clock();
    this->initializeRouteTable();
    timeToComputeRoutes_ = (double) (clock() - clock_start) / CLOCKS_PER_SEC;
}


RoutingCgrCentralized::~RoutingCgrCentralized()
{
}


void RoutingCgrCentralized::routeAndQueueBundle(BundlePkt *bundle, double simTime) {
    if (!printDebug_)
        cout.setstate(std::ios_base::failbit);

	simTime_ = simTime;

    // If no extensionBlock, find the best route for this bundle
    if (routingType_.find("extensionBlock:off") != std::string::npos) {
        this->cgrForward(bundle);

    } else {
        CgrRoute ebRoute = bundle->getCgrRoute();
        if (bundle->getCgrRoute().nextHop == EMPTY_ROUTE) {
            this->cgrForward(bundle);
            return;
        }

        // Non-empty EB: Use EB Route to route bundle
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

            // Check all contacts volume
            if (contactPlan_->getContactById((*hop)->getId())->getResidualVolume()
                    < bundle->getByteLength()) {
                // Not enough residual capacity from local view of the path
                ebRouteIsValid = false;
                break;
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

    // Re-enable cout if debug disabled
    if (!printDebug_)
        cout.clear();
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

        // criteria 1) filter route: capacity is depleted
        if (routeTable_.at(terminusNode).at(r).residualVolume < bundle->getByteLength()) {
            routeTable_.at(terminusNode).at(r).filtered = true;
            cout << "setting filtered true due to capacity to route next hop = "
                    << routeTable_.at(terminusNode).at(r).nextHop << endl;
        }

        // criteria 2) filter route: due time is passed
        if (routeTable_.at(terminusNode).at(r).toTime <= simTime_) {
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
        bestRoute = min_element(routeTable_.at(terminusNode).begin(), routeTable_.at(terminusNode).end());

        // if route is plausible (i.e. such route exists and is not filtered), enqueue bundle
        if (bestRoute->nextHop != NO_ROUTE_FOUND && !bestRoute->filtered) {
            // Enqueue bundle to route and update volumes
            this->cgrEnqueue(bundle, &(*bestRoute));
            return;
        }
    }

    // If the route is not plausible, enqueue to limbo
    bundle->setNextHopEid(NO_ROUTE_FOUND);
    sdr_->enqueueBundleToContact(bundle, -1);

    cout << "*BestRoute not found (enqueing to limbo)" << endl;
}


void RoutingCgrCentralized::cgrEnqueue(BundlePkt *bundle, CgrRoute *bestRoute) {
    cout << "*Best: routeTable[" << bestRoute->terminusNode << "][ ]: nextHop: " << bestRoute->nextHop << ", frm "
            << bestRoute->fromTime << " to " << bestRoute->toTime << ", arrival time: " << bestRoute->arrivalTime
            << ", volume: " << bestRoute->residualVolume << "/" << bestRoute->maxVolume << "Bytes" << endl;

    // Update route's contacts residual volume
    for (vector<Contact *>::iterator hop = bestRoute->hops.begin(); hop != bestRoute->hops.end(); ++hop) {
        (*hop)->setResidualVolume((*hop)->getResidualVolume() - bundle->getByteLength());

        // This should never happen. We'ĺl temporarily leave
        // this exit code 1 here to detect potential issues
        // with the volume booking algorithms
        if ((*hop)->getResidualVolume() < 0)
            exit(1);
    }

    // Update residual volume of all routes. This is a very expensive routine
    // that scales with large routes tables that need to happen in forwarding time.
    for (int n = 1; n < neighborsNum_; n++)
        for (unsigned int r = 0; r < routeTable_.at(n).size(); r++)
            for (vector<Contact *>::iterator contact = routeTable_.at(n).at(r).hops.begin();
            		contact != routeTable_.at(n).at(r).hops.end(); ++contact)
                if (routeTable_.at(n).at(r).residualVolume > (*contact)->getResidualVolume()) {
                    routeTable_.at(n).at(r).residualVolume = (*contact)->getResidualVolume();
                    cout << "*Rvol: routeTable[" << n << "][" << r << "]: updated to "
                            << (*contact)->getResidualVolume() << "Bytes (all contacts)" << endl;
                }

    // Save CgrRoute in header
    if (routingType_.find("extensionBlock:on") != std::string::npos)
        bundle->setCgrRoute(*bestRoute);

    // Enqueue bundle
    cout << "queuing bundle in contact " << bestRoute->hops.at(0)->getId() << endl;

    bundle->setNextHopEid(bestRoute->nextHop);
    sdr_->enqueueBundleToContact(bundle, bestRoute->hops.at(0)->getId());
}


// This is the procedure that should be done on earth to initialize the route table of each node.
void RoutingCgrCentralized::initializeRouteTable() {
    if (!printDebug_)
        cout.setstate(std::ios_base::failbit);

    cout << "Initializing node " << eid_ << endl;

    if (routingType_.find("routeListType:bfs") != std::string::npos) {
        fillRouteTableWithBfs();
    } else if (routingType_.find("routeListType:firstEnded") != std::string::npos) {
        fillRouteTableWithFirstEnded();
    } else {
        cerr << "Centralized error while initializing routing table: unknown route list type." << endl;
        exit(1);
    }
    if (routeTable_.at(eid_).size() != 0) exit(1);

    if (!printDebug_)
        cout.clear();
}


void RoutingCgrCentralized::fillRouteTableWithBfs() {
    double filterTime = 0.0;
    for (int i = 0; i < bfsIntervalNum_; i++, filterTime += bfsIntervalTime_) {
        double minEndTime = numeric_limits<double>::max();
        findRoutesBfs(filterTime, &minEndTime);

        // Skip all steps that would yield the same result
        if (minEndTime == numeric_limits<double>::max())
            continue;
        while (minEndTime - filterTime > bfsIntervalTime_) {
            filterTime += bfsIntervalTime_;
            i++;
        }
    }
}


void RoutingCgrCentralized::findRoutesBfs(double minEndTimeFilter, double* outRouteMinEndTime) {
    list<CgrRoute> routesToExplore;

    // Use priority queue to sort routes from worst to best, so the worst is always at the top.
    priority_queue<CgrRoute> routesToNode[neighborsNum_ + 1];
    Contact selfContact = Contact(-1, 0, numeric_limits<double>::max(), eid_, eid_, 1.0, 1.0, 0);
    CgrRoute baseRoute = CgrRoute::RouteFromContact(&selfContact);
    routesToExplore.push_back(baseRoute);

    while (!routesToExplore.empty()) {
        CgrRoute currentRoute = routesToExplore.front();
        routesToExplore.pop_front();

        if (routesToNode[currentRoute.terminusNode].size() == maxRoutesWithSameDst_ &&
            routesToNode[currentRoute.terminusNode].top() < currentRoute)
            // No need to explore a route which is already worse than the best K routes.
            continue;

        vector<int> neighborIds = contactPlan_->getContactsBySrc(currentRoute.terminusNode);
        for (vector<int>::iterator neighborId = neighborIds.begin(); neighborId != neighborIds.end(); neighborId++) {
            Contact* neighbor = contactPlan_->getContactById(*neighborId);

            if (neighbor->getEnd() <= minEndTimeFilter)
                continue;

            if (neighbor->getDestinationEid() == eid_)
                continue;

            if (currentRoute.nodeIsNotVisited(neighbor->getDestinationEid())) {
                if (currentRoute.arrivalTime < neighbor->getEnd()) {
                    priority_queue<CgrRoute>* routesToDst = &routesToNode[neighbor->getDestinationEid()];
                    CgrRoute newRoute = currentRoute.extendWithContact(neighbor);

                    if (maxRoutesWithSameDst_ > 0 &&
                            routesToDst->size() == maxRoutesWithSameDst_ &&
                            newRoute < routesToDst->top()) {

                        routesToDst->pop();
                    }

                    if (maxRoutesWithSameDst_ == -1 || routesToDst->size() < maxRoutesWithSameDst_) {
                        routesToDst->push(newRoute);

                        if (maxRouteHops_ == -1 || newRoute.hops.size() < maxRouteHops_) {
                            routesToExplore.push_back(newRoute);
                        }
                    }
                }
            }
        }
    }

    /*** Set route table with routes found ***/
    for (int i = 1; i <= neighborsNum_; i++) {
        while (!routesToNode[i].empty()) {
            CgrRoute routeFound = routesToNode[i].top();
            routesToNode[i].pop();

            *outRouteMinEndTime = std::min(*outRouteMinEndTime, routeFound.toTime);
            if (std::find(routeTable_.at(i).begin(), routeTable_.at(i).end(),
                    routeFound) == routeTable_.at(i).end()) {

                routeTable_.at(i).push_back(routeFound);
                routeLengthVector_.push_back(routeFound.hops.size());
                computedRoutes_++;
            }
        }
    }
}


void RoutingCgrCentralized::fillRouteTableWithFirstEnded() {
    createContactsWork();
    Contact* minEndContact = NULL;

    findRoutesDijkstra(&minEndContact);
    while (minEndContact != NULL) {
        ((Work*) minEndContact->work)->suppressed = true;
        minEndContact = NULL;
        findRoutesDijkstra(&minEndContact);
    }
    clearContactsWork();
}


void RoutingCgrCentralized::findRoutesDijkstra(Contact** outMinEndContact) {
    Contact selfContact = Contact(-1, 0, numeric_limits<double>::max(), eid_, eid_, 1.0, 1.0, 0);

    Contact* finalContacts[neighborsNum_ + 1];
    double arrivalTimes[neighborsNum_ + 1];
    bool visitedNodes[neighborsNum_ + 1];
    for (int i = 1; i <= neighborsNum_; i++) {
        finalContacts[i] = NULL;
        arrivalTimes[i] = numeric_limits<double>::max();
        visitedNodes[i] = false;
    }

    arrivalTimes[eid_] = 0.0;
    finalContacts[eid_] = &selfContact;
    priority_queue<std::pair<double, int> > pq;
    pq.push(std::make_pair(0, eid_));

    // Begin Dijkstra
    while (!pq.empty()) {
        double arrivalTime = -pq.top().first; // Queue negative value to sort from low to high
        int currentNode = pq.top().second;
        pq.pop();

        if (visitedNodes[currentNode])
            continue;
        visitedNodes[currentNode] = true;

        if (arrivalTime > arrivalTimes[currentNode])
            continue;

        vector<int> currentNeighbors = contactPlan_->getContactsBySrc(currentNode);
        for (vector<int>::iterator neighborId = currentNeighbors.begin(); neighborId != currentNeighbors.end(); ++neighborId) {
            Contact* neighborEdge = contactPlan_->getContactById(*neighborId);
            int neighborNode = neighborEdge->getDestinationEid();

            if (visitedNodes[neighborNode])
                continue;

            if (neighborEdge->getEnd() <= arrivalTime || ((Work*) neighborEdge->work)->suppressed)
                continue;

            double owlt = neighborEdge->getRange();
            double nArrivalTime = std::max(arrivalTime, neighborEdge->getStart()) + owlt;

            if (nArrivalTime < arrivalTimes[neighborNode]) {
                arrivalTimes[neighborNode] = nArrivalTime;
                finalContacts[neighborNode] = neighborEdge;

                pq.push(make_pair(-nArrivalTime, neighborNode));
            }
        }

    } // End Dijkstra

    // Build routes
    for (int dstNode = 1; dstNode <= neighborsNum_; dstNode++) {

        if (dstNode == eid_) continue;

        CgrRoute bestRoute;
        bestRoute.filtered = false;
        bestRoute.hops.clear();
        if (finalContacts[dstNode] != NULL) {
            bestRoute.arrivalTime = arrivalTimes[dstNode];
            bestRoute.confidence = 1.0;
            bestRoute.toTime = numeric_limits<double>::max();
            bestRoute.maxVolume = numeric_limits<double>::max();
            bestRoute.residualVolume = numeric_limits<double>::max();

            for (Contact* contact = finalContacts[dstNode]; contact != &selfContact;
                    contact = finalContacts[contact->getSourceEid()]) {

                bestRoute.maxVolume = std::min(bestRoute.maxVolume, contact->getVolume());
                bestRoute.residualVolume = std::min(bestRoute.residualVolume,
                        contact->getResidualVolume());
                bestRoute.confidence *= contact->getConfidence();
                bestRoute.hops.insert(bestRoute.hops.begin(), contact);

                if (*outMinEndContact == NULL || contact->getEnd() < (*outMinEndContact)->getEnd()) {
                    *outMinEndContact = contact;
                }
            }

            double accumulatedRange = 0;
            for (int i = 0; i < bestRoute.hops.size(); i++) {
                bestRoute.toTime = std::min(bestRoute.toTime,
                        bestRoute.hops.at(i)->getEnd() - accumulatedRange);

                accumulatedRange += bestRoute.hops.at(i)->getRange();
            }

            bestRoute.nextHop = bestRoute.hops[0]->getDestinationEid();
            bestRoute.fromTime = bestRoute.hops[0]->getStart();
            bestRoute.terminusNode = dstNode;

        } else {
            bestRoute.terminusNode = NO_ROUTE_FOUND;
            bestRoute.nextHop = NO_ROUTE_FOUND;
            bestRoute.arrivalTime = numeric_limits<double>::max();
        }

        if (bestRoute.nextHop != NO_ROUTE_FOUND &&
                std::find(routeTable_.at(dstNode).begin(), routeTable_.at(dstNode).end(),
                bestRoute) == routeTable_.at(dstNode).end()) {

            routeTable_.at(dstNode).push_back(bestRoute);
            routeLengthVector_.push_back(bestRoute.hops.size());
            computedRoutes_++;
        }
    }

}


void RoutingCgrCentralized::createContactsWork() {
    vector<Contact> * contacts = contactPlan_->getContacts();
    for (int i = 0; i < contacts->size(); i++) {
        contacts->at(i).work = new Work();
        ((Work*) contacts->at(i).work)->suppressed = false;
    }
}


void RoutingCgrCentralized::clearContactsWork() {
    vector<Contact> * contacts = contactPlan_->getContacts();
    for (int i = 0; i < contacts->size(); i++) {
        delete((Work*) (contacts->at(i).work));
        contacts->at(i).work = NULL;
    }
}


// stats gathering
int RoutingCgrCentralized::getComputedRoutes() {
	return computedRoutes_;
}


vector<int> RoutingCgrCentralized::getRouteLengthVector() {
	return routeLengthVector_;
}


double RoutingCgrCentralized::getTimeToComputeRoutes() {
    return timeToComputeRoutes_;
}


void RoutingCgrCentralized::clearRouteLengthVector() {
    routeLengthVector_.clear();
}
