#include "Fault.h"

Define_Module (Fault);

void Fault::initialize()
{
	// Get a pointer to graphics module
	graphicsModule = (Graphics *) this->getParentModule()->getSubmodule("graphics");

	// Get a pointer to net module
	netModule = (Net *) this->getParentModule()->getSubmodule("net");

	// Initialize faults
	if (this->par("enable").boolValue() == true)
	{
		meanTTF = this->par("meanTTF").doubleValue();
		meanTTR = this->par("meanTTR").doubleValue();

		cMessage *faultMsg = new ContactMsg("fault", FAULT_START_TIMER);
		scheduleAt(exponential(meanTTF), faultMsg);
	}
}

void Fault::handleMessage(cMessage *msg)
{
	if (msg->getKind() == FAULT_START_TIMER)
	{
		// Enable fault mode
		graphicsModule->setFaultOn();
		netModule->setOnFault(true);

		// Schedule fault recovery
		msg->setKind(FAULT_END_TIMER);
		scheduleAt(simTime() + exponential(meanTTR), msg);
	}
	else if (msg->getKind() == FAULT_END_TIMER)
	{
		// Disable dault mode
		graphicsModule->setFaultOff();
		netModule->setOnFault(false);

		// Schedule next fault
		msg->setKind(FAULT_START_TIMER);
		scheduleAt(simTime() + exponential(meanTTF), msg);
	}
}
