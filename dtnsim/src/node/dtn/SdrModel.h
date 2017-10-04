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
#include <omnetpp.h>
#include "utils/Subject.h"

#include "dtnsim_m.h"
#include "assert.h"

using namespace omnetpp;
using namespace std;

class SdrModel: public Subject
{
public:
	SdrModel();
	virtual ~SdrModel();

	// Init sdr
	virtual void setEid(int eid);
	virtual void setNodesNumber(int nodesNumber);
	virtual void setContactPlan(ContactPlan *contactPlan);
	virtual void setSize(int size);

	// Enqueue and dequeue from contact
	virtual bool enqueueBundleToContact(BundlePkt * bundle, int contactId);
	virtual bool isBundleForContact(int contactId);
	virtual BundlePkt * getNextBundleForContact(int contactId);
	virtual void popNextBundleForContact(int contactId);

	//Enqueue and dequeue carrying bundles (non routed)
	virtual void enqueueBundle(BundlePkt * bundle);
	virtual void removeBundle(long bundleId);
	virtual list<BundlePkt *> getCarryingBundles();

	// Get information
	virtual int getBundlesCountInSdr();
	virtual int getBundlesCountInContact(int cid);
	virtual int getBundlesCountInLimbo();
	virtual list<BundlePkt*> * getBundlesInLimbo();
	virtual int getBytesStoredInSdr();
	virtual int getBytesStoredToNeighbor(int eid);
	virtual SdrStatus getSdrStatus();
	virtual BundlePkt * getEnqueuedBundle(long bundleId);

	// Erase memory
	virtual void freeSdr(int eid);

	// Check if there is free space in sdr for a new packet
	bool isSdrFreeSpace(int sizeNewPacket);

private:

	// capacity of sdr in bytes
	int size_;

	int eid_;
	int nodesNumber_;


	ContactPlan *contactPlan_;
	map<int, list<BundlePkt *> > bundlesQueue_;
	int bundlesNumber_; //Amount of bundles enqueued in sdr_. It considers all contacts (i.e contact 0 is included)
};


#endif /* SRC_NODE_DTN_SDRMODEL_H_ */
