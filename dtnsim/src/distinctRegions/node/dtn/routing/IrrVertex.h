#ifndef SRC_DISTINCTREGIONS_NODE_DTN_ROUTING_IRRVERTEX_H_
#define SRC_DISTINCTREGIONS_NODE_DTN_ROUTING_IRRVERTEX_H_

#include <src/distinctRegions/node/dtn/routing/Vent.h>
#include <utility>
#include <string>

using namespace std;

namespace dtnsimdistinct {

class IrrVertex {

public:

	IrrVertex(){}

	IrrVertex(string fromRegion, string toRegion) {
		this->vent = Vent(0, fromRegion, toRegion);
		this->distanceFromOrigin = 0;
		this->visited = false;
		this->predecessor = NULL;
	}

	IrrVertex(Vent vent) {
		this->vent = vent;
		this->distanceFromOrigin = 0;
		this->visited = false;
		this->predecessor = NULL;
	}

	bool operator==(IrrVertex & v) const {

		bool equal = true;

		if (this->vent.nodeEid != v.vent.nodeEid) {
			return false;

		} else if((this->vent.fromRegion != v.vent.fromRegion) || (this->vent.toRegion != v.vent.toRegion)) {
			return false;
		}

		return equal;
	}

	bool operator!=(IrrVertex & v) const {
		return (! (*this == v) );
	}

	Vent vent;
	double distanceFromOrigin;
	bool visited;
	IrrVertex *predecessor;
};
}

#endif /* SRC_DISTINCTREGIONS_NODE_DTN_ROUTING_IRRVERTEX_H_ */

