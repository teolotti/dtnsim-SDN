#include "Com.h"

Define_Module(dtnsimhierarchical::Com);

namespace dtnsimhierarchical {

Com::Com() {
}

Com::~Com() {
}

void Com::initialize() {

	this->eid_ = this->getParentModule()->par("eid");
	this->homeRegion_ = this->getParentModule()->par("homeRegion").stringValue();
	this->outerRegion_ = this->getParentModule()->par("outerRegion").stringValue();
}

void Com::setHomeContactPlan(ContactPlan &contactPlan) {
	this->homeContactPlan_ = contactPlan;
}

void Com::setOuterContactPlan(ContactPlan &contactPlan) {
	this->outerContactPlan_ = contactPlan;
}

void Com::handleMessage(cMessage *msg) {

	cout << "COM of node " << eid_ << " received a message of type " << msg->getName() << endl;

	if (msg->getKind() == BUNDLE) {

		BundlePacketHIRR* bundle = check_and_cast<BundlePacketHIRR *>(msg);
		int nextHopEid = bundle->getNextHopEid();

		int sender = bundle->getSenderEid();
		cout << "FROM SENDER: " << sender << endl;

		if (eid_ == nextHopEid) {

			// current node is the intended receiver, send to DTN module
			send(msg, "gateToDtn$o");

		} else {
			bundle->setSenderEid(eid_);

			// current node is intended sender, push bundle out of node

			cModule* networkModule = this->getParentModule()->getParentModule();

			// find correct node index
			int regionNodeNumber = networkModule->par("regionNodeNumber");
			int pwNodeNumber = networkModule->par("passagewayNodeNumber");

			int index = 0;
			string type = "regionNode";

			for (int i = 0; i<regionNodeNumber; ++i) {
				cModule* regionModule = networkModule->getSubmodule("regionNode", i);
				if ((int)regionModule->par("eid") == nextHopEid) {
					index = i;
					type = "regionNode";
					break;
				}
			}

			for (int i = 0; i<pwNodeNumber; ++i) {
				cModule* pwModule = networkModule->getSubmodule("passagewayNode", i);
				if ((int)pwModule->par("eid") == nextHopEid) {
					index = i;
					type = "passagewayNode";
					break;
				}
			}


			char typeChar[type.length() + 1];
			strcpy(typeChar, type.c_str());
			cModule* destinationModule = networkModule->getSubmodule(typeChar, index)->getSubmodule("com");

			// TODO link delay with range
			sendDirect(msg, 0, 0, destinationModule, "gateToAir");

		}
	}
}
}
