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

#define TRAFFIC_TIMER 1
#define CONTACT_START_TIMER 2
#define CONTACT_END_TIMER 3
#define FREE_CHANNEL 4
#define BUNDLE 10

class Mac : public cSimpleModule
{
    public:
        Mac();
        virtual ~Mac();

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *);
        virtual void finish();

    private:

};

#endif /* MAC_H_ */
