//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

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
