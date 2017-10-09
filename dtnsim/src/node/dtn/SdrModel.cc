/*
 * SdrModel.cpp
 *
 *  Created on: Nov 25, 2016
 *      Author: juanfraire
 */

#include <dtn/SdrModel.h>

SdrModel::SdrModel()
{
	bundlesNumber_ = 0;
}

SdrModel::~SdrModel()
{
}

void SdrModel::setEid(int eid)
{
	this->eid_ = eid;
}

void SdrModel::setSize(int size)
{
	this->size_ = size;
}

void SdrModel::setNodesNumber(int nodesNumber)
{
	this->nodesNumber_ = nodesNumber;
}

void SdrModel::setContactPlan(ContactPlan *contactPlan)
{
	this->contactPlan_ = contactPlan;
}

bool SdrModel::enqueueBundleToContact(BundlePkt * bundle, int contactId)
{
	// if there is not enough space in sdr, the bundle is deleted
	// if another behaviour is required, the simpleCustodyModel should be used
	// to avoid bundle deletions
	if(!(this->isSdrFreeSpace(bundle->getByteLength())))
	{
		delete bundle;
		return false;
	}

	// Check is queue exits, if not, create it. Add bundle to queue.
	map<int, list<BundlePkt *> >::iterator it = bundlesQueue_.find(contactId);
	if (it != bundlesQueue_.end())
	{
		it->second.push_back(bundle);
	}
	else
	{
		list<BundlePkt *> q;
		q.push_back(bundle);
		bundlesQueue_[contactId] = q;
	}

	bundlesNumber_++;
	notify();
	return true;
}

bool SdrModel::isBundleForContact(int contactId)
{
	// This functions returns true if there is a queue
	// with bundles for the contactId. If it is empty
	// or non-existent, the function returns false

	map<int, list<BundlePkt *> >::iterator it = bundlesQueue_.find(contactId);

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
	map<int, list<BundlePkt *> >::iterator it = bundlesQueue_.find(contactId);

	// Just check if the function was called incorrectly
	if (it == bundlesQueue_.end())
		if (bundlesQueue_[contactId].empty())
		{
			cout << "***getBundle called from SdrModel but queue empty***" << endl;
			exit(1);
		}

	// Find and return pointer to bundle
	list<BundlePkt *> bundlesToTx = it->second;

	return bundlesToTx.front();
}

void SdrModel::popNextBundleForContact(int contactId)
{
	// Pop the next bundle for this contact
	map<int, list<BundlePkt *> >::iterator it = bundlesQueue_.find(contactId);
	list<BundlePkt *> bundlesToTx = it->second;

	bundlesToTx.pop_front();

	// Update queue after popping the bundle
	if (!bundlesToTx.empty())
		bundlesQueue_[contactId] = bundlesToTx;
	else
		bundlesQueue_.erase(contactId);

	bundlesNumber_--; notify();
}

int SdrModel::getBundlesCountInSdr()
{
	return bundlesNumber_;
//	int bundlesInSdr = 0;
//	for (map<int, list<BundlePkt *> >::iterator it = bundlesQueue_.begin(); it != bundlesQueue_.end(); ++it)
//		bundlesInSdr += (*it).second.size();
//	return bundlesInSdr;
}

int SdrModel::getBundlesCountInLimbo()
{
	return bundlesQueue_[0].size();
}

list<BundlePkt*> * SdrModel::getBundlesInLimbo()
{
	return &bundlesQueue_[0];
}


int SdrModel::getBundlesCountInContact(int cid)
{
	return bundlesQueue_[cid].size();
}

int SdrModel::getBytesStoredInSdr()
{
	int bytesStoredInSdr = 0;
	for (map<int, list<BundlePkt *> >::iterator it = bundlesQueue_.begin(); it != bundlesQueue_.end(); ++it)
	{
		list<BundlePkt *>::iterator ii1 = (*it).second.begin();
		list<BundlePkt *>::iterator ii2 = (*it).second.end();
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

	map<int, list<BundlePkt *> >::iterator it1 = bundlesQueue_.begin();
	map<int, list<BundlePkt *> >::iterator it2 = bundlesQueue_.end();

	for (; it1 != it2; ++it1)
	{
		int contactId = it1->first;

		// if it's not the limbo contact
		if (contactId != 0)
		{
			list<BundlePkt *> bundlesQueue = it1->second;

			Contact *contact = contactPlan_->getContactById(contactId);
			int source = contact->getSourceEid();
			assert(source == this->eid_);

			int destination = contact->getDestinationEid();

			if (eid == destination)
			{
				list<BundlePkt *>::iterator ii1 = bundlesQueue.begin();
				list<BundlePkt *>::iterator ii2 = bundlesQueue.end();
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
	map<int, list<BundlePkt *> >::iterator it1 = bundlesQueue_.begin();
	map<int, list<BundlePkt *> >::iterator it2 = bundlesQueue_.end();
	while (it1 != it2)
	{
		//int bundlesDeleted = 0;

		list<BundlePkt *> bundles = it1->second;

		while (!bundles.empty())
		{
			delete (bundles.back());
			bundles.pop_back();
			//bundlesDeleted++;
		}
		bundlesQueue_.erase(it1++);
	}
	bundlesNumber_ = 0; notify();

}

/**
 * Enqueue bundle to limbo.
 * */
void SdrModel::enqueueBundle(BundlePkt * bundle)
{
	this->enqueueBundleToContact(bundle, 0);
}

/**
 * Delete bundle with bundleId from limbo if it exists.
 * */
void SdrModel::removeBundle(long bundleId)
{
	for(list<BundlePkt *>::iterator it = bundlesQueue_[0].begin(); it != bundlesQueue_[0].end();it++)
		if((*it)->getBundleId() == bundleId)
		{
			delete (*it);
			bundlesQueue_[0].erase(it);
			bundlesNumber_--; notify();
			break;
		}
}

/**
 * Returns bundles stored in limbo.
 */
list<BundlePkt *> SdrModel::getCarryingBundles(){
	return bundlesQueue_[0];
}

/**
* Look in limbo if there is a bundle with bundleId, if exist its returns a pointer to this bundle. Otherwise
* a null pointer is returned.
*/
BundlePkt * SdrModel::getEnqueuedBundle(long bundleId)
{
	for(list<BundlePkt *>::iterator it = bundlesQueue_[0].begin(); it != bundlesQueue_[0].end();it++)
		if((*it)->getBundleId())
			return *it;

	return NULL;
}

// Check if there is free space in sdr for a new packet
bool SdrModel::isSdrFreeSpace(int sizeNewPacket)
{
	bool isSdrFreeSpace = true;
	if(this->size_ == 0)
	{
		return isSdrFreeSpace;
	}

	int bytesStoredInSdr = this->getBytesStoredInSdr();

	if (bytesStoredInSdr + sizeNewPacket <= this->size_)
	{
		isSdrFreeSpace = true;
	}
	else
	{
		isSdrFreeSpace = false;
	}

	return isSdrFreeSpace;
}
