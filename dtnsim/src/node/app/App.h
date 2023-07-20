#ifndef APP_H_
#define APP_H_

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

#include <src/node/dtn/Dtn.h>
#include "src/node/MsgTypes.h"
#include "src/dtnsim_m.h"

#include <src/node/app/Flow.h>

#include <fstream>
#include <iostream>
using namespace omnetpp;
using namespace std;

class App : public cSimpleModule {

    public:
        App();
        virtual ~App();

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);

    private:
        int eid_;

        void tokenizeInt(const char *input, vector<int> &output);
        void tokenizeIntTime(const char *input, vector<int> &output);

        vector<int> destinationEidsVec_;
        vector<int> startsVec_;
        string distribution_;
        int bundleSize_;
        int bundleTtl_;

        // Signal
        simsignal_t appBundleSent;
        simsignal_t appBundleReceived;
        simsignal_t appBundleReceivedHops;
        simsignal_t appBundleReceivedDelay;

};

#endif /* APP_H_ */
