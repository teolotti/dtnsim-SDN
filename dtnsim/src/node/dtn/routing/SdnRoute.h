
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
	int nextHop;				//Eid of the next hop
	double fromTime; 			// Init time
	double toTime;	 			// Due time (earliest contact end time among all)// Contact id of next hop
	bool active = false;
	float confidence;
	double arrivalTime;
	double maxVolume; 			// In Bytes
	double residualVolume;		// In Bytes
	bool filtered;
	int totOccupation = 0;
	vector<Contact*> hops;		// contact list
} SdnRoute;

#endif /* SRC_NODE_DTN_ROUTING_SDNROUTE_H_ */
