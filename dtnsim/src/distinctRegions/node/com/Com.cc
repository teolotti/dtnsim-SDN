#include "Com.h"

Define_Module(dtnsimdistinct::Com);

namespace dtnsimdistinct {

Com::Com() {
}

Com::~Com() {
}

void Com::initialize() {

	this->eid_ = this->getParentModule()->par("eid");
	this->region_ = this->getParentModule()->par("region").stringValue();
}

void Com::setRegionContactPlan(ContactPlan &regionContactPlan) {
	this->regionContactPlan_ = regionContactPlan;
}

void Com::setBackboneContactPlan(ContactPlan &backboneContactPlan) {
	this->backboneContactPlan_ = backboneContactPlan;
}

void Com::handleMessage(cMessage *msg) {

	//cout << "COM of node " << eid_ << " received a message of type " << msg->getName() << endl;

	if (msg->getKind() == BUNDLE) {

		BundlePacket* bundle = check_and_cast<BundlePacket *>(msg);

		int nextHopEid = bundle->getNextHopEid();
		string nextHopRegion = bundle->getNextHopRegion(); // TODO

		if (eid_ == nextHopEid) {
		//if (eid_ == nextHopEid && region_.compare(nextHopRegion) == 0) {

			// current node is the intended receiver, send to DTN module
			send(msg, "gateToDtn$o");

		} else { // TODO check if I'm intended sender with sourceEid param, else I wrongly received this bundle

			// current node is intended sender, push bundle out of node

			cModule* networkModule = this->getParentModule()->getParentModule();

			// find correct node index
			int regionNodeNumber = networkModule->par("regionNodeNumber");
			int pwNodeNumber = networkModule->par("passagewayNodeNumber");
			int bbNodeNumber = networkModule->par("backboneNodeNumber");

			int index = 0;
			string type = "regionNode";

			for (int i = 0; i<regionNodeNumber; ++i) {
				cModule* regionModule = networkModule->getSubmodule("regionNode", i);
				string region = regionModule->par("region").stringValue();
				if ((int)regionModule->par("eid") == nextHopEid) {
				//if (region.compare(nextHopRegion) == 0 && (int)regionModule->par("eid") == nextHopEid) {
					index = i;
					type = "regionNode";
					break;
				}
			}

			for (int i = 0; i<pwNodeNumber; ++i) {
				cModule* pwModule = networkModule->getSubmodule("passagewayNode", i);
				string region = pwModule->par("region").stringValue();
				if ((int)pwModule->par("eid") == nextHopEid) {
				//if (region.compare(nextHopRegion) == 0 && (int)pwModule->par("eid") == nextHopEid) {
					index = i;
					type = "passagewayNode";
					break;
				}
			}

			for (int i = 0; i<bbNodeNumber; ++i) {
				cModule* bbModule = networkModule->getSubmodule("backboneNode", i);
				string region = bbModule->par("region").stringValue();
				if ((int)bbModule->par("eid") == nextHopEid) {
				//if (region.compare(nextHopRegion) == 0 && (int)bbModule->par("eid") == nextHopEid) {
					index = i;
					type = "backboneNode";
					break;
				}
			}

			char typeChar[type.length() + 1];
			strcpy(typeChar, type.c_str());
			cModule* destinationModule = networkModule->getSubmodule(typeChar, index)->getSubmodule("com");

			// TODO contact propagation delay (range)

			sendDirect(msg, 0, 0, destinationModule, "gateToAir");

		}



	}
}

}
