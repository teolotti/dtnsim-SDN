/*
 * RoutingIRUCoPn.cpp
 *
 *  Created on: February 19, 2020
 *      Author: FRaverta
 */

#include <dtn/routing/RoutingIRUCoPn.h>

RoutingIRUCoPn::RoutingIRUCoPn(int eid, SdrModel * sdr, ContactPlan * contactPlan,  cModule * dtn, int max_copies, double probability_of_failure, int ts_duration, int numOfNodes, string pathPrefix, string pathPosfix)
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
				cout<<"RoutingIRUCoPn::RoutingIRUCoPn check if exist file: " << pathToRouting << endl;
				ifstream infile(pathToRouting);
				if (infile.good())
				{
					cout<<pathToRouting<<endl;
					this->bruf_function[to_string(target)][to_string(copies)] = json::parse(infile);
					//cout<<this->bruf_function.dump();
					infile.close();
				}
				else{
					cout<<"RoutingIRUCoPn::RoutingIRUCoPn there is not traffic destined to " << target << endl;
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
						cout<<"[Error] in RoutingIRUCoPn::RoutingIRUCoPn() : function destined to "<< target<<" with "<<copies << " copies should exist"<<endl;
						exit(1);
					}
				}
			}
		}
	}

}

RoutingIRUCoPn::~RoutingIRUCoPn()
{
	cout<<"Node "<<eid_<<" Calling RoutingIRUCoPn::~RoutingIRUCoPn()"<<endl;
	if (bag.size() != 0)
		cout<<"RoutingIRUCoPn::~RoutingIRUCoPn: Node "<<eid_<<" has "<<bag.size()<<" bundles in its bag but It should not have anyone"<<endl;

}

void RoutingIRUCoPn::msgToOtherArrive(BundlePkt * bundle, double simTime)
{
	// Check if there is a function loaded for routing this bundle. Otherwise, it will end with an error.
	if (this->bruf_function.find(to_string(bundle->getDestinationEid())) == this->bruf_function.end())
	{
		cout<<"[Error] in RoutingIRUCoPn::msgToOtherArrive there is not function loaded for routing traffic destined to  " << bundle->getDestinationEid() <<endl;
		exit(1);
	}

	//Check if it is a new message which was created in this node
	if (bundle->getHopCount() == 0 and bundle->getSourceEid() == this->eid_)
	{
		cout<<"RoutingIRUCoPn::msgToOtherArrive Node " << eid_ << ": Bundle " << bundle << "( "<< max_copies << " copies) was generated."<< endl;
		bundle->setBundlesCopies(max_copies); // Set the number of copies to current bundle
	}
	routeAndQueueBundle(bundle, simTime);
}

bool RoutingIRUCoPn::msgToMeArrive(BundlePkt * bundle)
{
	if (!isDeliveredBundle(bundle->getBundleId()))
	{
		deliveredBundles_.push_back(bundle->getBundleId());
		return true;
	}
	return false;
}

void RoutingIRUCoPn::contactStart(Contact *c)
{
	cout<<"RoutingIRUCoPn::contactStart "<<c->getId()<<endl;
	currentContacts.push_back(c->getId());

	// Enqueue bundles to be sent in this contact
	queueBundles();
}


void RoutingIRUCoPn::contactEnd(Contact *c)
{
	cout<<"RoutingIRUCoPn::contactEnd: "<<c->getId()<<endl;
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
	for(map<long, list<int>>::iterator it = bag.begin(); it != bag.end();)
	{
		list<int>::iterator ll;
		bool deleted = false;
		for(ll = it->second.begin(); ll !=  it->second.end(); ll++)
		{
			if ( *ll == c->getId() )
			{
				// We delete the map entry because a complete new routing decision will be make for all these remaining copies
				// And also we delete it from sdr_: it would be re-enqueued when calling routeAndQueueBundle. It may be done in other way,
				// this though inefficient is a easy way to solve the problem.
				bundleToReRoute.push_back(sdr_->getEnqueuedBundle(it->first)->dup());
				sdr_->removeBundle(it->first);
				bag.erase(it++);
				deleted = true;
				break;
			}
		}
		if(!deleted)
			it++;
	}
	for(list<BundlePkt *>::iterator it=bundleToReRoute.begin(); it != bundleToReRoute.end(); it++)
		routeAndQueueBundle(*it, c->getEnd());
}

