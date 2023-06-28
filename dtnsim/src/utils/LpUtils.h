#include "src/Config.h"

#ifdef USE_CPLEX_LIBRARY
#ifdef USE_BOOST_LIBRARIES

#ifndef LPUTILS_H_
#define LPUTILS_H_

#include "src/node/dtn/ContactPlan.h"
#include <boost/graph/adjacency_list.hpp>
#include "src/utils/TopologyGraphInfo.h"
#include "src/utils/RouterGraphInfo.h"
#include "src/utils/Lp.h"
#include <fstream>
#include <sstream>
#include <string>
#include <map>

namespace lpUtils
{

/// @brief Compute Flows of traffic from the Lp model.
/// @return map that associate one RouterGraph per state
map<double, RouterGraph> computeFlows(ContactPlan *contactPlan, int nodesNumber, Lp *lp);

/// @brief Gets delivery time of the last delivered traffic
double getMaxDeliveryTime(Lp *lp, int solutionNumber);

/// @brief Gets total transmissions in bytes
double getTotalTxBytes(Lp *lp, int solutionNumber);

} /* namespace lpUtils */

#endif /* LPUTILS_H_ */
#endif /* USE_BOOST_LIBRARIES */
#endif /* USE_CPLEX_LIBRARY */
