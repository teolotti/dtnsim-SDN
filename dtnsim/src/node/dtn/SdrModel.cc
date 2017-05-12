/*
 * SdrModel.cpp
 *
 *  Created on: Nov 25, 2016
 *      Author: juanfraire
 */

#include <dtn/SdrModel.h>

SdrModel::SdrModel()
{
}

SdrModel::~SdrModel()
{
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

void SdrModel::enqueueBundleToContact(BundlePkt * bundle, int contactId)
{
	// Check is queue exits, if not, create it. Add bundle to queue.
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

	bundlesToTx.pop_back();

	// Update queue after popping the bundle
	if (!bundlesToTx.empty())
		bundlesQueue_[contactId] = bundlesToTx;
	else
		bundlesQueue_.erase(contactId);
}

int SdrModel::getBundlesStoredInSdr()
{
	int bundlesInSdr = 0;
	for (map<int, deque<BundlePkt *> >::iterator it = bundlesQueue_.begin(); it != bundlesQueue_.end(); ++it)
		bundlesInSdr += (*it).second.size();
	return bundlesInSdr;
}

int SdrModel::getBundlesStoredInLimbo()
{
	return bundlesQueue_[0].size();
}

int SdrModel::getBytesStoredInSdr()
{
	int bytesStoredInSdr = 0;
	for (map<int, deque<BundlePkt *> >::iterator it = bundlesQueue_.begin(); it != bundlesQueue_.end(); ++it)
	{
		deque<BundlePkt *>::iterator ii1 = (*it).second.begin();
		deque<BundlePkt *>::iterator ii2 = (*it).second.end();
		for (; ii1 != ii2; ++ii1)
		{
			bytesStoredInSdr += (*ii1)->getByteLength();
		}
	}
	return bytesStoredInSdr;
}

int SdrModel::getBytesStoredToNeighbor(int eid)
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
		sdrStatus.size[i] = this->getBytesStoredToNeighbor(i);
	}

	return sdrStatus;
}

void SdrModel::freeSdr(int eid)
{
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
		bundlesQueue_.erase(it1++);
	}
}
