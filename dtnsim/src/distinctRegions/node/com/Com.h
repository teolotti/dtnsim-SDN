/*
 * The COM module only handles outgoing/incoming bundles.
 * If this node is the intended receiver (next hop), it
 * relays it to the DTN layer.
 * If this node is the intended sender, it makes sure to
 * send the bundle to the correct COM module of the intended
 * next hop node (according to the bundles next hop field).
 */

/*
 * TODO:
 * - custody report
 * - nicer next com selection
 * - link delay/range
 */
#ifndef SRC_DISTINCTREGIONS_NODE_COM_COM_H_
#define SRC_DISTINCTREGIONS_NODE_COM_COM_H_

#include <omnetpp.h>

#include <src/distinctRegions/node/dtn/contacts/ContactPlan.h>
#include <src/distinctRegions/node/MsgTypes.h>
#include <src/distinctRegions/RegionsNetwork_m.h>

using namespace omnetpp;

namespace dtnsimdistinct {

class Com: public cSimpleModule {

public:

	Com();
	virtual ~Com();

	virtual void setRegionContactPlan(ContactPlan &regionContactPlan);
	virtual void setBackboneContactPlan(ContactPlan &backboneContactPlan);

protected:

	virtual void initialize();
	virtual void handleMessage(cMessage *msg);

private:

	ContactPlan regionContactPlan_;
	ContactPlan backboneContactPlan_;

	int eid_;
	string region_;
};
}

#endif /* SRC_DISTINCTREGIONS_NODE_COM_COM_H_ */
