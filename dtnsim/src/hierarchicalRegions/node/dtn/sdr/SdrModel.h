#ifndef SRC_HIERARCHICALREGIONS_NODE_DTN_SDRMODEL_H_
#define SRC_HIERARCHICALREGIONS_NODE_DTN_SDRMODEL_H_

#include <src/hierarchicalRegions/node/dtn/contacts/ContactPlan.h>
#include <src/hierarchicalRegions/node/dtn/sdr/SdrStatus.h>

#include <queue>
#include <map>
#include <omnetpp.h>

// this class is a publisher
// -> can add/remove subscribers (observers)
// -> can notify all observers of new message (call update message on each observer)
#include "src/utils/Subject.h"

#include "src/hierarchicalRegions/HierarchicalRegionsNetwork_m.h"
#include "assert.h"

using namespace omnetpp;
using namespace std;

// TODO assumption: SDR is the memory for a DTN node, I'm going to treat it as such -> one vector per node
// TODO limbo? status? custody stuff...
namespace dtnsimhierarchical {

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
	virtual BundlePacketHIRR* getBundleWaiting(int eid);

	// insert/remove bundle from a specific queue
	virtual bool enqueueBundle(int eid, BundlePacketHIRR *bundle);
	virtual void dequeueBundle(int eid);
	//virtual void removeBundle(int eid, long bundleId);


private:

	int eid_;							// eid of the SDR's node
	//unordered_set<int> contactNodes_;	// list of node EIDs that could come in contact with SDR's node
	int totalCapacity_;					// capacity of SDR in bytes

	map<int, queue<BundlePacketHIRR *> > perNodeBundleQueue_;

};

}

#endif /* SRC_HIERARCHICALREGIONS_NODE_DTN_SDRMODEL_H_*/
