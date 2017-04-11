#ifndef CONTACT_H_
#define CONTACT_H_

#include<iostream>

using namespace std;

class Contact
{
public:

	Contact(int id, double start, double end, int sourceEid, int destinationEid, double dataRate, float confidence);
	virtual ~Contact();
	double getDataRate() const;
	int getDestinationEid() const;
	int getId() const;
	double getResidualCapacity() const;
	int getSourceEid() const;
	double getStart() const;
	double getEnd() const;
	double getDuration() const;
	float getConfidence() const;

	void setResidualCapacity(double residualCapacity);

	// A pointer to any external data
	// that might be required for the
	// routing algorithm.

	void * work;

private:

	int id_;
	double start_;
	double end_;
	int sourceEid_;
	int destinationEid_;
	double dataRate_; // In Bytes per seconds
	double residualCapacity_; // In Bytes
	float confidence_;
};

#endif /* CONTACT_H_ */
