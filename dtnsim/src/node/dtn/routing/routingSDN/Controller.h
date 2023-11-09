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


class Controller
{
private:
	static Controller* instancePtr; //pointer to the object, singleton

	Controller();

	ContactPlan* contactplan_;

	map<BundlePkt*, vector<int>> routes;

	int nodeNum_;

public:

	Controller(const Controller& obj) = delete;

	void setNodeNum(int nodeNum);

	void setContactPlan(ContactPlan* contactPlan);

	static Controller* getInstance(ContactPlan* contactPlan, int nodeNum);

	vector<pair<int, pair<int, int>>> getWeightsAvailableContacts(BundlePkt* bundle, double simTime);

	void getRoute(BundlePkt* bundle, double simTime);



	virtual ~Controller();
};

Controller* Controller::instancePtr = nullptr;

#endif /* SRC_NODE_DTN_ROUTING_ROUTINGSDN_CONTROLLER_H_ */
