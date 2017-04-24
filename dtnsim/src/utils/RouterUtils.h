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
	/// @brief Compute Flows of traffic from the BundleMaps files.
	/// @return map that associate one RouterGraph per state
	map<double, RouterGraph*> computeFlows(ContactPlan *contactPlan, int nodesNumber, string bundleMapsLocation);

	/// @brief Print Flows to dot and pdf files located in outFileLocation
	void printGraphs(map<double, RouterGraph*> *flows, vector<string> dotColors, map<pair<int, int>, unsigned int> flowIds, std::string outFileLocation);

	/// @brief Print a RoterGraph with the flows per State on screen
	void printGraph(RouterGraph routerGraph);

	/// @brief Gets a vector with string corresponding to colors
	/// usable to write files in dot format
	vector<string> getDotColors();

} /* namespace routerUtils */

#endif /* ROUTERUTILS_H_ */

#endif /* USE_BOOST_LIBRARIES */