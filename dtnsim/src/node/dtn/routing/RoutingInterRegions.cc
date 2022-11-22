#include <src/node/dtn/routing/RoutingInterRegions.h>

RoutingInterRegions::RoutingInterRegions(int eid, RegionDatabase * regionDatabase, string interRegionsRoutingType, bool printDebug)
{
	this->eid_ = eid;
	this->regionDatabase_ = regionDatabase;
	this->interRegionsRoutingType_ = interRegionsRoutingType;
	this->printDebug_ = printDebug;
	this->dijkstraCalls_ = 0;
	this->nodesOneDijkstra = 0;
	this->edgesOneDijkstra = 0;
}
RoutingInterRegions::~RoutingInterRegions()
{

}

IrrRoute RoutingInterRegions::computeInterRegionsRouting(BundlePkt * bundle, set<string> thisNodeRegionIds, set<string> destinationRegionIds)
{
	dijkstraCalls_ = 0;
	nodesOneDijkstra = 0;
	edgesOneDijkstra = 0;

	if (!printDebug_) // disable cout if degug disabled
		cout.setstate(std::ios_base::failbit);

	// interRegionsRoute
	IrrRoute irrRoute;

	// if source interRegionRouting is disabled
	// calculate route and set last region id in the bundle as the first region of the route
	if ((interRegionsRoutingType_.find("extensionBlock:off") != std::string::npos))
	{
		irrRoute = getShortestPath(bundle, thisNodeRegionIds, destinationRegionIds);
	}
	else
	{
		// if source interRegionRouting is enabled but the interRegionRoute is empty
		// calculate the route and save it into the bundle.
		// Also, set last region id in the bundle as the first region of the route
		if ((bundle->getIrrRoute().route.empty()))
		{
			irrRoute = getShortestPath(bundle, thisNodeRegionIds, destinationRegionIds);
			bundle->setIrrRoute(irrRoute);
		}
		// if source interRegionRouting is enabled and the interRegionRoute is not empty
		// use the calculated route
		else
		{
			irrRoute = bundle->getIrrRoute();
		}
	}

	if (!printDebug_)
		cout.clear();

	return irrRoute;
}

