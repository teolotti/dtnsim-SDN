#include <src/node/dtn/routing/RoutingCgrCentralized.h>

RoutingCgrCentralized::RoutingCgrCentralized(int eid, int neighborsNum, SdrModel *sdr, ContactPlan *localContactPlan,
        string routingType)
    : RoutingDeterministic(eid, sdr, localContactPlan)
{
    routingType_ = routingType;
    neighborsNum_ = neighborsNum;
    routeTable_.resize(neighborsNum);

    this->initializeRouteTable();
}

RoutingCgrCentralized::~RoutingCgrCentralized()
{
}

void RoutingCgrCentralized::routeAndQueueBundle(BundlePkt *bundle, double simTime) {

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
        bestRoute = min_element(routeTable_.at(terminusNode).begin(), routeTable_.at(terminusNode).end(),
                this->compareRoutes);

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

// This is the procedure that should be done on earth to initialize the route table of each node.
void RoutingCgrCentralized::initializeRouteTable() {

    // Find all routes to the i-th node
    for (int terminusNode = 1; terminusNode <= routeTable_.size(); terminusNode++) {
        if (terminusNode == eid_) {
            continue;
        }

        // Temporarily store the residual capacity of each
        // contact so we can use in the route calculation
        // stage to determine how this capacity would get
        // occupied as traffic is sent to this target node.
        vector<double> contactVolume(contactPlan_->getContacts()->size());
        vector<double>::iterator it1 = contactVolume.begin();
        vector<Contact>::iterator it2 = contactPlan_->getContacts()->begin();
        for (; it1 != contactVolume.end(); ++it1, ++it2)
            (*it1) = (*it2).getResidualVolume();

        vector<int> suppressedContactIds;

        while (1) {
            CgrRoute route;
            this->findNextBestRoute(suppressedContactIds, terminusNode, &route);

            // If no more routes were found, stop search loop
            if (route.nextHop == NO_ROUTE_FOUND)
                break;

            // Add new valid route to route table
            routeTable_.at(terminusNode).push_back(route);

            // Consume route residual volume and suppress the contact
            // with least residual volume on the route
            vector<Contact *>::iterator hop;
            for (hop = route.hops.begin(); hop != route.hops.end(); ++hop) {

                // Consume route residual volume
                (*hop)->setResidualVolume((*hop)->getResidualVolume() - route.residualVolume);

                // If this is the limiting contact, suppress
                // it from further route searches (other contacts
                // in the path will remain with a reduced residual
                // volume.
                if ((*hop)->getResidualVolume() == 0) {
                    suppressedContactIds.push_back((*hop)->getId());
                }
            }
            // tableEntriesCreated++;
        }

        // Restore original residual capacities in the contact plan
        it1 = contactVolume.begin();
        it2 = contactPlan_->getContacts()->begin();
        for (; it1 != contactVolume.end(); ++it1, ++it2)
            (*it2).setResidualVolume(*it1);
    }
}

// This function is the Dijkstra search over the contact-graph.
// It is based on current implementation in ION but adds a few corrections
// such as visited nodes list to avoid topological loops. From the implementation
// perspective it needs severe improvements as it currently overutilizes pointer
// operations which render the code very difficult to read and to debug.
// In general, each contact has a work pointer where temporal information only
// valid and related to the current Dijkstra search is stored.
void RoutingCgrCentralized::findNextBestRoute(vector<int> suppressedContactIds, int terminusNode, CgrRoute * route) {
    // increment metrics counter
    // dijkstraCalls++;

    // Create rootContact and its corresponding rootWork
    // id=0, start=0, end=inf, src=me, dst=me, rate=0, conf=1
    Contact * rootContact = new Contact(0, 0, numeric_limits<double>::max(), eid_, eid_, 0, 1.0, 0);
    Work rootWork;
    rootWork.arrivalTime = simTime_;
    rootContact->work = &rootWork;

    // Create and initialize working area in each contact.
    for (vector<Contact>::iterator it = contactPlan_->getContacts()->begin(); it != contactPlan_->getContacts()->end();
            ++it) {
        Work *contactWork = new Work;
        (*it).work = contactWork;
        contactWork->arrivalTime = numeric_limits<double>::max();
        contactWork->predecessor = 0;
        contactWork->visited = false;

        // Suppress contacts as indicated in the suppressed list
        if (find(suppressedContactIds.begin(), suppressedContactIds.end(), (*it).getId()) != suppressedContactIds.end())
            contactWork->suppressed = true;
        else
            contactWork->suppressed = false;
    }

    // Start Dijkstra
    Contact * currentContact = rootContact;
    Contact * finalContact = NULL;
    double earliestFinalArrivalTime = numeric_limits<double>::max();

    while (currentContact != NULL) {
        Work * currentContactWork = (Work *) (currentContact->work);

        // increment counter
        // dijkstraLoops++;

        //cout << currentContact->getDestinationEid() << ",";

        // Get local neighbor set and evaluate them
        vector<Contact> currentNeighbors = contactPlan_->getContactsBySrc(currentContact->getDestinationEid());
        for (vector<Contact>::iterator neighbor = currentNeighbors.begin(); neighbor != currentNeighbors.end(); ++neighbor) {
            Work *neighborWork = (Work *) (*neighbor).work;

            // If this contact is suppressed/visited, ignore it.
            if (neighborWork->suppressed || neighborWork->visited)
                continue;

            // If this contact is finished, ignore it.
            if ((*neighbor).getEnd() <= currentContactWork->arrivalTime)
                continue;

            // If the residual volume is 0, ignore it.
            if ((*neighbor).getResidualVolume() == 0)
                continue;

            // Get owlt (one way light time). If none found, ignore contact
            double owlt = contactPlan_->getRangeBySrcDst((*neighbor).getSourceEid(), (*neighbor).getDestinationEid());
            if (owlt == -1)
            {
                cout << "warning, range not available for nodes " << (*neighbor).getSourceEid() << "-" << (*neighbor).getDestinationEid() << ", assuming range=0" << endl;
                owlt = 0;
            }
            //double owltMargin = ((MAX_SPEED_MPH / 3600) * owlt) / 186282;
            //owlt += owltMargin;

            // Calculate the cost for this contact (Arrival Time)
            double arrivalTime = std::max(
                    (*neighbor).getStart(),
                    currentContactWork->arrivalTime
                );
            arrivalTime += owlt;

            // Update the cost if better or equal
            if (arrivalTime < neighborWork->arrivalTime) {
                neighborWork->arrivalTime = arrivalTime;
                neighborWork->predecessor = currentContact;

                // Mark if destination reached
                if ((*neighbor).getDestinationEid() == terminusNode)
                    if (neighborWork->arrivalTime < earliestFinalArrivalTime) {
                        earliestFinalArrivalTime = neighborWork->arrivalTime;
                        finalContact = contactPlan_->getContactById((*neighbor).getId());
                    }
            }
        }

        // End exploring next hop contact, mark current as visited
        currentContactWork->visited = true;

        // Select next (best) contact to move to in next iteration
        Contact * nextContact = NULL;
        double earliestArrivalTime = numeric_limits<double>::max();
        for (vector<Contact>::iterator it = contactPlan_->getContacts()->begin();
                it != contactPlan_->getContacts()->end(); ++it) {
            // Do not evaluate suppressed or visited contacts
            if (((Work *) (*it).work)->suppressed || ((Work *) (*it).work)->visited)
                continue;

            // If the arrival time is worst than the best found so far, ignore
            if (((Work *) (*it).work)->arrivalTime > earliestFinalArrivalTime)
                continue;

            // Then this might be the best candidate contact
            if (((Work *) (*it).work)->arrivalTime < earliestArrivalTime) {
                nextContact = &(*it);
                earliestArrivalTime = ((Work *) (*it).work)->arrivalTime;
            }
        }

        currentContact = nextContact;
    }

    // End contact graph exploration
    //cout << endl;

    // If we got a final contact to destination
    // then it is the best route and we need to
    // recover the data from the work area
    if (finalContact != NULL) {
        route->arrivalTime = earliestFinalArrivalTime;
        route->confidence = 1.0;

        double earliestEndTime = numeric_limits<double>::max();
        double maxVolume = numeric_limits<double>::max();
        double maxResidualVolume = numeric_limits<double>::max();

        // Go through all contacts in the path
        for (Contact * contact = finalContact; contact != rootContact; contact =
                ((Work *) (*contact).work)->predecessor) {
            // Get earliest end time
            if (contact->getEnd() < earliestEndTime)
                earliestEndTime = contact->getEnd();

            // Get the minimal capacity
            // (TODO: this calculation assumes non-overlapped contacts
            // can be made more accurate. Indeed it is always assumed
            // that contacts are booked from the beginning (time=0), but
            // in fact long contacts might be booked from intermediate
            // points which is not currently reflected in this calculation)
            if (contact->getVolume() < maxVolume)
                maxVolume = contact->getVolume();
            if (contact->getResidualVolume() < maxResidualVolume)
                maxResidualVolume = contact->getResidualVolume();

            // Update confidence
            route->confidence *= contact->getConfidence();

            // Store hop
            route->hops.insert(route->hops.begin(), contact);
        }

        route->terminusNode = terminusNode;
        route->nextHop = route->hops[0]->getDestinationEid();
        route->fromTime = route->hops[0]->getStart();
        route->toTime = earliestEndTime;
        route->maxVolume = maxVolume;
        route->residualVolume = maxResidualVolume;
    } else {
        // No route found
        route->terminusNode = NO_ROUTE_FOUND;
        route->nextHop = NO_ROUTE_FOUND;
        route->arrivalTime = numeric_limits<double>::max();         // never chosen as best route
    }

    // Delete working area in each contact.
    for (vector<Contact>::iterator it = contactPlan_->getContacts()->begin(); it != contactPlan_->getContacts()->end();
            ++it) {
        delete ((Work *) (*it).work);
    }
}
