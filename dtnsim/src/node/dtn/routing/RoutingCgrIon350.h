
#ifndef SRC_NODE_DTN_ROUTINGCGRION350_H_
#define SRC_NODE_DTN_ROUTINGCGRION350_H_

#define CGR_DEBUG 1

#include <dtn/routing/Routing.h>
#include "bp/include/cgr.h"
#include "ici/include/psm.h"
#include "ici/include/ion.h"
#include <map>
#include <omnetpp.h>
#include <fstream>
#include "dtnsim_m.h"
#include "ionadmin.h"
#include "bpadmin.h"
#include <dtn/SdrStatus.h>


typedef struct
{
	int nodesNumber;
	int nodeEid;
	string path;
	string contactsFile;
	int wmKeyStart;
	int portStart;
} ConfigInfo;

typedef struct {
	uvast selectedNode;
	PsmAddress	hops; // contacts of the best route
} TraceState;

class RoutingCgrIon350 : public Routing
{
public:
	RoutingCgrIon350(int eid, SdrModel * sdr, ContactPlan * contactPlan, int nodesNumber);
	virtual ~RoutingCgrIon350();
	virtual void routeAndQueueBundle(BundlePkt *bundle, double simTime);
	virtual void cgrEnqueue(BundlePkt * bundle, int neighborNodeNbr, int contactId);

	virtual void initializeIonNode();
	virtual void createIonstartFile(ConfigInfo *configInfo);
	virtual void createIonrcFile(ConfigInfo *configInfo);
	virtual void createIonconfigFile(ConfigInfo *configInfo);
	virtual void createIonsecrcFile(ConfigInfo *configInfo);
	virtual void createLtprcFile(ConfigInfo *configInfo);
	virtual void createBprcFile(ConfigInfo *configInfo);
	virtual void createIonipnrcFile(ConfigInfo *configInfo);
	virtual time_t getUtcSimulationTime(double simTime);


private:

	int eid_;
	SdrModel * sdr_;
	ContactPlan * contactPlan_;
	time_t startUtcTime_;
	int nodesNumber_;

};


#endif /* SRC_NODE_DTN_ROUTINGCGRION350_H_ */
