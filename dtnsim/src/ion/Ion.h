
#ifndef ION_H_
#define ION_H_

#include <string>
#include <iostream>
#include <map>
#include <omnetpp.h>
#include "dtnsim_m.h"
#include <stdlib.h>
#include <fstream>
#include "App.h"

using namespace omnetpp;
using namespace std;

class Ion : public cSimpleModule
{
public:
	Ion();
	virtual ~Ion();
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();

    virtual void createIonstartFile(int nodeEid);
    virtual void createIonstopFile(int nodeEid);
    virtual void createIonrcFile(int nodeEid);
    virtual void createIonconfigFile(int nodeEid);
    virtual void createIonsecrcFile(int nodeEid);
    virtual void createLtprcFile(int nodeEid);
    virtual void createBprcFile(int nodeEid);
    virtual void createIonipnrcFile(int nodeEid);

    virtual void createMainScript();


private:

    int nodesNumber_;

    int portStart_;
    int wmKeyStart_;

    // nodeEid -> port
    map<int, int> ports_;
};

#endif /* ION_H_ */
