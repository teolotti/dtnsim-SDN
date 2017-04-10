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
		this->eid_ = this->getParentModule()->getIndex() + 1;
		this->onFault = false;
		this->meanTTR = 60 * 5; // to retry if neigbor fails, and to poll new bundles in queue
		this->saveBundleMap_ = par("saveBundleMap");
		this->generateOutputGraph_ = par("generateOutputGraph");

		// Get a pointer to graphics module
		graphicsModule = (Graphics *) this->getParentModule()->getSubmodule("graphics");

		if (saveBundleMap_)
		{
			// BundleMap Init
			char intStr[30];
			sprintf(intStr, "results/BundleMap_Node%02d.csv", eid_);
			bundleMap_.open(intStr);
			bundleMap_ << "SimTime" << "," << "SRC" << "," << "DST" << "," << "TSRC" << "," << "TDST" << "," << "BitLenght" << "," << "DurationSec" << endl;
		}

		// Initialize contact plan
		contactPlan_.parseContactPlanFile(par("contactsFile"));

		// Schedule local contact messages
		vector<Contact> localContacts = contactPlan_.getContactsBySrc(this->eid_);
		for (vector<Contact>::iterator it = localContacts.begin(); it != localContacts.end(); ++it)
		{
			ContactMsg *contactMsg = new ContactMsg("contactStart", CONTACT_START_TIMER);
			contactMsg->setSchedulingPriority(4);
			contactMsg->setId((*it).getId());
			contactMsg->setStart((*it).getStart());
			contactMsg->setEnd((*it).getEnd());
			contactMsg->setDuration((*it).getEnd() - (*it).getStart());
			contactMsg->setSourceEid((*it).getSourceEid());
			contactMsg->setDestinationEid((*it).getDestinationEid());
			contactMsg->setDataRate((*it).getDataRate());
			scheduleAt((*it).getStart(), contactMsg);
		}

		// Initialize routing
		this->sdr_.setEid(eid_);
		this->sdr_.setNodesNumber(this->getParentModule()->getParentModule()->par("nodesNumber"));
		this->sdr_.setContactPlan(&contactPlan_);
		string routeString = par("routing");
		if (routeString.compare("direct") == 0)
			routing = new RoutingDirect(eid_, &sdr_, &contactPlan_);
		if (routeString.compare("cgrModel") == 0)
			routing = new RoutingCgrModel350(eid_, &sdr_, &contactPlan_);
		if (routeString.compare("cgrModelYen") == 0)
			routing = new RoutingCgrModelYen(eid_, &sdr_, &contactPlan_);
		if (routeString.compare("cgrIon350") == 0)
		{
			int nodesNumber = this->getParentModule()->getParentModule()->par("nodesNumber");
			routing = new RoutingCgrIon350(eid_, &sdr_, &contactPlan_, nodesNumber);
		}

		// Initialize faults
		if (this->getParentModule()->par("enableFaults").boolValue() == true)
		{
			meanTTF = this->getParentModule()->par("meanTTF").doubleValue();
			meanTTR = this->getParentModule()->par("meanTTR").doubleValue();
			cMessage *faultMsg = new ContactMsg("fault", FAULT_START_TIMER);
			scheduleAt(exponential(meanTTF), faultMsg);
		}

		// Initialize stats
		netTxBundles.setName("netTxBundle");
		netRxBundles.setName("netRxBundle");
		netRxHopCount.setName("netRxHopCount");
		netReRoutedBundles.setName("netReRoutedBundles");
		reRoutedBundles = 0;
		netEffectiveFailureTime.setName("netEffectiveFailureTime");
		effectiveFailureTime = 0;
		sdrBundlesInSdr.setName("sdrBundlesInSdr");
		sdrBundleInLimbo.setName("sdrBundleInLimbo");
		sdr_.setStatsHandle(&sdrBundlesInSdr, &sdrBundleInLimbo);
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
	// Fault Start and End Timers:
	///////////////////////////////////////////
	else if (msg->getKind() == FAULT_START_TIMER)
	{
		// Enable dault mode
		graphicsModule->setFaultOn();
		this->onFault = true;

		// Schedule fault recovery
		msg->setKind(FAULT_END_TIMER);
		scheduleAt(simTime() + exponential(meanTTR), msg);
	}
	else if (msg->getKind() == FAULT_END_TIMER)
	{
		// Disable dault mode
		graphicsModule->setFaultOff();
		this->onFault = false;

		// Schedule next fault
		msg->setKind(FAULT_START_TIMER);
		scheduleAt(simTime() + exponential(meanTTF), msg);
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
		contactMsg->setSchedulingPriority(3);
		scheduleAt(simTime() + contactMsg->getDuration(), contactMsg);

		// Visualize contact line on
		graphicsModule->setContactOn(contactMsg);

		// Schedule start of transmission
		FreeChannelMsg* freeChannelMsg = new FreeChannelMsg("FreeChannelMsg", FREE_CHANNEL);
		freeChannelMsg->setSchedulingPriority(1);
		freeChannelMsg->setNeighborEid(contactMsg->getDestinationEid());
		freeChannelMsg->setContactId(contactMsg->getId());
		freeChannelMsgs_[contactMsg->getId()] = freeChannelMsg;
		scheduleAt(simTime(), freeChannelMsg);
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

			routing->routeBundle(bundle, simTime().dbl());
			netReRoutedBundles.record(reRoutedBundles++);
		}

		// Visualize contact line off
		graphicsModule->setContactOff(contactMsg);

		// Delete contactMsg
		cancelAndDelete(freeChannelMsgs_[contactMsg->getId()]);
		delete contactMsg;
	}
	///////////////////////////////////////////
	// Forwarding Stage
	///////////////////////////////////////////
	else if (msg->getKind() == FREE_CHANNEL)
	{
		FreeChannelMsg* freeChannelMsg = check_and_cast<FreeChannelMsg *>(msg);
		int neighborEid = freeChannelMsg->getNeighborEid();
		int contactId = freeChannelMsg->getContactId();

		// save freeChannelMsg to cancel event if necessary
		freeChannelMsgs_[freeChannelMsg->getContactId()] = freeChannelMsg;

		// if there are messages in the queue for this contact
		if (sdr_.isBundleForContact(contactId))
		{
			Net * neighborNet = check_and_cast<Net *>(this->getParentModule()->getParentModule()->getSubmodule("node", neighborEid - 1)->getSubmodule("net"));
			if ((!neighborNet->onFault) && (!this->onFault))
			{
				// If local/remote node are responsive, then transmit bundle normally.
				double transmissionDuration = transmitBundle(neighborEid, contactId);
				scheduleAt(simTime() + transmissionDuration, freeChannelMsg);
			}
			else
			{
				// If local/remote node unresponsive, then retry transmission later.
				scheduleAt(simTime() + meanTTR / 2, freeChannelMsg);
				effectiveFailureTime += meanTTR / 2;
				netEffectiveFailureTime.record(effectiveFailureTime);
			}
		}
		// if there are no messages in the queue for this contact
		else
		{
			// Retry retransmission later // TODO: this needs to be changed.
			scheduleAt(simTime() + meanTTR / 2, freeChannelMsg);
			//freeChannelMsgs_[freeChannelMsg->getContactId()] = nullptr;
			//delete freeChannelMsg;
		}
	}
}

