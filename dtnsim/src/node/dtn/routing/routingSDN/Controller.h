/*
 * Controller.h
 *
 *  Created on: Nov 8, 2023
 *      Author: matteo
 */

#ifndef SRC_NODE_DTN_ROUTING_ROUTINGSDN_CONTROLLER_H_
#define SRC_NODE_DTN_ROUTING_ROUTINGSDN_CONTROLLER_H_

#include <bits/stdc++.h>
#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <climits>
#include <src/node/dtn/ContactPlan.h>
#include <src/node/dtn/SdrModel.h>
#include <src/node/dtn/routing/RoutingSDN.h>

class RoutingSDN;

class Controller
{
private:

	Controller();

	static Controller* instance;

	ContactPlan* contactplan_;

	std::vector<pair<BundlePkt*, vector<int>>> routes;   //int sono id del nodo

	int nodeNum_;

	vector<RoutingSDN*> nodes;

	friend class RoutingSDN;

public:

	Controller(const Controller& obj) = delete;

	void setNodeNum(int nodeNum);

	void setContactPlan(ContactPlan* contactPlan);

	static Controller* getInstance(ContactPlan* contactPlan, int nodeNum);

	vector<pair<int, pair<int, int>>> getWeightsAvailableContacts(BundlePkt* bundle, double simTime);

	vector<int> buildRoute(BundlePkt* bundle, double simTime, string routingType);

	void addRoutingSDNInstance(RoutingSDN* routingSDNInstance);





	virtual ~Controller();
};

#endif /* SRC_NODE_DTN_ROUTING_ROUTINGSDN_CONTROLLER_H_ */
