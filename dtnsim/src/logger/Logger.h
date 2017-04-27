#ifndef __LOGGER_H_
#define __LOGGER_H_

#include <omnetpp.h>
#include <string>
#include <iostream>
#include <ContactPlan.h>
#include <map>
#include <utility>
#include <limits>
#include <vector>

#include "../utils/Lp.h"
#include "utils/TopologyUtils.h"
#include "utils/RouterUtils.h"
#include "utils/LpUtils.h"
#include "App.h"
#include "Net.h"

using namespace omnetpp;
using namespace std;

namespace dtnsim
{

class Logger: public cSimpleModule
{
public:

	Logger();
	virtual ~Logger();
	void finish();
	virtual void initialize();

private:

	/// @brief Fill flowIds_ structure with App traffic data generators
	void computeFlowIds();

	/// @brief Compute Topology from  contactPlan_ and save it
	/// in dot and pdf files inside "results" folder
	void saveTopology();

	/// @brief Compute Flows from BundleMaps files and save them
	/// in dot and pdf files inside "results" folder
	void saveFlows();

	void saveLpFlows();

	map<int, map<int, map<int, double > > > getTraffics();

	double getState(double trafficStart);

	// Contact Plan passed to the nodes
	ContactPlan contactPlan_;

	// Nodes Number in the network
	int nodesNumber_;

	// flowIds map: (src,dst) -> flowId
	// save flow ids corresponding to traffic data
	// generated in App layer
	map<pair<int, int>, unsigned int> flowIds_;

};

}


#endif
