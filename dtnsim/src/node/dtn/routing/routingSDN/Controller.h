/*
 * Controller.h
 *
 *  Created on: Nov 8, 2023
 *      Author: matteo
 */

#ifndef SRC_NODE_DTN_ROUTING_ROUTINGSDN_CONTROLLER_H_
#define SRC_NODE_DTN_ROUTING_ROUTINGSDN_CONTROLLER_H_

#include <bits/stdc++.h>


class Controller
{
private:
	static Controller* instancePtr; //pointer to the object, singleton

	Controller();

	ContactPlan* contactplan_;

public:
	Controller(const Controller& obj) = delete;

	void setContactPlan(ContactPlan* contactPlan);

	static Controller* getInstance(ContactPlan* contactPlan);






	virtual ~Controller();
};

#endif /* SRC_NODE_DTN_ROUTING_ROUTINGSDN_CONTROLLER_H_ */
