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

public:
	Controller(const Controller& obj) = delete;

	static Controller* getInstance(){

		if (instancePtr == nullptr){

			instancePtr = new Controller();
			return instancePtr;

		} else {

			return instancePtr;

		}
	}


	virtual ~Controller();
};

#endif /* SRC_NODE_DTN_ROUTING_ROUTINGSDN_CONTROLLER_H_ */
