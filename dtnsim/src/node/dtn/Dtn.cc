#include <dtn/Dtn.h>
#include "App.h"

Define_Module (Dtn);

Dtn::Dtn()
{

}

Dtn::~Dtn()
{

}

void Dtn::setContactPlan(ContactPlan &contactPlan)
{
	this->contactPlan_ = contactPlan;
}

void Dtn::setContactTopology(ContactPlan &contactTopology)
{
	this->contactTopology_ = contactTopology;
}

int Dtn::numInitStages() const
{
	int stages = 2;
	return stages;
}

void Dtn::initialize(int stage)
{
	if (stage == 1)
	{
		// Store this node eid
		this->eid_ = this->getParentModule()->getIndex();

		// Get a pointer to graphics module
		graphicsModule = (Graphics *) this->getParentModule()->getSubmodule("graphics");
		graphicsModule->setBundlesInSdr(sdr_.getBundlesStoredInSdr());

		// Schedule local contact messages
		vector<Contact> localContacts = contactTopology_.getContactsBySrc(this->eid_);
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
		// If nonFaultsAware, end time events of contacts in the contactPlan which are not present in the contactTopology
		// must be scheduled in order to perform the necessary reroutings of bundles incorrectly routed in contacts that
		// will not happen.
		bool faultsAware = this->getParentModule()->getParentModule()->getSubmodule("central")->par("faultsAware");
		if(!faultsAware)
		{
			vector<Contact> diffContacts = contactPlanUtils::getDifferenceContacts(contactPlan_, contactTopology_);

			for(size_t i = 0; i<diffContacts.size(); i++)
			{
				ContactMsg *contactMsg = new ContactMsg("contactStart", CONTACT_START_TIMER);

				contactMsg->setSchedulingPriority(CONTACT_END_TIMER);
				contactMsg->setId(diffContacts.at(i).getId());
				scheduleAt(diffContacts.at(i).getEnd(), contactMsg);
			}
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
		{
			ContactPlan * globalContactPlan = ((Dtn *) this->getParentModule()->getParentModule()->getSubmodule("node", 0)->getSubmodule("dtn"))->getContactPlanPointer();
			routing = new RoutingCgrModelRev17(eid_, this->getParentModule()->getVectorSize(), &sdr_, &contactPlan_, globalContactPlan, par("routingType"), par("printRoutingDebug"));
		}
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

		// Register signals
		dtnBundleSentToCom = registerSignal("dtnBundleSentToCom");
		dtnBundleSentToApp = registerSignal("dtnBundleSentToApp");
		dtnBundleSentToAppHopCount = registerSignal("dtnBundleSentToAppHopCount");
		dtnBundleSentToAppRevisitedHops = registerSignal("dtnBundleSentToAppRevisitedHops");
		dtnBundleReceivedFromCom = registerSignal("dtnBundleReceivedFromCom");
		dtnBundleReceivedFromApp = registerSignal("dtnBundleReceivedFromApp");
		dtnBundleReRouted = registerSignal("dtnBundleReRouted");
		sdrBundleStored = registerSignal("sdrBundleStored");
		sdrBytesStored = registerSignal("sdrBytesStored");
		routeCgrDijkstraCalls = registerSignal("routeCgrDijkstraCalls");
		routeCgrDijkstraLoops = registerSignal("routeCgrDijkstraLoops");
		routeCgrRouteTableEntriesCreated = registerSignal("routeCgrRouteTableEntriesCreated");
		routeCgrRouteTableEntriesExplored = registerSignal("routeCgrRouteTableEntriesExplored");

		if (eid_ != 0)
		{
			emit(sdrBundleStored, sdr_.getBundlesStoredInSdr());
			emit(sdrBytesStored, sdr_.getBytesStoredInSdr());
		}

		// Initialize BundleMap
		this->saveBundleMap_ = par("saveBundleMap");
		if (saveBundleMap_ && eid_ != 0)
		{
			// create result folder if it doesn't exist
			struct stat st =
			{ 0 };
			if (stat("results", &st) == -1)
			{
				mkdir("results", 0700);
			}

			string fileStr = "results/BundleMap_Node" + to_string(eid_) + ".csv";
			bundleMap_.open(fileStr);
			bundleMap_ << "SimTime" << "," << "SRC" << "," << "DST" << "," << "TSRC" << "," << "TDST" << "," << "BitLenght" << "," << "DurationSec" << endl;
		}
	}
}

void Dtn::finish()
{
	// Last call to sample-hold type metrics
	if (eid_ != 0)
	{
		emit(sdrBundleStored, sdr_.getBundlesStoredInSdr());
		emit(sdrBytesStored, sdr_.getBytesStoredInSdr());
	}

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
}

void Dtn::handleMessage(cMessage * msg)
{
	///////////////////////////////////////////
	// New Bundle (from App or Com):
	///////////////////////////////////////////
	if (msg->getKind() == BUNDLE)
	{
		if (msg->arrivedOn("gateToCom$i"))
			emit(dtnBundleReceivedFromCom, true);
		if (msg->arrivedOn("gateToApp$i"))
			emit(dtnBundleReceivedFromApp, true);

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

			emit(dtnBundleReRouted, true);
			routing->routeAndQueueBundle(bundle, simTime().dbl());
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
			// If local/remote node are responsive, then transmit bundle
			Dtn * neighborDtn = check_and_cast<Dtn *>(this->getParentModule()->getParentModule()->getSubmodule("node", neighborEid)->getSubmodule("dtn"));
			if ((!neighborDtn->onFault) && (!this->onFault))
			{
				// Get bundle pointer from sdr
				BundlePkt* bundle = sdr_.getNextBundleForContact(contactId);

				// Calculate datarate and Tx duration
				double dataRate = contactTopology_.getContactById(contactId)->getDataRate();
				double txDuration = (double) bundle->getByteLength() / dataRate;

				// Set bundle metadata (set by intermediate nodes)
				bundle->setSenderEid(eid_);
				bundle->setHopCount(bundle->getHopCount() + 1);
				bundle->getVisitedNodes().push_back(eid_);
				bundle->setXmitCopiesCount(0);

				//cout<<"-----> sending bundle to node "<<bundle->getNextHopEid()<<endl;
				send(bundle, "gateToCom$o");

				if (saveBundleMap_)
					bundleMap_ << simTime() << "," << eid_ << "," << neighborEid << "," << bundle->getSourceEid() << "," << bundle->getDestinationEid() << "," << bundle->getBitLength() << "," << txDuration << endl;

				sdr_.popNextBundleForContact(contactId);

				graphicsModule->setBundlesInSdr(sdr_.getBundlesStoredInSdr());
				emit(dtnBundleSentToCom, true);
				emit(sdrBundleStored, sdr_.getBundlesStoredInSdr());
				emit(sdrBytesStored, sdr_.getBytesStoredInSdr());

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

void Dtn::dispatchBundle(BundlePkt *bundle)
{
	if (this->eid_ == bundle->getDestinationEid())
	{
		// We are the destination, send to App
		emit(dtnBundleSentToApp, true);
		emit(dtnBundleSentToAppHopCount, bundle->getHopCount());
		bundle->getVisitedNodes().sort();
		bundle->getVisitedNodes().unique();
		emit(dtnBundleSentToAppRevisitedHops, bundle->getHopCount() - bundle->getVisitedNodes().size());

		send(bundle, "gateToApp$o");
	}
	else
	{
		// Route and enqueue bundle
		routing->routeAndQueueBundle(bundle, simTime().dbl());

		// Emit routing specific stats
		string routeString = par("routing");
		if (routeString.compare("cgrModel350") == 0)
		{
			emit(routeCgrDijkstraCalls, ((RoutingCgrModel350*) routing)->getDijkstraCalls());
			emit(routeCgrDijkstraLoops, ((RoutingCgrModel350*) routing)->getDijkstraLoops());
			emit(routeCgrRouteTableEntriesExplored, ((RoutingCgrModel350*) routing)->getRouteTableEntriesExplored());
		}
		if (routeString.compare("cgrModelRev17") == 0)
		{
			emit(routeCgrDijkstraCalls, ((RoutingCgrModelRev17*) routing)->getDijkstraCalls());
			emit(routeCgrDijkstraLoops, ((RoutingCgrModelRev17*) routing)->getDijkstraLoops());
			emit(routeCgrRouteTableEntriesCreated, ((RoutingCgrModelRev17*) routing)->getRouteTableEntriesCreated());
			emit(routeCgrRouteTableEntriesExplored, ((RoutingCgrModelRev17*) routing)->getRouteTableEntriesExplored());
		}
		emit(sdrBundleStored, sdr_.getBundlesStoredInSdr());
		emit(sdrBytesStored, sdr_.getBytesStoredInSdr());

		// update srd size text
		graphicsModule->setBundlesInSdr(sdr_.getBundlesStoredInSdr());

		// Wake-up un scheduled forwarding threads
		this->refreshForwarding();
	}
}

void Dtn::refreshForwarding()
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

void Dtn::setOnFault(bool onFault)
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
			Dtn * remoteDtn = (Dtn *) this->getParentModule()->getParentModule()->getSubmodule("node", forwardingMsg->getNeighborEid())->getSubmodule("dtn");
			remoteDtn->refreshForwarding();
		}
	}
}

ContactPlan* Dtn::getContactPlanPointer(void)
{
	return &this->contactPlan_;
}



