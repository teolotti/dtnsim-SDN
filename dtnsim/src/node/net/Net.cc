#include "Net.h"
#include "App.h"

Define_Module(Net);

void Net::initialize()
{
	this->eid_ = this->getParentModule()->getIndex() + 1;
	this->parseContacts(par("contactsFile"));

	string routeString = par("routing");
	if (routeString.compare("direct") == 0)
		routing = new RoutingDirect();
	if (routeString.compare("cgrModel") == 0)
		routing = new RoutingCgrModel();
	if (routeString.compare("cgrIon350") == 0)
		routing = new RoutingCgrIon350();
	routing->setLocalNode(eid_);
	routing->setQueue(&bundlesQueue_);
	routing->setContactPlan(&contactPlan_);
}

void Net::handleMessage(cMessage * msg)
{
	if (msg->getKind() == BUNDLE)
	{
		EV << "BUNDLE" << endl;

		Bundle* bundle = check_and_cast<Bundle *>(msg);

		bubble("dispatching bundle");
		dispatchBundle(bundle);
	}
	else if (msg->getKind() == CONTACT_START_TIMER)
	{
		EV << "CONTACT START" << endl;
		bubble("contact start");

		ContactMsg* contactMsg = check_and_cast<ContactMsg *>(msg);
		contactMsg->setKind(CONTACT_END_TIMER);
		contactMsg->setName("ContactEnd");
		contactMsg->setSchedulingPriority(3);
		scheduleAt(simTime() + contactMsg->getDuration(), contactMsg);

		FreeChannelMsg* freeChannelMsg = new FreeChannelMsg("FreeChannelMsg", FREE_CHANNEL);
		freeChannelMsg->setSchedulingPriority(1);
		freeChannelMsg->setNeighborEid(contactMsg->getDestinationEid());
		freeChannelMsg->setContactId(contactMsg->getId());
		freeChannelMsgs_[contactMsg->getId()] = freeChannelMsg;
		scheduleAt(simTime(), freeChannelMsg);
	}
	else if (msg->getKind() == CONTACT_END_TIMER)
	{
		EV << "CONTACT END" << endl;
		bubble("contact end");

		ContactMsg* contactMsg = check_and_cast<ContactMsg *>(msg);
		int contactId = contactMsg->getId();
		cancelAndDelete(freeChannelMsgs_[contactId]);
		delete contactMsg;
	}
	else if (msg->getKind() == FREE_CHANNEL)
	{
		EV << "FREE CHANNEL" << endl;

		FreeChannelMsg* freeChannelMsg = check_and_cast<FreeChannelMsg *>(msg);
		int neighborEid = freeChannelMsg->getNeighborEid();
		int contactId = freeChannelMsg->getContactId();

		// save freeChannelMsg to cancel event if necessary
		freeChannelMsgs_[freeChannelMsg->getContactId()] = freeChannelMsg;

		map<int, queue<Bundle *> >::iterator it = bundlesQueue_.find(contactId);

		// if there are messages in the queue for this contact
		if (it != bundlesQueue_.end())
		{
			// transmit bundle and get transmissionDuration
			double transmissionDuration = transmitBundle(neighborEid, contactId);

			// simulate bundle transmission duration by scheduling freeChannelMsg
			scheduleAt(simTime() + transmissionDuration, freeChannelMsg);
		}
		// if there aren't messages for this contact, delete freeChannelMsg to stop trying to send bundles through this contact
		else
		{
			freeChannelMsgs_[freeChannelMsg->getContactId()] = nullptr;
			delete freeChannelMsg;
		}
	}
}

void Net::dispatchBundle(Bundle *bundle)
{
	EV << "DISPATCH BUNDLE" << endl;

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
	EV << "TRANSMIT BUNDLE" << " contactId: " << contactId << endl;

	double transmissionDuration = 0.0;

	map<int, queue<Bundle *> >::iterator it = bundlesQueue_.find(contactId);

	// if there is a bundlesQueue for the contact
	if (it != bundlesQueue_.end())
	{
		queue<Bundle *> bundlesToTx = it->second;

		// if the queue is not empty
		// send one bundle to Mac Module
		// and erase bundle pointer from the queue
		if (!bundlesToTx.empty())
		{
			Bundle* bundle = bundlesToTx.front();
			double dataRate = this->contactPlan_.getContactById(contactId)->getDataRate();
			transmissionDuration = (double) bundle->getBitLength() / dataRate;

			// Set things that changes on each hop:
			bundle->setSenderEid(eid_);
			bundle->setDlvConfidence(0);
			bundle->setXmitCopiesCount(0);

			send(bundle, "gateToMac$o");
			bundlesToTx.pop();

			if (!bundlesToTx.empty())
			{
				bundlesQueue_[contactId] = bundlesToTx;
			}
			else
			{
				bundlesQueue_.erase(contactId);
			}
		}
	}

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
			contactPlan_.addContact(id, start, end, sourceEid, destinationEid, dataRate, (float)1.0);

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
	//delete enqueued bundles that could not be delivered
	map<int, queue<Bundle *> >::iterator it1 = bundlesQueue_.begin();
	map<int, queue<Bundle *> >::iterator it2 = bundlesQueue_.end();
	while (it1 != it2)
	{
		queue<Bundle *> bundles = it1->second;

		while (!bundles.empty())
		{
			delete(bundles.front());
			bundles.pop();
		}
		bundlesQueue_.erase(it1++);
	}
}

Net::Net()
{

}

Net::~Net()
{

}

