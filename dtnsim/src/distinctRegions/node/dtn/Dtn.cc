#include "Dtn.h"

Define_Module(dtnsimdistinct::Dtn);

namespace dtnsimdistinct {

Dtn::Dtn() {
}

Dtn::~Dtn() {
}

void Dtn::setRegionContactPlan(ContactPlan &regionContactPlan) {
	this->regionContactPlan_ = regionContactPlan;
}

void Dtn::addRegionContactPlan(string region, ContactPlan &contactPlan) {
	this->contactPlans_.insert(make_pair(region, contactPlan));
}

void Dtn::setBackboneContactPlan(ContactPlan &backboneContactPlan) {
	this->backboneContactPlan_ = backboneContactPlan; // obsolete
}

void Dtn::setRegionDatabase(RegionDatabase &regionDatabase) {
	this->regionDatabase_ = regionDatabase;
}

int Dtn::numInitStages() const {
	int stages = 2;
	return stages;
}

void Dtn::scheduleContacts(vector<Contact*> contacts, short priorityStart, short priorityEnd,
		string messageNameStart, string messageNameEnd) {

	char msgStart[messageNameStart.length() + 1];
	strcpy(msgStart, messageNameStart.c_str());
	char msgEnd[messageNameEnd.length() + 1];
	strcpy(msgEnd, messageNameEnd.c_str());

	for (auto &contact : contacts) {

		int id = 					contact->getId();
		double start = 				contact->getStart();
		double end = 				contact->getEnd();
		double duration = 			end - start;
		int sourceEid = 			contact->getSourceEid();
		int destinationEid = 		contact->getDestinationEid();
		double dataRate = 			contact->getDataRate();

		ContactsMsg *contactMsgStart = new ContactsMsg(msgStart, priorityStart);

		contactMsgStart->setSchedulingPriority(priorityStart);
		contactMsgStart->setId(id);
		contactMsgStart->setStart(start);
		contactMsgStart->setEnd(end);
		contactMsgStart->setDuration(duration);
		contactMsgStart->setSourceEid(sourceEid);
		contactMsgStart->setDestinationEid(destinationEid);
		contactMsgStart->setDataRate(dataRate);

		scheduleAt(start, contactMsgStart);

		ContactsMsg *contactMsgEnd = new ContactsMsg(msgEnd, priorityEnd);

		contactMsgEnd->setSchedulingPriority(priorityEnd);
		contactMsgEnd->setId(id);
		contactMsgEnd->setStart(start);
		contactMsgEnd->setEnd(end);
		contactMsgEnd->setDuration(duration);
		contactMsgEnd->setSourceEid(sourceEid);
		contactMsgEnd->setDestinationEid(destinationEid);
		contactMsgEnd->setDataRate(dataRate);

		scheduleAt(start+duration, contactMsgEnd);

		EV << "node " << eid_ << ": " <<
				"scheduled a contact +" << start << " +" << end <<
				" from " << sourceEid <<
				" to " << destinationEid <<
				" at " << dataRate << " bytes per second " << endl;
	}


}

void Dtn::initialize(int stage) {

	if (stage == 1) {

		// get all parameters necessary for DTN module
		this->eid_ = this->getParentModule()->par("eid");
		this->region_ = this->getParentModule()->par("region").stringValue();
		this->type_ = this->getParentModule()->par("type").stringValue();

		// fetch the module's graphics submodule to visualize messages and packets
		this->graphicsModule_ = (Graphics *) this->getParentModule()->getSubmodule("graphics");

		// initialize the on-board storage
		this->sdr_ = new SdrModel(eid_, this->par("sdrCapacity"));
		sdr_->addObserver(this);
		this->update();


		if (!type_.compare("backbone") == 0) { // region or PW node

			// schedule regional contacts (start and ends) for both regional and passageway nodes
			vector<Contact*> localContacts = regionContactPlan_.getContactsBySrc(eid_);
			this->scheduleContacts(localContacts, CONTACT_START_TIMER, CONTACT_END_TIMER,
					"regionContactStart", "regionContactEnd");

			// set intra-regional routing
			string intraRoutingType = this->getParentModule()->getParentModule()->par("intraRouting").stringValue();

			if (intraRoutingType.compare("irr") == 0) {

				// LRK as implemented by IRR paper (without RX/TX distinction) (if staticIrr == false)
				bool staticIrr = this->getParentModule()->getParentModule()->par("staticIrr").boolValue();
				this->intraRouting_ = new RoutingInterRegions(eid_, sdr_, contactPlans_, &regionDatabase_, false, staticIrr);

			} else {
				this->intraRouting_ = new RoutingCgrModelYen(eid_, sdr_, contactPlans_, false);
			}

		}

		if (!type_.compare("region") == 0) { // PW or BB node (obsolete)

			// schedule backbone contacts for passageway and backbone nodes
			//vector<Contact> globalContacts = backboneContactPlan_.getContactsBySrcRegion(eid_, region_);
			//this->scheduleContacts(globalContacts, CONTACT_START_TIMER, CONTACT_END_TIMER,
			//		"backboneContactStart", "backboneContactEnd");
			// set inter-regional routing
			//this->interRouting_ = new RoutingCgrModel350(eid_, sdr_, &backboneContactPlan_, true);
					//new RoutingDistinct(eid_, sdr_, &backboneContactPlan_);
		}

		/* (obsolete)
		if (type_.compare("region") == 0) {
			// Each region node knows the EIDs of all passageways belonging to their region
			this->identifyRegionalPassageways();
		} else {
			// Each PW/BB node knows the region to EIDs mapping of all other PW/BB nodes
			this->identifyGlobalPassageways();
		}
		*/

		////
		// Register signals
		////

		routeExecutionTimeUs = registerSignal("routeExecutionTimeUs");
		routeCgrRouteTableEntriesCreated = registerSignal("routeCgrRouteTableEntriesCreated");
		routeCgrRouteTableEntriesExplored = registerSignal("routeCgrRouteTableEntriesExplored");
		routeComplexity = registerSignal("routeComplexity");
		routeTableSize = registerSignal("routeTableSize");
		routeSearchCalls = registerSignal("routeSearchCalls");
		routeSearchStarts = registerSignal("routeSearchStarts");
		yenIterations = registerSignal("yenIterations");

		sdrBundleStored = registerSignal("sdrBundleStored");
		sdrBytesStored = registerSignal("sdrBytesStored");
		emit(sdrBundleStored, sdr_->getBundlesCountInSdr());
		emit(sdrBytesStored, sdr_->getBytesStoredInSdr());

		bundleDropped = registerSignal("bundleDropped");
		bundleReceivedFromCom = registerSignal("bundleReceivedFromCom");
		firstHop = registerSignal("firstHop");

	}
}

void Dtn::identifyRegionalPassageways() {

	int passagewayNodeNumber = this->getParentModule()->getParentModule()->par("passagewayNodeNumber");

	for (int i = 0; i < passagewayNodeNumber; ++i) {

		cModule* passageway = this->getParentModule()->getParentModule()->getSubmodule("passagewayNode", i);
		string region = passageway->par("region");

		if (region.compare(region_) == 0) {
			regionalPassageways_.push_back(passageway->par("eid"));
		}
	}

}

void Dtn::identifyGlobalPassageways() {

	int passagewayNodeNumber = this->getParentModule()->getParentModule()->par("passagewayNodeNumber");
	for (int i = 0; i < passagewayNodeNumber; ++i) {

		cModule* passageway = this->getParentModule()->getParentModule()->getSubmodule("passagewayNode", i);
		string region = passageway->par("region");
		int eid = passageway->par("eid");

		if (globalPassageways_.count(region) <= 0) {
			vector<int> eids;
			eids.push_back(eid);
			globalPassageways_.insert({region, eids});
		} else {
			globalPassageways_.at(region).push_back(eid);
		}
	}

	int backboneNodeNumber = this->getParentModule()->getParentModule()->par("backboneNodeNumber");
	for (int i = 0; i < backboneNodeNumber; ++i) {

		cModule* backbone = this->getParentModule()->getParentModule()->getSubmodule("backboneNode", i);
		string region = backbone->par("region");
		int eid = backbone->par("eid");

		if (globalPassageways_.count(region) <= 0) {
			vector<int> eids;
			eids.push_back(eid);
			globalPassageways_.insert({region, eids});
		} else {
			globalPassageways_.at(region).push_back(eid);
		}
	}
}

void Dtn::handleMessage(cMessage *msg) {

	//EV << "Node with EID " << eid_ << " received a " << msg->getName() << " message." << endl;

	///////////
	// HANDLE CONTACT MESSAGES
	//////////

	if (msg->getKind() == CONTACT_START_TIMER) {

		ContactsMsg* contactMsgStart = check_and_cast<ContactsMsg *>(msg);
		int contactId = 			contactMsgStart->getId();
		int neighborEid = 			contactMsgStart->getDestinationEid();

		// visualize this contact with a corresponding arrow
		this->graphicsModule_->setContactOn(contactMsgStart);

		if (strcmp(msg->getName(), "regionContactStart") == 0) {

			// call to intra routing algorithm
			//intraRouting_->contactStart(regionContactPlan_.getContactById(contactId));

			// schedule start of transmission (forwarding message)
			StartForwardingMsg* forwardingMsg = new StartForwardingMsg("regionForwardingMsgStart", FORWARDING_MSG_START);
			forwardingMsg->setSchedulingPriority(FORWARDING_MSG_START);
			forwardingMsg->setContactId(contactId);
			forwardingMsg->setNeighborEid(neighborEid);

			forwardingMsgs_[contactId] = forwardingMsg;
			scheduleAt(simTime(), forwardingMsg);

			EV << "Node with EID " << eid_ << " just scheduled a region forwarding msg start." << endl;

			delete contactMsgStart;

		} else {

			// call to inter routing algorithm
			//interRouting_->contactStart(regionContactPlan_.getContactById(contactId));

			// schedule start of transmission (forwarding message)
			StartForwardingMsg* forwardingMsg = new StartForwardingMsg("backboneForwardingMsgStart", FORWARDING_MSG_START);
			forwardingMsg->setSchedulingPriority(FORWARDING_MSG_START);
			forwardingMsg->setContactId(contactId);
			forwardingMsg->setNeighborEid(neighborEid);

			forwardingMsgs_[contactId] = forwardingMsg;
			scheduleAt(simTime(), forwardingMsg);

			EV << "Node with EID " << eid_ << " just scheduled a backbone forwarding msg start." << endl;

			delete contactMsgStart;
		}

	} else if (msg->getKind() == CONTACT_END_TIMER) {

		ContactsMsg* contactMsgEnd = check_and_cast<ContactsMsg *>(msg);
		int contactId = 			contactMsgEnd->getId();
		int neighborEid = 			contactMsgEnd->getDestinationEid();

		if (strcmp(msg->getName(), "regionContactEnd") == 0) {

			// TODO reroute bundles still in contact

			// call to intra routing algorithm (contact end)
			//intraRouting_->contactEnd(regionContactPlan_.getContactById(contactId));

			// remove the visualization (arrow) of this contact
			this->graphicsModule_->setContactOff(contactMsgEnd);

			// cancel and delete forwarding msg for this specific contact
			cancelAndDelete(forwardingMsgs_[contactId]);
			forwardingMsgs_.erase(contactId);
			delete contactMsgEnd;

		} else {

			// TODO reroute bundles still in contact

			// call to inter routing algorithm (contact end)
			//interRouting_->contactEnd(backboneContactPlan_.getContactById(contactId));

			// remove the visualization (arrow) of this contact
			this->graphicsModule_->setContactOff(contactMsgEnd);

			// cancel and delete forwarding msg for this specific contact
			cancelAndDelete(forwardingMsgs_[contactId]);
			forwardingMsgs_.erase(contactId);
			delete contactMsgEnd;
		}


	///////////
	// HANDLE FORWARDING MESSAGES
	//////////


	} else if (msg->getKind() == FORWARDING_MSG_START) {

		StartForwardingMsg* forwardingMsgStart = check_and_cast<StartForwardingMsg *>(msg);
		int neighborEid = 			forwardingMsgStart->getNeighborEid();
		//cout << "FWD START EID: " << neighborEid << endl;
		int contactId = 			forwardingMsgStart->getContactId();
		forwardingMsgs_[contactId] = forwardingMsgStart;

		char senderRegion[region_.length() + 1];
		strcpy(senderRegion, region_.c_str());

		//cout << "CHECKING FOR BUNDLE" << endl;

		// are there bundles waiting for this specific neighbor node?
		if (sdr_->isBundleWaiting(neighborEid)) {

			//cout << "BUNDLE WAITING" << endl;

			Contact* contact;

			// this will be an intra-region transmission
			if (strcmp(msg->getName(), "regionForwardingMsgStart") == 0) {
				contact = regionContactPlan_.getContactById(contactId);

			} else {
				contact = backboneContactPlan_.getContactById(contactId);
			}

			BundlePacket* bundle = sdr_->getBundleWaiting(neighborEid);

			double dataRate = contact->getDataRate();
			double range = contact->getRange();
			double transmissionDuration = (double) bundle->getByteLength() / dataRate;

			// check if bundle can be fully transmitted before the end of the contact
			if ((simTime() + transmissionDuration + range) <= contact->getEnd()) {

				if (bundle->getHopCount() == 0) {
					emit(firstHop, neighborEid);
				}

				bundle->setSenderEid(eid_);
				bundle->setSenderRegion(senderRegion);
				bundle->setHopCount(bundle->getHopCount() + 1);

				send(bundle, "gateToCom$o");
				sdr_->dequeueBundle(neighborEid);
			}

			// TODO fix below, gives error
			//emit(sdrBundleStored, sdr_->getBundlesCountInSdr());
			//emit(sdrBytesStored, sdr_->getBytesStoredInSdr());

			// contact is still ongoing -> forwarding still possible
			scheduleAt(simTime() + transmissionDuration, forwardingMsgStart);

			// Schedule forwarding message end
			EndForwardingMsg* forwardingMsgEnd = new EndForwardingMsg("forwardingMsgEnd", FORWARDING_MSG_END);
			forwardingMsgEnd->setSchedulingPriority(FORWARDING_MSG_END);
			forwardingMsgEnd->setNeighborEid(neighborEid);
			forwardingMsgEnd->setContactId(contactId);
			forwardingMsgEnd->setBundleId(bundle->getBundleId());

			forwardingMsgEnd->setSentToDestination(neighborEid == bundle->getDestinationEid()); //TODO deal with encapsulated bundles
			scheduleAt(simTime() + transmissionDuration, forwardingMsgEnd);

		} else {
			// There are no messages in the queue for this contact
			// Do nothing, if new data arrives, a refreshForwarding
			// will wake up this forwarding thread
		}


	} else if (msg->getKind() == FORWARDING_MSG_END) {

		EndForwardingMsg* forwardingMsgEnd = check_and_cast<EndForwardingMsg *>(msg);
		int neighborEid = 			forwardingMsgEnd->getNeighborEid();
		int bundleId = 				forwardingMsgEnd->getBundleId();
		int contactId = 			forwardingMsgEnd->getContactId();
		bool sent = 				forwardingMsgEnd->getSentToDestination();

		if (strcmp(forwardingMsgEnd->getName(), "regionForwardingMsgEnd") == 0) {
			Contact* contact = regionContactPlan_.getContactById(contactId);
			//intraRouting_->successfulBundleForwarded(bundleId, contact, sent);
		} else {
			Contact* contact = backboneContactPlan_.getContactById(contactId);
			//interRouting_->successfulBundleForwarded(bundleId, contact, sent);
		}

		delete forwardingMsgEnd;


	//////
	// HANDLE BUNDLE PACKETS
	//////

	} else if (msg->getKind() == BUNDLE) {

		if (msg->arrivedOn("gateToCom$i")) {
			emit(bundleReceivedFromCom, true);
		}

		BundlePacket* bundle = check_and_cast<BundlePacket *>(msg);
		dispatchBundle(bundle);
	}
}

void Dtn::dispatchBundle(BundlePacket *bundle){

	int destinationEid = bundle->getDestinationEid();
	string destinationRegion = bundle->getDestinationRegion();

	if (this->eid_== destinationEid) {

		// bundle is encapsulated, decapsulate and circulate
		if (bundle->getEnc()) {
			BundlePacket* decBundle = check_and_cast<BundlePacket *>(bundle->decapsulate());
			delete bundle;
			//cout << "decaps at EID " << eid_ << " into dst EID " << decBundle->getDestinationEid() << " and circulate" << endl;
			scheduleAt(simTime(), decBundle);

		// bundle arrived at final destination
		} else {
			//cout << "final destination at EID " << eid_ << endl;
			// TODO emit signals
			send(bundle, "gateToApp$o");
		}

	} else if (this->region_.compare(destinationRegion) == 0) {

		// we're in the right region, finish routing (intra-regional)
		//cout << "INTRA REGIONAL ROUTING" << endl;

		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		intraRouting_->msgToOtherArrive(bundle, simTime().dbl(), bundle->getDestinationEid());
		high_resolution_clock::time_point t2 = high_resolution_clock::now();

		auto executionTimeUs = duration_cast < microseconds > (t2 - t1).count();
		emit(routeExecutionTimeUs, executionTimeUs);

		string intraRoutingType = this->getParentModule()->getParentModule()->par("intraRouting").stringValue();
		if (intraRoutingType.compare("yen") == 0) {
			//emit(routeCgrRouteTableEntriesCreated, ((RoutingCgrModelYen*) intraRouting_)->getRouteTableEntriesCreated());
			//emit(routeCgrRouteTableEntriesExplored, ((RoutingCgrModelYen*) intraRouting_)->getRouteTableEntriesExplored());
			//emit(routeTableSize, ((RoutingCgrModelYen*) intraRouting_)->getRouteTableSize());

			//if (((RoutingCgrModelYen*) intraRouting_)->getBundleDropped()) {
			//	emit(bundleDropped, 1);
			//}

		} else {
			emit(routeCgrRouteTableEntriesCreated, ((RoutingCgrModelYen*) intraRouting_)->getRouteTableEntriesCreated());
			emit(routeCgrRouteTableEntriesExplored, ((RoutingCgrModelYen*) intraRouting_)->getRouteTableEntriesExplored());
			emit(routeComplexity, ((RoutingCgrModelYen*) intraRouting_)->getDijkstraCalls() * ((RoutingCgrModelYen*) intraRouting_)->getOneDijkstraComplexity());


			emit(routeTableSize, ((RoutingCgrModelYen*) intraRouting_)->getRouteTableSize());
			emit(routeSearchCalls, ((RoutingCgrModelYen*) intraRouting_)->getDijkstraCalls());
			emit(routeSearchStarts, ((RoutingCgrModelYen*) intraRouting_)->getRouteSearchStarts());
			emit(yenIterations, ((RoutingCgrModelYen*) intraRouting_)->getYenIterations());

			if (((RoutingCgrModelYen*) intraRouting_)->getBundleDropped()) {
				emit(bundleDropped, 1);
			}
		}

		emit(sdrBundleStored, sdr_->getBundlesCountInSdr());
		emit(sdrBytesStored, sdr_->getBytesStoredInSdr());

		this->refreshForwarding(); // TODO down?

	} else {

		// inter-regional routing -> get bundle to a PW first (encapsulated)
		if (this->type_.compare("region") == 0) {

			int temporaryDestination = 0; //TODO
					//regionContactPlan_.getNextPassageway(eid_, regionalPassageways_, simTime().dbl()); //TODO

			BundlePacket* encBundle = new BundlePacket("encBundle", BUNDLE);
			encBundle->setSchedulingPriority(BUNDLE);
			encBundle->setBundleId(encBundle->getId());
			string name = "ENC OF " + to_string(bundle->getId());
			char nameChar[name.length()+1];
			strcpy(nameChar, name.c_str());
			encBundle->setName(nameChar);

			encBundle->setBitLength(bundle->getBitLength()); // TODO
			encBundle->setByteLength(bundle->getByteLength());

			encBundle->setSourceEid(this->eid_);
			char source[this->region_.length() + 1];
			strcpy(source, this->region_.c_str());
			encBundle->setSourceRegion(source);

			encBundle->setDestinationEid(temporaryDestination); // TODO
			encBundle->setDestinationRegion(source); //TODO

			encBundle->setCreationTimestamp(simTime());
			encBundle->setTtl(bundle->getTtl());
			encBundle->setEnc(true);

			encBundle->encapsulate(bundle); //TODO

			//cout << "encaps at EID " << eid_ << " into dst EID " << encBundle->getDestinationEid() << " and circulate" << endl;
			scheduleAt(simTime(), encBundle);

		// inter-regional routing -> get bundle to next inter-regional hop
		} else {

			// TODO encaps/decaps story
			int destinationPassageway = globalPassageways_.at(destinationRegion).at(0); // TODO

			BundlePacket* encBundle = new BundlePacket("encBundle", BUNDLE);
			encBundle->setSchedulingPriority(BUNDLE);
			encBundle->setBundleId(encBundle->getId());
			string name = "ENC OF " + to_string(bundle->getId());
			char nameChar[name.length()+1];
			strcpy(nameChar, name.c_str());
			encBundle->setName(nameChar);

			encBundle->setBitLength(bundle->getBitLength()); // TODO
			encBundle->setByteLength(bundle->getByteLength());

			encBundle->setSourceEid(this->eid_);
			char source[this->region_.length() + 1];
			strcpy(source, this->region_.c_str());
			encBundle->setSourceRegion(source);

			encBundle->setDestinationEid(destinationPassageway); // TODO
			encBundle->setDestinationRegion(source); //TODO

			encBundle->setCreationTimestamp(simTime());
			encBundle->setTtl(bundle->getTtl());
			encBundle->setEnc(true);

			encBundle->encapsulate(bundle); //TODO

			//cout << "encaps at EID " << eid_ << " into dst EID " << encBundle->getDestinationEid() << " and circulate" << endl;
			//cout << "INTER REGIONAL ROUTING" << endl;
			interRouting_->msgToOtherArrive(encBundle, simTime().dbl(), encBundle->getDestinationEid());
			this->refreshForwarding(); // TODO down?

			// TODO favor paths through BBs?

		}
	}
}

void Dtn::refreshForwarding() {

	// Check all on-going forwardingMsgs threads
	// and wake up those not scheduled.
	map<int, StartForwardingMsg *>::iterator it;
	for (it = forwardingMsgs_.begin(); it != forwardingMsgs_.end(); ++it) {

		StartForwardingMsg* forwardingMsg = it->second;
		if (!forwardingMsg->isScheduled()) {
			scheduleAt(simTime(), forwardingMsg);
		}
	}
}

void Dtn::update(void) {

	this->graphicsModule_->setBundlesInSdr(sdr_->getBundlesCountInSdr());
}


void Dtn::finish() {

	emit(sdrBundleStored, sdr_->getBundlesCountInSdr());
	emit(sdrBytesStored, sdr_->getBytesStoredInSdr());

	// cancel and delete all forwarding messages
	std::map<int, StartForwardingMsg *>::iterator it;
	for (it = forwardingMsgs_.begin(); it != forwardingMsgs_.end(); ++it) {
		StartForwardingMsg* forwardingMsg = it->second;
		cancelAndDelete(forwardingMsg);
	}

	sdr_->freeSdr();
	if (!type_.compare("backbone") == 0) { // region or PW node
		delete intraRouting_;
	}
	if (!type_.compare("region") == 0) { // PW or BB node
		delete interRouting_;
	}
}


}





