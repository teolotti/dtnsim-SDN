/*
 * The APP module is responsible for generating traffic
 * and accepting/rejecting bundles that end up here.
 */
#ifndef SRC_DISTINCTREGIONS_NODE_APP_APP_H_
#define SRC_DISTINCTREGIONS_NODE_APP_APP_H_

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <fstream>
#include <iostream>

#include <src/distinctRegions/node/app/Flow.h>

using namespace omnetpp;
using namespace std;

namespace dtnsimdistinct {

class App : public cSimpleModule {

    public:
        App();
        virtual ~App();

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);

    private:
        int eid_;
        string region_;

        void tokenizeString(const char *input, vector<string> &output);
        void tokenizeInt(const char *input, vector<int> &output);

        vector<string> destinationEidsVec_; // e.g. A1, A2, C3, ... (EIDs and regions)
        vector<int> startsVec_;
        string distribution_;
        int bundleSize_;
        int bundleTtl_;

        // signals
        simsignal_t appBundleSent_;
        simsignal_t appBundleReceived_;
        simsignal_t appBundleReceivedDelay_;
        simsignal_t appBundleReceivedHops_;
        simsignal_t appBundleReceivedFirstHop_;

};
}

#endif /* SRC_DISTINCTREGIONS_NODE_APP_APP_H_ */
