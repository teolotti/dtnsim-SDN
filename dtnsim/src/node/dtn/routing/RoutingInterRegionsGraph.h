
#ifndef ROUTINGINTERREGIONSGRAPH_H_
#define ROUTINGINTERREGIONSGRAPH_H_

#include <utility>
#include <string>

using namespace std;

class InterRegionsVertex {

public:

	InterRegionsVertex() {
		passagewayNodeEid = 0;
		distanceFromOrigin = 0;
		visited = false;
		predecessorVertex = NULL;
	}

	bool operator==(InterRegionsVertex & v) const {

		bool equal = true;

		if (this->passagewayNodeEid != v.passagewayNodeEid) {
			return false;
		} else if(this->vent != v.vent) {
			return false;
		}

		return equal;
	}

	bool operator!=(InterRegionsVertex & v) const {
		return (! (*this == v) );
	}

	pair<string, string> vent;
	int passagewayNodeEid;

	double distanceFromOrigin;
	bool visited;
	InterRegionsVertex *predecessorVertex;
};

#endif /* ROUTINGINTERREGIONSGRAPH__H_ */

