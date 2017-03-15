#ifndef ION_H_
#define ION_H_

#include <string>
#include <iostream>
#include <map>
#include <omnetpp.h>

using namespace omnetpp;
using namespace std;


class Ion: public cSimpleModule
{
public:
	Ion();
	virtual ~Ion();
	virtual void initialize();
	virtual void handleMessage(cMessage *msg);
	virtual void finish();


private:

	// specify if there are ion nodes in the simulation
	bool ionNodes_;

};

#endif /* ION_H_ */
