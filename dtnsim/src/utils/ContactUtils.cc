/*
 * ContactUtils.cc
 *
 *  Created on: Nov 29, 2021
 *      Author: simon
 */

#include "ContactUtils.h"

namespace contactUtils
{

Contact getContactAfterDiscovery(Contact* c, double end)
{
	return Contact((*c).getId(), (*c).getStart(), end, (*c).getSourceEid(), (*c).getDestinationEid(), (*c).getDataRate(), (*c).getConfidence(), (*c).getRange());
}

}

