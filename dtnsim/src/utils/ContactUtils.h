#ifndef CONTACTUTILS_H_
#define CONTACTUTILS_H_

#include "src/node/dtn/Contact.h"

using namespace std;

namespace contactUtils
{
	//called when a discovered contact ends to get the resulting contact with correct end time
	Contact getContactAfterDiscovery(Contact* c, double end);


}

#endif /* CONTACTUTILS_H_ */
