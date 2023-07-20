#ifndef SRC_DISTINCTREGIONS_NODE_DTN_ROUTING_IRRROUTE_H_
#define SRC_DISTINCTREGIONS_NODE_DTN_ROUTING_IRRROUTE_H_

#include <src/distinctRegions/node/dtn/routing/Vent.h>
#include <list>
#include <utility>
#include <string>
#include <iostream>

using namespace std;

namespace dtnsimdistinct {

typedef struct {
	list<Vent> vents; // list of NodeID(fromRegion, toRegion)
} IrrRoute;

}

#endif /* SRC_DISTINCTREGIONS_NODE_DTN_ROUTING_IRRROUTE_H_ */
