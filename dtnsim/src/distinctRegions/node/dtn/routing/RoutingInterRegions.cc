#include <src/distinctRegions/node/dtn/routing/RoutingInterRegions.h>

namespace dtnsimdistinct {

RoutingInterRegions::RoutingInterRegions(int eid, SdrModel *sdr, map<string, ContactPlan> &contactPlans, RegionDatabase *regionDatabase, bool printDebug, bool staticIrr) : RoutingCgrModelYen(eid, sdr, contactPlans, printDebug) {
	regionDatabase_ = regionDatabase;
	staticIrr_ = staticIrr;
}

RoutingInterRegions::~RoutingInterRegions() {
}

void RoutingInterRegions::routeAndQueueBundle(BundlePacket *bundle, double simTime, int destinationEid) {

	if (!printDebug_) // disable cout if degug disabled
		cout.setstate(std::ios_base::failbit);

	// Reset counters
	dijkstraCalls = 0;
	dijkstraLoops = 0;
	nodesOneDijkstra = 0;
	edgesOneDijkstra = 0;

	tableEntriesCreated = 0;
	tableEntriesExplored = 0;
	routeTableSize = 0;

	routeSearchStarts = 0;
	yenIterations = 0;

	if (staticIrr_) {
		int nextDestinationEid = determineNextDestinationStatic(bundle);
		if (nextDestinationEid == 0) {
			cout << "Dropping bundle - no route found!" << endl;
			bundleDropped = true;
			delete bundle;
		} else {
			set<string> destinationNodeRegions = regionDatabase_->getRegions(bundle->getDestinationEid());
			if (currentRegion_ != "B" && *destinationNodeRegions.begin() == "B") {
				directEnqueue(bundle, nextDestinationEid);
			} else {
				cgrForward(bundle, simTime, nextDestinationEid);
			}
		}

	} else {

		// Normal IRR

		// determine destination of next vent using region database
		int nextDestinationEid = determineNextDestination(bundle);
		if (nextDestinationEid == 0) {
			cout << "Dropping bundle - no route found!" << endl;
			bundleDropped = true;
			delete bundle;
		}

		// Call cgrForward from ion (route and forwarding)
		cgrForward(bundle, simTime, nextDestinationEid);
	}

	if (!printDebug_)
		cout.clear();
}

int RoutingInterRegions::determineNextDestination(BundlePacket *bundle) {

	// Get all regions of which current node is part of
	set<string> currentNodeRegions = regionDatabase_->getRegions(eid_);

	// Get all regions of which bundle's destination node is part of
	set<string> destinationNodeRegions = regionDatabase_->getRegions(bundle->getDestinationEid());

	// Get all regions current and destination node have in common
	set<string> regionsInCommon;
	set_intersection(currentNodeRegions.begin(), currentNodeRegions.end(),
			destinationNodeRegions.begin(), destinationNodeRegions.end(), inserter(regionsInCommon, regionsInCommon.begin()));

	// If there is a single common region, both the current
	// and destination node are in the same region and we can
	// use normal CGR
	if (regionsInCommon.size() == 1) {
		currentRegion_ = *regionsInCommon.begin();
		return bundle->getDestinationEid();
	}

	// Else, we need to construct the inter-regional route
	// and find the node in the current node's regions that
	// functions as the route's entry node, i.e. entry passageway
	// We then use normal CGR to send bundle to that PW
	IrrRoute route = computeInterRegionalRoute(bundle, currentNodeRegions, destinationNodeRegions);

	// Iterate through regions of current node, i.e. potential starting regions
	for (auto & sourceRegion : currentNodeRegions) {

		// Check if starting region is part of any hop in IRR route
		for (auto & vent : route.vents) {

			if (vent.fromRegion == sourceRegion) {

				int pwNodeEid = vent.nodeEid;

				if (pwNodeEid == 0) {
					currentRegion_ = sourceRegion;
					return bundle->getDestinationEid();

				} else if (pwNodeEid != eid_) {
					currentRegion_ = sourceRegion;
					return pwNodeEid;

				} else {
					continue;
				}
			}
		}
	}

	// No next destination EID found...
	return 0;
}

int RoutingInterRegions::determineNextDestinationStatic(BundlePacket *bundle) {

	// Get all regions of which current node is part of
	set<string> currentNodeRegions = regionDatabase_->getRegions(eid_);

	// Get all regions of which bundle's destination node is part of
	set<string> destinationNodeRegions = regionDatabase_->getRegions(bundle->getDestinationEid());

	// Get all regions current and destination node have in common
	set<string> regionsInCommon;
	set_intersection(currentNodeRegions.begin(), currentNodeRegions.end(),
			destinationNodeRegions.begin(), destinationNodeRegions.end(), inserter(regionsInCommon, regionsInCommon.begin()));

	// If there is a single common region, both the current
	// and destination node are in the same region and we can
	// use normal CGR
	if (regionsInCommon.size() == 1) {
		currentRegion_ = *regionsInCommon.begin();
		return bundle->getDestinationEid();
	}

	// We need to cross one region border

	// In our network, final destination can only ever belong to one region
	string sourceRegion = *currentNodeRegions.begin();
	string destinationRegion = *destinationNodeRegions.begin();

	// With 3 relays, only ever one node represents a vent to the destination region
	int dedicatedPwNode = 0;
	map<int, set<Vent>> allVents = regionDatabase_->getAllVents();
	for (auto & node : allVents) {
		for (auto & vent : node.second) {
			if (vent.fromRegion == sourceRegion && vent.toRegion == destinationRegion) {
				dedicatedPwNode = vent.nodeEid;
				currentRegion_ = vent.fromRegion;
				break;
			}
		}
		if (dedicatedPwNode != 0) {
			break;
		}
	}

	// 0 if no next destination EID found...
	return dedicatedPwNode;
}

void RoutingInterRegions::directEnqueue(BundlePacket *bundle, int nextHopNode) {

	bundle->setNextHopEid(nextHopNode);
	vector<Contact *> noHops;
	bool enqueued = sdr_->enqueueBundle(nextHopNode, bundle, noHops);

	if (bundle->getFirstHopEid() == 0) {
		bundle->setFirstHopEid(nextHopNode);
	}

	if (!enqueued) {
		cout << "Dropping Bundle failed enqueue" << endl;
		bundleDropped = true;
		delete bundle;
	}
}


IrrRoute RoutingInterRegions::computeInterRegionalRoute(BundlePacket *bundle, set<string> currentNodeRegions, set<string> destinationNodeRegions) {

	IrrRoute route;
	map<int, set<Vent>> allVents = regionDatabase_->getAllVents();

	// Check if a possible route even exists
	bool destinationVentFound = false;
	for (auto & node : allVents) {
		for (auto & vent : node.second) {
			if (destinationNodeRegions.count(vent.toRegion) > 0) {
				destinationVentFound = true;
				break;
			}
		}
	}
	if (!destinationVentFound) {
		cout << "No route found..." << endl;
		return route;
	}

	// Shortest path algorithm over vent graph

	// Vertices of search graph
	vector<IrrVertex> vertices;

	// Notational vertices
	IrrVertex notationalSource("source", "source");
	vertices.push_back(notationalSource);
	IrrVertex notationalDestination("destination", "destination");
	notationalDestination.distanceFromOrigin = numeric_limits<double>::max();
	vertices.push_back(notationalDestination);

	// Set of notational sources and destinations
	set<pair<string, string>> sources;
	set<pair<string, string>> destinations;

	for (auto & region : currentNodeRegions) {
		IrrVertex vertexSource("source", region);
		vertexSource.distanceFromOrigin = numeric_limits<double>::max();
		vertices.push_back(vertexSource);
		sources.insert(make_pair("source", region));
	}

	for (auto & region : destinationNodeRegions) {
		IrrVertex vertexDestination(region, "destination");
		vertexDestination.distanceFromOrigin = numeric_limits<double>::max();
		vertices.push_back(vertexDestination);
		destinations.insert(make_pair(region, "destination"));
	}

	for (auto & node : allVents) {
		for (auto & vent : node.second) {
			IrrVertex vertex(vent);
			vertex.distanceFromOrigin = numeric_limits<double>::max();
			vertices.push_back(vertex);
		}
	}

	IrrVertex* currentVertex = NULL;
	while (true) {

		double minDistance = numeric_limits<double>::max();
		for (auto & vertex : vertices) {
			if (!vertex.visited) {
				if (vertex.distanceFromOrigin < minDistance) {
					minDistance = vertex.distanceFromOrigin;
					currentVertex = &vertex;
				}
			}
		}

		if (*currentVertex == notationalDestination) {
			break;

		} else {
			for (auto & vertex : vertices) {
				if (vertex != *currentVertex) {
					if (vertex.vent.fromRegion == currentVertex->vent.toRegion) {

						bool condition1 = ((*currentVertex == notationalSource) && (sources.find(make_pair(vertex.vent.fromRegion, vertex.vent.toRegion)) != sources.end()));
						bool condition2 = ((destinations.find(make_pair(vertex.vent.fromRegion, vertex.vent.toRegion)) != destinations.end()) && (vertex == notationalDestination));

						double newDistanceFromOrigin;

						if (condition1) {
							newDistanceFromOrigin = 0;
						} else if (condition2) {
							newDistanceFromOrigin = currentVertex->distanceFromOrigin;
						} else {
							newDistanceFromOrigin = currentVertex->distanceFromOrigin + 1.0;
						}

						if (newDistanceFromOrigin < vertex.distanceFromOrigin) {
							vertex.distanceFromOrigin = newDistanceFromOrigin;
							vertex.predecessor = currentVertex;
						}
					}
				}
			}

			currentVertex->visited = true;
		}
	} // while loop end updating distance cost metric at each vertex

	while (true) {

		if (currentVertex == NULL) {
			break;
		}

		IrrVertex* predecessor = currentVertex ->predecessor;
		currentVertex = predecessor;

		if (predecessor == NULL) {
			break;
		}

		if (*predecessor != notationalSource) {
			Vent hop = predecessor->vent;
			route.vents.push_front(hop);
		}

	} // while loop end constructing IRR route

	return route;
}

}