void Net::dispatchBundle(BundlePkt *bundle)
{
	int destinationEid = bundle->getDestinationEid();
	int ownEid = this->eid_;

	// if this node is the destination, send the bundle to Application Module
	if (ownEid == destinationEid)
	{
		netRxHopCount.record(bundle->getHopCount());
		send(bundle, "gateToApp$o");
	}
	// else, route and enqueue bundle
	else
	{
		netRxBundles.record(simTime());
		routing->routeBundle(bundle, simTime().dbl());
		// TODO: Wakeup contacts if asleep
	}
}

double Net::transmitBundle(int neighborEid, int contactId)
{
	double transmissionDuration = 0.0;

	// There is a bundle waiting for this contact.
	BundlePkt* bundle = sdr_.getNextBundleForContact(contactId);

	// Calculate datarate and Tx duration
	// TODO: In the future, this should be driven by the Mac layer.
	double dataRate = this->contactPlan_.getContactById(contactId)->getDataRate();
	transmissionDuration = (double) bundle->getBitLength() / dataRate;

	// Set bundle parameters that are udated on each hop:
	bundle->setSenderEid(eid_);
	bundle->setHopCount(bundle->getHopCount() + 1);
	bundle->setDlvConfidence(0);
	bundle->setXmitCopiesCount(0);
	send(bundle, "gateToMac$o");

	netTxBundles.record(simTime());

	if (saveBundleMap_)
		bundleMap_ << simTime() << "," << eid_ << "," << neighborEid << "," << bundle->getSourceEid() << "," << bundle->getDestinationEid() << "," << bundle->getBitLength() << "," << transmissionDuration << endl;

	sdr_.popNextBundleForContact(contactId);

	return transmissionDuration;
}

void Net::finish()
{
	// Delete all stored bundles
	sdr_.freeSdr(eid_);

	// BundleMap End
	if (saveBundleMap_)
	{
		bundleMap_.close();
	}

	if (generateOutputGraph_)
	{
		outputGraph_.close();
	}
}

Net::Net()
{

}

Net::~Net()
{

}

