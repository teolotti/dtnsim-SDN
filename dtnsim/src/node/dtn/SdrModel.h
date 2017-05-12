/*
 * SdrModel.h
 *
 *  Created on: Nov 25, 2016
 *      Author: juanfraire
 */

#ifndef SRC_NODE_DTN_SDRMODEL_H_
#define SRC_NODE_DTN_SDRMODEL_H_

#include <dtn/ContactPlan.h>
#include <dtn/SdrStatus.h>
#include <map>
#include <queue>
#include <deque>
#include <omnetpp.h>

#include "dtnsim_m.h"
#include "assert.h"

using namespace omnetpp;
using namespace std;

class SdrModel
{
public:
	SdrModel();
	virtual ~SdrModel();

	// Init sdr
	virtual void setEid(int eid);
	virtual void setNodesNumber(int nodesNumber);
	virtual void setContactPlan(ContactPlan *contactPlan);

	// Enqueue and dequeue
	virtual void enqueueBundleToContact(BundlePkt * bundle, int contactId);
	virtual bool isBundleForContact(int contactId);
	virtual BundlePkt * getNextBundleForContact(int contactId);
	virtual void popNextBundleForContact(int contactId);

	// Get information
	virtual int getBundlesStoredInSdr();
	virtual int getBundlesStoredInLimbo();
	virtual int getBytesStoredInSdr();
	virtual int getBytesStoredToNeighbor(int eid);
	virtual SdrStatus getSdrStatus();

	// Erase memory
	virtual void freeSdr(int eid);

private:

	int eid_;
	int nodesNumber_;

	ContactPlan *contactPlan_;
	map<int, deque<BundlePkt *> > bundlesQueue_;
};


#endif /* SRC_NODE_DTN_SDRMODEL_H_ */
