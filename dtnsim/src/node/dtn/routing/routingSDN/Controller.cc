/*
 * Controller.cc
 *
 *  Created on: Nov 8, 2023
 *      Author: matteo
 */

#include "Controller.h"
using namespace std;
#include <iterator>
#include <map>

Controller::Controller(){
}

void Controller::setContactPlan(ContactPlan* contactPlan){
	contactplan_ = contactPlan;
}

void Controller::setNodeNum(int nodeNum){
	nodeNum_ = nodeNum;
}

Controller* Controller::getInstance(ContactPlan* contactPlan, int nodeNum){

	if (Controller::instancePtr == nullptr){

		instancePtr = new Controller();
		instancePtr->setContactPlan(contactPlan);
		instancePtr->setNodeNum(nodeNum);
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
			weightedContacts.push_back(make_pair(contact.getId(), make_pair(w_var, w_fix)));
		}
	}
	return weightedContacts;
}

void Controller::getRoute(BundlePkt* bundle, double simTime){
	int source = bundle->getSenderEid();

	vector<int> sum_weight(nodeNum_, INT_MAX);
	vector<int> predecessor(nodeNum_, -1);

	sum_weight[source] = 0; //weight from source node to itself is 0

    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    pq.push({0, source});

    while(!pq.empty()){
    	int u = pq.top().second;
    	pq.pop();

    	for(const pair<int, pair<int, int>> contact : getWeightsAvailableContacts(bundle, simTime)){
    		Contact * current = contactplan_->getContactById(contact.first);
    		if(current->getSourceEid() == u){  //if source node of this contact is u ok, else next
    			int v = current->getDestinationEid();

    			double w_var = contact.second.first;
    			double w_fis = contact.second.second;

    			double t_end = current->getEnd();

    			if(w_var > sum_weight[u]){
    				w_var -= sum_weight[u];
    			} else {
    				if((t_end > sum_weight[u]) && ((t_end-sum_weight[u]) > w_fis))
    					w_var = 0;
    				else if((t_end < sum_weight[u]) || ((t_end-sum_weight[u]) < w_fis))
    					w_var = INT_MAX;
    			}

    			int new_weight = sum_weight[u] + w_var + w_fis;

    			if(new_weight < sum_weight[v]){
    				sum_weight[v] = new_weight;
    				predecessor[v] = u;
    				pq.push({new_weight, v});
    			}
    		}
    	}
    }

    //Build the route and store it in map<BundlePkt*, vector<int>> routes;

    vector<int> shortest_route;
    int current = bundle->getDestinationEid();
    while(current != -1) {
    	shortest_route.push_back(current);
    	current = predecessor[current];
    }

    reverse(shortest_route.begin(), shortest_route.end());

    routes[bundle] = shortest_route;
}

Controller::~Controller()
{
	// TODO Auto-generated destructor stub
}

