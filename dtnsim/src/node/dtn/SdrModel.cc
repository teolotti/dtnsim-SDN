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
	bytesStored_ = 0;
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
	// if another behavior is required, the simpleCustodyModel should be used
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
	bytesStored_ += bundle->getByteLength();
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

	int size = bundlesToTx.front()->getByteLength();
	bundlesToTx.pop_front();

	// Update queue after popping the bundle
	if (!bundlesToTx.empty())
		bundlesQueue_[contactId] = bundlesToTx;
	else
		bundlesQueue_.erase(contactId);

	bundlesNumber_--; bytesStored_ -= size; notify();
}

int SdrModel::getBundlesCountInSdr()
{
	return bundlesNumber_;
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
	return bytesStored_;
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
		list<BundlePkt *> bundles = it1->second;

		while (!bundles.empty())
		{
			delete (bundles.back());
			bundles.pop_back();
		}
		bundlesQueue_.erase(it1++);
	}

	//delete all messages in carriedBundles
	while(!carriedBundles_.empty())
	{
		delete (carriedBundles_.back());
		carriedBundles_.pop_back();
	}

	bundlesNumber_ = 0; bytesStored_ = 0; notify();

}

/**
 * Enqueue bundle to carriedBundles_.
 * */
bool SdrModel::enqueueBundle(BundlePkt * bundle)
{
	// if there is not enough space in sdr, the bundle is deleted
	// if another behaviour is required, the simpleCustodyModel should be used
	// to avoid bundle deletions
	if(!(this->isSdrFreeSpace(bundle->getByteLength())))
	{
		cout<<"SDRModel::enqueuBundle(BundlePkt * bundle): Bundle exceed sdr capacity so it was not enqueue."<<endl;
		delete bundle;
		return false;
	}

	carriedBundles_.push_back(bundle);
	bundlesNumber_++;
	bytesStored_ += bundle->getByteLength();
	notify();
	return true;
}

/**
 * Delete bundle with bundleId from carriedBundles_ if it exists.
 * */
void SdrModel::removeBundle(long bundleId)
{
	for(list<BundlePkt *>::iterator it = carriedBundles_.begin(); it != carriedBundles_.end();it++)
		if((*it)->getBundleId() == bundleId)
		{
			int size = (*it)->getByteLength();
			delete (*it);
			carriedBundles_.erase(it);
			bundlesNumber_--; bytesStored_ -= size; notify();
			break;
		}
}

/**
 * Returns bundles which current node is carrying.
 */
list<BundlePkt *> SdrModel::getCarryingBundles(){
	return carriedBundles_;
}

/**
* Look in carriedBundles_ if there is a bundle with bundleId, if exist its returns a pointer to this bundle. Otherwise
* a null pointer is returned.
*/
BundlePkt * SdrModel::getEnqueuedBundle(long bundleId)
{
	for(list<BundlePkt *>::iterator it = carriedBundles_.begin(); it != carriedBundles_.end();it++)
		if((*it)->getBundleId())
			return *it;

	return NULL;
}

// Check if there is free space in sdr for a new packet
bool SdrModel::isSdrFreeSpace(int sizeNewPacket)
{
	if(this->size_ == 0)
		return true;
	else
		return (bytesStored_ + sizeNewPacket <= this->size_)? true : false;
}
