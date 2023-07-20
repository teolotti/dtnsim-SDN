#include <src/distinctRegions/node/dtn/contacts/Contact.h>

namespace dtnsimdistinct {

Contact::~Contact() {

}

Contact::Contact(int id, double start, double end, int sourceEid, int destinationEid, double range, double dataRate) {

	this->id_ = id;
	this->start_ = start;
	this->end_ = end;
	this->sourceEid_ = sourceEid;
	this->destinationEid_ = destinationEid;
	this->dataRate_ = dataRate;
	this->range_ = range;
	this->capacity_ = (end - start) * dataRate;
	this->remainingCapacity_ = capacity_;

	// Initialize all working areas
	vector<int> visitedNodes;
	this->routeSearchWork = {NULL, numeric_limits<double>::max(), false, visitedNodes};

	vector<int> suppressedNextContacts;
	this->routeManagementWork = {false, suppressedNextContacts};

	this->forwardingWork = {0, 0, 0, 0};
}

int Contact::getId() const {
	return id_;
}

double Contact::getStart() const {
	return start_;
}

double Contact::getEnd() const {
	return end_;
}

double Contact::getDuration() const {
	return (end_ - start_);
}

int Contact::getSourceEid() const {
	return sourceEid_;
}

int Contact::getDestinationEid() const {
	return destinationEid_;
}

double Contact::getDataRate() const {
	return dataRate_;
}

double Contact::getRange() const {
	return range_;
}

double Contact::getCapacity() const {
	return (end_ - start_) * dataRate_;
}

void Contact::clearRouteSearchWorkingArea() {
	vector<int> visitedNodes;
	this->routeSearchWork = {NULL, numeric_limits<double>::max(), false, visitedNodes};
}

void Contact::clearRouteManagementWorkingArea() {
	vector<int> suppressedNextContacts;
	this->routeManagementWork = {false, suppressedNextContacts};
}

void Contact::clearForwardingWorkingArea() {
	this->forwardingWork = {0, 0, 0, 0};
}

double Contact::getRemainingCapacity() {
	return remainingCapacity_;
}

void Contact::setRemainingCapacity(double cap) {
	remainingCapacity_ = cap;
}

}

