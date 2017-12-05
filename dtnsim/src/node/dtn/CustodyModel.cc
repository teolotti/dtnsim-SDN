/*
 * CustodyModel.cpp
 *
 *  Created on: Dec 5, 2017
 *      Author: juanfraire
 */

#include <CustodyModel.h>

CustodyModel::CustodyModel()
{
}

CustodyModel::~CustodyModel()
{
}

void CustodyModel::setEid(int eid)
{
	this->eid_ = eid;
}

void CustodyModel::setSdr(SdrModel * sdr)
{
	this->sdr_ = sdr;
}

//void CustodyModel::setDtn(Dtn * dtn)
//{
//	this->dtn_ = dtn;
//}

BundlePkt * CustodyModel::bundleWithCustodyRequestedArrived(BundlePkt * bundleInCustody)
{
	BundlePkt * custodyReport;

	if (sdr_->isSdrFreeSpace(bundleInCustody->getByteLength()) || this->eid_ == bundleInCustody->getDestinationEid())
	{
		// Accept custody, send custody report to previous custodian
		cout << "Custody accept" << endl;
		custodyReport = this->getNewCustodyReport(true, bundleInCustody);
		bundleInCustody->setCustodianEid(eid_);
	}
	else
	{
		// Reject custody, send custody report to previous custodian
		cout << "Custody reject" << endl;
		custodyReport = this->getNewCustodyReport(false, bundleInCustody);
	}
	return custodyReport;
}

void CustodyModel::custodyReportArrived(BundlePkt * bundle)
{

	if (bundle->getCustodyAccepted())
	{
		// Custody was accepted by remote node, release custodyBundleId


	}
	else
	{
		// Custody was rejected by remote node, reroute custodyBundleId

	}
	delete bundle;
}

BundlePkt * CustodyModel::getNewCustodyReport(bool accept, BundlePkt *bundleInCustody)
{
	int custodyReportByteSize = 50;

	BundlePkt* custodyReport = new BundlePkt("custodyReport", BUNDLE);
	custodyReport->setSchedulingPriority(BUNDLE);

	// Bundle properties
	char bundleName[10];
	sprintf(bundleName, "Src:%d,Dst:%d(id:%d)", this->eid_, bundleInCustody->getCustodianEid(), (int) custodyReport->getId());

	cout << "Report: " << bundleName << endl;

	custodyReport->setBundleId(custodyReport->getId());
	custodyReport->setName(bundleName);
	custodyReport->setBitLength(custodyReportByteSize * 8);
	custodyReport->setByteLength(custodyReportByteSize);

	// Bundle fields (set by source node)
	custodyReport->setSourceEid(this->eid_);
	custodyReport->setDestinationEid(bundleInCustody->getCustodianEid());
	custodyReport->setReturnToSender(false);
	custodyReport->setCritical(false);
	custodyReport->setCustodyTransferRequested(false);
	custodyReport->setTtl(9000000);
	custodyReport->setCreationTimestamp(simTime());
	custodyReport->setQos(2);

	custodyReport->setBundleIsCustodyReport(true);
	custodyReport->setCustodyBundleId(bundleInCustody->getBundleId());

	// Bundle meta-data (set by intermediate nodes)
	custodyReport->setHopCount(0);
	custodyReport->setNextHopEid(0);
	custodyReport->setSenderEid(0);
	custodyReport->setCustodianEid(0);
	custodyReport->getVisitedNodes().clear();
	CgrRoute emptyRoute;
	emptyRoute.nextHop = EMPTY_ROUTE;
	custodyReport->setCgrRoute(emptyRoute);

	return custodyReport;
}
