#include "LpUtils.h"

#ifdef USE_CPLEX_LIBRARY
#ifdef USE_BOOST_LIBRARIES

using namespace std;

namespace lpUtils
{

map<double, RouterGraph> computeFlows(ContactPlan *contactPlan, int nodesNumber, Lp *lp)
{
	map<double, RouterGraph> flows;
	vector<RouterGraph> lpFlows = lp->getSolvedStates();

	vector<pair<int, int> > intervals = lp->getIntervals();

	assert(lpFlows.size() == intervals.size());

	int k = 0;
	vector<pair<int, int> >::iterator it1 = intervals.begin();
	vector<pair<int, int> >::iterator it2 = intervals.end();
	for(; it1 != it2; ++it1)
	{
		double stateStart = it1->first;
		flows[stateStart] = lpFlows.at(k);
		k++;
	}

	return flows;
}

}

#endif /* USE_BOOST_LIBRARIES */
#endif /* USE_CPLEX_LIBRARY */
