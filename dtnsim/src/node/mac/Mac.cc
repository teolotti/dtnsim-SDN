#include "Mac.h"
#include "App.h"

Define_Module (Mac);

void Mac::initialize()
{
	// Store this node eid
	this->eid_ = this->getParentModule()->getIndex();
}

void Mac::handleMessage(cMessage *msg)
{
	if (msg->getKind() == BUNDLE)
	{
		BundlePkt* bundle = check_and_cast<BundlePkt *>(msg);

		if (eid_ == bundle->getNextHopEid())
		{
			send(msg, "gateToNet$o");
		}
		else
		{
			// Get a pointer to the next hop mac module
			cModule *destinationModule = this->getParentModule()->getParentModule()->getSubmodule("node", bundle->getNextHopEid())->getSubmodule("mac");

			sendDirect(msg, destinationModule, "gateToAir");
		}
	}
}

void Mac::finish()
{

}

Mac::Mac()
{

}

Mac::~Mac()
{

}

