#include "Config.h"

#ifdef USE_BOOST_LIBRARIES

#ifndef ROUTERUTILS_H_
#define ROUTERUTILS_H_

#include "TopologyUtils.h"
#include "ContactPlan.h"
#include <boost/graph/adjacency_list.hpp>
#include "RouterGraphInfo.h"
#include <string>
#include <fstream>

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, RouterVertexInfo, RouterEdgeInfo> RouterGraph;

namespace routerUtils
{
	map<double, RouterGraph*> computeFlows(ContactPlan *contactPlan, int nodesNumber, string bundleMapsLocation);

	void printGraphs(map<double, RouterGraph*> *flows, vector<string> dotColors, map<pair<int, int>, unsigned int> flowIds, std::string outFileLocation);

	void printGraph(RouterGraph routerGraph);

} /* namespace routerUtils */

#endif /* ROUTERUTILS_H_ */

#endif /* USE_BOOST_LIBRARIES */
