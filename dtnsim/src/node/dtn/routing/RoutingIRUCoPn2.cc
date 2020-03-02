/*
 * RoutingIRUCoPn22.cpp
 *
 *  Created on: February 19, 2020
 *      Author: FRaverta
 */

#include <dtn/routing/RoutingIRUCoPn2.h>

void from_json(const json& j, RoutingDecision& rd){
    j.at("copies").get_to(rd.copies_);
    j.at("route").get_to(rd.contact_ids_);
}

RoutingIRUCoPn2::RoutingIRUCoPn2(int eid, SdrModel * sdr, ContactPlan * contactPlan,  cModule * dtn, int max_copies, double probability_of_failure, int ts_duration, int numOfNodes, string pathPrefix, string pathPosfix)
	: RoutingDeterministic(eid, sdr, contactPlan)
{
	this->dtn_ = dtn;
	this->contact_failure_probability = probability_of_failure;
	this->ts_duration = ts_duration;
	this->max_copies = max_copies;

	//It looks up for all routing files  {pathPrefix}{target}-{copies}{pathPosfix}
	for(int target=1; target <= numOfNodes; target++)
	{
		if (eid_ != target)
		{
			int copies;
			for(copies=1; copies<=max_copies; copies++)
			{
				ostringstream stream;
				stream << pathPrefix << target-1 << "-" << copies << pathPosfix;
				string pathToRouting = stream.str();
				cout<<"RoutingIRUCoPn2::RoutingIRUCoPn2 check if exist file: " << pathToRouting << endl;
				ifstream infile(pathToRouting);
				if (infile.good())
				{
					cout<<pathToRouting<<endl;
					this->bruf_function[to_string(target)][to_string(copies)] = json::parse(infile);
					//cout<<this->bruf_function.dump();
					infile.close();
				}
				else{
					cout<<"RoutingIRUCoPn2::RoutingIRUCoPn2 there is not traffic destined to " << target << endl;
					break;
					//exit(1);
				}
			}
			//Check if exists function for traffic destined to target. If it exists check that there is a function for all possible copies
			if (this->bruf_function.find(to_string(target)) != this->bruf_function.end())
			{
				for(copies=1; copies<=max_copies; copies++)
				{
					if(this->bruf_function[to_string(target)].find(to_string(copies)) == this->bruf_function[to_string(target)].end())
					{
						cout<<"[Error] in RoutingIRUCoPn2::RoutingIRUCoPn2() : function destined to "<< target<<" with "<<copies << " copies should exist"<<endl;
						exit(1);
					}
				}
			}
		}
	}

}

RoutingIRUCoPn2::~RoutingIRUCoPn2()
{
	cout<<"Node "<<eid_<<" Calling RoutingIRUCoPn2::~RoutingIRUCoPn2()"<<endl;
	if (bag.size() != 0)
		cout<<"RoutingIRUCoPn2::~RoutingIRUCoPn2: Node "<<eid_<<" has "<<bag.size()<<" bundles in its bag but It should not have anyone"<<endl;

}

