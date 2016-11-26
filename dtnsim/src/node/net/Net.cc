#include "Net.h"
#include "App.h"

Define_Module (Net);

void Net::initialize()
{
	this->eid_ = this->getParentModule()->getIndex() + 1;
	this->onFault = false;

	if (hasGUI())
	{
		// Arrange graphical stuff: icon
		cDisplayString& dispStr = this->getParentModule()->getDisplayString();
		string icon_path = "device/";
		string icon = this->getParentModule()->par("icon");
		icon_path.append(icon);
		dispStr.setTagArg("i", 0, icon_path.c_str());
		// Arrange graphical stuff: circular position
		posRadius = this->getParentModule()->getVectorSize() * 250 / (2 * (3.1415));
		posAngle = 2 * (3.1415) / ((float) this->getParentModule()->getVectorSize());
		posX = posRadius * cos((eid_ - 1) * posAngle) + posRadius;
		posY = posRadius * sin((eid_ - 1) * posAngle) + posRadius;
		dispStr.setTagArg("p", 0, posX);
		dispStr.setTagArg("p", 1, posY);
	}

	// Parse contacts
	this->parseContacts(par("contactsFile"));

	// Initialize routing
	string routeString = par("routing");
	if (routeString.compare("direct") == 0)
		routing = new RoutingDirect();
	if (routeString.compare("cgrModel") == 0)
		routing = new RoutingCgrModel();
	if (routeString.compare("cgrIon350") == 0)
		routing = new RoutingCgrIon350();
	routing->setLocalNode(eid_);
	routing->setSdr(&sdr_);
	routing->setContactPlan(&contactPlan_);

	// Initialize faults
	if (this->getParentModule()->par("enableFaults").boolValue() == true)
	{
		meanTTF = this->getParentModule()->par("meanTTF").doubleValue();
		meanTTR = this->getParentModule()->par("meanTTR").doubleValue();

		cMessage *faultMsg = new ContactMsg("fault", FAULT_START_TIMER);
		//faultMsg->setSchedulingPriority(4);
		scheduleAt(exponential(meanTTF), faultMsg);
	}
	else
	{
		meanTTR = 60 * 5; // to retry if neigbor fails
	}

	// Initialize stats
	netTxBundles.setName("netTxBundle");
	netReRoutedBundles.setName("netReRoutedBundles");
	reRoutedBundles = 0;
	netEffectiveFailureTime.setName("netEffectiveFailureTime");
	effectiveFailureTime = 0;
	sdrBundlesInSdr.setName("sdrBundlesInSdr");
	sdrBundleInLimbo.setName("sdrBundleInLimbo");
	sdr_.setStatsHandle(&sdrBundlesInSdr, &sdrBundleInLimbo);

}

void Net::handleMessage(cMessage * msg)
{
	if (msg->getKind() == BUNDLE)
	{
		Bundle* bundle = check_and_cast<Bundle *>(msg);

		bubble("dispatching bundle");
		dispatchBundle(bundle);
	}
	else if (msg->getKind() == FAULT_START_TIMER)
	{
		if (hasGUI())
		{
			// Visualize fault start
			cDisplayString& dispStr = this->getParentModule()->getDisplayString();
			string faultColor = "red";
			dispStr.setTagArg("i", 1, faultColor.c_str());
			dispStr.setTagArg("i2", 0, "status/stop");
		}

		// Enable dault mode
		this->onFault = true;

		// Schedule fault recovery
		msg->setKind(FAULT_END_TIMER);
		scheduleAt(simTime() + exponential(meanTTR), msg);
	}
	else if (msg->getKind() == FAULT_END_TIMER)
	{
		if (hasGUI())
		{
			// Visualize fault end
			cDisplayString& dispStr = this->getParentModule()->getDisplayString();
			dispStr.setTagArg("i", 1, "");
			dispStr.setTagArg("i2", 0, "");
		}

		// Disable dault mode
		this->onFault = false;

		// Schedule next fault
		msg->setKind(FAULT_START_TIMER);
		scheduleAt(simTime() + exponential(meanTTF), msg);
	}
	else if (msg->getKind() == CONTACT_START_TIMER)
	{
		// Schedule end of contact
		ContactMsg* contactMsg = check_and_cast<ContactMsg *>(msg);
		contactMsg->setKind(CONTACT_END_TIMER);
		contactMsg->setName("ContactEnd");
		contactMsg->setSchedulingPriority(3);
		scheduleAt(simTime() + contactMsg->getDuration(), contactMsg);

		// Schedule start of transmission
		FreeChannelMsg* freeChannelMsg = new FreeChannelMsg("FreeChannelMsg", FREE_CHANNEL);
		freeChannelMsg->setSchedulingPriority(1);
		freeChannelMsg->setNeighborEid(contactMsg->getDestinationEid());
		freeChannelMsg->setContactId(contactMsg->getId());
		freeChannelMsgs_[contactMsg->getId()] = freeChannelMsg;
		scheduleAt(simTime(), freeChannelMsg);

		// Visualize contact line
		if (hasGUI())
		{
			cCanvas *canvas = getParentModule()->getParentModule()->getCanvas();
			string lineName = "line";
			lineName.append(to_string(contactMsg->getDestinationEid()));
			cLineFigure *line = new cLineFigure(lineName.c_str());
			line->setStart(cFigure::Point(posX, posY));
			line->setEnd(cFigure::Point(posRadius * cos((contactMsg->getDestinationEid() - 1) * posAngle) + posRadius, posRadius * sin((contactMsg->getDestinationEid() - 1) * posAngle) + posRadius));
			line->setLineWidth(2);
			line->setEndArrowhead(cFigure::ARROW_BARBED);
			lines.push_back(line);
			canvas->addFigure(line);
		}

	}
	else if (msg->getKind() == CONTACT_END_TIMER)
	{
		ContactMsg* contactMsg = check_and_cast<ContactMsg *>(msg);

		// Visualize contact line end
		if (hasGUI())
		{
			cCanvas *canvas = getParentModule()->getParentModule()->getCanvas();
			string lineName = "line";
			lineName.append(to_string(contactMsg->getDestinationEid()));
			canvas->removeFigure(canvas->findFigureRecursively(lineName.c_str()));
		}

		int contactId = contactMsg->getId();
		cancelAndDelete(freeChannelMsgs_[contactId]);
		delete contactMsg;

		// Check if bundles are left in contact and re-route them
		while (sdr_.isBundleForContact(contactId))
		{
			Bundle* bundle = sdr_.getNextBundleForContact(contactId);
			// Reset delivery confidence, the contact did not succedded.
			bundle->setDlvConfidence(0);

			routing->routeBundle(bundle, simTime().dbl());
			sdr_.popNextBundleForContact(contactId);
			netReRoutedBundles.record(reRoutedBundles++);
		}
	}
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
			// A very simple fault model: Node refrains from transmitting
			// a bundle if this node or the next hop are in fault mode.
			Net * neighborNet = check_and_cast<Net *>(this->getParentModule()->getParentModule()->getSubmodule("node", neighborEid - 1)->getSubmodule("net"));
			if ((!neighborNet->onFault) && (!this->onFault))
			{
				// Transmit bundle normally.
				double transmissionDuration = transmitBundle(neighborEid, contactId);
				scheduleAt(simTime() + transmissionDuration, freeChannelMsg);
			}
			else
			{
				// Local or remote node in fault mode. Wait meanTTR/2 time (?) to retry transmission.
				scheduleAt(simTime() + meanTTR / 2, freeChannelMsg);
				effectiveFailureTime += meanTTR / 2;
				netEffectiveFailureTime.record(effectiveFailureTime);
			}
		}
		// if there aren't messages for this contact, delete freeChannelMsg to stop trying to send bundles through this contact
		else
		{
			// TODO: this needs to be changed, if new bundles are sent
			// from the App or received, they will not be transmitted after this.
			freeChannelMsgs_[freeChannelMsg->getContactId()] = nullptr;
			delete freeChannelMsg;
		}
	}
}

