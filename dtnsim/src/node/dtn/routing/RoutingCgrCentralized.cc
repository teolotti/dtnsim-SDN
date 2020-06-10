#include <src/node/dtn/routing/RoutingCgrCentralized.h>

RoutingCgrCentralized::RoutingCgrCentralized(int eid, int neighborsNum, SdrModel *sdr, ContactPlan *localContactPlan,
        bool printDebug, string routingType, int maxRouteHops, int maxRoutesWithSameDst)
    : RoutingDeterministic(eid, sdr, localContactPlan)
{
    routingType_ = routingType;
    neighborsNum_ = neighborsNum;
    routeTable_.resize(neighborsNum + 1);
    maxRouteHops_ = maxRouteHops;
    maxRoutesWithSameDst_ = maxRoutesWithSameDst;
    computedRoutes_ = 0;
    printDebug_ = printDebug;

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
        // TODO: grid time call
        fillRouteTableBfs(0);
    } else if (routingType_.find("routeListType:firstEnded") != std::string::npos) {
        fillRouteTableWithContactFilter(Contact::endTimeComparison);
    } else {
        cerr << "Centralized error while initializing routing table: unknown route list type." << endl;
        exit(1);
    }

    if (!printDebug_)
        cout.clear();
}

void RoutingCgrCentralized::fillRouteTableBfs(double minEndTime) {
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

            if (neighbor->getEnd() <= minEndTime)
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
            if (std::find(routeTable_.at(i).begin(), routeTable_.at(i).end(),
                    routesToNode[i].top()) != routeTable_.at(i).end()) {

                routeTable_.at(i).push_back(routesToNode[i].top());
                routeLengthVector_.push_back(routesToNode[i].top().hops.size());
                computedRoutes_++;
            }
            routesToNode[i].pop();
        }
    }
}

void RoutingCgrCentralized::fillRouteTableWithContactFilter(bool comparisonFunc (const Contact*, const Contact*)) {
    createContactsWork();

    for (int i = 1; i <= neighborsNum_; i++) {
        if (i == eid_) continue;

        initializeContactsWork();
        CgrRoute bestRoute = findBestRoute(i);
        while (bestRoute.nextHop != NO_ROUTE_FOUND) {
            routeTable_.at(i).push_back(bestRoute);
            computedRoutes_++;
            routeLengthVector_.push_back(bestRoute.hops.size());

            vector<Contact*>::iterator leastEnd;
            leastEnd = min_element(bestRoute.hops.begin(),
                bestRoute.hops.end(), comparisonFunc);

            ((Work*) (*leastEnd)->work)->suppressed = true;
            resetContactsWork();
            bestRoute = findBestRoute(i);
        }
    }

    clearContactsWork();
}

