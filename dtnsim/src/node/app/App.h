#ifndef APP_H_
#define APP_H_

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include "dtnsim_m.h"

using namespace omnetpp;
using namespace std;

#define TRAFFIC_TIMER 1
#define CONTACT_START_TIMER 2
#define CONTACT_END_TIMER 3
#define FREE_CHANNEL 4
#define BUNDLE 10

class App : public cSimpleModule
{
    public:
        App();
        virtual ~App();
        int getEid() const;

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *);
        virtual void finish();

    private:
        int eid_;
};

#endif /* APP_H_ */
