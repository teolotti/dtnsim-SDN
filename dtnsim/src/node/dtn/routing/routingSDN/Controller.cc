/*
 * Controller.cc
 *
 *  Created on: Nov 8, 2023
 *      Author: matteo
 */

#include "Controller.h"

Controller::Controller(){
}

Controller::setContactPlan(ContactPlan* contactPlan){
	contactplan_ = contactPlan;
}

Controller::getInstance(ContactPlan* contactPlan){

	if (instancePtr == nullptr){

		instancePtr = new Controller();
		instancePtr->setContactPlan(contactPlan);
		return instancePtr;

		} else {

		return instancePtr;

		}
}


Controller::~Controller()
{
	// TODO Auto-generated destructor stub
}

