/*
 * Controller.cc
 *
 *  Created on: Nov 8, 2023
 *      Author: matteo
 */

#include "Controller.h"
using namespace std;
#include <iterator>
#include <iostream>
#include <map>

Controller* Controller::instance = nullptr;

Controller::Controller(){
}

void Controller::setContactPlan(ContactPlan* contactPlan){
	contactplan_ = contactPlan;
}

void Controller::setNodeNum(int nodeNum){
	nodeNum_ = nodeNum;
}

Controller* Controller::getInstance(ContactPlan* contactPlan, int nodeNum){
	if(!instance){
		instance = new Controller();
		instance->contactplan_ = contactPlan;
		instance->nodeNum_ = nodeNum;
	}

	return instance;
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

vector<int> Controller::buildRoute(BundlePkt* bundle, double simTime, string routingType){
	int source = bundle->getSourceEid();

	vector<int> sum_weight(nodeNum_, INT_MAX);
	vector<int> predecessor(nodeNum_, -1); //vector, predecessor[idNode] is the contactID of the contact that lead to that node

	sum_weight[source] = 0; //weight from source node to itself is 0

    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    pq.push({0, source});

    while(!pq.empty()){
    	int u = pq.top().second;
    	pq.pop();

    	if(routingType.find("weight:arrivalTime") != std::string::npos){

			for(pair<int, pair<int, int>> contact : getWeightsAvailableContacts(bundle, simTime)){
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
						predecessor[v] = current->getId(); //contactId that leads to v from u
						pq.push({new_weight, v});
					}
				}
			}
    	}

    	else if(routingType.find("weight:hopsNum") != std::string::npos){
    		for(pair<int, pair<int, int>> contact : getWeightsAvailableContacts(bundle, simTime)){
    	    	Contact * current = contactplan_->getContactById(contact.first);
    	    	if(current->getSourceEid() == u){  //if source node of this contact is u ok, else next
    	   			int v = current->getDestinationEid();

    	    		int new_weight = sum_weight[u] + 1;

    	    		if(new_weight < sum_weight[v]){
    	   				sum_weight[v] = new_weight;
    	   				predecessor[v] = current->getId(); //contactId that leads to v from u
    	   				pq.push({new_weight, v});
    	   			}
        		}
   	    	}
    	}
    	else {
    		cout<<"routingType not valid!"<<endl;
    	}

    }

    //Build the route and store it in map<BundlePkt*, vector<int>> routes;

    vector<int> shortest_route;
    vector<int> route_nodes;
    int destID = bundle->getDestinationEid();
    route_nodes.push_back(destID);
    int currentContact = predecessor[destID];
    while(currentContact != -1) {
    	int first = contactplan_->getContactById(currentContact)->getSourceEid();
    	shortest_route.push_back(currentContact);
    	currentContact = predecessor[first];
    	route_nodes.push_back(first);
    }

    reverse(shortest_route.begin(), shortest_route.end());
    reverse(route_nodes.begin(), route_nodes.end());

    auto it = find_if(routes.begin(), routes.end(),
            [bundle](const std::pair<BundlePkt*, std::vector<int>>& element) {
                return element.first == bundle;
            });

    if (it != routes.end())
    	it->second = shortest_route;
    else
    	routes.push_back(make_pair(bundle, shortest_route));

    for(auto node : nodes){
    	for (auto eid : route_nodes){
    		if (node->eid_==eid){
    		    auto it = find_if((node->routes).begin(), (node->routes).end(),
    		            [bundle](const std::pair<BundlePkt*, std::vector<int>>& element) {
    		                return element.first == bundle;
    		            });

    		    if (it != (node->routes).end())
    		    	it->second = shortest_route;
    		    else
    		    	node->routes.push_back(make_pair(bundle, shortest_route));
    		}
    	}
    }

    return shortest_route;
}

void Controller::addRoutingSDNInstance(RoutingSDN* routingSDNInstance) {
    nodes.push_back(routingSDNInstance);
}

Controller::~Controller()
{
	// TODO Auto-generated destructor stub
}

