#ifndef MAC_H_
#define MAC_H_

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <fstream>
#include <iomanip>
#include "dtnsim_m.h"

using namespace std;
using namespace omnetpp;

#define BUNDLE 10

class Mac: public cSimpleModule
{
public:
	Mac();
	virtual ~Mac();

protected:
	virtual void initialize();
	virtual void handleMessage(cMessage *);
	virtual void finish();

private:

	int eid_;

};

#endif /* MAC_H_ */