void RoutingIRUCoPn2::msgToOtherArrive(BundlePkt * bundle, double simTime)
{
	// Check if there is a function loaded for routing this bundle. Otherwise, it will end with an error.
	if (this->bruf_function.find(to_string(bundle->getDestinationEid())) == this->bruf_function.end())
	{
		cout<<"[Error] in RoutingIRUCoPn2::msgToOtherArrive there is not function loaded for routing traffic destined to  " << bundle->getDestinationEid() <<endl;
		exit(1);
	}

	//Check if it is a new message which was created in this node
	if (bundle->getHopCount() == 0 and bundle->getSourceEid() == this->eid_)
	{
		cout<<"RoutingIRUCoPn2::msgToOtherArrive Node " << eid_ << ": Bundle " << bundle << "( "<< max_copies << " copies) was generated."<< endl;
		bundle->setBundlesCopies(max_copies); // Set the number of copies to current bundle
		routeAndQueueBundle(bundle, simTime);
	}
	else if (!this->isDeliveredBundle(bundle->getBundleId()))
	{
		//The bundle was transmitted from other node. So, it checks if the bundle has to be sent now or if it has to be stored.
		CgrRoute * route = &(bundle->getCgrRoute());
		if (route->terminusNode == eid_)
		{
			// Bundle has to be stored
			simTime =  ts_duration * (int(simTime/this->ts_duration) + 1); // To make a decision for this bundle but in the next time stamp
			routeAndQueueBundle(bundle, simTime);
		}
		else
		{
			// Send bundle throw the next hop in its route
			route->nextHop++; //Increment route next hop. Notice that this field is used as an index to enter in route->hops
			Contact * next_contact = route->hops.at(route->nextHop);
			if (next_contact->getSourceEid() != eid_) // Sanity check
				reportErrorAndExit("RoutingIRUCoPn2::msgToOtherArrive", "Error next hop's sender it not the current bundle carrier");
			if (find(finishedContacts.begin(), finishedContacts.end(), next_contact->getId()) != finishedContacts.end())
				reportErrorAndExit("RoutingIRUCoPn2::msgToOtherArrive", "Error bundle must be enqueue in a contact but it has already ended");
			//if (find(currentContacts.begin(), currentContacts.end(), next_contact->getId()) == currentContacts.end()) // Sanity check
				//This wouldn't be an error if a contact doesn't start at the beggining of each time stamp.
				//In that case, we should enqueue the bundle and add the routing decision to be made when the contact starts.
				//However, for now we will assume a tiny simplification and it will be considered as an Error.
				//reportErrorAndExit("RoutingIRUCoPn2::msgToOtherArrive", "Error bundle must be enqueue in a contact but it wasn't started yet");

			bundle->setNextHopEid(next_contact->getDestinationEid());
			sdr_->enqueueBundleToContact(bundle, next_contact->getId());
		}
	}
	else
		delete bundle;
}


bool RoutingIRUCoPn2::msgToMeArrive(BundlePkt * bundle)
{
	if (!isDeliveredBundle(bundle->getBundleId()))
	{
		deliveredBundles_.push_back(bundle->getBundleId());
		return true;
	}
	return false;
}

void RoutingIRUCoPn2::contactStart(Contact *c)
{
	cout<<"RoutingIRUCoPn2::contactStart "<<c->getId()<<endl;
	currentContacts.push_back(c->getId());

	// Enqueue bundles to be sent in this contact
	queueBundles();
}


void RoutingIRUCoPn2::contactEnd(Contact *c)
{
	cout<<"RoutingIRUCoPn2::contactEnd: "<<c->getId()<<endl;
	currentContacts.remove(c->getId());
	finishedContacts.push_back(c->getId());
	while (sdr_->isBundleForContact(c->getId()))
	{
		BundlePkt* bundle = sdr_->getNextBundleForContact(c->getId());
		sdr_->popNextBundleForContact(c->getId());

		//emit(dtnBundleReRouted, true);
		routeAndQueueBundle(bundle, simTime().dbl());
	}

	//Re-route those bundles that were waiting for this contact if contact has not happened
	list<BundlePkt *> bundleToReRoute;
	for(map<long, list<RoutingDecision>>::iterator it = bag.begin(); it != bag.end();)
	{
		list<int>::iterator ll;
		bool eraseEntry = false;
		for(list<RoutingDecision>::iterator routingDecisionsIt = it->second.begin(); routingDecisionsIt!=it->second.end(); routingDecisionsIt++)
		{
			if (routingDecisionsIt->contact_ids_.at(0) == c->getId())
			{
				// We delete the map entry because a complete new routing decision will be make for all these remaining copies
				// And also we delete it from sdr_: it would be re-enqueued when calling routeAndQueueBundle. It may be done in other way,
				// this though inefficient is a easy way to solve the problem.
				bundleToReRoute.push_back(sdr_->getEnqueuedBundle(it->first)->dup());
				eraseEntry = true;
				break;
			}
		}
		if (eraseEntry){
			sdr_->removeBundle(it->first);
			bag.erase(it++);
		}
		else
			it++;
	}
	for(list<BundlePkt *>::iterator it=bundleToReRoute.begin(); it != bundleToReRoute.end(); it++)
		routeAndQueueBundle(*it, c->getEnd());
}

