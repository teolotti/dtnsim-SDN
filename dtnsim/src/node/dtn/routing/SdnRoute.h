
#ifndef SRC_NODE_DTN_ROUTING_SDNROUTE_H_
#define SRC_NODE_DTN_ROUTING_SDNROUTE_H_

#include <src/node/dtn/Contact.h>
#include <vector>

using namespace std;


#define NO_ROUTE_FOUND (-1)
#define EMPTY_ROUTE (-2)

typedef struct {
	int bundleId; 				//Control bundle that generates the route
	int terminusNode;			// Destination node
	int nextHop; 				// Entry node
	vector<Contact *> hops;		// contact list
} SdnRoute;

#endif /* SRC_NODE_DTN_ROUTING_SDNROUTE_H_ */
