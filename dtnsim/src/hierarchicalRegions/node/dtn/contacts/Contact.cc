#include <src/hierarchicalRegions/node/dtn/contacts/Contact.h>

namespace dtnsimhierarchical {

Contact::~Contact() {

}

Contact::Contact(int id, double start, double end, int sourceEid, int destinationEid, double dataRate, string sourceRegion, string destinationRegion) {

	this->id_ = id;
	this->start_ = start;
	this->end_ = end;
	this->sourceEid_ = sourceEid;
	this->destinationEid_ = destinationEid;
	this->dataRate_ = dataRate;
	this->confidence_ = (float) 1.0; //TODO
	this->residualVolume_ = (end - start) * dataRate;
	this->range_ = 0; //TODO
	this->sourceRegion_ = sourceRegion;
	this->destinationRegion_ = destinationRegion;
}


double Contact::getDataRate() const {
	return dataRate_;
}

int Contact::getDestinationEid() const {
	return destinationEid_;
}

int Contact::getId() const {
	return id_;
}

double Contact::getResidualVolume() const {
	return residualVolume_;
}

void Contact::setResidualVolume(double residualVolume) {
	this->residualVolume_ = residualVolume;
}

int Contact::getSourceEid() const {
	return sourceEid_;
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

double Contact::getVolume() const {
	return (end_ - start_) * dataRate_;
}

float Contact::getConfidence() const {
	return confidence_;
}

double Contact::getRange() const {
	return range_;
}

string Contact::getSourceRegion() const {
	return sourceRegion_;
}

string Contact::getDestinationRegion() const {
	return destinationRegion_;
}
}

