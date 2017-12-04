#include <com/Com.h>
#include "App.h"

Define_Module (Com);

void Com::initialize()
{
	// Store this node eid
	this->eid_ = this->getParentModule()->getIndex();
}

void Com::setContactTopology(ContactPlan &contactTopology)
{
	this->contactTopology_ = contactTopology;
}

void Com::handleMessage(cMessage *msg)
{
	if (msg->getKind() == BUNDLE)
	{
		BundlePkt* bundle = check_and_cast<BundlePkt *>(msg);

		if (eid_ == bundle->getNextHopEid())
		{
			send(msg, "gateToDtn$o");
		}
		else
		{
			// Get a pointer to the next hop mac module
			cModule *destinationModule = this->getParentModule()->getParentModule()->getSubmodule("node", bundle->getNextHopEid())->getSubmodule("com");

			//Zero delay send:
			//sendDirect(msg, destinationModule, "gateToAir");

			//Delayed send
			double linkDelay = contactTopology_.getRangeBySrcDst(eid_, bundle->getNextHopEid());
			sendDirect(msg, linkDelay, 0, destinationModule, "gateToAir");
		}
	}
}

void Com::finish()
{

}

Com::Com()
{

}

Com::~Com()
{

}

