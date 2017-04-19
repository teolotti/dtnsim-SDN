#include "Config.h"

#ifdef USE_BOOST_LIBRARIES

#ifndef TOPOLOGYUTILS_H_
#define TOPOLOGYUTILS_H_

#include "ContactPlan.h"
#include <boost/graph/adjacency_list.hpp>
#include "utils/TopologyGraphInfo.h"
#include <fstream>
#include <sstream>
#include <string>
#include <map>

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, TopologyVertexInfo, TopologyEdgeInfo> TopologyGraph;

namespace topologyUtils
{

map<double, TopologyGraph*> computeTopology(ContactPlan *contactPlan, int nodesNumber);

void printGraphs(map<double, TopologyGraph*> *topology, std::string outFileLocation);

void printGraph(TopologyGraph topologyGraph);

} /* namespace topologyUtils */

#endif /* TOPOLOGYUTILS_H_ */

#endif /* USE_BOOST_LIBRARIES */
