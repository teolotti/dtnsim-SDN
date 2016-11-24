#include "App.h"

Define_Module(App);

void App::initialize()
{
	this->eid_ = this->getParentModule()->getIndex() + 1;

	// Configure Traffic Generator
	if (par("enable"))
	{
		const char *bundlesNumberChar = par("bundlesNumber");
		cStringTokenizer bundlesNumberTokenizer(bundlesNumberChar, ",");
		while (bundlesNumberTokenizer.hasMoreTokens())
			bundlesNumberVec_.push_back(atoi(bundlesNumberTokenizer.nextToken()));

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
			destinationEidVec_.push_back(destinationEid);
		}

		const char *sizeChar = par("size");
		cStringTokenizer sizeTokenizer(sizeChar, ",");
		while (sizeTokenizer.hasMoreTokens())
			sizeVec_.push_back(atoi(sizeTokenizer.nextToken()));

		const char *startChar = par("start");
		cStringTokenizer startTokenizer(startChar, ",");
		while (startTokenizer.hasMoreTokens())
			startVec_.push_back(atof(startTokenizer.nextToken()));

		if ((bundlesNumberVec_.size() != destinationEidVec_.size()) || (bundlesNumberVec_.size() != sizeVec_.size() || (bundlesNumberVec_.size() != startVec_.size())))
		{
			cout << "Node[" << this->getParentModule()->getIndex() << "]" << "bundlesNumberVec, destinationEidVec, sizeVec, startVec: sizes missmatch!" << endl;
			exit(1);
		}

		for (unsigned int i = 0; i < bundlesNumberVec_.size(); i++)
		{
			TrafficGeneratorMsg * trafficGenMsg = new TrafficGeneratorMsg("trafGenMsg");
			trafficGenMsg->setSchedulingPriority(0);
			trafficGenMsg->setKind(TRAFFIC_TIMER);
			trafficGenMsg->setBundlesNumber(bundlesNumberVec_.at(i));
			trafficGenMsg->setDestinationEid(destinationEidVec_.at(i));
			trafficGenMsg->setSize(sizeVec_.at(i));
			trafficGenMsg->setInterval(par("interval").doubleValue());
			trafficGenMsg->setTtl(par("ttl").doubleValue());
			scheduleAt(startVec_.at(i), trafficGenMsg);
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
		bundle->setSenderEid(this->eid_);
		bundle->setDestinationEid(trafficGenMsg->getDestinationEid());
		bundle->setBitLength(trafficGenMsg->getSize() * 8);
		bundle->setByteLength(trafficGenMsg->getSize());
		bundle->setTtl(trafficGenMsg->getTtl());
		bundle->setCreationTimestamp(simTime());
		bundle->setDlvConfidence(0);
		bundle->setReturnToSender(par("returnToSender"));
		bundle->setCritical(par("critical"));
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
			EV << "Bundle Received" << endl;
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

vector<int> App::getBundlesNumberVec()
{
	return this->bundlesNumberVec_;
}

vector<int> App::getDestinationEidVec()
{
	return this->destinationEidVec_;
}

vector<int> App::getSizeVec()
{
	return this->sizeVec_;
}

vector<double> App::getStartVec()
{
	return this->startVec_;
}
