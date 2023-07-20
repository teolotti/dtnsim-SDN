#ifndef SRC_DISTINCTREGIONS_NODE_DTN_CONTACT_CONTACTS_H_
#define SRC_DISTINCTREGIONS_NODE_DTN_CONTACT_CONTACTS_H_

#include <iostream>
#include <string>
#include <limits>
#include <vector>

using namespace std;
namespace dtnsimdistinct {

class Contact {

public:

	Contact(int id, double start, double end, int sourceEid, int destinationEid, double range, double dataRate);
	virtual ~Contact();

	// A contact Id (unique)
	int getId() const;

	// Get basic parameters
	double getStart() const;
	double getEnd() const;
	double getDuration() const;
	int getSourceEid() const;
	int getDestinationEid() const;
	double getDataRate() const;
	double getRange() const;		// Average distance (in seconds) between source and destination
	double getCapacity() const;		// Max. capacity of contact (also called volume) [Bytes]

	double getRemainingCapacity();
	void setRemainingCapacity(double cap);

	// Working areas of contact

	// Route search working area
	typedef struct {
		Contact* predecessor;		// Predecessor contact in route
		double arrivalTime;			// Best case delivery time to contact's receiving node
		bool visited;				// Contact has already been considered in previous loop
		vector<int> visitedNodes;	// List of EIDs of previously visited nodes
	} RouteSearchWork;
	RouteSearchWork routeSearchWork;

	// Route management working area
	typedef struct {
		bool suppressed;					// Contact has been suppressed
		vector<int> suppressedNextContacts;	// List of IDs of suppressed next hop contacts
	} RouteManagementWork;
	RouteManagementWork routeManagementWork;

	// Forwarding working area
	typedef struct {
		double fbtx;	// first byte transmission time
		double lbtx;	// last byte transmission time
		double lbrx;	// last byte arrival time
		double evl;		// effective volume limit
	} ForwardingWork;
	ForwardingWork forwardingWork;

	void clearRouteSearchWorkingArea();
	void clearRouteManagementWorkingArea();
	void clearForwardingWorkingArea();

private:

	int id_;
	double start_;
	double end_;
	int sourceEid_;
	int destinationEid_;
	double dataRate_; 			// [Bytes/second]
	double range_;				// [seconds]
	double capacity_;			// [Bytes/second]
	double remainingCapacity_; 	// [Bytes]
};
}

#endif /* SRC_DISTINCTREGIONS_NODE_DTN_CONTACT_CONTACTS_H_ */
