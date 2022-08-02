#include "src/node/dtn/SdrModel.h"

SdrModel::SdrModel()
{
	bundlesNumber_ = 0;
	bytesStored_ = 0;
}

SdrModel::~SdrModel()
{
}

/////////////////////////////////////
// Initialization and configuration
//////////////////////////////////////

void SdrModel::setEid(int eid)
{
	this->eid_ = eid;
}

void SdrModel::setNodesNumber(int nodesNumber)
{
	this->nodesNumber_ = nodesNumber;
}

void SdrModel::setSize(int size)
{
	this->size_ = size;
}

void SdrModel::setContactPlan(ContactPlan *contactPlan)
{
	this->contactPlan_ = contactPlan;
}

void SdrModel::freeSdr(int eid)
{
	// Delete all enqueued bundles
	map<int, list<BundlePkt *> >::iterator it1 = perContactBundleQueue_.begin();
	map<int, list<BundlePkt *> >::iterator it2 = perContactBundleQueue_.end();
	while (it1 != it2)
	{
		list<BundlePkt *> bundles = it1->second;

		while (!bundles.empty())
		{
			delete (bundles.back());
			bundles.pop_back();
		}
		perContactBundleQueue_.erase(it1++);
	}

	//delete all messages in carriedBundles
	while (!genericBundleQueue_.empty())
	{
		delete (genericBundleQueue_.back());
		genericBundleQueue_.pop_back();
	}

	//delete all messages in transmittedBundlesInCustody
		while (!transmittedBundlesInCustody_.empty())
		{
			delete (transmittedBundlesInCustody_.back());
			transmittedBundlesInCustody_.pop_back();
		}

	bundlesNumber_ = 0;
	bytesStored_ = 0;
	notify();
}


/////////////////////////////////////
// Get information
//////////////////////////////////////

int SdrModel::getBundlesCountInSdr()
{
	return bundlesNumber_;
}

int SdrModel::getBundlesCountInLimbo()
{
	return perContactBundleQueue_[0].size();
}

list<BundlePkt*> * SdrModel::getBundlesInLimbo()
{
	return &perContactBundleQueue_[0];
}

int SdrModel::getBundlesCountInContact(int cid)
{
	return perContactBundleQueue_[cid].size();
}

int SdrModel::getBytesStoredInSdr()
{
	return bytesStored_;
}

