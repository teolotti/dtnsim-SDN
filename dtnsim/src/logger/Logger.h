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
#include "utils/TopologyUtils.h"
#include "utils/RouterUtils.h"
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

	// fill flowIds_ structure with App data generators
	void computeFlowIds();
	void saveTopology();
	void saveFlows();
	vector<string> getDotColors();

	ContactPlan contactPlan_;

	int nodesNumber_;

	// (src,dst) -> flowId
	map<pair<int, int>, unsigned int> flowIds_;

};

}


#endif
