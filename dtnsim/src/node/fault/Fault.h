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

#ifndef __DTNSIM_FAULT_H_
#define __DTNSIM_FAULT_H_

#include <omnetpp.h>

#include "Graphics.h"
#include "Net.h"

#include "dtnsim_m.h"

#define FAULT_START_TIMER 20
#define FAULT_END_TIMER 21

using namespace omnetpp;

class Fault: public cSimpleModule
{
protected:
	virtual void initialize();
	virtual void handleMessage(cMessage *msg);

private:
	double meanTTF, meanTTR;

	// Pointer to grahics module
	Graphics *graphicsModule;

	// Pointer to net module
	Net *netModule;
};

#endif
