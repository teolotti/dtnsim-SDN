#include <com/Com.h>
#include "App.h"

Define_Module (Com);

void Com::initialize()
{
	// Store this node eid
	this->eid_ = this->getParentModule()->getIndex();
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

			sendDirect(msg, destinationModule, "gateToAir");
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