void RoutingIRUCoPn2::routeAndQueueBundle(BundlePkt * bundle, double simTime)
{
	map<long, list<RoutingDecision>>::iterator it = this->bag.find(bundle->getBundleId());
	BundlePkt * bb;
	if (it != this->bag.end())
	{
		bb = sdr_->getEnqueuedBundle(it->first);
		bb->setBundlesCopies(bb->getBundlesCopies() + bundle->getBundlesCopies());
		delete bundle;
	}
	else{
		if (sdr_->getEnqueuedBundle(bundle->getBundleId()) != NULL)
		{
			cout<<"Error RoutingIRUCoPn2::routeAndQueueBundle: Node "<< eid_ <<" tries to enqueue a bundle that have been already enqueued"<<endl;
			exit(1);
		}
		sdr_->enqueueBundle(bundle);
		this->bag.insert(pair<long, list<RoutingDecision>>(bundle->getBundleId(), list<RoutingDecision>()));
		bb = bundle;
		it = this->bag.find(bundle->getBundleId());
	}
	int source = bb->getSourceEid();
	int target = bb->getDestinationEid();
	int copies = bb->getBundlesCopies();
	int ts = simTime / this->ts_duration;

	string key = to_string(this->eid_) + ":" + to_string(bb->getBundlesCopies());

	list<RoutingDecision> routing_decisions;
	while(routing_decisions.size()==0 && this->bruf_function[to_string(target)][to_string(copies)].find(to_string(ts)) != this->bruf_function[to_string(target)][to_string(copies)].end())
	{
		cout<<"RoutingIRUCoPn2::routeAndQueueBundl searching "<<source <<" - "<<target<<" - "<<copies<<" - "<<ts<<" - "<<key<<endl;
		if(this->bruf_function[to_string(target)][to_string(copies)][to_string(ts)].value(key, list<RoutingDecision>()).size() > 0)
			routing_decisions = this->bruf_function[to_string(target)][to_string(copies)][to_string(ts)][key].get<list<RoutingDecision>>(); //La clave podria no estar y puedo tener que buscarla 1 ts mas adelante
		ts++;
	}

	if (routing_decisions.size() == 0)
	{
		removeBundleFromBag(bb->getBundleId()); // delete stored bundle
		cout<<"RoutingIRUCoPn2::routeAndQueueBundle bundle source: "<<source<<" - target: "<< target <<" - copies: "<< copies << " has been dropped at Node "<< eid_ <<" - Ts: "<< int( simTime / this->ts_duration)<<" = "<<simTime<<"/"<<ts_duration <<endl;
	}else{
		it->second = routing_decisions;
		for(list<RoutingDecision>::iterator routingDecisionsIt = it->second.begin(); routingDecisionsIt != it->second.end(); routingDecisionsIt++)
		{
			cout<<"RoutingIRUCoPn2::routeAndQueueBundle bundle source: "<<source<<" target: "<< target << " copies: "<< copies << " has been routed using contacts: ";
			vector<int>::iterator ll = routingDecisionsIt->contact_ids_.begin();
			bool invalidRouting = this->contactPlan_->getContactById(*ll)->getSourceEid() != eid_;
			bool finishedContact = find(finishedContacts.begin(), finishedContacts.end(), *ll) != finishedContacts.end();
			for(;ll != routingDecisionsIt->contact_ids_.end(); ++ll)
				cout<<*ll<<" ";

			cout<<endl;
			if (invalidRouting)
			{
				cout<<"RoutingIRUCoPn2::routeAndQueueBundle: Node "<< eid_ <<" routed a bundle through a contact for which it isn't is source "<<endl;
				exit(1);
			}
			if (finishedContact)
			{
				cout<<"RoutingIRUCoPn2::routeAndQueueBundle: Node "<< eid_ <<" routed a bundle through a contact that had finished."<<endl;
				exit(1);
			}
		}
	}
	queueBundles();
}

