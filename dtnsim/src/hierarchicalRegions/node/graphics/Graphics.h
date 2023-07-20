#ifndef SRC_HIERARCHICALREGIONS_NODE_GRAPHICS_GRAPHICS_H_
#define SRC_HIERARCHICALREGIONS_NODE_GRAPHICS_GRAPHICS_H_

#include <omnetpp.h>
#include <vector>
#include <sstream>

#include "src/distinctRegions/node/MsgTypes.h"
#include "src/hierarchicalRegions/HierarchicalRegionsNetwork_m.h"

using namespace omnetpp;
using namespace std;

namespace dtnsimhierarchical {
class Graphics: public cSimpleModule {

public:
	Graphics();
	virtual ~Graphics();

	//void setFaultOn();
	//void setFaultOff();
	void setContactOn(ContactsMsgHIRR* contactMsg);
	void setContactOff(ContactsMsgHIRR* contactMsg);
	void setBundlesInSdr(int bundleNumber);

protected:
	virtual void initialize();
	virtual void finish();

	cCanvas* networkCanvas_;

	double posX_;
	double posY_;

	vector<cLineFigure *> lines;
};

}

#endif /* SRC_HIERARCHICALREGIONS_NODE_GRAPHICS_GRAPHICS_H_ */
