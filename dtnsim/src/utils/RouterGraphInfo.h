#ifdef USE_BOOST_LIBRARIES

#ifndef ROUTERGRAPHINFO_H_
#define ROUTERGRAPHINFO_H_

#include <boost/multi_array.hpp>

using namespace boost;

struct RouterVertexInfo
{
    // vertex id
	int eid;
};


struct RouterEdgeInfo
{
    // contact id
	int id;

	// state capacity in bytes
	double stateCapacity;

	// flows
	multi_array<double, 2> flows;

	RouterEdgeInfo()
	{
	    stateCapacity = 0.0;
		flows.resize(extents[20][20]);
	}
};

struct RouterGraphInfo
{
	// state start
	double stateStart;

	// state end
	double stateEnd;
};

#endif /* ROUTERGRAPHINFO_H_ */

#endif /* USE_BOOST_LIBRARIES */
