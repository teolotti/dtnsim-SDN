#include "Net.h"
#include "App.h"

Define_Module (Net);

int Net::numInitStages() const
{
	int stages = 2;
	return stages;
}

void Net::initialize(int stage)
{
	if (stage == 1)
	{
		// Store this node eid
		this->eid_ = this->getParentModule()->getIndex();

		// Get a pointer to graphics module
		graphicsModule = (Graphics *) this->getParentModule()->getSubmodule("graphics");

		// Init parameters
		this->saveBundleMap_ = par("saveBundleMap");
		this->generateOutputGraph_ = par("generateOutputGraph");

		// Initialize contact plan
		contactPlan_.parseContactPlanFile(par("contactsFile"));

		// Schedule local contact messages
		vector<Contact> localContacts = contactPlan_.getContactsBySrc(this->eid_);
		for (vector<Contact>::iterator it = localContacts.begin(); it != localContacts.end(); ++it)
		{
			ContactMsg *contactMsg = new ContactMsg("contactStart", CONTACT_START_TIMER);

			// Smaller numeric value are executed first
			contactMsg->setSchedulingPriority(CONTACT_START_TIMER);
			contactMsg->setId((*it).getId());
			contactMsg->setStart((*it).getStart());
			contactMsg->setEnd((*it).getEnd());
			contactMsg->setDuration((*it).getEnd() - (*it).getStart());
			contactMsg->setSourceEid((*it).getSourceEid());
			contactMsg->setDestinationEid((*it).getDestinationEid());
			contactMsg->setDataRate((*it).getDataRate());
			scheduleAt((*it).getStart(), contactMsg);
			EV << "node " << eid_ << ": " << "a contact +" << (*it).getStart() << " +" << (*it).getEnd() << " " << (*it).getSourceEid() << " " << (*it).getDestinationEid() << " " << (*it).getDataRate() << endl;
		}

		// Initialize routing
		this->sdr_.setEid(eid_);
		this->sdr_.setNodesNumber(this->getParentModule()->getParentModule()->par("nodesNumber"));
		this->sdr_.setContactPlan(&contactPlan_);
		string routeString = par("routing");
		if (routeString.compare("direct") == 0)
			routing = new RoutingDirect(eid_, &sdr_, &contactPlan_);
		else if (routeString.compare("cgrModel350") == 0)
			routing = new RoutingCgrModel350(eid_, &sdr_, &contactPlan_, par("printRoutingDebug"));
		else if (routeString.compare("cgrModelYen") == 0)
			routing = new RoutingCgrModelYen(eid_, &sdr_, &contactPlan_, par("printRoutingDebug"));
		else if (routeString.compare("cgrModelRev17") == 0)
			routing = new RoutingCgrModelRev17(eid_, this->getParentModule()->getVectorSize(), &sdr_, &contactPlan_, par("printRoutingDebug"));
		else if (routeString.compare("cgrIon350") == 0)
		{
			int nodesNumber = this->getParentModule()->getParentModule()->par("nodesNumber");
			routing = new RoutingCgrIon350(eid_, &sdr_, &contactPlan_, nodesNumber);
		}
		else
		{
			cout << "dtnsim error: unknown routing type: " << routeString << endl;
			exit(1);
		}

		// Initialize stats
		netTxBundles.setName("netTxBundle");
		netRxBundles.setName("netRxBundle");
		netRxHopCount.setName("netRxHopCount");
		netReRoutedBundles.setName("netReRoutedBundles");
		reRoutedBundles = 0;
		sdrBundlesInSdr.setName("sdrBundlesInSdr");
		sdrBundleInLimbo.setName("sdrBundleInLimbo");
		sdr_.setStatsHandle(&sdrBundlesInSdr, &sdrBundleInLimbo);

		// Initialize BundleMap
		if (saveBundleMap_)
		{
			char intStr[30];
			sprintf(intStr, "results/BundleMap_Node%02d.csv", eid_);
			bundleMap_.open(intStr);
			bundleMap_ << "SimTime" << "," << "SRC" << "," << "DST" << "," << "TSRC" << "," << "TDST" << "," << "BitLenght" << "," << "DurationSec" << endl;
		}
	}
}

