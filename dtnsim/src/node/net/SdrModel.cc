/*
 * SdrModel.cpp
 *
 *  Created on: Nov 25, 2016
 *      Author: juanfraire
 */

#include "SdrModel.h"

SdrModel::SdrModel()
{
}

SdrModel::~SdrModel()
{
	// TODO Auto-generated destructor stub
}

void SdrModel::setStatsHandle(cOutVector * sdrBundlesInSdr, cOutVector * sdrBundleInLimbo)
{
	sdrBundlesInSdr_ = sdrBundlesInSdr;
	sdrBundleInLimbo_ = sdrBundleInLimbo;
	sdrBundleInLimbo = 0;

	lastUdateTime = 0;
	bundlesInSdrPerTime = 0;
}

void SdrModel::setEid(int eid)
{
	this->eid_ = eid;
}

void SdrModel::setNodesNumber(int nodesNumber)
{
	this->nodesNumber_ = nodesNumber;
}

void SdrModel::setContactPlan(ContactPlan *contactPlan)
{
	this->contactPlan_ = contactPlan;
}

void SdrModel::updateStats()
{
//	// Only update limbo if it has changed
//	if (bundlesQueue_[0].size() > sdrBundleInLimbo)
//	{
//		sdrBundleInLimbo_->record(bundlesQueue_[0].size());
//		sdrBundleInLimbo=bundlesQueue_[0].size();
//	}

// Allways update total Sdr storage
	int bundlesInSdr = 0;
	for (map<int, deque<BundlePkt *> >::iterator it = bundlesQueue_.begin(); it != bundlesQueue_.end(); ++it)
		bundlesInSdr += (*it).second.size();
	//sdrBundlesInSdr_->record(bundlesInSdr);

	bundlesInSdrPerTime += bundlesInSdr * (simTime().dbl() - lastUdateTime);
	lastUdateTime = simTime().dbl();

}

void SdrModel::enqueueBundleToContact(BundlePkt * bundle, int contactId)
{
	// Check is queue exits, if not, create it.
	// Add bundle to queue.
	map<int, deque<BundlePkt *> >::iterator it = bundlesQueue_.find(contactId);
	if (it != bundlesQueue_.end())
	{
		it->second.push_front(bundle);
	}
	else
	{
		deque<BundlePkt *> q;
		q.push_front(bundle);
		bundlesQueue_[contactId] = q;
	}

	// Update Sdr stats
	this->updateStats();

//	if (bundle->getId() == 2382 || bundle->getId() == 2035)
//		cout << "Node: " << eid_ << ", bundle inserted to contactId:" << contactId << endl;
}

bool SdrModel::isBundleForContact(int contactId)
{
	// This functions returns true if there is a queue
	// with bundles for the contactId. If it is empy
	// or not-existant, the function returns false

	map<int, deque<BundlePkt *> >::iterator it = bundlesQueue_.find(contactId);

	if (it != bundlesQueue_.end())
	{
		if (!bundlesQueue_[contactId].empty())
			return true;
		else
			return false;
	}
	else
	{
		return false;
	}
}

BundlePkt * SdrModel::getNextBundleForContact(int contactId)
{
	map<int, deque<BundlePkt *> >::iterator it = bundlesQueue_.find(contactId);

	// Just check if the function was called incorrectly
	if (it == bundlesQueue_.end())
		if (bundlesQueue_[contactId].empty())
		{
			cout << "***getBundle called from SdrModel but queue empty***" << endl;
			exit(1);
		}

	// Find and return pointer to bundle
	deque<BundlePkt *> bundlesToTx = it->second;

	return bundlesToTx.back();
}

void SdrModel::popNextBundleForContact(int contactId)
{
	// Pop the next bundle for this contact
	map<int, deque<BundlePkt *> >::iterator it = bundlesQueue_.find(contactId);
	deque<BundlePkt *> bundlesToTx = it->second;

//	if (bundlesToTx.front()->getId() == 2382 || bundlesToTx.front()->getId() == 2035)
//		cout << "Node: " << eid_ << ", bundle removed from contactId:" << contactId << endl;

	bundlesToTx.pop_back();

	// Update queue after popping the bundle
	if (!bundlesToTx.empty())
		bundlesQueue_[contactId] = bundlesToTx;
	else
		bundlesQueue_.erase(contactId);

	// Update Sdr stats
	this->updateStats();
}

void SdrModel::freeSdr(int eid)
{
	sdrBundlesInSdr_->record(bundlesInSdrPerTime);
	sdrBundleInLimbo_->record(bundlesQueue_[0].size());

	// Delete all enqueued bundles
	map<int, deque<BundlePkt *> >::iterator it1 = bundlesQueue_.begin();
	map<int, deque<BundlePkt *> >::iterator it2 = bundlesQueue_.end();
	while (it1 != it2)
	{
		int bundlesDeleted = 0;

		deque<BundlePkt *> bundles = it1->second;

		while (!bundles.empty())
		{
			delete (bundles.back());
			bundles.pop_back();
			bundlesDeleted++;
		}

		//cout << "cId:" << it1->first << "(" << bundlesDeleted << "), ";
		bundlesQueue_.erase(it1++);
	}

	//cout << endl;
}

int SdrModel::getBundlesSizeEnqueuedToNeighbor(int eid)
{
	int size = 0;

	map<int, deque<BundlePkt *> >::iterator it1 = bundlesQueue_.begin();
	map<int, deque<BundlePkt *> >::iterator it2 = bundlesQueue_.end();

	for (; it1 != it2; ++it1)
	{
		int contactId = it1->first;

		// if it's not the limbo contact
		if (contactId != 0)
		{
			deque<BundlePkt *> bundlesQueue = it1->second;

			Contact *contact = contactPlan_->getContactById(contactId);
			int source = contact->getSourceEid();
			assert(source == this->eid_);

			int destination = contact->getDestinationEid();

			if (eid == destination)
			{
				deque<BundlePkt *>::iterator ii1 = bundlesQueue.begin();
				deque<BundlePkt *>::iterator ii2 = bundlesQueue.end();
				for (; ii1 != ii2; ++ii1)
				{
					size += (*ii1)->getByteLength();
				}
			}
		}
	}

	return size;
}

SdrStatus SdrModel::getSdrStatus()
{
	SdrStatus sdrStatus;

	for (int i = 1; i <= nodesNumber_; i++)
	{
		sdrStatus.size[i] = this->getBundlesSizeEnqueuedToNeighbor(i);
	}

	return sdrStatus;
}
