/*
 * CgrRoute.h
 *
 *  Created on: Apr 20, 2017
 *      Author: juanfraire
 */

#ifndef SRC_NODE_NET_ROUTING_CGRROUTE_H_
#define SRC_NODE_NET_ROUTING_CGRROUTE_H_

#include "Contact.h"

using namespace std;

#define NO_ROUTE_FOUND (-1)
#define EMPTY_ROUTE (-2)

typedef struct
{
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

#endif /* SRC_NODE_NET_ROUTING_CGRROUTE_H_ */