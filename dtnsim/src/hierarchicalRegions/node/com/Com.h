#ifndef SRC_HIERARCHICALREGIONS_NODE_COM_COM_H_
#define SRC_HIERARCHICALREGIONS_NODE_COM_COM_H_

#include <omnetpp.h>

#include <src/hierarchicalRegions/node/dtn/contacts/ContactPlan.h>
#include <src/hierarchicalRegions/node/MsgTypes.h>
#include <src/hierarchicalRegions/HierarchicalRegionsNetwork_m.h>

using namespace omnetpp;

namespace dtnsimhierarchical {

class Com: public cSimpleModule {

public:

	Com();
	virtual ~Com();

	virtual void setHomeContactPlan(ContactPlan &contactPlan);
	virtual void setOuterContactPlan(ContactPlan &contactPlan);

protected:

	virtual void initialize();
	virtual void handleMessage(cMessage *msg);

private:

	ContactPlan homeContactPlan_;
	ContactPlan outerContactPlan_;

	int eid_;
	string homeRegion_;
	string outerRegion_;
};
}

#endif /* SRC_HIERARCHICALREGIONS_NODE_COM_COM_H_ */
