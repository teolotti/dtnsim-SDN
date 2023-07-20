#ifndef SRC_DISTINCTREGIONS_NODE_DTN_ROUTING_VENT_H_
#define SRC_DISTINCTREGIONS_NODE_DTN_ROUTING_VENT_H_

#include <list>
#include <utility>
#include <string>
#include <iostream>

using namespace std;

namespace dtnsimdistinct {

class Vent {

public:

	Vent(){}

	Vent(int nodeEid, string fromRegion, string toRegion) {
		this->nodeEid = nodeEid;
		this->fromRegion = fromRegion;
		this->toRegion = toRegion;
	}

	bool operator==(Vent & v) const {

		bool equal = true;

		if (this->nodeEid != v.nodeEid) {
			return false;

		} else if((this->fromRegion != v.fromRegion) && (this->toRegion != v.toRegion)) {
			return false;
		}

		return equal;
	}

	bool operator!=(Vent & v) const {
		return (! (*this == v) );
	}

	bool operator<(const Vent& other) const {
		return (this->fromRegion < other.fromRegion) || (this->fromRegion == other.fromRegion && this->toRegion < other.toRegion);
	}

	int nodeEid;		// Vent (i.e. passageway) EID
	string fromRegion;	// Source region (EID can at least receive in this region)
	string toRegion;	// Destination region (EID can at least send in this region)
};

}

#endif /* SRC_DISTINCTREGIONS_NODE_DTN_ROUTING_VENT_H_ */
