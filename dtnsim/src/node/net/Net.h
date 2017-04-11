#ifndef _NET_H_
#define _NET_H_

#include <cstdio>
#include <string>
#include <omnetpp.h>
#include <fstream>
#include <sstream>
#include <map>
#include <queue>

#include "MsgTypes.h"
#include "dtnsim_m.h"

#include "ContactPlan.h"
#include "Graphics.h"
#include "SdrModel.h"

#include "Routing.h"
#include "RoutingDirect.h"
#include "RoutingCgrIon350.h"
#include "RoutingCgrModelYen.h"
#include "Ion.h"
#include "RoutingCgrModel350.h"

using namespace omnetpp;
using namespace std;



class Net: public cSimpleModule
{
public:
	Net();
	virtual ~Net();

	virtual void setOnFault(bool onFault);
	virtual void refreshForwarding();

protected:
	virtual void initialize(int stage);
	virtual int numInitStages() const;
	virtual void handleMessage(cMessage *msg);
	virtual void finish();

	virtual void dispatchBundle(BundlePkt *bundle);

private:

	int eid_;
	bool onFault = false;

	// Pointer to grahics module
	Graphics *graphicsModule;

	// Forwarding threads
	map<int, ForwardingMsg *> forwardingMsgs_;

	// Routing and storage
	Routing * routing;
	ContactPlan contactPlan_;
	SdrModel sdr_;

	// Stats
	cOutVector netTxBundles;
	cOutVector netRxBundles;
	cOutVector netRxHopCount;
	cOutVector netReRoutedBundles;
	unsigned int reRoutedBundles;
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

