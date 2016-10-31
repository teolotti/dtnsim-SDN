
#ifndef ION_INTERFACE_H_
#define ION_INTERFACE_H_

#include <omnetpp.h>
#include "dtnsim_m.h"
#include "ion_interface/ION_interface.h"

using namespace omnetpp;
using namespace std;

class ION_interface
{
public:
	ION_interface();
	virtual ~ION_interface();

	virtual void cgrForward(Bundle *bundle, int destinationEid);

};

#endif /* ION_INTERFACE_H_ */
