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
}

void SdrModel::updateStats()
{
	// Only update limbo if it has changed
	if (bundlesQueue_[0].size() > sdrBundleInLimbo)
	{
		sdrBundleInLimbo_->record(bundlesQueue_[0].size());
		sdrBundleInLimbo=bundlesQueue_[0].size();
	}

	// Allways update total Sdr storage
	int bundlesInSdr = 0;
	for (map<int, queue<Bundle *> >::iterator it = bundlesQueue_.begin(); it != bundlesQueue_.end(); ++it)
		bundlesInSdr += (*it).second.size();
	sdrBundlesInSdr_->record(bundlesInSdr);
}

void SdrModel::enqueueBundleToContact(Bundle * bundle, int contactId)
{
	// Check is queue exits, if not, create it.
	// Add bundle to queue.
	map<int, queue<Bundle *> >::iterator it = bundlesQueue_.find(contactId);
	if (it != bundlesQueue_.end())
	{
		it->second.push(bundle);
	}
	else
	{
		queue<Bundle *> q;
		q.push(bundle);
		bundlesQueue_[contactId] = q;
	}

	// Update Sdr stats
	this->updateStats();
}

bool SdrModel::isBundleForContact(int contactId)
{
	// This functions returns true if there is a queue
	// with bundles for the contactId. If it is empy
	// or not-existant, the function returns false

	map<int, queue<Bundle *> >::iterator it = bundlesQueue_.find(contactId);

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

Bundle * SdrModel::getNextBundleForContact(int contactId)
{
	map<int, queue<Bundle *> >::iterator it = bundlesQueue_.find(contactId);

	// Just check if the function was called incorrectly
	if (it == bundlesQueue_.end())
		if (bundlesQueue_[contactId].empty())
		{
			cout << "***getBundle called from SdrModel but queue empty***" << endl;
			exit(1);
		}

	// Find and return pointer to bundle
	queue<Bundle *> bundlesToTx = it->second;

	return bundlesToTx.front();
}

void SdrModel::popNextBundleForContact(int contactId)
{
	// Pop the next bundle for this contact
	map<int, queue<Bundle *> >::iterator it = bundlesQueue_.find(contactId);
	queue<Bundle *> bundlesToTx = it->second;
	bundlesToTx.pop();

	// Update queue after popping the bundle
	if (!bundlesToTx.empty())
		bundlesQueue_[contactId] = bundlesToTx;
	else
		bundlesQueue_.erase(contactId);

	// Update Sdr stats
	this->updateStats();
}

void SdrModel::freeSdr()
{

	// Delete all enqueued bundles
	map<int, queue<Bundle *> >::iterator it1 = bundlesQueue_.begin();
	map<int, queue<Bundle *> >::iterator it2 = bundlesQueue_.end();
	while (it1 != it2)
	{
		queue<Bundle *> bundles = it1->second;

		while (!bundles.empty())
		{
			delete (bundles.front());
			bundles.pop();
		}
		bundlesQueue_.erase(it1++);
	}
}