IrrRoute RoutingInterRegions::getShortestPath(BundlePkt * bundle, set<string> thisNodeRegionIds, set<string> destinationRegionIds)
{
	cout << "!!!!!!!! !!!!!!!!!! getShortestPath" << endl;
	dijkstraCalls_++;

	IrrRoute irrRoute;

	// if there is no vent connecting to any of the destination regions, return empty irrRoute
	bool foundVentConnectingDestinationRegion = false;
	map<int, set<pair<string, string> > > nodesVents = regionDatabase_->getNodesVents();

	for(auto it = nodesVents.begin(); it != nodesVents.end(); ++it)
	{
		set<pair<string, string> > vents = it->second;
		for(auto it2 = vents.begin(); it2 != vents.end(); ++it2)
		{
			string ventDestinationRegion = it2->second;

			if(destinationRegionIds.find(ventDestinationRegion) != destinationRegionIds.end())
			{
				foundVentConnectingDestinationRegion = true;
				break;
			}
		}

		if(foundVentConnectingDestinationRegion)
		{
			break;
		}
	}
	if(!foundVentConnectingDestinationRegion)
	{
		// return empty irrRoute
		cout<<"returning empty route"<<endl;
		return irrRoute;
	}

// populate vertices
	vector<InterRegionsVertex> vertices;

// set of notional sources
	set < pair<string, string> > sources;

// set of notional destinations
	set < pair<string, string> > destinations;

// create notional vertices

	InterRegionsVertex notionalSource;
	notionalSource.vent = make_pair("source", "source");
	notionalSource.distanceFromOrigin = 0;
	vertices.push_back(notionalSource);

	InterRegionsVertex notionalDestination;
	notionalDestination.vent = make_pair("destination", "destination");
	notionalDestination.distanceFromOrigin = numeric_limits<double>::max();
	vertices.push_back(notionalDestination);

	for (auto it = thisNodeRegionIds.begin(); it != thisNodeRegionIds.end(); ++it)
	{
		nodesOneDijkstra ++;

		string sourceRegionId = *it;

		// create notional vertices
		cout << "sourceRegionId = " << sourceRegionId << endl;

		InterRegionsVertex vertexSource;
		vertexSource.vent = make_pair("source", sourceRegionId);
		vertexSource.distanceFromOrigin = numeric_limits<double>::max();
		vertices.push_back(vertexSource);

		// insert notional source in the set of notional sources
		sources.insert(make_pair("source", sourceRegionId));
	}

	for (auto it = destinationRegionIds.begin(); it != destinationRegionIds.end(); ++it)
	{
		nodesOneDijkstra ++;

		string destinationRegionId = *it;

		// create notional vertices
		cout << "destinationRegionId = " << destinationRegionId << endl;

		InterRegionsVertex vertexDestination;
		vertexDestination.vent = make_pair(destinationRegionId, "destination");
		vertexDestination.distanceFromOrigin = numeric_limits<double>::max();
		vertices.push_back(vertexDestination);

		// insert notional destination in the set of notional destinations
		destinations.insert(make_pair(destinationRegionId, "destination"));
	}

// create one vertice per vent
	for (auto it = nodesVents.begin(); it != nodesVents.end(); ++it)
	{
		nodesOneDijkstra ++;

		int passagewayNode = it->first;
		set < pair<string, string> > vents = it->second;
		for (auto it2 = vents.begin(); it2 != vents.end(); ++it2)
		{
			pair < string, string > vent = *it2;

			InterRegionsVertex vertex;
			vertex.passagewayNodeEid = passagewayNode;
			vertex.vent = vent;
			vertex.distanceFromOrigin = numeric_limits<double>::max();
			vertices.push_back(vertex);
		}
	}

	InterRegionsVertex *currentVertex = NULL;
	while (true)
	{
		cout << "new loop in while 1" << endl;

		// select currentVertex as the vertex non visited closest to the origin
		double minDistance = numeric_limits<double>::max();
		for (auto it = vertices.begin(); it != vertices.end(); ++it)
		{
			if (!it->visited)
			{
				if (it->distanceFromOrigin < minDistance)
				{
					currentVertex = &(*it);
					minDistance = currentVertex->distanceFromOrigin;
				}
			}
		}

		cout << "############################ currentVertex = " << currentVertex->passagewayNodeEid << "," << currentVertex->vent.first << "->" << currentVertex->vent.second << endl;

		// if the currentVertex is the destinationVertex break the loop
		if (*currentVertex == notionalDestination)
		{
			cout << "break destination found" << endl;
			break;
		}
		else
		{
			cout << "traversing neighbors" << endl;
			// for each vertex
			for (auto it = vertices.begin(); it != vertices.end(); ++it)
			{
				// if it is not the same vertex
				if (*it != *currentVertex)
				{
					// which is a neighbor of the current vertex,
					// where a neighbor is a vertex whose receptionRegionId is equal to the transmissionRegionId of the current vertex.

					cout << "possible neighbor: " << it->passagewayNodeEid << "," << it->vent.first << "->" << it->vent.second << endl;
					if (it->vent.first == currentVertex->vent.second)
					{
						edgesOneDijkstra++;

						cout << "-------------------> neighbor found: " << it->passagewayNodeEid << "," << it->vent.first << "->" << it->vent.second << endl;

						bool condition1 = ((*currentVertex == notionalSource) && (sources.find(it->vent) != sources.end()));
						bool condition2 = ((destinations.find(currentVertex->vent) != destinations.end()) && (*it == notionalDestination));
						cout << "condition1 = " << condition1 << endl;
						cout << "condition2 = " << condition2 << endl;

						// compute new distance from origin to that neighbor vertex
						double newDistanceFromOrigin;
						if (condition1)
						{
							newDistanceFromOrigin = 0;
						}
						else if (condition2)
						{
							newDistanceFromOrigin = currentVertex->distanceFromOrigin;
						}
						else
						{
							newDistanceFromOrigin = currentVertex->distanceFromOrigin + computeDistanceBetweenVertices(*currentVertex, *it);
						}

						cout << "newDistance = " << newDistanceFromOrigin << endl;

						// if the new computed distance is less than the previous distance from origin to the neighbor
						// change that distance and set the predecessor vertex of the neighbor to currentVertex
						if (newDistanceFromOrigin < it->distanceFromOrigin)
						{
							it->distanceFromOrigin = newDistanceFromOrigin;
							it->predecessorVertex = currentVertex;

							cout << "-------------------------> newDistanceFromOrigin < it->distanceFromOrigin" << endl;
							cout << "-------------------------> it->predecessorVertex = currentVertex" << endl;
						}
					}
				}
			}

			// mark the current vertex as visited
			currentVertex->visited = true;
		}
	}

// extract the route as a sequence of predecessor vertices from currentVertex


	list < pair<int, pair<string,string> > > irrRouteAux;
	while (true)
	{
		if (currentVertex != NULL)
		{
			InterRegionsVertex * predecessor = currentVertex->predecessorVertex;

			if (predecessor != NULL)
			{
				//not consider the notionalSource vertex
				if (*predecessor != notionalSource)
				{
					irrRouteAux.push_front(make_pair(predecessor->passagewayNodeEid, make_pair(predecessor->vent.first, predecessor->vent.second) ) );
				}
			}
			else
			{
				cout << "predecessor = NULL" << endl;
				break;
			}
			currentVertex = predecessor;
		}
		else
		{
			cout << "currentVertex = NULL" << endl;
			break;
		}
	}

	irrRoute.route = irrRouteAux;

	//irrRoute.printRoute();

	return irrRoute;
}

double RoutingInterRegions::computeDistanceBetweenVertices(InterRegionsVertex v1, InterRegionsVertex v2)
{
// todo
// improve with distance computed as distance between nodes in a pseudo-space
	return 1.0;
}

RegionDatabase* RoutingInterRegions::getRegionDatabase()
{
	return regionDatabase_;
}

double RoutingInterRegions::getDijkstraCalls() const
{
	return dijkstraCalls_;
}

double RoutingInterRegions::getEdgesOneDijkstra() const
{
	return edgesOneDijkstra;
}

double RoutingInterRegions::getNodesOneDijkstra() const
{
	return nodesOneDijkstra;
}

double RoutingInterRegions::getOneDijkstraComplexity() const
{
	return (edgesOneDijkstra + nodesOneDijkstra * log(nodesOneDijkstra));
}