int SdrModel::getBytesStoredToNeighbor(int eid)
{
	int size = 0;

	map<int, list<BundlePkt *> >::iterator it1 = perContactBundleQueue_.begin();
	map<int, list<BundlePkt *> >::iterator it2 = perContactBundleQueue_.end();

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

bool SdrModel::isSdrFreeSpace(int sizeNewPacket)
{
	if (this->size_ == 0)
		return true;
	else
		return (bytesStored_ + sizeNewPacket <= this->size_) ? true : false;
}


/////////////////////////////////////
// Enqueue and dequeue from perContactBundleQueue_
//////////////////////////////////////

bool SdrModel::enqueueBundleToContact(BundlePkt * bundle, int contactId)
{
	// if there is not enough space in sdr, the bundle is deleted
	// if another behavior is required, the simpleCustodyModel should be used
	// to avoid bundle deletions
	if (!(this->isSdrFreeSpace(bundle->getByteLength())))
	{
		delete bundle;
		return false;
	}

	// Check is queue exits, if not, create it. Add bundle to queue.
	map<int, list<BundlePkt *> >::iterator it = perContactBundleQueue_.find(contactId);
	if (it != perContactBundleQueue_.end())
	{
		// if custody report, enqueue it at the front so it is prioritized
		// over data bundles already in the queue
		if(bundle->getBundleIsCustodyReport())
			it->second.push_front(bundle);
		else
			it->second.push_back(bundle);
	}
	else
	{
		list<BundlePkt *> q;
		q.push_back(bundle);
		perContactBundleQueue_[contactId] = q;
	}

	bundle->setStored(true);
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

	map<int, list<BundlePkt *> >::iterator it = perContactBundleQueue_.find(contactId);

	if (it != perContactBundleQueue_.end())
	{
		if (!perContactBundleQueue_[contactId].empty())
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
	map<int, list<BundlePkt *> >::iterator it = perContactBundleQueue_.find(contactId);

	// Just check if the function was called incorrectly
	if (it == perContactBundleQueue_.end())
		if (perContactBundleQueue_[contactId].empty())
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
	map<int, list<BundlePkt *> >::iterator it = perContactBundleQueue_.find(contactId);
	list<BundlePkt *> bundlesToTx = it->second;

	BundlePkt *bundle = bundlesToTx.front();
	int size = bundle->getByteLength();
	bundlesToTx.pop_front();

	// Update queue after popping the bundle
	if (!bundlesToTx.empty())
		perContactBundleQueue_[contactId] = bundlesToTx;
	else
		perContactBundleQueue_.erase(contactId);

	if (bundle->getStored()) {
		bundle->setStored(false);
		bundlesNumber_--;
		bytesStored_ -= size;
	}
	notify();
}

/////////////////////////////////////
// Enqueue and dequeue from perNodeBundleQueue_
//////////////////////////////////////

bool SdrModel::enqueueBundleToNode(BundlePkt * bundle, int nodeId)
{
	// if there is not enough space in sdr, the bundle is deleted
	// if another behavior is required, the simpleCustodyModel should be used
	// to avoid bundle deletions
	if (!(this->isSdrFreeSpace(bundle->getByteLength())))
	{
		delete bundle;
		return false;
	}

	// Check is queue exits, if not, create it. Add bundle to queue.
	map<int, list<BundlePkt *> >::iterator it = perNodeBundleQueue_.find(nodeId);
	if (it != perNodeBundleQueue_.end())
	{
		// if custody report, enqueue it at the front so it is prioritized
		// over data bundles already in the queue
		if(bundle->getBundleIsCustodyReport())
			it->second.push_front(bundle);
		else
			it->second.push_back(bundle);
	}
	else
	{
		list<BundlePkt *> q;
		q.push_back(bundle);
		perNodeBundleQueue_[nodeId] = q;
	}

	if (!bundle->getStored()) {
		bundle->setStored(true);
		bundlesNumber_++;
		bytesStored_ += bundle->getByteLength();
	}
	notify();
	return true;
}

// put b in the queue for c
bool SdrModel::transferToContact(Contact * c, BundlePkt * b)
{
	int cid = c->getId();

	// check if a queue already exists for this contact
	map<int, list<BundlePkt *> >::iterator cbq = perContactBundleQueue_.find(cid);
	if (cbq != perContactBundleQueue_.end())
	{
		// a bundle queue already exists, so transfer the bundle to the queue
		cbq->second.push_back(b);
	} else {
		// no queue exists, so create one, put the bundle in it
		list<BundlePkt*> q;
		q.push_back(b);
		perContactBundleQueue_[cid] = q;
	}

	notify();
	return true; // transferred a bundle
}

// if there are any bundles stored in the nodeBundleQueue that could use c
// then put them in the queue for c
bool SdrModel::transferToContact(Contact * c)
{
	int nextHop = c->getDestinationEid();
	int cid = c->getId();

	// check if there are bundles queued for this contact's destination node
	map<int, list<BundlePkt *> >::iterator nbq = perNodeBundleQueue_.find(nextHop);
	if (nbq != perNodeBundleQueue_.end())
	{
		// found bundles to transfer
		// check if a queue already exists for this contact
		map<int, list<BundlePkt *> >::iterator cbq = perContactBundleQueue_.find(cid);
		if (cbq != perContactBundleQueue_.end())
		{
			// a bundle queue already exists, so transfer the bundles to the queue
			cbq->second.splice(cbq->second.end(), nbq->second);
		} else {
			// no queue exists, so create one, put all the bundles in it, and empty
			// the node bundle queue
			perContactBundleQueue_[cid] = nbq->second;
			perNodeBundleQueue_.erase(nextHop);
		}

		notify();
		return true; // transferred some bundles
	}

	return false; // nothing to transfer
}

/////////////////////////////////////
// Enqueue and dequeue from genericBundleQueue_
//////////////////////////////////////

bool SdrModel::enqueueBundle(BundlePkt * bundle)
{
	// if there is not enough space in sdr, the bundle is deleted
	// if another behaviour is required, the simpleCustodyModel should be used
	// to avoid bundle deletions
	if (!(this->isSdrFreeSpace(bundle->getByteLength())))
	{
		cout << "SDRModel::enqueuBundle(BundlePkt * bundle): Bundle exceed sdr capacity so it was not enqueue." << endl;
		delete bundle;
		return false;
	}

	genericBundleQueue_.push_back(bundle);
	bundlesNumber_++;
	bytesStored_ += bundle->getByteLength();
	notify();
	return true;
}

// Delete bundle with bundleId from genericBundleQueue_ if it exists.
void SdrModel::removeBundle(long bundleId)
{
	for (list<BundlePkt *>::iterator it = genericBundleQueue_.begin(); it != genericBundleQueue_.end(); it++)
		if ((*it)->getBundleId() == bundleId)
		{
			int size = (*it)->getByteLength();
			delete (*it);
			genericBundleQueue_.erase(it);
			bundlesNumber_--;
			bytesStored_ -= size;
			notify();
			break;
		}
}

list<BundlePkt *> SdrModel::getCarryingBundles()
{
	return genericBundleQueue_;
}


BundlePkt * SdrModel::getEnqueuedBundle(long bundleId)
{
	for (list<BundlePkt *>::iterator it = genericBundleQueue_.begin(); it != genericBundleQueue_.end(); it++)
		if ((*it)->getBundleId())
			return *it;

	return NULL;
}


/////////////////////////////////////
// Enqueue and dequeue from transmittedBundlesInCustody_
//////////////////////////////////////

bool SdrModel::enqueueTransmittedBundleInCustody(BundlePkt * bundle)
{
	cout << "Node " << eid_ << " enqueueTransmittedBundleInCustody bundleId: " << bundle->getBundleId() << endl;

	// If the bundle is already in memory, there is nothing to do
	BundlePkt * bundleInCustody = this->getTransmittedBundleInCustody(bundle->getBundleId());
	if (bundleInCustody != NULL)
		return true;

	// if there is not enough space in sdr, the bundle is deleted
	// if another behaviour is required, the simpleCustodyModel should be used
	// to avoid bundle deletions
	if (!(this->isSdrFreeSpace(bundle->getByteLength())))
	{
		cout << "SDRModel::enqueueTransmittedBundleInCustody(BundlePkt * bundle): Bundle exceed sdr capacity so it was not enqueue." << endl;
		delete bundle;
		return false;
	}

	transmittedBundlesInCustody_.push_back(bundle);
	bundlesNumber_++;
	bytesStored_ += bundle->getByteLength();
	notify();
	return true;
}

void SdrModel::removeTransmittedBundleInCustody(long bundleId)
{
	cout << "Node " << eid_ << " removeTransmittedBundleInCustody bundleId: " << bundleId << endl;

	for (list<BundlePkt *>::iterator it = transmittedBundlesInCustody_.begin(); it != transmittedBundlesInCustody_.end(); it++)
		if ((*it)->getBundleId() == bundleId)
		{
			int size = (*it)->getByteLength();
			delete (*it);
			it = transmittedBundlesInCustody_.erase(it);
			bundlesNumber_--;
			bytesStored_ -= size;
			notify();
			//break; // remove all possible instances of the same id
		}
}

BundlePkt * SdrModel::getTransmittedBundleInCustody(long bundleId)
{
	for (list<BundlePkt *>::iterator it = transmittedBundlesInCustody_.begin(); it != transmittedBundlesInCustody_.end(); it++)
		if ((*it)->getBundleId() == bundleId)
			return (*it);
	return NULL;
}

list<BundlePkt *> SdrModel::getTransmittedBundlesInCustody(){
	return transmittedBundlesInCustody_;
}
