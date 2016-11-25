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
        virtual vector<int> getBundlesNumberVec();
        virtual vector<int> getDestinationEidVec();
        virtual vector<int> getSizeVec();
        virtual vector<double> getStartVec();

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *);
        virtual void finish();

    private:
        int eid_;

        std::vector<int> bundlesNumberVec_;
        std::vector<int> destinationEidVec_;
        std::vector<int> sizeVec_;
        std::vector<double> startVec_;

        // Stats
        cOutVector appRxBundleDelayTime;
};

#endif /* APP_H_ */