void RoutingIRUCoPn::routeAndQueueBundle(BundlePkt * bundle, double simTime)
{
	map<long, list<int>>::iterator it = this->bag.find(bundle->getBundleId());
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
			cout<<"Error RoutingIRUCoPn::routeAndQueueBundle: Node "<< eid_ <<" tries to enqueue a bundle that have been already enqueued"<<endl;
			exit(1);
		}
		sdr_->enqueueBundle(bundle);
		this->bag.insert(pair<long, list<int>>(bundle->getBundleId(), list<int>()));
		bb = bundle;
		it = this->bag.find(bundle->getBundleId());
	}
	int source = bb->getSourceEid();
	int target = bb->getDestinationEid();
	int copies = bb->getBundlesCopies();
	int ts = simTime / this->ts_duration;

	string key = to_string(this->eid_) + ":" + to_string(bb->getBundlesCopies());

	list<int> routing_decisions;
	while(routing_decisions.size() == 0 && this->bruf_function[to_string(target)][to_string(copies)].find(to_string(ts)) != this->bruf_function[to_string(target)][to_string(copies)].end())
	{
		cout<<"RoutingIRUCoPn::routeAndQueueBundl searching "<<source <<" - "<<target<<" - "<<copies<<" - "<<ts<<" - "<<key<<endl;
		routing_decisions = this->bruf_function[to_string(target)][to_string(copies)][to_string(ts)].value(key, list<int>()); //La clave podria no estar y puedo tener que buscarla 1 ts mas adelante
		ts++;
	}

	if (routing_decisions.size() == 0)
	{
		removeBundleFromBag(bb->getBundleId()); // delete stored bundle
		cout<<"RoutingIRUCoPn::routeAndQueueBundle bundle source: "<<source<<" - target: "<< target <<" - copies: "<< copies << " has been dropped at Node "<< eid_ <<" - Ts: "<< int( simTime / this->ts_duration)<<" = "<<simTime<<"/"<<ts_duration <<endl;
	}else{
		it->second = routing_decisions;
		cout<<"RoutingIRUCoPn::routeAndQueueBundle bundle source: "<<source<<" target: "<< target << " copies: "<< copies << " has been routed using contacts: ";
		bool invalidRouting = false;
		bool finishedContact = false;
		for(list<int>::iterator ll = routing_decisions.begin(); ll != routing_decisions.end(); ++ll)
		{	cout<<*ll<<" ";
			if (this->contactPlan_->getContactById(*ll)->getSourceEid() != eid_)
				invalidRouting = true;
			if (find(finishedContacts.begin(), finishedContacts.end(), *ll) != finishedContacts.end())
				finishedContact = true;
		}
		cout<<endl;
		if (invalidRouting)
		{
			cout<<"RoutingIRUCoPn::routeAndQueueBundle: Node "<< eid_ <<" routed a bundle through a contact for which it isn't is source "<<endl;
			exit(1);
		}
		if (finishedContact)
		{
			cout<<"RoutingIRUCoPn::routeAndQueueBundle: Node "<< eid_ <<" routed a bundle through a contact that had finished."<<endl;
			exit(1);
		}
	}
	queueBundles();
}

void RoutingIRUCoPn::queueBundles()
{
	for(list<int>::iterator contactIt = currentContacts.begin(); contactIt != currentContacts.end(); contactIt++){
		int contactId = *contactIt;
		for(map<long, list<int>>::iterator it = bag.begin(); it != bag.end();)
		{
			int cant = 0;
			for(list<int>::iterator ll = it->second.begin(); ll !=  it->second.end();)
			{
				if ( *ll == contactId )
				{
					cant++;
					it->second.erase(ll++);
				}else
					++ll;

			}
			if (cant > 0)
			{
				// To be fair with other algorithms like S&W, we are not sending bundles to its final destination
				// if they have been already delivered.
				RoutingIRUCoPn * other = check_and_cast<RoutingIRUCoPn *>(check_and_cast<Dtn *>(
																			dtn_->getParentModule()->getParentModule()->getSubmodule("node", contactPlan_->getContactById(contactId)->getDestinationEid())
																			->getSubmodule("dtn"))->getRouting());

				if (other->isDeliveredBundle(it->first)){
					this->deliveredBundles_.push_back(it->first);
					// Remove bundle
					sdr_->removeBundle(it->first);
					bag.erase(it++);
				}else{
					//Enqueue bundle to be sent
					BundlePkt * bundle =  sdr_->getEnqueuedBundle(it->first);
					if (bundle->getBundlesCopies() < cant)
					{
						cout<<"Error in RoutingIRUCoPn::queueBundles: Node "<< eid_ <<" tries to route more copies than the ones it was carrying."<<endl;
						exit(1);
					}
					bundle->setBundlesCopies(bundle->getBundlesCopies() - cant);
					BundlePkt * bundleCopy = bundle->dup();
					bundleCopy->setNextHopEid(this->contactPlan_->getContactById(contactId)->getDestinationEid());
					bundleCopy->setBundlesCopies(cant);
					sdr_->enqueueBundleToContact(bundleCopy, contactId);
					if (bundle->getBundlesCopies() == 0)
					{
						// Remove bundle
						sdr_->removeBundle(it->first);
						bag.erase(it++);
					}
					else
						++it;
				}
			}else
				++it;
		}
	}
}

void RoutingIRUCoPn::removeBundleFromBag(long bundleId){
	map<long, list<int>>::iterator it = bag.find(bundleId);
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
void RoutingIRUCoPn::successfulBundleForwarded(long bundleId, Contact * contact,  bool sentToDestination)
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
bool RoutingIRUCoPn::isDeliveredBundle(long bundleId)
{
	cout<<"Node "<<eid_<<" isDeliveredBundle has size "<< deliveredBundles_.size()<<endl;
	for(list<long>::iterator it = deliveredBundles_.begin(); it != deliveredBundles_.end(); ++it)
		if( (*it) == bundleId )
			return true;
	return false;
}
