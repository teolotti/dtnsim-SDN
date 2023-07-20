#include "Dtn.h"

Define_Module(dtnsimhierarchical::Dtn);

namespace dtnsimhierarchical {

Dtn::Dtn() {
}

Dtn::~Dtn() {
}

void Dtn::setHomeContactPlan(ContactPlan &contactPlan) {
	this->homeContactPlan_ = contactPlan;
}

void Dtn::setOuterContactPlan(ContactPlan &contactPlan) {
	this->outerContactPlan_ = contactPlan;
}

int Dtn::numInitStages() const {
	int stages = 2;
	return stages;
}

void Dtn::scheduleContacts(vector<Contact> contacts) {

	for (Contact contact : contacts) {

		int id = 					contact.getId();
		double start = 				contact.getStart();
		double end = 				contact.getEnd();
		double duration = 			end - start;
		int sourceEid = 			contact.getSourceEid();
		int destinationEid = 		contact.getDestinationEid();
		double dataRate = 			contact.getDataRate();
		string sourceRegion =		contact.getSourceRegion();
		string destinationRegion = 	contact.getDestinationRegion();

		char source[sourceRegion.length() + 1];
		strcpy(source, sourceRegion.c_str());
		char destination[destinationRegion.length() + 1];
		strcpy(destination, destinationRegion.c_str());

		ContactsMsgHIRR *contactMsgStart = new ContactsMsgHIRR("contactStart", CONTACT_START_TIMER);

		contactMsgStart->setSchedulingPriority(CONTACT_START_TIMER);
		contactMsgStart->setId(id);
		contactMsgStart->setStart(start);
		contactMsgStart->setEnd(end);
		contactMsgStart->setDuration(duration);
		contactMsgStart->setSourceEid(sourceEid);
		contactMsgStart->setDestinationEid(destinationEid);
		contactMsgStart->setDataRate(dataRate);
		contactMsgStart->setSourceRegion(source);
		contactMsgStart->setDestinationRegion(destination);

		scheduleAt(start, contactMsgStart);

		ContactsMsgHIRR *contactMsgEnd = new ContactsMsgHIRR("contactEnd", CONTACT_END_TIMER);

		contactMsgEnd->setSchedulingPriority(CONTACT_END_TIMER);
		contactMsgEnd->setId(id);
		contactMsgEnd->setStart(start);
		contactMsgEnd->setEnd(end);
		contactMsgEnd->setDuration(duration);
		contactMsgEnd->setSourceEid(sourceEid);
		contactMsgEnd->setDestinationEid(destinationEid);
		contactMsgEnd->setDataRate(dataRate);
		contactMsgStart->setSourceRegion(source);
		contactMsgStart->setDestinationRegion(destination);

		scheduleAt(start+duration, contactMsgEnd);

		EV << "node " << eid_ << ": " <<
				"scheduled a contact +" << start << " +" << end <<
				" from " << sourceEid << ", " << sourceRegion <<
				" to " << destinationEid << ", " << destinationRegion <<
				" at " << dataRate << " bytes per second " << endl;
	}


}

void Dtn::initialize(int stage) {

	if (stage == 1) {

		// get all parameters necessary for DTN module
		this->eid_ = this->getParentModule()->par("eid");
		this->homeRegion_ = this->getParentModule()->par("homeRegion").stringValue();
		this->outerRegion_ = this->getParentModule()->par("outerRegion").stringValue();
		this->type_ = this->getParentModule()->par("type").stringValue();

		// fetch the module's graphics submodule to visualize messages and packets
		this->graphicsModule_ = (Graphics *) this->getParentModule()->getSubmodule("graphics");

		// initialize the on-board storage
		this->sdr_ = new SdrModel(eid_, this->par("sdrCapacity"));
		sdr_->addObserver(this);
		this->update();


		routingContactPlan_.addContactPlan(homeContactPlan_);

		// schedule home and outer contacts (start and ends)
		vector<Contact> homeContacts = homeContactPlan_.getContactsBySrc(eid_);
		this->scheduleContacts(homeContacts);

		if (type_.compare("passageway") == 0) {
			routingContactPlan_.addContactPlan(outerContactPlan_);
			vector<Contact> outerContacts = outerContactPlan_.getContactsBySrc(eid_);
			this->scheduleContacts(outerContacts);
		}

		// initialize routing
		this->routing_ = new RoutingCgrModel350(eid_, sdr_, &routingContactPlan_, true);

		// identify all peers and their provenance
		this->identifyPeers();


	}

}

void Dtn::identifyPeers() {

	//cout << "NODE EID: " << eid_ << endl;

	//cout << "HOME CP" << endl;
	//homeContactPlan_.printContactPlan();
	this->homePeers_ = homeContactPlan_.getPassageways();

	//cout << "OUTER CP" << endl;
	//outerContactPlan_.printContactPlan();
	this->outerPeers_ = outerContactPlan_.getPassageways();
	// TODO correct? remove myself


}


void Dtn::handleMessage(cMessage *msg) {

	EV << "Node with EID " << eid_ << " received a " << msg->getName() << " message." << endl;

	///////////
	// HANDLE CONTACT MESSAGES
	//////////

	if (msg->getKind() == CONTACT_START_TIMER) {

		ContactsMsgHIRR* contactMsgStart = check_and_cast<ContactsMsgHIRR *>(msg);
		int contactId = 			contactMsgStart->getId();
		int neighborEid = 			contactMsgStart->getDestinationEid();
		string neighborRegion = 	contactMsgStart->getDestinationRegion();
		char region[neighborRegion.length() + 1];
		strcpy(region, neighborRegion.c_str());

		// visualize this contact with a corresponding arrow
		this->graphicsModule_->setContactOn(contactMsgStart);

		// schedule start of transmission (forwarding message)
		StartForwardingMsgHIRR* forwardingMsg = new StartForwardingMsgHIRR("forwardingMsgStart", FORWARDING_MSG_START);
		forwardingMsg->setSchedulingPriority(FORWARDING_MSG_START);
		forwardingMsg->setContactId(contactId);
		forwardingMsg->setNeighborEid(neighborEid);
		forwardingMsg->setNeighborRegion(region);

		forwardingMsgs_[contactId] = forwardingMsg;
		scheduleAt(simTime(), forwardingMsg);

		EV << "Node with EID " << eid_ << " just scheduled a forwarding msg start." << endl;

		delete contactMsgStart;


	} else if (msg->getKind() == CONTACT_END_TIMER) {

		ContactsMsgHIRR* contactMsgEnd = check_and_cast<ContactsMsgHIRR *>(msg);
		int contactId = 			contactMsgEnd->getId();
		//int neighborEid = 			contactMsgEnd->getDestinationEid();
		string neighborRegion = 	contactMsgEnd->getDestinationRegion();
		char region[neighborRegion.length() + 1];
		strcpy(region, neighborRegion.c_str());

		// TODO reroute bundles still in contact

		// remove the visualization (arrow) of this contact
		this->graphicsModule_->setContactOff(contactMsgEnd);

		// cancel and delete forwarding msg for this specific contact
		cancelAndDelete(forwardingMsgs_[contactId]);
		forwardingMsgs_.erase(contactId);
		delete contactMsgEnd;


	///////////
	// HANDLE FORWARDING MESSAGES
	//////////


	} else if (msg->getKind() == FORWARDING_MSG_START) {

		StartForwardingMsgHIRR* forwardingMsgStart = check_and_cast<StartForwardingMsgHIRR *>(msg);
		int neighborEid = 			forwardingMsgStart->getNeighborEid();
		//cout << "FWD START EID: " << neighborEid << endl;
		string neighborRegion = 	forwardingMsgStart->getNeighborRegion();
		int contactId = 			forwardingMsgStart->getContactId();
		forwardingMsgs_[contactId] = forwardingMsgStart;

		char region[neighborRegion.length() + 1];
		strcpy(region, neighborRegion.c_str());


		//cout << "CHECKING FOR BUNDLE" << endl;

		// are there bundles waiting for this specific neighbor node?
		if (sdr_->isBundleWaiting(neighborEid)) {

			Contact* contact = routingContactPlan_.getContactById(contactId);
			double dataRate = contact->getDataRate();
			double end = contact->getEnd();

			BundlePacketHIRR* bundle = sdr_->getBundleWaiting(neighborEid);

			double transmissionDuration = (double) bundle->getByteLength() / dataRate;
			//cout << "transmission duration: " << transmissionDuration << endl;
			// TODO linkDelay

			// check if bundle can be fully transmitted before the end of the contact
			if ((simTime() + transmissionDuration) <= end) {

				send(bundle, "gateToCom$o");
				sdr_->dequeueBundle(neighborEid);
			}

			// TODO signals

			// contact is still ongoing -> forwarding still possible
			scheduleAt(simTime() + transmissionDuration, forwardingMsgStart);

			// Schedule forwarding message end
			EndForwardingMsgHIRR* forwardingMsgEnd = new EndForwardingMsgHIRR("forwardingMsgEnd", FORWARDING_MSG_END);
			forwardingMsgEnd->setSchedulingPriority(FORWARDING_MSG_END);
			forwardingMsgEnd->setNeighborEid(neighborEid);
			forwardingMsgEnd->setNeighborRegion(region);
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

		EndForwardingMsgHIRR* forwardingMsgEnd = check_and_cast<EndForwardingMsgHIRR *>(msg);
		int neighborEid = 			forwardingMsgEnd->getNeighborEid();
		string neighborRegion = 	forwardingMsgEnd->getNeighborRegion();
		int bundleId = 				forwardingMsgEnd->getBundleId();
		int contactId = 			forwardingMsgEnd->getContactId();
		bool sent = 				forwardingMsgEnd->getSentToDestination();

		//if (strcmp(forwardingMsgEnd->getName(), "regionForwardingMsgEnd") == 0) {
		//	Contact* contact = regionContactPlan_.getContactById(contactId);
			//intraRouting_->successfulBundleForwarded(bundleId, contact, sent);
		//} else {
		//	Contact* contact = backboneContactPlan_.getContactById(contactId);
			//interRouting_->successfulBundleForwarded(bundleId, contact, sent);
		//} TODO

		delete forwardingMsgEnd;


	//////
	// HANDLE BUNDLE PACKETS
	//////


	} else if (msg->getKind() == BUNDLE) {

		// TODO signals

		BundlePacketHIRR* bundle = check_and_cast<BundlePacketHIRR *>(msg);
		dispatchBundle(bundle);

	}

}

int Dtn::findProvenance(int pwEid) {

	if (pwEid == -1) {
		return 0; // unknown provenance
	}

	if (find(homePeers_.begin(), homePeers_.end(), pwEid) != homePeers_.end()) {
		return 1; // previous PW is a home peer -> provenance home
	}

	if (find(outerPeers_.begin(), outerPeers_.end(), pwEid) != outerPeers_.end()) {
		return 2; // previous PW is an outer peer -> provenance outer
	}

	return 0;
}



void Dtn::handleBlacklistBundle(BundlePacketHIRR *bundle) {

	int sourceEid = bundle->getSourceEid(); // passageway that could not find any peers (nor own path to original destination)
	//int destinationEid = bundle->getDestinationEid(); // prior passageway that needs to be informed of the above
	//int priorPassageway = bundle->getPriorPassageway();
	int missedDestination = bundle->getMissedDestination(); // the original destination that source PW cannot reach

	// add passageway to blacklist
	vector<int> blacklistedPassageways;
	if (blacklist_.find(missedDestination) != blacklist_.end()) {
		blacklistedPassageways = blacklist_.at(missedDestination);

		// blacklist does not contain prior PW yet for missed destination
		if (find(blacklistedPassageways.begin(), blacklistedPassageways.end(), sourceEid) == blacklistedPassageways.end()) {
			blacklist_.at(missedDestination).push_back(sourceEid);
		} else {
			return;
		}
	} else {
		vector<int> placeholder;
		blacklist_.insert(make_pair(missedDestination, placeholder));
		blacklist_.at(missedDestination).push_back(sourceEid);
	}

	// check if blacklist message needs to be propagated
	// i.e. send message on only if:
	// I can't reach the missed destination AND all of my peers
	// with the same provenance as the sender can be found in the blacklist for
	// the missed destination
	if (!routingContactPlan_.getContactsByDst(missedDestination).empty()) {
		return; // recipient is terminal and can reach missed destination
	}

	vector<int> list = blacklist_.at(missedDestination);
	bool blacklisted = true;
	int provenance = findProvenance(sourceEid);
	if (provenance == 1) {
		for (auto &homePeer : homePeers_) {
			if (find(list.begin(), list.end(), homePeer) != list.end()) {
				blacklisted = false;
				break;
			}
		}
	} else {
		for (auto &outerPeer: outerPeers_) {
			if (find(list.begin(), list.end(), outerPeer) != list.end()) {
				blacklisted = false;
				break;
			}
		}
	}

	vector<int> nextPeers;
	if (blacklisted) {
		if (provenance == 1) {
			for (auto &outerPeer : outerPeers_) {
				if (find(list.begin(), list.end(), outerPeer) == list.end()) {
					nextPeers.push_back(outerPeer);
				}
			}
		} else {
			for (auto &homePeer: homePeers_) {
				if (find(list.begin(), list.end(), homePeer) != list.end()) {
					nextPeers.push_back(homePeer);
				}
			}
		}
	}

	for (auto &peer : nextPeers) {

		BundlePacketHIRR* blacklistBundle = new BundlePacketHIRR("blacklistBundle", BUNDLE);
		blacklistBundle->setSchedulingPriority(BUNDLE);
		blacklistBundle->setBundleId(blacklistBundle->getId());

		string bundleNameStr = "BL, unreachable Dst " + to_string(missedDestination) + " through PW " + to_string(eid_);
		char bundleName[bundleNameStr.length()+1];
		strcpy(bundleName, bundleNameStr.c_str());
		blacklistBundle->setName(bundleName);

		blacklistBundle->setBitLength(1024 * 8);
		blacklistBundle->setByteLength(1024);

		blacklistBundle->setSourceEid(eid_);
		blacklistBundle->setDestinationEid(peer);

		blacklistBundle->setCreationTimestamp(simTime());
		blacklistBundle->setTtl(9000);
		blacklistBundle->setPriorPassageway(eid_);
		blacklistBundle->setType(1);
		blacklistBundle->setMissedDestination(missedDestination);

		routing_->msgToOtherArrive(blacklistBundle, simTime().dbl(), peer);
	}



}

void Dtn::dispatchBundle(BundlePacketHIRR *bundle) {

	// special blacklist bundle handling
	int type = bundle->getType();
	if (type == 1) {
		handleBlacklistBundle(bundle);
		return;
	}

	int sourceEid = bundle->getSourceEid();
	int destinationEid = bundle->getDestinationEid();
	int priorPassageway = bundle->getPriorPassageway();

	// backwards learning
	for (auto &outerPeer : outerPeers_) {
		if (outerPeer != priorPassageway && priorPassageway != -1) {
			if (blacklist_.count(sourceEid) <= 0) {
				vector<int> placeholder;
				blacklist_.insert(make_pair(sourceEid, placeholder));
			}
			if (find(blacklist_.at(sourceEid).begin(), blacklist_.at(sourceEid).end(), outerPeer) == blacklist_.at(sourceEid).end()) {
				blacklist_.at(sourceEid).push_back(outerPeer);
			}
		}
	}
	for (auto &homePeer : homePeers_) {
		if (homePeer != priorPassageway && priorPassageway != -1) {
			if (blacklist_.count(sourceEid) <= 0) {
				vector<int> placeholder;
				blacklist_.insert(make_pair(sourceEid, placeholder));
			}
			if (find(blacklist_.at(sourceEid).begin(), blacklist_.at(sourceEid).end(), homePeer) == blacklist_.at(sourceEid).end()) {
				blacklist_.at(sourceEid).push_back(homePeer);
			}
		}
	}

	//cout << "BLACKLIST OF EID " << eid_ << endl;
	//for (auto &list : blacklist_) {
	//	cout << "NODE : " << list.first;
	//	for (auto &pw : list.second) {
	//		cout << " PW: " << pw << ", ";
	//	}
	//	cout << endl;
	//}


	// actual routing and forwarding
	if (this->eid_== destinationEid) {

		// Bundle arrived at its final destination, send to APP
		send(bundle, "gateToApp$o");

	} else if (this->type_.compare("region") == 0) {

		if (!routingContactPlan_.getContactsByDst(destinationEid).empty()) {
			// do normal CGR, destination is part of current node's region
			routing_->msgToOtherArrive(bundle, simTime().dbl(), destinationEid);

		} else {
			// do HIRR, send copy of bundle to non-blacklisted peer with a different
			// provenance to the provenance of the prior passageway

			// TODO: do NOT do this at region nodes! they do not have outer peers...

			vector<int> nextHopPassageways;

			int priorPassagewayProvenance = findProvenance(priorPassageway);

			vector<int> blacklistedPassageways;
			if (blacklist_.find(destinationEid) != blacklist_.end()) {
				blacklistedPassageways = blacklist_.at(destinationEid);
			}

			if (priorPassagewayProvenance == 1 || priorPassagewayProvenance == 0) {
				// prior PW has provenance home (or unknown), only consider outer peers
				for (auto &outerPeer : outerPeers_) {
					if (outerPeer == eid_) {
						continue;
					}
					if (find(blacklistedPassageways.begin(), blacklistedPassageways.end(), outerPeer)!= blacklistedPassageways.end()) {
						// peer is blacklisted
						continue;
					}
					nextHopPassageways.push_back(outerPeer);
				}
			}

			if (priorPassagewayProvenance == 2 || priorPassagewayProvenance == 0) {
				// prior PW has provenance outer (or unknown), only consider home peers
				for (auto &homePeer : homePeers_) {
					if (homePeer == eid_) {
						continue;
					}
					if (find(blacklistedPassageways.begin(), blacklistedPassageways.end(), homePeer)!= blacklistedPassageways.end()) {
						// peer is blacklisted
						continue;
					}
					nextHopPassageways.push_back(homePeer);
				}
			}

			for (auto &nextPeer : nextHopPassageways) {
				// send copy to next peer, not changing priorPW attribute (since I'm a region node)
				BundlePacketHIRR* bundleCopy = bundle->dup(); // TODO change fields? delete old bundle? how to handle copies...
				routing_->msgToOtherArrive(bundleCopy, simTime().dbl(), nextPeer);
			}

			// blacklist sending not possible if region node (true? TODO)
		}
		this->refreshForwarding();


	} else {

		if (!routingContactPlan_.getContactsByDst(destinationEid).empty()) { // TODO only consider one CP? better performance...
			// do normal CGR, destination is part of current node's region
			routing_->msgToOtherArrive(bundle, simTime().dbl(), destinationEid);

		} else {
			// do HIRR, send copy of bundle to non-blacklisted peer with a different
			// provenance to the provenance of the prior passageway

			vector<int> nextHopPassageways;

			int priorPassagewayProvenance = findProvenance(priorPassageway);

			vector<int> blacklistedPassageways;
			if (blacklist_.find(destinationEid) != blacklist_.end()) {
				blacklistedPassageways = blacklist_.at(destinationEid);
			}

			if (priorPassagewayProvenance == 1 || priorPassagewayProvenance == 0) {
				// prior PW has provenance home (or unknown), only consider outer peers
				for (auto &outerPeer : outerPeers_) {
					if (outerPeer == eid_) {
						continue;
					}
					if (find(blacklistedPassageways.begin(), blacklistedPassageways.end(), outerPeer)!= blacklistedPassageways.end()) {
						// peer is blacklisted
						continue;
					}
					nextHopPassageways.push_back(outerPeer);
				}
			}

			if (priorPassagewayProvenance == 2 || priorPassagewayProvenance == 0) {
				// prior PW has provenance outer (or unknown), only consider home peers
				for (auto &homePeer : homePeers_) {
					if (homePeer == eid_) {
						continue;
					}
					if (find(blacklistedPassageways.begin(), blacklistedPassageways.end(), homePeer)!= blacklistedPassageways.end()) {
						// peer is blacklisted
						continue;
					}
					nextHopPassageways.push_back(homePeer);
				}
			}

			for (auto &nextPeer : nextHopPassageways) {
				// send copy to next peer, and change priorOW value
				BundlePacketHIRR* bundleCopy = bundle->dup(); // TODO change fields?
				bundleCopy->setPriorPassageway(this->eid_);
				routing_->msgToOtherArrive(bundleCopy, simTime().dbl(), nextPeer);
			}

			// destination is not in my own region(s) nor reachable by any peer,
			// send blacklist message to prior PW
			if (nextHopPassageways.empty()) {

				BundlePacketHIRR* blacklistBundle = new BundlePacketHIRR("blacklistBundle", BUNDLE);
				blacklistBundle->setSchedulingPriority(BUNDLE);
				blacklistBundle->setBundleId(blacklistBundle->getId());

				string bundleNameStr = "BL, unreachable Dst " + to_string(destinationEid) + " through PW " + to_string(eid_);
				char bundleName[bundleNameStr.length()+1];
				strcpy(bundleName, bundleNameStr.c_str());
				blacklistBundle->setName(bundleName);

				blacklistBundle->setBitLength(1024 * 8);
				blacklistBundle->setByteLength(1024);

				blacklistBundle->setSourceEid(eid_);
				blacklistBundle->setDestinationEid(priorPassageway);// TODO

				blacklistBundle->setCreationTimestamp(simTime());
				blacklistBundle->setTtl(9000);
				blacklistBundle->setPriorPassageway(eid_);
				blacklistBundle->setType(1);
				blacklistBundle->setMissedDestination(destinationEid);

				routing_->msgToOtherArrive(blacklistBundle, simTime().dbl(), priorPassageway);
			}
		}
		this->refreshForwarding();

	}
}

void Dtn::refreshForwarding() {

	// Check all on-going forwardingMsgs threads
	// and wake up those not scheduled.
	map<int, StartForwardingMsgHIRR *>::iterator it;
	for (it = forwardingMsgs_.begin(); it != forwardingMsgs_.end(); ++it) {

		StartForwardingMsgHIRR* forwardingMsg = it->second;
		if (!forwardingMsg->isScheduled()) {
			scheduleAt(simTime(), forwardingMsg);
		}
	}
}

void Dtn::update(void) {

	this->graphicsModule_->setBundlesInSdr(sdr_->getBundlesCountInSdr());
}


void Dtn::finish() {

	// TODO some signals

	// cancel and delete all forwarding messages
	std::map<int, StartForwardingMsgHIRR *>::iterator it;
	for (it = forwardingMsgs_.begin(); it != forwardingMsgs_.end(); ++it) {
		StartForwardingMsgHIRR* forwardingMsg = it->second;
		cancelAndDelete(forwardingMsg);
	}

	sdr_->freeSdr();
	delete routing_;
}

}





