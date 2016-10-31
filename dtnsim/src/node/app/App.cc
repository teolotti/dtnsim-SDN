#include "App.h"

Define_Module(App);

void App::initialize()
{
	this->eid_ = this->getParentModule()->getIndex() + 1;

	if (par("enable"))
	{
		std::vector<int> bundlesNumberVec;
		const char *bundlesNumberChar = par("bundlesNumber");
		cStringTokenizer bundlesNumberTokenizer(bundlesNumberChar, ",");
		while (bundlesNumberTokenizer.hasMoreTokens())
			bundlesNumberVec.push_back(atoi(bundlesNumberTokenizer.nextToken()));

		std::vector<int> destinationEidVec;
		const char *destinationEidChar = par("destinationEid");
		cStringTokenizer destinationEidTokenizer(destinationEidChar, ",");
		while (destinationEidTokenizer.hasMoreTokens())
		{
			string destinationEidStr = destinationEidTokenizer.nextToken();
			int destinationEid = stoi(destinationEidStr);
			if (destinationEid > this->getParentModule()->getVectorSize())
			{
				throw cException((string("Error: wrong destinationEid = ") + destinationEidStr).c_str());
			}
			destinationEidVec.push_back(destinationEid);
		}

		std::vector<int> sizeVec;
		const char *sizeChar = par("size");
		cStringTokenizer sizeTokenizer(sizeChar, ",");
		while (sizeTokenizer.hasMoreTokens())
			sizeVec.push_back(atoi(sizeTokenizer.nextToken()));

		std::vector<double> startVec;
		const char *startChar = par("start");
		cStringTokenizer startTokenizer(startChar, ",");
		while (startTokenizer.hasMoreTokens())
			startVec.push_back(atof(startTokenizer.nextToken()));

		if ((bundlesNumberVec.size() != destinationEidVec.size()) || (bundlesNumberVec.size() != sizeVec.size() || (bundlesNumberVec.size() != startVec.size())))
		{
			cout << "Node[" << this->getParentModule()->getIndex() << "]" << "bundlesNumberVec, destinationEidVec, sizeVec, startVec: sizes missmatch!" << endl;
			exit(1);
		}

		for (unsigned int i = 0; i < bundlesNumberVec.size(); i++)
		{
			TrafficGeneratorMsg * trafficGenMsg = new TrafficGeneratorMsg("trafGenMsg");
			trafficGenMsg->setSchedulingPriority(0);
			trafficGenMsg->setKind(TRAFFIC_TIMER);
			trafficGenMsg->setBundlesNumber(bundlesNumberVec[i]);
			trafficGenMsg->setDestinationEid(destinationEidVec[i]);
			trafficGenMsg->setSize(sizeVec[i]);
			trafficGenMsg->setInterval(par("interval").doubleValue());
			trafficGenMsg->setTtl(par("ttl").doubleValue());
			scheduleAt(startVec[i], trafficGenMsg);
		}
	}
}

void App::handleMessage(cMessage *msg)
{
	if (msg->getKind() == TRAFFIC_TIMER)
	{
		TrafficGeneratorMsg* trafficGenMsg = check_and_cast<TrafficGeneratorMsg *>(msg);
		Bundle* bundle = new Bundle("123", BUNDLE);
		bundle->setSchedulingPriority(0);

		char bundleName[10];
		sprintf(bundleName, "Src:%d,Dst:%d", this->eid_, trafficGenMsg->getDestinationEid());
		bundle->setName(bundleName);
		bundle->setSourceEid(this->eid_);
		bundle->setDestinationEid(trafficGenMsg->getDestinationEid());
		bundle->setBitLength(trafficGenMsg->getSize() * 8);
		bundle->setByteLength(trafficGenMsg->getSize());
		bundle->setTtl(trafficGenMsg->getTtl());
		bundle->setCreationTimestamp(simTime());
		bundle->getOriginalRoute().clear();
		bundle->getTakenRoute().clear();

		trafficGenMsg->setBundlesNumber((trafficGenMsg->getBundlesNumber() - 1));
		if (trafficGenMsg->getBundlesNumber() == 0)
			delete msg;
		else
			scheduleAt(simTime() + trafficGenMsg->getInterval(), msg);

		send(bundle, "gateToNet$o");
		return;
	}
	else if (msg->getKind() == BUNDLE)
	{
		Bundle* bundle = check_and_cast<Bundle *>(msg);

		int destinationEid = bundle->getDestinationEid();

		if (this->eid_ == destinationEid)
		{
			bubble("bundle received");
			delete msg;
		}
		else
		{
			throw cException("Error: message received in wrong destination");
		}
	}
}

void App::finish()
{

}

App::App()
{

}

App::~App()
{

}

int App::getEid() const
{
	return eid_;
}