void Net::handleMessage(cMessage * msg)
{
	///////////////////////////////////////////
	// New Bundle (from App or Mac):
	///////////////////////////////////////////
	if (msg->getKind() == BUNDLE)
	{
		BundlePkt* bundle = check_and_cast<BundlePkt *>(msg);
		dispatchBundle(bundle);
	}

	///////////////////////////////////////////
	// Contact Start and End
	///////////////////////////////////////////
	else if (msg->getKind() == CONTACT_START_TIMER)
	{
		// Schedule end of contact
		ContactMsg* contactMsg = check_and_cast<ContactMsg *>(msg);
		contactMsg->setKind(CONTACT_END_TIMER);
		contactMsg->setName("ContactEnd");
		contactMsg->setSchedulingPriority(CONTACT_END_TIMER);
		scheduleAt(simTime() + contactMsg->getDuration(), contactMsg);

		// Visualize contact line on
		graphicsModule->setContactOn(contactMsg);

		// Schedule start of transmission
		ForwardingMsg* forwardingMsg = new ForwardingMsg("forwardingMsg", FORWARDING_MSG);
		forwardingMsg->setSchedulingPriority(FORWARDING_MSG);
		forwardingMsg->setNeighborEid(contactMsg->getDestinationEid());
		forwardingMsg->setContactId(contactMsg->getId());
		forwardingMsgs_[contactMsg->getId()] = forwardingMsg;
		scheduleAt(simTime(), forwardingMsg);
	}
	else if (msg->getKind() == CONTACT_END_TIMER)
	{
		// Finish transmission: If bundles are left in contact re-route them
		ContactMsg* contactMsg = check_and_cast<ContactMsg *>(msg);
		while (sdr_.isBundleForContact(contactMsg->getId()))
		{
			BundlePkt* bundle = sdr_.getNextBundleForContact(contactMsg->getId());
			sdr_.popNextBundleForContact(contactMsg->getId());

			// Reset bundle values
			bundle->setDlvConfidence(0);
			//bundle->setXmitCopiesCount(0);

			routing->routeAndQueueBundle(bundle, simTime().dbl());
			netReRoutedBundles.record(reRoutedBundles++);
		}

		// Visualize contact line off
		graphicsModule->setContactOff(contactMsg);

		// Delete contactMsg
		cancelAndDelete(forwardingMsgs_[contactMsg->getId()]);
		forwardingMsgs_.erase(contactMsg->getId());
		delete contactMsg;
	}

	///////////////////////////////////////////
	// Forwarding Stage
	///////////////////////////////////////////
	else if (msg->getKind() == FORWARDING_MSG)
	{
		ForwardingMsg* forwardingMsg = check_and_cast<ForwardingMsg *>(msg);
		int neighborEid = forwardingMsg->getNeighborEid();
		int contactId = forwardingMsg->getContactId();

		// save freeChannelMsg to cancel event if necessary
		forwardingMsgs_[forwardingMsg->getContactId()] = forwardingMsg;

		// if there are messages in the queue for this contact
		if (sdr_.isBundleForContact(contactId))
		{
			Net * neighborNet = check_and_cast<Net *>(this->getParentModule()->getParentModule()->getSubmodule("node", neighborEid - 1)->getSubmodule("net"));
			if ((!neighborNet->onFault) && (!this->onFault))
			{
				// If local/remote node are responsive, then transmit bundle

				// Get bundle pointer from sdr
				BundlePkt* bundle = sdr_.getNextBundleForContact(contactId);

				// Calculate datarate and Tx duration
				double dataRate = contactPlan_.getContactById(contactId)->getDataRate();
				double txDuration = (double) bundle->getByteLength() / dataRate;

				// Set bundle parameters that are udated on each hop:
				bundle->setSenderEid(eid_);
				bundle->setHopCount(bundle->getHopCount() + 1);
				bundle->setDlvConfidence(0);
				bundle->setXmitCopiesCount(0);
				send(bundle, "gateToMac$o");

				netTxBundles.record(simTime());

				if (saveBundleMap_)
					bundleMap_ << simTime() << "," << eid_ << "," << neighborEid << "," << bundle->getSourceEid() << "," << bundle->getDestinationEid() << "," << bundle->getBitLength() << "," << txDuration << endl;

				sdr_.popNextBundleForContact(contactId);

				scheduleAt(simTime() + txDuration, forwardingMsg);
			}
			else
			{
				// If local/remote node unresponsive, then do nothing.
				// fault recovery will trigger a local and remote refreshForwarding
			}
		}
		// if there are no messages in the queue for this contact
		else
		{
			// Do nothing, if new data arrives, a refreshForwarding
			// will wake up this forwarding thread
		}
	}
}

void Net::dispatchBundle(BundlePkt *bundle)
{
	if (this->eid_ == bundle->getDestinationEid())
	{
		// We are the destination, send to App
		netRxHopCount.record(bundle->getHopCount());
		send(bundle, "gateToApp$o");
	}
	else
	{
		// Route and enqueue bundle
		netRxBundles.record(simTime());
		routing->routeAndQueueBundle(bundle, simTime().dbl());

		// Wake-up un scheduled forwarding threads
		this->refreshForwarding();
	}
}

void Net::refreshForwarding()
{
	// Check all on-going forwardingMsgs threads
	// (contacts) and wake up those not scheduled.

	std::map<int, ForwardingMsg *>::iterator it;
	for (it = forwardingMsgs_.begin(); it != forwardingMsgs_.end(); ++it)
	{
		ForwardingMsg * forwardingMsg = it->second;
		if (!forwardingMsg->isScheduled())
			scheduleAt(simTime(), forwardingMsg);
	}
}

void Net::setOnFault(bool onFault)
{
	this->onFault = onFault;

	// Local and remote forwarding recovery
	if (onFault == false)
	{
		// Wake-up local un-scheduled forwarding threads
		this->refreshForwarding();

		// Wake-up remote un-scheduled forwarding threads
		std::map<int, ForwardingMsg *>::iterator it;
		for (it = forwardingMsgs_.begin(); it != forwardingMsgs_.end(); ++it)
		{
			ForwardingMsg * forwardingMsg = it->second;
			Net * remoteNet = (Net *) this->getParentModule()->getParentModule()->getSubmodule("node", forwardingMsg->getNeighborEid())->getSubmodule("net");
			remoteNet->refreshForwarding();
		}
	}
}

void Net::finish()
{
	// Delete scheduled forwardingMsg
	std::map<int, ForwardingMsg *>::iterator it;
	for (it = forwardingMsgs_.begin(); it != forwardingMsgs_.end(); ++it)
	{
		ForwardingMsg * forwardingMsg = it->second;
		cancelAndDelete(forwardingMsg);
	}

	// Delete all stored bundles
	sdr_.freeSdr(eid_);

	// BundleMap End
	if (saveBundleMap_)
		bundleMap_.close();

	if (generateOutputGraph_)
		outputGraph_.close();
}

Net::Net()
{

}

Net::~Net()
{

}

