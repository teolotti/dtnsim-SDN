#include "Graphics.h"

Define_Module (dtnsimdistinct::Graphics);

namespace dtnsimdistinct {

void Graphics::initialize() {

	// Store a pointer to network canvas
	networkCanvas_ = this->getParentModule()->getParentModule()->getCanvas();

	// Get the x and y position of this specific node (calculated and set by the ini_generator.py script
	posX_ = this->getParentModule()->par("posX").doubleValue();
	posY_ = this->getParentModule()->par("posY").doubleValue();

}

// set the contact line (arrow from source to destination)
void Graphics::setContactOn(ContactsMsg* contactMsg) {

	if (hasGUI() && this->par("enable")) {

		string lineName = "line";
		lineName.append(to_string(contactMsg->getId()));
		cLineFigure *line = new cLineFigure(lineName.c_str());

		line->setLineWidth(2);
		line->setLineColor("#A2A2A2");
		line->setEndArrowhead(cFigure::ARROW_BARBED);
		line->setStart(cFigure::Point(posX_, posY_));

		// TODO nicer
		int dstEid = contactMsg->getDestinationEid();
		float endPosX;
		float endPosY;

		int rNumber = this->getParentModule()->getParentModule()->par("regionNodeNumber");
		for (int i = 0; i<rNumber; ++i) {
			cModule* regionNode = this->getParentModule()->getParentModule()->getSubmodule("regionNode", i);
			int eid = regionNode->par("eid");
			if (eid == dstEid) {
				endPosX = regionNode->par("posX").doubleValue();
				endPosY = regionNode->par("posY").doubleValue();
				break;
			}
		}
		int pwNumber = this->getParentModule()->getParentModule()->par("passagewayNodeNumber");
		for (int i = 0; i<pwNumber; ++i) {
			cModule* passagewayNode = this->getParentModule()->getParentModule()->getSubmodule("passagewayNode", i);
			int eid = passagewayNode->par("eid");
			if (eid == dstEid) {
				endPosX = passagewayNode->par("posX").doubleValue();
				endPosY = passagewayNode->par("posY").doubleValue();
				break;
			}
		}
		int bbNumber = this->getParentModule()->getParentModule()->par("backboneNodeNumber");
		for (int i = 0; i<bbNumber; ++i) {
			cModule* backboneNode = this->getParentModule()->getParentModule()->getSubmodule("backboneNode", i);
			int eid = backboneNode->par("eid");
			if (eid == dstEid) {
				endPosX = backboneNode->par("posX").doubleValue();
				endPosY = backboneNode->par("posY").doubleValue();
				break;
			}
		}

		line->setEnd(cFigure::Point(endPosX, endPosY));

		lines.push_back(line);
		networkCanvas_->addFigure(line);
	}
}

void Graphics::setContactOff(ContactsMsg* contactMsg) {

	if (hasGUI() && this->par("enable")) {

		string lineName = "line";
		lineName.append(to_string(contactMsg->getId()));

		cFigure* figure = networkCanvas_->findFigureRecursively(lineName.c_str());

		if (figure != NULL) {
			networkCanvas_->removeFigure(figure);
		}
	}
}

void Graphics::setBundlesInSdr(int bundleNumber) {

	stringstream str;
	str << "sdr:" << bundleNumber;
	this->getParentModule()->getDisplayString().setTagArg("tt", 0, str.str().c_str());
}


void Graphics::finish() {

	// Remove and delete visualization lines
	for (vector<cLineFigure *>::iterator it = lines.begin(); it != lines.end(); ++it) {
		if (networkCanvas_->findFigure((*it)) != -1)
			networkCanvas_->removeFigure((*it));
		delete (*it);
	}
}

Graphics::Graphics() {
}

Graphics::~Graphics() {
}

}

