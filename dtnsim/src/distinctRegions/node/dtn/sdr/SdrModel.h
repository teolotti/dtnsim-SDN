#ifndef SRC_DISTINCTREGIONS_NODE_DTN_SDRMODEL_H_
#define SRC_DISTINCTREGIONS_NODE_DTN_SDRMODEL_H_

#include <src/distinctRegions/node/dtn/contacts/ContactPlan.h>

#include <queue>
#include <map>
#include <omnetpp.h>

// this class is a publisher
// -> can add/remove subscribers (observers)
// -> can notify all observers of new message (call update message on each observer)
#include "src/utils/Subject.h"

#include "src/distinctRegions/RegionsNetwork_m.h"
#include "assert.h"

using namespace omnetpp;
using namespace std;

namespace dtnsimdistinct {

class SdrModel: public Subject {

public:

	SdrModel(int eid, int totalCapacity);
	virtual ~SdrModel();

	// free the entire bundle queue of a node
	virtual void freeSdr();

	// Get information
	virtual int getBundlesCountInSdr();
	virtual int getBytesStoredInSdr();

	virtual int getBundlesCountInQueue(int eid);
	virtual int getBytesStoredInQueue(int eid);

	virtual bool isBundleWaiting(int eid);
	virtual BundlePacket* getBundleWaiting(int eid);

	// insert/remove bundle from a specific queue
	virtual bool enqueueBundle(int eid, BundlePacket *bundle, vector<Contact*> hops);
	virtual void dequeueBundle(int eid);
	//virtual void removeBundle(int eid, long bundleId);


private:

	int eid_;							// eid of the SDR's node
	//unordered_set<int> contactNodes_;	// list of node EIDs that could come in contact with SDR's node
	int totalCapacity_;					// capacity of SDR in bytes

	map<int, queue<pair<BundlePacket *, vector<Contact*>>>> perNodeBundleQueue_;

};

}

#endif /* SRC_DISTINCTREGIONS_NODE_DTN_SDRMODEL_H_*/
