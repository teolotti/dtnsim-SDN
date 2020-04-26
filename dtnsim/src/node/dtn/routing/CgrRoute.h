
#ifndef SRC_NODE_DTN_ROUTING_CGRROUTE_H_
#define SRC_NODE_DTN_ROUTING_CGRROUTE_H_

#include <assert.h>
#include <src/node/dtn/Contact.h>
#include <vector>

using namespace std;

#define NO_ROUTE_FOUND (-1)
#define EMPTY_ROUTE (-2)

typedef struct CgrRoute
{
	bool filtered;
	int terminusNode;			// Destination node
	int nextHop; 				// Entry node
	double fromTime; 			// Init time
	double toTime;	 			// Due time (earliest contact end time among all)
	float confidence;
	double arrivalTime;
	double maxVolume; 			// In Bytes
	double residualVolume;		// In Bytes
	vector<Contact *> hops;	 	// Contact list

	static CgrRoute RouteFromContact(Contact* contact) {
	    CgrRoute route;
	    route.filtered = false;
	    route.terminusNode = contact->getDestinationEid();
	    route.nextHop = contact->getDestinationEid();
	    route.fromTime = contact->getStart();
	    route.toTime = contact->getEnd();
	    route.confidence = contact->getConfidence();
	    route.arrivalTime = contact->getStart() + contact->getRange();
	    route.maxVolume = contact->getVolume();
	    route.residualVolume = contact->getResidualVolume();
	    route.hops = vector<Contact *> { contact };

	    return route;
	}

	CgrRoute extendWithContact(Contact* contact) {
	    assert(this->arrivalTime < contact->getEnd());

	    CgrRoute newRoute = *this;
	    newRoute.terminusNode = contact->getDestinationEid();
	    // No need to set newRoute.nextHop
	    newRoute.toTime = std::min(newRoute.toTime, contact->getEnd());
	    newRoute.confidence *= contact->getConfidence();
	    newRoute.arrivalTime = std::max(newRoute.arrivalTime, contact->getStart()) + contact->getRange();
	    newRoute.maxVolume = std::min(newRoute.maxVolume, contact->getVolume());
	    newRoute.residualVolume = std::min(newRoute.residualVolume, contact->getResidualVolume());
	    newRoute.hops.push_back(contact);
	    return newRoute;
	}

	bool nodeIsNotVisited(int nodeId) {
	    if (hops.size() == 0) {
	        return true;
	    }
	    bool result = nodeId != hops[0]->getSourceEid();
	    for (int i = 0; i < hops.size() && result; i++) {
	        result = result && nodeId != hops[i]->getDestinationEid();
	    }

	    return result;
	}

    bool operator<(const CgrRoute& other) const {
        if (!filtered && other.filtered)
            return true;
        if (filtered && !other.filtered)
            return false;

        // If both are not filtered, then compare criteria,
        // If both are filtered, return any of them.

        // criteria 1) lowest arrival time
        if (arrivalTime < other.arrivalTime)
            return true;
        else if (arrivalTime == other.arrivalTime) {
            // if equal, criteria 2) lowest hop count
            if (hops.size() < other.hops.size())
                return true;
            else if (hops.size() == other.hops.size()) {
                // if equal, criteria 3) larger residual volume
                if (residualVolume > other.residualVolume)
                    return true;;
            }
        }
        return false;
    }

} CgrRoute;

#endif /* SRC_NODE_DTN_ROUTING_CGRROUTE_H_ */
