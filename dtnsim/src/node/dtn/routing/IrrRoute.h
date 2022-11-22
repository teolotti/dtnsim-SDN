
#ifndef SRC_NODE_DTN_ROUTING_IRRROUTE_H_
#define SRC_NODE_DTN_ROUTING_IRRROUTE_H_

#include <list>
#include <utility>
#include <string>
#include <iostream>

using namespace std;

typedef struct
{
	// transRegionalRoute conformed by vents: NodeID(fromRegion, toRegion)
	list<pair<int, pair<string, string> > > route;

	/*void IrrRoute()
	{

	}

	void printRoute()
	{
		for(auto it = route.begin(); it != route.end(); ++it)
		{
			cout<<it->first<<"("<<it->second.first<<"->"<<it->second.second<<") ";
		}
		cout<<endl;
	}*/

} IrrRoute;

#endif /* SRC_NODE_DTN_ROUTING_IRRROUTE_H_ */
