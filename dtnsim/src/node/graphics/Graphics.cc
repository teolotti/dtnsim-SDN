#include "Graphics.h"

Define_Module (Graphics);

void Graphics::initialize()
{
	// Store this node eid
	this->eid_ = this->getParentModule()->getIndex() + 1;
	// Store total neighbors
	this->neighbors_ = this->getParentModule()->getVectorSize();

	// Store a pointer to local node module
	nodeModule = this->getParentModule();

	// Store a pointer to network canvas
	networkCanvas = nodeModule->getParentModule()->getCanvas();

	if (hasGUI())
	{
		// Arrange graphical stuff: icon
		cDisplayString& dispStr = nodeModule->getDisplayString();
		string icon_path = "device/";
		string icon = nodeModule->par("icon");
		icon_path.append(icon);
		dispStr.setTagArg("i", 0, icon_path.c_str());

		// Arrange graphical stuff: circular position
		posRadius = nodeModule->getVectorSize() * 250 / (2 * (3.1415));
		posAngle = 2 * (3.1415) / ((float) nodeModule->getVectorSize());
		posX = 160 + posRadius * cos((eid_ - 1) * posAngle) + posRadius;
		posY = 40 + posRadius * sin((eid_ - 1) * posAngle) + posRadius;
		dispStr.setTagArg("p", 0, posX);
		dispStr.setTagArg("p", 1, posY);

		nodeModule->getParentModule()->getDisplayString().setTagArg("bgb", 0, 2 * 160 + 2 * posRadius);
		nodeModule->getParentModule()->getDisplayString().setTagArg("bgb", 1, 2 * 80 + 2 * posRadius);

		//networkCanvas->setTagArg();
	}
}

void Graphics::setFaultOn()
{
	// Visualize fault start
	if (hasGUI())
	{
		cDisplayString& dispStr = nodeModule->getDisplayString();
		string faultColor = "red";
		dispStr.setTagArg("i", 1, faultColor.c_str());
		dispStr.setTagArg("i2", 0, "status/stop");
	}

}

void Graphics::setFaultOff()
{
	if (hasGUI())
	{
		cDisplayString& dispStr = nodeModule->getDisplayString();
		dispStr.setTagArg("i", 1, "");
		dispStr.setTagArg("i2", 0, "");
	}
}

void Graphics::setContactOn(ContactMsg* contactMsg)
{
	if (hasGUI())
	{
		string lineName = "line";
		lineName.append(to_string(contactMsg->getId()));
		cLineFigure *line = new cLineFigure(lineName.c_str());
		line->setStart(cFigure::Point(posX, posY));
		line->setEnd(cFigure::Point(posRadius * cos((contactMsg->getDestinationEid() - 1) * posAngle) + posRadius, posRadius * sin((contactMsg->getDestinationEid() - 1) * posAngle) + posRadius));
		line->setLineWidth(2);
		line->setEndArrowhead(cFigure::ARROW_BARBED);
		lines.push_back(line);
		networkCanvas->addFigure(line);
	}
}

void Graphics::setContactOff(ContactMsg* contactMsg)
{
	if (hasGUI())
	{
		string lineName = "line";
		lineName.append(to_string(contactMsg->getId()));
		networkCanvas->removeFigure(networkCanvas->findFigureRecursively(lineName.c_str()));
	}
}

void Graphics::finish()
{
	// Remove and delete visualization lines
	cCanvas *canvas = getParentModule()->getParentModule()->getCanvas();
	for (vector<cLineFigure *>::iterator it = lines.begin(); it != lines.end(); ++it)
	{
		if (canvas->findFigure((*it)) != -1)
			canvas->removeFigure((*it));
		delete (*it);
	}
}

Graphics::Graphics()
{
}

Graphics::~Graphics()
{
}

