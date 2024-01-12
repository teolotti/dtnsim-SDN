#ifndef APP_H_
#define APP_H_

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

#include "src/node/MsgTypes.h"
#include "src/dtnsim_m.h"

#include <fstream>
#include <iostream>
using namespace omnetpp;
using namespace std;

class App : public cSimpleModule
{
    public:
        App();
        virtual ~App();
        int getEid() const;
        virtual vector<int> getBundlesNumberVec();
        virtual vector<int> getDestinationEidVec();
        virtual vector<int> getTargetEidVec();
        virtual vector<int> getSizeVec();
        virtual vector<double> getStartVec();
        virtual vector<int> getBundlesNumberVecControl();
        virtual vector<int> getSizeVecControl();


    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *);
        virtual void finish();

    private:
        int eid_;

        std::vector<int> bundlesNumberVec_;
        std::vector<int> destinationEidVec_;
        std::vector<int> targetEidVec_;
        std::vector<int> sizeVec_;
        std::vector<double> startVec_;
        std::vector<int> bundlesNumberVec_control;
        std::vector<int> sizeVec_control;
        std::vector<double> startVec_control;

        // Signal
        simsignal_t appBundleSent;
        simsignal_t appBundleReceived;
        simsignal_t appBundleReceivedHops;
        simsignal_t appBundleReceivedDelay;

        //simsignal_t appBundleArrivalTime;

};

#endif /* APP_H_ */
