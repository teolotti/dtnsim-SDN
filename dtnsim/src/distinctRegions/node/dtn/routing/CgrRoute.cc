#include <src/distinctRegions/node/dtn/routing/CgrRoute.h>


namespace dtnsimdistinct {

CgrRoute::~CgrRoute() {
}

CgrRoute::CgrRoute() {

	this->destination = 0;
	this->nextHopNode = -1;
	this->fromTime = 0;
	this->toTime = numeric_limits<double>::max();
	this->limitContact = NULL;
	this->arrivalTime = 0;
	this->capacity = numeric_limits<double>::max();
	this->rootPathLength = 0;

	this->forwardingWork = {0, 0, 0};
}

void CgrRoute::refreshMetrics() {

	if (hops.empty()) {
		return;
	}

	destination = hops.back()->getDestinationEid();
	nextHopNode = hops.front()->getDestinationEid();
	fromTime = hops.front()->getStart();

	toTime = numeric_limits<double>::max();
	arrivalTime = 0;
	capacity = numeric_limits<double>::max();
	for (auto & hop : hops) {
		toTime = min(toTime, hop->getEnd());
		arrivalTime = max(arrivalTime+hop->getRange(), hop->getStart()+hop->getRange());

		if (hop->getCapacity() < capacity) {
			capacity = hop->getCapacity();
			limitContact = hop;
		}
	}

	// TODO capacity calculations as in PyCGR
}

}

