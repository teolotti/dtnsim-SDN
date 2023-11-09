/*
 * Controller.cc
 *
 *  Created on: Nov 8, 2023
 *      Author: matteo
 */

#include "Controller.h"

Controller::Controller(){
}

void Controller::setContactPlan(ContactPlan* contactPlan){
	contactplan_ = contactPlan;
}

Controller* Controller::getInstance(ContactPlan* contactPlan){

	if (Controller::instancePtr == nullptr){

		instancePtr = new Controller();
		instancePtr->setContactPlan(contactPlan);
		return instancePtr;

		} else {

		return instancePtr;

		}
}

vector<pair<int, pair<int,int>>> Controller::getWeightsAvailableContacts(BundlePkt* bundle, double simTime){
	vector<pair<int, pair<int,int>>> weightedContacts;
	for(auto& contact : *contactplan_->getContacts()){
		if(contact.getEnd()>=simTime){ //if contact is available
			double w_var;
			double w_fix;
			if (simTime<contact.getStart())
				w_var = contact.getStart()-simTime;
			else
				w_var = 0;
			double t_tx = bundle->getByteLength()/contact.getDataRate();
			double t_p = contact.getRange(); //range in speed-of-light time
			w_fix = t_p + t_tx;
			weightedContacts.insert(make_pair(contact.getId(), make_pair(w_var, __w_fix)));
		}
	}


}


Controller::~Controller()
{
	// TODO Auto-generated destructor stub
}

