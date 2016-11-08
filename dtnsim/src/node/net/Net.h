#ifndef _NET_H_
#define _NET_H_

#include <cstdio>
#include <string>
#include <omnetpp.h>
#include <fstream>
#include <map>
#include <queue>
#include "ContactPlan.h"
#include "dtnsim_m.h"
//#include "ion_interface/ION_interface.h"

using namespace omnetpp;
using namespace std;

#define TRAFFIC_TIMER 1
#define CONTACT_START_TIMER 2
#define CONTACT_END_TIMER 3
#define FREE_CHANNEL 4
#define BUNDLE 10

class Net: public cSimpleModule {
public:
    Net();
    virtual ~Net();

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void dispatchBundle(Bundle *bundle);
    virtual void routeBundle(Bundle *bundle);
    virtual double transmitBundle(int neighborEid, int contactId);
    virtual void parseContacts(string fileName);
    virtual void finish();

private:

    int eid_;

    ContactPlan contactPlan_;

    // (neighbor eid --> bundles)
    map<int, queue<Bundle *> > bundlesQueue_;

    // (contact id --> freeChannelMsg)
    map<int, FreeChannelMsg *> freeChannelMsgs_;
};

#endif /* NET_H_ */

