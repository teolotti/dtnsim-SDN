/*
 * CustodyModel.h
 *
 *  Created on: Dec 5, 2017
 *      Author: juanfraire
 */

#ifndef SRC_NODE_DTN_CUSTODYMODEL_H_
#define SRC_NODE_DTN_CUSTODYMODEL_H_

#include <dtn/SdrModel.h>
#include "MsgTypes.h"

//#include <dtn/Dtn.h>

class CustodyModel
{
public:
	CustodyModel();
	virtual ~CustodyModel();

	void setEid(int eid);
	void setSdr(SdrModel * sdr);
	//void setDtn(Dtn * dtn);

	BundlePkt * bundleWithCustodyRequestedArrived(BundlePkt * bundle);
	void custodyReportArrived(BundlePkt * bundle);

	BundlePkt * getNewCustodyReport(bool accept, BundlePkt * bundle);

private:
	int eid_;
	SdrModel * sdr_;
	//Dtn * dtn_;
};

#endif /* SRC_NODE_DTN_CUSTODYMODEL_H_ */
