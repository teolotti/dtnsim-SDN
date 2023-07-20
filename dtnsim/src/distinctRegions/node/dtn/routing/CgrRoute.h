#ifndef SRC_DISTINCTREGIONS_NODE_DTN_ROUTING_CGRROUTE_H_
#define SRC_DISTINCTREGIONS_NODE_DTN_ROUTING_CGRROUTE_H_

#include <src/distinctRegions/node/dtn/contacts/Contact.h>
#include <vector>

using namespace std;

namespace dtnsimdistinct {

class CgrRoute {

public:

	CgrRoute();
	virtual ~CgrRoute();

	void refreshMetrics();
	//bool comparator(const CgrRoute& r1, const CgrRoute& r2) const;


	// TODO getters and setters would be nicer
	vector<Contact *> hops;	// Sequence of contacts
	int destination;		// EID of destination node
	int nextHopNode;		// EID of next hop node
	double fromTime;		// Start time of first hop contact
	double toTime;			// Earliest end time of all contacts
	Contact* limitContact;	// Contact with earliest end time
	double arrivalTime;		// Best-case delivery time of route
	double capacity;		// Max capacity of route (bytes)
	int rootPathLength;	// Used for Yen's algorithm

	// Forwarding working area
	typedef struct {
		double eto;		// earliest transmission opportunity
		double pat;		// projected arrival time
		double evl;		// effective volume limit
	} ForwardingWork;
	ForwardingWork forwardingWork;

};

}

#endif /* SRC_DISTINCTREGIONS_NODE_DTN_ROUTING_CGRROUTE_H_ */
