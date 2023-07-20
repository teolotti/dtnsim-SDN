#include <src/node/dtn/Dtn.h>
#include "src/node/app/App.h"

Define_Module (Dtn);

Dtn::Dtn() {

}

Dtn::~Dtn() {

}

void Dtn::setContactTopology(ContactPlan &contactTopology) {
	this->contactTopology_ = contactTopology;
}

void Dtn::setRegionDatabase(RegionDatabase &regionDatabase) {
	this->regionDatabase_ = regionDatabase;
}

int Dtn::numInitStages() const {
	int stages = 2;
	return stages;
}

void Dtn::initialize(int stage) {

	if (stage == 1) {

		// Store this node eid
		this->eid_ = this->getParentModule()->getIndex();

		this->custodyTimeout_ = par("custodyTimeout");
		this->custodyModel_.setEid(eid_);
		this->custodyModel_.setSdr(&sdr_);
		this->custodyModel_.setCustodyReportByteSize(par("custodyReportByteSize"));

		// Get a pointer to graphics module
		graphicsModule = (Graphics *) this->getParentModule()->getSubmodule("graphics");

		// Register this object as sdr obsever, in order to display bundles stored in sdr properly.
		sdr_.addObserver(this);
		update();

		// Schedule local starts contact messages.
		// Only contactTopology starts contacts are scheduled.
		vector<Contact> localContacts1 = contactTopology_.getContactsBySrc(this->eid_);

		for (vector<Contact>::iterator it = localContacts1.begin(); it != localContacts1.end(); ++it) {

			ContactMsg *contactMsgStart = new ContactMsg("contactStart", CONTACT_START_TIMER);

			contactMsgStart->setSchedulingPriority(CONTACT_START_TIMER);
			contactMsgStart->setId((*it).getId());
			contactMsgStart->setStart((*it).getStart());
			contactMsgStart->setEnd((*it).getEnd());
			contactMsgStart->setDuration((*it).getEnd() - (*it).getStart());
			contactMsgStart->setSourceEid((*it).getSourceEid());
			contactMsgStart->setDestinationEid((*it).getDestinationEid());
			contactMsgStart->setDataRate((*it).getDataRate());

			scheduleAt((*it).getStart(), contactMsgStart);

			EV << "node " << eid_ << ": " << "a contact +" << (*it).getStart() << " +" << (*it).getEnd() << " " << (*it).getSourceEid() << " " << (*it).getDestinationEid() << " " << (*it).getDataRate() << endl;
		}

		// Schedule local ends contact messages.
		// All ends contacts of the contact Topology are scheduled.
		// to trigger re-routings of bundles queued in contacts that did not happen.
		vector<Contact> localContacts2;

		if (regionDatabase_.getNodesRxRegionSize() != 0 || regionDatabase_.getNodesTxRegionSize() != 0) {
			localContacts2 = contactTopology_.getContactsBySrc(this->eid_);
		} else {
			localContacts2 = contactPlans_[""].getContactsBySrc(this->eid_);
		}

		for (vector<Contact>::iterator it = localContacts2.begin(); it != localContacts2.end(); ++it) {

			ContactMsg *contactMsgEnd = new ContactMsg("contactEnd", CONTACT_END_TIMER);

			contactMsgEnd->setName("ContactEnd");
			contactMsgEnd->setSchedulingPriority(CONTACT_END_TIMER);
			contactMsgEnd->setId((*it).getId());
			contactMsgEnd->setStart((*it).getStart());
			contactMsgEnd->setEnd((*it).getEnd());
			contactMsgEnd->setDuration((*it).getEnd() - (*it).getStart());
			contactMsgEnd->setSourceEid((*it).getSourceEid());
			contactMsgEnd->setDestinationEid((*it).getDestinationEid());
			contactMsgEnd->setDataRate((*it).getDataRate());

			scheduleAt((*it).getStart() + (*it).getDuration(), contactMsgEnd);
		}

		// Initialize routing
		this->sdr_.setEid(eid_);
		this->sdr_.setSize(par("sdrSize"));
		this->sdr_.setNodesNumber(this->getParentModule()->getParentModule()->par("nodesNumber"));
		this->sdr_.setContactPlan(&contactTopology_);

		if (regionDatabase_.getNodesRxRegionSize() != 0 || regionDatabase_.getNodesTxRegionSize() != 0) {
			routingInterRegions_ = new RoutingInterRegions(eid_, &regionDatabase_, par("interRegionsRoutingType"), par("printInterRegionsRoutingDebug"));

		} else {
			routingInterRegions_ = NULL;
		}

		string routeString = par("routing");

		if (routeString.compare("direct") == 0) {
			routing = new RoutingDirect(eid_, &sdr_, &contactPlans_[""]);
		} else if (routeString.compare("cgrModel350") == 0) {
			routing = new RoutingCgrModel350(eid_, &sdr_, &contactPlans_, routingInterRegions_, par("printRoutingDebug"));
		} else if (routeString.compare("cgrModel350_Hops") == 0) {
			routing = new RoutingCgrModel350_Hops(eid_, &sdr_, &contactPlans_[""], par("printRoutingDebug"));
		} else if (routeString.compare("cgrModelYen") == 0) {
			bool debugPar = par("printRoutingDebug");
			routing = new RoutingCgrModelYen(eid_, &sdr_, &contactPlans_, debugPar);
		} else if (routeString.compare("cgrModelRev17") == 0) {
			ContactPlan * globalContactPlan = ((Dtn *) this->getParentModule()->getParentModule()->getSubmodule("node", 0)->getSubmodule("dtn"))->getContactPlanPointer();
			routing = new RoutingCgrModelRev17(eid_, this->getParentModule()->getVectorSize(), &sdr_, &contactPlans_[""], globalContactPlan, par("routingType"), par("printRoutingDebug"));
		} else if (routeString.compare("epidemic") == 0) {
			routing = new RoutingEpidemic(eid_, &sdr_, this);
		} else if (routeString.compare("sprayAndWait") == 0) {
			int bundlesCopies = par("bundlesCopies");
			routing = new RoutingSprayAndWait(eid_, &sdr_, this, bundlesCopies, false);
		} else if (routeString.compare("binarySprayAndWait") == 0) {
			int bundlesCopies = par("bundlesCopies");
			routing = new RoutingSprayAndWait(eid_, &sdr_, this, bundlesCopies, true);
		} else if (routeString.compare("cgrModel350_Proactive") == 0) {
			routing = new RoutingCgrModel350_Proactive(eid_, &sdr_, &contactPlans_[""], par("printRoutingDebug"), this);
		} else if (routeString.compare("cgrModel350_Probabilistic") == 0) {
			double sContactProb = par("sContactProb");
			routing = new RoutingCgrModel350_Probabilistic(eid_, &sdr_, &contactPlans_[""], par("printRoutingDebug"), this, sContactProb);

		} else {
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
		routeDijkstraCallsXnodes = registerSignal("routeDijkstraCallsXnodes");
		routeDijkstraCallsXcomplexity = registerSignal("routeDijkstraCallsXcomplexity");
		routeExecutionTimeUs = registerSignal("routeExecutionTimeUs");
		routeTableSize = registerSignal("routeTableSize");

		if (eid_ != 0) {
			emit(sdrBundleStored, sdr_.getBundlesCountInSdr());
			emit(sdrBytesStored, sdr_.getBytesStoredInSdr());
		}

		// Initialize BundleMap
		this->saveBundleMap_ = par("saveBundleMap");
		if (saveBundleMap_ && eid_ != 0) {
			// create result folder if it doesn't exist
			struct stat st =
			{ 0 };

			if (stat("results", &st) == -1) {
				mkdir("results", 0700);
			}

			string fileStr = "results/BundleMap_Node" + to_string(eid_) + ".csv";
			bundleMap_.open(fileStr);
			bundleMap_ << "SimTime" << "," << "BundleId" << "," << "ContactId" << "," << "SRC" << "," << "DST" << "," << "TSRC" << "," << "TDST" << "," << "BitLenght" << "," << "DurationSec" << endl;
		}
	}
}

void Dtn::finish() {

	// Last call to sample-hold type metrics
	if (eid_ != 0) {
		emit(sdrBundleStored, sdr_.getBundlesCountInSdr());
		emit(sdrBytesStored, sdr_.getBytesStoredInSdr());
	}

	// Delete scheduled forwardingMsg
	std::map<int, ForwardingMsgStart *>::iterator it;
	for (it = forwardingMsgs_.begin(); it != forwardingMsgs_.end(); ++it) {
		ForwardingMsgStart * forwardingMsg = it->second;
		cancelAndDelete(forwardingMsg);
	}

	// Delete all stored bundles
	sdr_.freeSdr(eid_);

	// BundleMap End
	if (saveBundleMap_)
		bundleMap_.close();

	delete routing;
}

void Dtn::handleMessage(cMessage * msg) {

	///////////////////////////////////////////
	// New Bundle (from App or Com):
	///////////////////////////////////////////
	if (msg->getKind() == BUNDLE || msg->getKind() == BUNDLE_CUSTODY_REPORT) {

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
	else if (msg->getKind() == CONTACT_START_TIMER) {

		// Schedule end of contact
		ContactMsg* contactMsg = check_and_cast<ContactMsg *>(msg);

		// Visualize contact line on
		graphicsModule->setContactOn(contactMsg);

		// Call to routing algorithm
		routing->contactStart(contactTopology_.getContactById(contactMsg->getId()));

		// Schedule start of transmission
		ForwardingMsgStart* forwardingMsg = new ForwardingMsgStart("forwardingMsgStart", FORWARDING_MSG_START);
		forwardingMsg->setSchedulingPriority(FORWARDING_MSG_START);
		forwardingMsg->setNeighborEid(contactMsg->getDestinationEid());
		forwardingMsg->setContactId(contactMsg->getId());
		forwardingMsgs_[contactMsg->getId()] = forwardingMsg;
		scheduleAt(simTime(), forwardingMsg);

		delete contactMsg;

	} else if (msg->getKind() == CONTACT_END_TIMER) {

		// Finish transmission: If bundles are left in contact re-route them
		ContactMsg* contactMsg = check_and_cast<ContactMsg *>(msg);

		for (int i = 0; i < sdr_.getBundlesCountInContact(contactMsg->getId()); i++)
			emit(dtnBundleReRouted, true);

		high_resolution_clock::time_point t1 = high_resolution_clock::now();

		if (regionDatabase_.getNodesRxRegionSize() != 0 || regionDatabase_.getNodesTxRegionSize() != 0) {
			routing->contactEnd(contactTopology_.getContactById(contactMsg->getId()));
		} else {
			routing->contactEnd(contactPlans_[""].getContactById(contactMsg->getId()));
		}

		high_resolution_clock::time_point t2 = high_resolution_clock::now();

		auto executionTimeUs = duration_cast < microseconds > (t2 - t1).count();

		if (routingInterRegions_ != NULL) {
			emit(routeExecutionTimeUs, executionTimeUs);
			emit(routeDijkstraCallsXnodes, (routingInterRegions_->getDijkstraCalls()) * routingInterRegions_->getRegionDatabase()->getVentsNumber());
			emit(routeDijkstraCallsXcomplexity, (routingInterRegions_->getDijkstraCalls()) * routingInterRegions_->getOneDijkstraComplexity());
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
	else if (msg->getKind() == FORWARDING_MSG_START) {

		ForwardingMsgStart* forwardingMsgStart = check_and_cast<ForwardingMsgStart *>(msg);
		int neighborEid = forwardingMsgStart->getNeighborEid();
		int contactId = forwardingMsgStart->getContactId();

		// save freeChannelMsg to cancel event if necessary
		forwardingMsgs_[forwardingMsgStart->getContactId()] = forwardingMsgStart;

		// if there are messages in the queue for this contact
		if (sdr_.isBundleWaiting(neighborEid)) {

			// Get bundle pointer from sdr
			BundlePkt* bundle = sdr_.getBundleWaiting(neighborEid);

			// Calculate data rate and Tx duration
			double dataRate = contactTopology_.getContactById(contactId)->getDataRate();
			double txDuration = (double) bundle->getByteLength() / dataRate;
			double linkDelay = contactTopology_.getRangeBySrcDst(eid_, neighborEid);

			Contact * contact = contactTopology_.getContactById(contactId);

			// if the message can be fully transmitted before the end of the contact, transmit it
			if ((simTime() + txDuration + linkDelay) <= contact->getEnd()) {

				// Set bundle metadata (set by intermediate nodes)
				bundle->setSenderEid(eid_);
				bundle->setHopCount(bundle->getHopCount() + 1);
				bundle->getVisitedNodes().push_back(eid_);
				bundle->setXmitCopiesCount(0);

				//cout<<"-----> sending bundle to node "<<bundle->getNextHopEid()<<endl;
				send(bundle, "gateToCom$o");

				if (saveBundleMap_)
					bundleMap_ << simTime() << "," << bundle->getId() << "," << contact->getId() << "," << eid_ << "," << neighborEid << "," << bundle->getSourceEid() << "," << bundle->getDestinationEid() << ","
							<< bundle->getBitLength() << "," << txDuration << endl;

				sdr_.dequeueBundle(neighborEid);

				// If custody requested, store a copy of the bundle until report received
				if (bundle->getCustodyTransferRequested()) {
					sdr_.enqueueTransmittedBundleInCustody(bundle->dup());
					this->custodyModel_.printBundlesInCustody();

					// Enqueue a retransmission event in case custody acceptance not received
					CustodyTimout * custodyTimeout = new CustodyTimout("custodyTimeout", CUSTODY_TIMEOUT);
					custodyTimeout->setBundleId(bundle->getBundleId());
					scheduleAt(simTime() + this->custodyTimeout_, custodyTimeout);
				}

				emit(dtnBundleSentToCom, true);
				emit(sdrBundleStored, sdr_.getBundlesCountInSdr());
				emit(sdrBytesStored, sdr_.getBytesStoredInSdr());

				// Schedule next transmission
				scheduleAt(simTime() + txDuration, forwardingMsgStart);

				// Schedule forwarding message end
				ForwardingMsgEnd * forwardingMsgEnd = new ForwardingMsgEnd("forwardingMsgEnd", FORWARDING_MSG_END);
				forwardingMsgEnd->setSchedulingPriority(FORWARDING_MSG_END);
				forwardingMsgEnd->setNeighborEid(neighborEid);
				forwardingMsgEnd->setContactId(contactId);
				forwardingMsgEnd->setBundleId(bundle->getBundleId());
				forwardingMsgEnd->setSentToDestination(neighborEid == bundle->getDestinationEid());

			} else {
				// If local/remote node unresponsive, then do nothing.
				// fault recovery will trigger a local and remote refreshForwarding
			}

		} else {
			// There are no messages in the queue for this contact
			// Do nothing, if new data arrives, a refreshForwarding
			// will wake up this forwarding thread
		}

	} else if (msg->getKind() == FORWARDING_MSG_END) {
		// A bundle was successfully forwarded. Notify routing schema in order to it makes proper decisions.
		ForwardingMsgEnd* forwardingMsgEnd = check_and_cast<ForwardingMsgEnd *>(msg);
		int bundleId = forwardingMsgEnd->getBundleId();
		int contactId = forwardingMsgEnd->getContactId();
		Contact * contact = contactTopology_.getContactById(contactId);

		routing->successfulBundleForwarded(bundleId, contact, forwardingMsgEnd->getSentToDestination());
		delete forwardingMsgEnd;
	}

	///////////////////////////////////////////
	// Custody retransmission timer
	///////////////////////////////////////////
	else if (msg->getKind() == CUSTODY_TIMEOUT) {
		// Custody timer expired, check if bundle still in custody memory space and retransmit it if positive
		CustodyTimout* custodyTimout = check_and_cast<CustodyTimout *>(msg);
		BundlePkt * reSendBundle = this->custodyModel_.custodyTimerExpired(custodyTimout);

		if (reSendBundle != NULL)
			this->dispatchBundle(reSendBundle);

		delete custodyTimout;
	}
}

void Dtn::dispatchBundle(BundlePkt *bundle) {

	//char bundleName[10];
	//sprintf(bundleName, "Src:%d,Dst:%d(id:%d)", bundle->getSourceEid() , bundle->getDestinationEid(), (int) bundle->getId());
	//cout << "Dispatching " << bundleName << endl;

	if (this->eid_ == bundle->getDestinationEid()) {

		// We are the final destination of this bundle
		emit(dtnBundleSentToApp, true);
		emit(dtnBundleSentToAppHopCount, bundle->getHopCount());
		bundle->getVisitedNodes().sort();
		bundle->getVisitedNodes().unique();
		emit(dtnBundleSentToAppRevisitedHops, bundle->getHopCount() - bundle->getVisitedNodes().size());

		// Check if this bundle has previously arrived here
		if (routing->msgToMeArrive(bundle)) {

			// This is the first time this bundle arrives
			if (bundle->getBundleIsCustodyReport()) {
				// This is a custody report destined to me
				BundlePkt * reSendBundle = this->custodyModel_.custodyReportArrived(bundle);

				// If custody was rejected, reroute
				if (reSendBundle != NULL)
					this->dispatchBundle(reSendBundle);
			} else {
				// This is a data bundle destined to me
				if (bundle->getCustodyTransferRequested())
					this->dispatchBundle(this->custodyModel_.bundleWithCustodyRequestedArrived(bundle));

				// Send to app layer
				send(bundle, "gateToApp$o");
			}

		} else
			// A copy of this bundle was previously received
			delete bundle;

	} else {
		// This is a bundle in transit
		//cout << "#### NODE: " << eid_ << endl;

		// Manage custody transfer
		if (bundle->getCustodyTransferRequested())
			this->dispatchBundle(this->custodyModel_.bundleWithCustodyRequestedArrived(bundle));

		// check if inter routing is needed

		high_resolution_clock::time_point t1 = high_resolution_clock::now();

		// if IRR is applied, this variable is changed for a passageNodeID
		int terminusNode = bundle->getDestinationEid();
		string currentRegionId;
		if (routingInterRegions_ != NULL) {

			// get region(s) where current node can transmit in
			set < string > thisNodeRegionIds = regionDatabase_.getTxRegionIds(this->eid_);

			// get region(s) where destination node can receive in
			set < string > destinationRegionIds = regionDatabase_.getRxRegionIds(bundle->getDestinationEid());

			set < string > regionsInCommon = this->getRegionsInCommon(thisNodeRegionIds, destinationRegionIds);

			// current and destination node are in the same region and have the correct capabilities,
			// no inter-regional routing necessary
			if (regionsInCommon.size() == 1) {
				currentRegionId = *regionsInCommon.begin();
			}

			// find the passageway node in current nodes region that is the entry node for the transregional route
			else if (regionsInCommon.size() == 0 || regionsInCommon.size() >= 2) {

				// calculate shortest path of vents from source region to destination region
				// route = [(nodeID, (rxRegion, txRegion)), ...]
				IrrRoute irrRoute = routingInterRegions_->computeInterRegionsRouting(bundle, thisNodeRegionIds, destinationRegionIds);
				list < pair<int, pair<string, string> > > route = irrRoute.route;

				// iterate through regions in which current node can send in = source region
				for (auto it1 = thisNodeRegionIds.begin(); it1 != thisNodeRegionIds.end(); ++it1) {

					string sourceRegion = *it1;

					// iterate through hops of the route computed previously
					for (auto it2 = route.begin(); it2 != route.end(); ++it2) {

						// if the rxRegion of this vent is the same as the current source region, get the vent's node ID
						if (it2->second.first == sourceRegion) {

							int passagewayNodeEid = it2->first;

							if (passagewayNodeEid == 0) { // TODO?
								terminusNode = bundle->getDestinationEid();
								currentRegionId = it2->second.first;
								break;
							}

							// if the passageway is different from the current node, change the terminusNode and
							// let intra-region routing take care of it
							// else continue the loop
							else if (passagewayNodeEid != this->eid_) {
								terminusNode = passagewayNodeEid;
								currentRegionId = it2->second.first;
								break;
							}

							else {
								continue;
							}
						}
					}
				}

				emit(routeDijkstraCallsXnodes, (routingInterRegions_->getDijkstraCalls()) * routingInterRegions_->getRegionDatabase()->getVentsNumber());
				emit(routeDijkstraCallsXcomplexity, (routingInterRegions_->getDijkstraCalls()) * routingInterRegions_->getOneDijkstraComplexity());

			}

			// before calling to intra region routing, we set the currentRegionId
			if (RoutingDeterministic *rptr = dynamic_cast<RoutingDeterministic *>(routing)) {
				rptr->setCurrentRegionId(currentRegionId);
			}
		}

		//cout << "calling to intra region routing in NODE " << eid_ << endl;

		// Either accepted or rejected custody, route bundle
		routing->msgToOtherArrive(bundle, simTime().dbl(), terminusNode);

		high_resolution_clock::time_point t2 = high_resolution_clock::now();
		auto executionTimeUs = duration_cast < microseconds > (t2 - t1).count();
		emit(routeExecutionTimeUs, executionTimeUs);

		// Emit routing specific statistics
		string routeString = par("routing");
		if (routeString.compare("cgrModel350") == 0)
		{
			emit(routeCgrDijkstraCalls, ((RoutingCgrModel350*) routing)->getDijkstraCalls());
			emit(routeCgrDijkstraLoops, ((RoutingCgrModel350*) routing)->getDijkstraLoops());
			emit(routeCgrRouteTableEntriesExplored, ((RoutingCgrModel350*) routing)->getRouteTableEntriesExplored());
			emit(routeDijkstraCallsXnodes, ((RoutingCgrModel350*) routing)->getDijkstraCalls() * ((RoutingDeterministic*) routing)->getContactPlan()->getContacts()->size());
			emit(routeDijkstraCallsXcomplexity, ((RoutingCgrModel350*) routing)->getDijkstraCalls() * ((RoutingCgrModel350*) routing)->getOneDijkstraComplexity());
			emit(routeTableSize, ((RoutingCgrModel350*) routing)->getRouteTableSize());
		}
		if (routeString.compare("cgrModel350_Hops") == 0)
		{
			emit(routeCgrDijkstraCalls, ((RoutingCgrModel350_Hops*) routing)->getDijkstraCalls());
			emit(routeCgrDijkstraLoops, ((RoutingCgrModel350_Hops*) routing)->getDijkstraLoops());
			emit(routeCgrRouteTableEntriesExplored, ((RoutingCgrModel350_Hops*) routing)->getRouteTableEntriesExplored());
		}
		if (routeString.compare("cgrModelRev17") == 0)
		{
			emit(routeCgrDijkstraCalls, ((RoutingCgrModelRev17*) routing)->getDijkstraCalls());
			emit(routeCgrDijkstraLoops, ((RoutingCgrModelRev17*) routing)->getDijkstraLoops());
			emit(routeCgrRouteTableEntriesCreated, ((RoutingCgrModelRev17*) routing)->getRouteTableEntriesCreated());
			emit(routeCgrRouteTableEntriesExplored, ((RoutingCgrModelRev17*) routing)->getRouteTableEntriesExplored());
		}
		emit(sdrBundleStored, sdr_.getBundlesCountInSdr());
		emit(sdrBytesStored, sdr_.getBytesStoredInSdr());

		// Wake-up sleeping forwarding threads
		this->refreshForwarding();
	}
}

void Dtn::refreshForwarding()
{
	// Check all on-going forwardingMsgs threads
	// (contacts) and wake up those not scheduled.
	std::map<int, ForwardingMsgStart *>::iterator it;
	for (it = forwardingMsgs_.begin(); it != forwardingMsgs_.end(); ++it)
	{
		ForwardingMsgStart * forwardingMsg = it->second;
		int cid = forwardingMsg->getContactId();
		if (!sdr_.isBundleForContact(cid))
			//notify routing protocol that it has messages to send and contacts for routing
			routing->refreshForwarding(contactTopology_.getContactById(cid));
		if (!forwardingMsg->isScheduled())
		{
			scheduleAt(simTime(), forwardingMsg);
		}
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
		std::map<int, ForwardingMsgStart *>::iterator it;
		for (it = forwardingMsgs_.begin(); it != forwardingMsgs_.end(); ++it)
		{
			ForwardingMsgStart * forwardingMsg = it->second;
			Dtn * remoteDtn = (Dtn *) this->getParentModule()->getParentModule()->getSubmodule("node", forwardingMsg->getNeighborEid())->getSubmodule("dtn");
			remoteDtn->refreshForwarding();
		}
	}
}

ContactPlan* Dtn::getContactPlanPointer(void)
{
	return &this->contactPlans_[""];
}

Routing * Dtn::getRouting(void)
{
	return this->routing;
}

RegionDatabase * Dtn::getRegionDatabase()
{
	return &this->regionDatabase_;
}

set<string> Dtn::getRegionsInCommon(set<string> la, set<string> lb)
{
	set < string > regionsInCommon;

	for (auto it1 = la.begin(); it1 != la.end(); ++it1)
	{
		if (lb.find(*it1) != lb.end())
		{
			regionsInCommon.insert(*it1);
		}
	}

	return regionsInCommon;
}

/**
 * Implementation of method inherited from observer to update gui according to the number of
 * bundles stored in sdr.
 */
void Dtn::update(void)
{
	// update srd size text
	graphicsModule->setBundlesInSdr(sdr_.getBundlesCountInSdr());
}

void Dtn::setContactPlans(map<string, ContactPlan> contactPlans)
{
	contactPlans_ = contactPlans;
}
