#include "Mac.h"
#include "App.h"

Define_Module(Mac);

void Mac::initialize()
{

}

void Mac::handleMessage(cMessage *msg)
{
	if (msg->getKind() == BUNDLE)
	{
		Bundle* bundle = check_and_cast<Bundle *>(msg);

		int destinationEid = bundle->getDestinationEid();
		int ownEid = check_and_cast<App *>(this->getParentModule()->getSubmodule("app"))->getEid();

		if (ownEid == destinationEid)
		{
			send(msg, "gateToNet$o");
		}
		else
		{
			int destinationNode = destinationEid - 1;
			cModule *destinationModule = this->getParentModule()->getParentModule()->getSubmodule("node", destinationNode)->getSubmodule("mac");
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

