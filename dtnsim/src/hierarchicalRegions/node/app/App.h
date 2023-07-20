#ifndef SRC_HIERARCHICALREGIONS_NODE_APP_APP_H_
#define SRC_HIERARCHICALREGIONS_NODE_APP_APP_H_

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

#include <src/hierarchicalRegions/node/app/Flow.h>

#include <fstream>
#include <iostream>
using namespace omnetpp;
using namespace std;

namespace dtnsimhierarchical {

class App : public cSimpleModule {

    public:
        App();
        virtual ~App();

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);

    private:
        int eid_;
        string homeRegion_;
        string outerRegion_;

        void tokenizeString(const char *input, vector<string> &output);
        void tokenizeInt(const char *input, vector<int> &output);

        vector<string> destinationEidsVec_; // e.g. A1, A2, C3, ... (EIDs and regions)
        vector<string> distributionsVec_; // e.g. uniform, exponential, ...
        vector<int> numbersVec_;
        vector<int> startsVec_; //TODO double?
        vector<int> sizesVec_;
        vector<int> ttlsVec_;

        // signals
        simsignal_t appBundleSent_;
        simsignal_t appBundleReceived_;
        simsignal_t appBundleReceivedDelay_;

};
}

#endif /* SRC_HIERARCHICALREGIONS_NODE_APP_APP_H_ */
