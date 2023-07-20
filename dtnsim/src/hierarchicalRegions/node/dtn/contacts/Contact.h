#ifndef SRC_HIERARCHICALREGIONS_NODE_DTN_CONTACT_CONTACTS_H_
#define SRC_HIERARCHICALREGIONS_NODE_DTN_CONTACT_CONTACTS_H_

#include <iostream>
#include <string>

using namespace std;

namespace dtnsimhierarchical {

class Contact {

public:

	Contact(int id, double start, double end, int sourceEid, int destinationEid, double dataRate, string sourceRegion, string destinationRegion);
	virtual ~Contact();

	// A contact Id (unique)
	int getId() const;

	// Get basic parameters
	double getStart() const;
	double getEnd() const;
	int getSourceEid() const;
	int getDestinationEid() const;
	double getDataRate() const;
	double getVolume() const;
	double getDuration() const;
	float getConfidence() const;
	double getRange() const;
	string getSourceRegion() const;
	string getDestinationRegion() const;

	// Get and Set residual capacity (Bytes)
	double getResidualVolume() const;
	void setResidualVolume(double residualVolume);


	// A pointer to external structures
	// (used by routing algorithms)
	void* work;

private:

	int id_;
	double start_;
	double end_;
	int sourceEid_;
	int destinationEid_;
	double dataRate_; // In Bytes per seconds
	double residualVolume_; // In Bytes
	float confidence_;
	double range_;
	string sourceRegion_;
	string destinationRegion_;
};
}

#endif /* SRC_HIERARCHICALREGIONS_NODE_DTN_CONTACT_CONTACTS_H_ */
