#ifndef _NET_H_
#define _NET_H_

#include <cstdio>
#include <string>
#include <omnetpp.h>
#include <fstream>
#include <sstream>
#include <map>
#include <queue>

#include "dtnsim_m.h"

#include "ContactPlan.h"
#include "Graphics.h"
#include "SdrModel.h"
#include "Routing.h"
#include "RoutingDirect.h"
#include "RoutingCgrIon350.h"
#include "RoutingCgrModelYen.h"
#include "Ion.h"
#include "routing/RoutingCgrModel350.h"

using namespace omnetpp;
using namespace std;

#define TRAFFIC_TIMER 1
#define CONTACT_START_TIMER 2
#define CONTACT_END_TIMER 3
#define FREE_CHANNEL 4
#define BUNDLE 10
#define FAULT_START_TIMER 20
#define FAULT_END_TIMER 21

class Net: public cSimpleModule
{
public:
	Net();
	virtual ~Net();

protected:
	virtual void initialize(int stage);
	virtual int numInitStages() const;
	virtual void handleMessage(cMessage *msg);

	virtual void dispatchBundle(BundlePkt *bundle);
	virtual double transmitBundle(int neighborEid, int contactId);

	virtual void finish();

private:

	int eid_;

	Routing * routing;

	ContactPlan contactPlan_;
	SdrModel sdr_;

	// A data structure to track the forwarding process
	map<int, FreeChannelMsg *> freeChannelMsgs_;

	// Pointer to grahics module
	Graphics *graphicsModule;

	// Fault parameters
	bool onFault;
	double meanTTF, meanTTR;

	// Stats
	cOutVector netTxBundles;
	cOutVector netRxBundles;
	cOutVector netRxHopCount;
	cOutVector netReRoutedBundles;
	unsigned int reRoutedBundles;
	cOutVector netEffectiveFailureTime;
	double effectiveFailureTime;
	cOutVector sdrBundlesInSdr;
	cOutVector sdrBundleInLimbo;

    // BundlesMap
    bool saveBundleMap_;
    ofstream bundleMap_;

    // OutputGraph
    bool generateOutputGraph_;
    ofstream outputGraph_;
};

#endif /* NET_H_ */

