#ifndef SRC_DISTINCTREGIONS_NODE_DTN_ROUTING_H_
#define SRC_DISTINCTREGIONS_NODE_DTN_ROUTING_H_

#include <src/distinctRegions/node/dtn/contacts/ContactPlan.h>
#include <src/distinctRegions/node/dtn/sdr/SdrModel.h>

#include <map>
#include <queue>
#include <limits>
#include <algorithm>
#include "src/distinctRegions/RegionsNetwork_m.h"

using namespace omnetpp;
using namespace std;

namespace dtnsimdistinct {
class Routing {

public:

	Routing(int eid, SdrModel *sdr) {
		eid_ = eid;
		sdr_ = sdr;
	}

	virtual ~Routing(){};

	/**
	 * Method that will be called by Dtn module when a message to other destination in the
	 * network arrives.
	 */
	virtual void msgToOtherArrive(BundlePacket *bundle, double simTime, int terminusNode) = 0;

	/**
	 * Method that will be called by Dtn module when a message to this node arrives.
	 *
	 * @Return - true if bundle must be send to app layer.
	 */
	virtual bool msgToMeArrive(BundlePacket *bundle) = 0;

	/**
	 * Method that will be called by Dtn module when a contact starts.
	 */
	virtual void contactStart(Contact *c) = 0;

	/**
	 * Method that will be called by Dtn module when a contact ends.
	 */
	virtual void contactEnd(Contact *c) = 0;

	virtual void  refreshForwarding(Contact *c) = 0;

	/**
	 * Method that will be called by Dtn module when some bundle is forwarded successfully
	 */
	virtual void successfulBundleForwarded(long bundleId, Contact *contact, bool sentToDestination)=0;



protected:

	//Endpoint id
	int eid_;

	//Sdr model to enqueue bundles for transmission
	SdrModel* sdr_;

};
}

#endif /* SRC_DISTINCTREGIONS_NODE_DTN_ROUTING_H_ */
