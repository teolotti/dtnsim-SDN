/*
 * SdrModel.h
 *
 *  Created on: Nov 25, 2016
 *      Author: juanfraire
 */

#ifndef SRC_NODE_NET_SDRMODEL_H_
#define SRC_NODE_NET_SDRMODEL_H_

#include <map>
#include <queue>
#include <deque>
#include <omnetpp.h>

#include "dtnsim_m.h"
#include "SdrStatus.h"
#include "ContactPlan.h"
#include "assert.h"

using namespace omnetpp;
using namespace std;

class SdrModel
{
public:
	SdrModel();
	virtual ~SdrModel();
	virtual void setStatsHandle(cOutVector * sdrBundlesInSdr, cOutVector * sdrBundleInLimbo);
	virtual void setEid(int eid);
	virtual void setNodesNumber(int nodesNumber);
	virtual void setContactPlan(ContactPlan *contactPlan);
	virtual void enqueueBundleToContact(BundlePkt * bundle, int contactId);
	virtual bool isBundleForContact(int contactId);
	virtual BundlePkt * getNextBundleForContact(int contactId);
	virtual void popNextBundleForContact(int contactId);
	virtual void freeSdr(int eid);

	virtual int getBundlesInSdr();
	virtual int getBundlesSizeEnqueuedToNeighbor(int eid);
	virtual SdrStatus getSdrStatus();

private:

	void updateStats();

	int eid_;

	int nodesNumber_;

	ContactPlan *contactPlan_;

	map<int, deque<BundlePkt *> > bundlesQueue_;

	// Stats
	double lastUdateTime;
	double bundlesInSdrPerTime;
	cOutVector * sdrBundlesInSdr_;
	cOutVector * sdrBundleInLimbo_;
	unsigned int sdrBundleInLimbo;
};


#endif /* SRC_NODE_NET_SDRMODEL_H_ */
