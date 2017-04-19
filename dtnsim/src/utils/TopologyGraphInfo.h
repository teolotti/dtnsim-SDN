#ifdef USE_BOOST_LIBRARIES

#ifndef TOPOLOGYGRAPHINFO_H_
#define TOPOLOGYGRAPHINFO_H_

struct TopologyVertexInfo
{
    // vertex id
	int eid;
};


struct TopologyEdgeInfo
{
    // edge id
	int id;

	// state capacity in bytes
	double stateCapacity;
};

#endif /* TOPOLOGYGRAPHINFO_H_ */

#endif /* USE_BOOST_LIBRARIES */
