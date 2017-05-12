#ifndef COM_H_
#define COM_H_

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <fstream>
#include <iomanip>

#include "MsgTypes.h"
#include "dtnsim_m.h"

using namespace std;
using namespace omnetpp;

class Com: public cSimpleModule
{
public:
	Com();
	virtual ~Com();

protected:
	virtual void initialize();
	virtual void handleMessage(cMessage *);
	virtual void finish();

private:

	int eid_;

};

#endif /* COM_H_ */