void RoutingIRUCoPn2::queueBundles()
{
	for(list<int>::iterator contactIt = currentContacts.begin(); contactIt != currentContacts.end(); contactIt++){
		int contactId = *contactIt;
		for(map<long, list<RoutingDecision>>::iterator it = bag.begin(); it != bag.end();)
		{
			bool eraseEntry = false;
			// To be fair with other algorithms like S&W, we are not sending bundles to its final destination
			// if they have been already delivered.
			RoutingIRUCoPn2 * other = check_and_cast<RoutingIRUCoPn2 *>(check_and_cast<Dtn *>(
																		dtn_->getParentModule()->getParentModule()->getSubmodule("node", contactPlan_->getContactById(contactId)->getDestinationEid())
																		->getSubmodule("dtn"))->getRouting());
			if (!other->isDeliveredBundle(it->first)){
				for(list<RoutingDecision>::iterator routingDecisionsIt = it->second.begin(); routingDecisionsIt!=it->second.end();)
				{
					if (routingDecisionsIt->contact_ids_.at(0) == contactId)
					{
							//Enqueue bundle to be sent
							BundlePkt * bundle =  sdr_->getEnqueuedBundle(it->first);
							routeBundle(bundle, routingDecisionsIt->copies_, routingDecisionsIt->contact_ids_);
							it->second.erase(routingDecisionsIt++);
							if (bundle->getBundlesCopies() == 0)
								eraseEntry = true;
					}
					else
						routingDecisionsIt++;
				}
			}
			else
			{
				eraseEntry = true;
				this->deliveredBundles_.push_back(it->first);
			}
			if(eraseEntry)
			{
				// Remove bundle
				sdr_->removeBundle(it->first);
				bag.erase(it++);
			}else
				it++;
		}
	}
}

void RoutingIRUCoPn2::removeBundleFromBag(long bundleId){
	map<long, list<RoutingDecision>>::iterator it = bag.find(bundleId);
	if (it != bag.end())
	{
		sdr_->removeBundle(bundleId);
		bag.erase(it);
	}
}

/***
 * Check if bundle successful forwarded is a delivered to its destination,
 * if that happens, it deletes this from bundles queue since there is no sense
 * in carrying a delivered to destination bundle.
 */
void RoutingIRUCoPn2::successfulBundleForwarded(long bundleId, Contact * contact,  bool sentToDestination)
{
	//Check if bundle was deliver to its final destination.
	//Note it is necessary to check if bundle is enqueue since bundle could be sent
	//to its final destination and deleted from queue while it was sending to another relay node.
	if (sentToDestination){
		deliveredBundles_.push_back(bundleId);
		removeBundleFromBag(bundleId);
	}
}

/**
 * Check if bundleId has been delivered to App.
 */
bool RoutingIRUCoPn2::isDeliveredBundle(long bundleId)
{
	cout<<"Node "<<eid_<<" isDeliveredBundle has size "<< deliveredBundles_.size()<<endl;
	for(list<long>::iterator it = deliveredBundles_.begin(); it != deliveredBundles_.end(); ++it)
		if( (*it) == bundleId )
			return true;
	return false;
}

void RoutingIRUCoPn2::reportErrorAndExit(string method, string msg)
{
	cout<< method <<" - node " << eid_ << ": " << msg <<endl;
	exit(1);
}

/**
 * Send bundle throw the selected route
 */
void RoutingIRUCoPn2::routeBundle(BundlePkt * bundle, int copies, vector<int> route)
{
		cout<<"RoutingIRUCoPn2::routeBundle " << bundle<<endl;
		Contact * first_hop_contact = contactPlan_->getContactById(route.at(0));
		Contact * last_hop_contact = contactPlan_->getContactById(route.back());

		//generate the route
		CgrRoute cgrRoute;
		cgrRoute.nextHop = 0;
		cgrRoute.terminusNode = last_hop_contact->getDestinationEid();
		for(auto contact_it = route.begin(); contact_it != route.end(); ++contact_it)
			cgrRoute.hops.push_back(contactPlan_->getContactById(*contact_it));


		// Sanity check, node must have enough copies of bundle to be sent
		if ( copies <= bundle->getBundlesCopies())
		{
			bundle->setBundlesCopies(bundle->getBundlesCopies() - copies);
			BundlePkt * bundleCopy = bundle->dup();
			bundleCopy->setBundlesCopies(copies);
			bundleCopy->setNextHopEid(first_hop_contact->getDestinationEid());
			bundleCopy->setCgrRoute(cgrRoute);
			if (first_hop_contact->getSourceEid() != eid_)
			{
				ostringstream stream; stream << "Error routing bundle " << bundle << " . First hop contact sender is not who is performing this routing decision.";
				reportErrorAndExit("RoutingIRUCoPn2::routeBundle", stream.str());
			}


			sdr_->enqueueBundleToContact(bundleCopy, first_hop_contact->getId());
		}
		else
			reportErrorAndExit("RoutingIRUCoPn2::routeBundle", "bundles has least copies than the required");
}
