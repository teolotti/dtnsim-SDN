/*
 * This code is outdated and needs to be adapted. Currently, if the
 * GUI is even used, the x and y coordinate of a node are determined
 * in their own script and set for each individual node.
 */

#ifndef SRC_DISTINCTREGIONS_NODE_GRAPHICS_GRAPHICS_H_
#define SRC_DISTINCTREGIONS_NODE_GRAPHICS_GRAPHICS_H_

#include <omnetpp.h>
#include <vector>
#include <sstream>

#include "src/distinctRegions/node/MsgTypes.h"
#include "src/distinctRegions/RegionsNetwork_m.h"

using namespace omnetpp;
using namespace std;

namespace dtnsimdistinct {
class Graphics: public cSimpleModule {

public:
	Graphics();
	virtual ~Graphics();

	//void setFaultOn();
	//void setFaultOff();
	void setContactOn(ContactsMsg* contactMsg);
	void setContactOff(ContactsMsg* contactMsg);
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

#endif /* SRC_DISTINCTREGIONS_NODE_GRAPHICS_GRAPHICS_H_ */
