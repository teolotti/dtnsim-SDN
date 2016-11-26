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
#include <omnetpp.h>
#include "dtnsim_m.h"

using namespace omnetpp;
using namespace std;

class SdrModel
{
public:
	SdrModel();
	virtual ~SdrModel();
	virtual void setStatsHandle(cOutVector * sdrBundlesInSdr, cOutVector * sdrBundleInLimbo);
	virtual void setEid(int eid);
	virtual void enqueueBundleToContact(Bundle * bundle, int contactId);
	virtual bool isBundleForContact(int contactId);
	virtual Bundle * getNextBundleForContact(int contactId);
	virtual void popNextBundleForContact(int contactId);
	virtual void freeSdr(int eid);

private:

	void updateStats();

	int eid_;

	map<int, queue<Bundle *> > bundlesQueue_;

	// Stats
	cOutVector * sdrBundlesInSdr_;
	cOutVector * sdrBundleInLimbo_;
	unsigned int sdrBundleInLimbo;
};


#endif /* SRC_NODE_NET_SDRMODEL_H_ */