void Net::dispatchBundle(Bundle *bundle)
{
	int destinationEid = bundle->getDestinationEid();
	int ownEid = this->eid_;

	// if this node is the destination, send the bundle to Application Module
	if (ownEid == destinationEid)
	{
		send(bundle, "gateToApp$o");
	}
	// else, route and enqueue bundle
	else
	{
		routing->routeBundle(bundle, simTime().dbl());
	}
}

double Net::transmitBundle(int neighborEid, int contactId)
{
	double transmissionDuration = 0.0;

	// If we got this point, is because there is a
	// bundle waiting for this contact.
	Bundle* bundle = sdr_.getNextBundleForContact(contactId);

	// Calcualte datarate and Tx duration
	// TODO: In the future, this should be made by the Mac layer.
	double dataRate = this->contactPlan_.getContactById(contactId)->getDataRate();
	transmissionDuration = (double) bundle->getBitLength() / dataRate;

	// Set bundle parameters that are udated on each hop:
	bundle->setSenderEid(eid_);
	bundle->setDlvConfidence(0);
	bundle->setXmitCopiesCount(0);
	send(bundle, "gateToMac$o");

	netTxBundles.record(simTime());
	sdr_.popNextBundleForContact(contactId);

	return transmissionDuration;
}

void Net::parseContacts(string fileName)
{
	int id = 1;
	double start = 0.0;
	double end = 0.0;
	int sourceEid = 0;
	int destinationEid = 0;
	double dataRate = 0.0;

	string aux = "#";
	string a;
	string command;
	ifstream file;
	file.open(fileName.c_str());

	if (!file.is_open())
		throw cException(("Error: wrong path to contacts file " + string(fileName)).c_str());

	while (true)
	{
		if (aux.empty())
			getline(file, aux, '\n');
		else if (aux.at(0) == '#')
			getline(file, aux, '\n');
		else
			break;
	}

	stringstream ss(aux);
	ss >> a >> command >> start >> end >> sourceEid >> destinationEid >> dataRate;

	do
	{
		if ((command.compare("contact") == 0))
		{
			contactPlan_.addContact(id, start, end, sourceEid, destinationEid, dataRate, (float) 1.0);
			if (this->eid_ == sourceEid)
			{
				ContactMsg *contactMsg = new ContactMsg("contactStart", CONTACT_START_TIMER);
				contactMsg->setSchedulingPriority(4);
				contactMsg->setId(id);
				contactMsg->setStart(start);
				contactMsg->setEnd(end);
				contactMsg->setDuration(end - start);
				contactMsg->setSourceEid(sourceEid);
				contactMsg->setDestinationEid(destinationEid);
				contactMsg->setDataRate(dataRate);
				scheduleAt(start, contactMsg);
			}
			id++;
		}
	} while (file >> a >> command >> start >> end >> sourceEid >> destinationEid >> dataRate);

	file.close();
}

void Net::finish()
{
	// Delete all stored bundles
	sdr_.freeSdr();

	// Remove and delete visualization lines
	cCanvas *canvas = getParentModule()->getParentModule()->getCanvas();
	for (vector<cLineFigure *>::iterator it = lines.begin(); it != lines.end(); ++it)
	{
		if (canvas->findFigure((*it)) != -1)
			canvas->removeFigure((*it));
		delete (*it);
	}
}

bool Net::isOnFault()
{
	return this->onFault;
}

Net::Net()
{

}

Net::~Net()
{

}

