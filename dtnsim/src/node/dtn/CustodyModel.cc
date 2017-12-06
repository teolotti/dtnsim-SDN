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
		cout << "Node " << eid_ << " ** Custody accept for bundleId " << bundleInCustody->getBundleId() << " - ";
		custodyReport = this->getNewCustodyReport(true, bundleInCustody);
		bundleInCustody->setCustodianEid(eid_);
	}
	else
	{
		// Reject custody, send custody report to previous custodian
		cout << "Node " << eid_ << " ** Custody reject for bundleId " << bundleInCustody->getBundleId() << " - ";
		custodyReport = this->getNewCustodyReport(false, bundleInCustody);
	}
	return custodyReport;
}

void CustodyModel::custodyReportArrived(BundlePkt * custodyReport)
{
	if (custodyReport->getSourceEid() == eid_)
	{
		// I sent this report to myself, meaning I was the generator of the bundle in custody
		delete custodyReport;
		return;
	}

	if (custodyReport->getCustodyAccepted())
	{
		// Custody was accepted by remote node, release custodyBundleId
		cout << "Node " << eid_ << " ** Releasing custody of bundleId " << custodyReport->getCustodyBundleId() << endl;
		sdr_->removeTransmittedBundleInCustody(custodyReport->getCustodyBundleId());
	}
	else
	{
		// Custody was rejected by remote node,
		// TODO: reroute custodyBundleId
		cout << "Node " << eid_ <<  " ** NOT releasing custody of bundleId " << custodyReport->getCustodyBundleId() << " rejected... do something!" << endl;
	}
	delete custodyReport;
}

BundlePkt * CustodyModel::getNewCustodyReport(bool accept, BundlePkt *bundleInCustody)
{
	int custodyReportByteSize = 50;

	BundlePkt* custodyReport = new BundlePkt("custodyReport", BUNDLE);
	custodyReport->setSchedulingPriority(BUNDLE);

	// Bundle properties
	char bundleName[15];
	sprintf(bundleName, "Src:%d,Dst:%d(id:%d)", this->eid_, bundleInCustody->getCustodianEid(), (int) custodyReport->getId());
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

	// Custody fields
	custodyReport->setBundleIsCustodyReport(true);
	custodyReport->setCustodyBundleId(bundleInCustody->getBundleId());
	custodyReport->setCustodyAccepted(accept);

	// Bundle meta-data (set by intermediate nodes)
	custodyReport->setHopCount(0);
	custodyReport->setNextHopEid(0);
	custodyReport->setSenderEid(0);
	custodyReport->setCustodianEid(0);
	custodyReport->getVisitedNodes().clear();
	CgrRoute emptyRoute;
	emptyRoute.nextHop = EMPTY_ROUTE;
	custodyReport->setCgrRoute(emptyRoute);

	cout << "Sending report: " << bundleName << endl;

	return custodyReport;
}