CgrRoute RoutingCgrCentralized::findBestRoute(int terminusNode) {
    Contact selfContact = Contact(-1, 0, numeric_limits<double>::max(), eid_, eid_, 1.0, 1.0, 0);
    Work selfWork;
    selfWork.visited = false;
    selfWork.arrivalTime = simTime_;
    selfContact.work = &selfWork;

    Contact* finalContact = NULL;
    Contact* currentContact = &selfContact;
    double earliestFinalArrivalTime = numeric_limits<double>::max();

    priority_queue<Contact*, vector<Contact*>, Work> pq;
    pq.push(currentContact);

    // Begin Dijkstra
    while (!pq.empty()) {
        currentContact = pq.top();
        if (currentContact->getDestinationEid() == terminusNode) {
            break;
        }
        pq.pop();

        Work* currentContactWork = (Work*) (currentContact->work);
        if (currentContactWork->visited)
            continue;

        currentContactWork->visited = true;

        // If the arrival time is worst than the best found so far, ignore
        if (currentContactWork->arrivalTime > earliestFinalArrivalTime)
            continue;

        vector<int> currentNeighbors = contactPlan_->getContactsBySrc(currentContact->getDestinationEid());
        for (vector<int>::iterator neighborId = currentNeighbors.begin(); neighborId != currentNeighbors.end(); ++neighborId) {
            Contact* neighbor = contactPlan_->getContactById(*neighborId);
            Work* neighborWork = (Work*) (neighbor->work);

            if (neighborWork->suppressed || neighborWork->visited)
                continue;

            // If this contact is finished, ignore it.
            if (neighbor->getEnd() <= currentContactWork->arrivalTime)
                continue;

            if (neighbor->getResidualVolume() == 0)
                continue;

            // Get owlt (one way light time)
            double owlt = neighbor->getRange();
            if (owlt == -1) {
                cout << "warning, range not available for nodes " << neighbor->getSourceEid() << "-" << neighbor->getDestinationEid() << ", assuming range=0" << endl;
                owlt = 0;
            }

            // Calculate the cost for this contact (Arrival Time)
            double arrivalTime = std::max(
                    neighbor->getStart(),
                    currentContactWork->arrivalTime
                );
            arrivalTime += owlt;

            // Update the cost if better or equal
            if (arrivalTime < neighborWork->arrivalTime) {
                neighborWork->arrivalTime = arrivalTime;
                neighborWork->predecessor = currentContact;

                // Mark if destination reached
                if (neighbor->getDestinationEid() == terminusNode) {
                    if (neighborWork->arrivalTime < earliestFinalArrivalTime) {
                        earliestFinalArrivalTime = neighborWork->arrivalTime;
                        finalContact = contactPlan_->getContactById(neighbor->getId());
                    }
                }

                pq.push(neighbor);
            }
        }
    } // End Dijkstra

    // Build route
    CgrRoute bestRoute;
    if (finalContact != NULL) {
        bestRoute.arrivalTime = earliestFinalArrivalTime;
        bestRoute.confidence = 1.0;
        bestRoute.toTime = numeric_limits<double>::max();
        bestRoute.maxVolume = numeric_limits<double>::max();
        bestRoute.residualVolume = numeric_limits<double>::max();

        for (Contact* contact = finalContact; contact != &selfContact;
                contact = ((Work*) contact->work)->predecessor) {

            bestRoute.maxVolume = std::min(bestRoute.maxVolume, contact->getVolume());
            bestRoute.residualVolume = std::min(bestRoute.residualVolume,
                contact->getResidualVolume());
            bestRoute.confidence *= contact->getConfidence();
            bestRoute.hops.insert(bestRoute.hops.begin(), contact);
        }

        double accumulatedRange = 0;
        for (int i = 0; i < bestRoute.hops.size(); i++) {
            bestRoute.toTime = std::min(bestRoute.toTime,
                    bestRoute.hops.at(i)->getEnd() - accumulatedRange);

            accumulatedRange += std::max(bestRoute.hops.at(i)->getRange(), 0.0);
        }

        bestRoute.nextHop = bestRoute.hops[0]->getDestinationEid();
        bestRoute.fromTime = bestRoute.hops[0]->getStart();
        bestRoute.terminusNode = terminusNode;

    } else {
        bestRoute.terminusNode = NO_ROUTE_FOUND;
        bestRoute.nextHop = NO_ROUTE_FOUND;
        bestRoute.arrivalTime = numeric_limits<double>::max();
    }

    return bestRoute;
}

void RoutingCgrCentralized::createContactsWork() {
    vector<Contact> * contacts = contactPlan_->getContacts();
    for (int i = 0; i < contacts->size(); i++) {
        contacts->at(i).work = new Work();
        ((Work*) contacts->at(i).work)->suppressed = false;
    }
}

void RoutingCgrCentralized::initializeContactsWork() {
    vector<Contact> * contacts = contactPlan_->getContacts();
    for (int i = 0; i < contacts->size(); i++) {
        ((Work*) contacts->at(i).work)->suppressed = false;
    }

    resetContactsWork();
}

void RoutingCgrCentralized::resetContactsWork() {
    vector<Contact> * contacts = contactPlan_->getContacts();
    for (int i = 0; i < contacts->size(); i++) {
        Work* contactWork = (Work*) (contacts->at(i).work);
        contactWork->visited = false;
        contactWork->arrivalTime = numeric_limits<double>::max();
        contactWork->predecessor = NULL;
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
