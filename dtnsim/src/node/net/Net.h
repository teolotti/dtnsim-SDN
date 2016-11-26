#ifndef _NET_H_
#define _NET_H_

#include <cstdio>
#include <string>
#include <omnetpp.h>
#include <fstream>
#include <map>
#include <queue>
#include "ContactPlan.h"
#include "dtnsim_m.h"
//#include "ion_interface/ION_interface.h"
#include "SdrModel.h"
#include "Routing.h"
#include "RoutingDirect.h"
#include "RoutingCgrModel.h"
#include "RoutingCgrIon350.h"

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
	virtual void initialize();
	virtual void handleMessage(cMessage *msg);
	virtual void dispatchBundle(Bundle *bundle);
	virtual double transmitBundle(int neighborEid, int contactId);
	virtual void parseContacts(string fileName);
	virtual void finish();
	virtual bool isOnFault();

private:

	int eid_;
	bool onFault;

	Routing * routing;
	ContactPlan contactPlan_;

	// (contact id --> freeChannelMsg)
	map<int, FreeChannelMsg *> freeChannelMsgs_;

	// (contact id --> bundles)
	SdrModel sdr_;

	// Fault parameters
	double meanTTF, meanTTR;

	// Visualization
	float posX, posY, posAngle, posRadius;
	vector<cLineFigure *> lines;

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

};

#endif /* NET_H_ */

