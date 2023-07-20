
#ifndef SRC_HIERARCHICALREGIONS_NODE_DTN_ROUTING_CGRROUTE_H_
#define SRC_HIERARCHICALREGIONS_NODE_DTN_ROUTING_CGRROUTE_H_

#include <src/hierarchicalRegions/node/dtn/contacts/Contact.h>
#include <vector>

using namespace std;

namespace dtnsimhierarchical {

#define NO_ROUTE_FOUND (-1)
#define EMPTY_ROUTE (-2)

typedef struct {
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
} CgrRoute;

}

#endif /* SRC_HIERARCHICALREGIONS_NODE_DTN_ROUTING_CGRROUTE_H_ */
