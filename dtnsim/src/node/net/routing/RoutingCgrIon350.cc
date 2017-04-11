/*
 * RoutingCgrIon350.cpp
 *
 *  Created on: Nov 8, 2016
 *      Author: juanfraire
 */

#include "RoutingCgrIon350.h"

static void handleTraceState(void *data, unsigned int lineNbr, CgrTraceType traceType, va_list args);
static void traceFnDefault(void *data, unsigned int lineNbr, CgrTraceType traceType, ...);
static int getDirective(uvast nodeNbr, Object plans, Bundle *bundle, FwdDirective *directive);

RoutingCgrIon350::RoutingCgrIon350(int eid, SdrModel * sdr, ContactPlan * contactPlan, int nodesNumber)
{
	// Do nothing for eid=0 (unnused in ion)
	if(eid==0){
		return;
	}

	eid_ = eid;
	sdr_ = sdr;
	contactPlan_ = contactPlan;
	nodesNumber_ = nodesNumber;

	this->initializeIonNode();
}

RoutingCgrIon350::~RoutingCgrIon350()
{

}

void RoutingCgrIon350::initializeIonNode()
{
	// set environment variable to allow one ion node per folder
	chdir("ion");
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	setenv("ION_NODE_LIST_DIR", cwd, 1);

	this->startUtcTime_ = time(NULL);

	pid_t pid = fork();
	if (pid < 0)
	{
		perror("fork");
		exit(1);
	}
	else if (pid == 0)
	{
		//child
		ConfigInfo *configInfo = new ConfigInfo;
		configInfo->nodesNumber = nodesNumber_;
		configInfo->nodeEid = this->eid_;
		configInfo->path = "node" + to_string(this->eid_);
		string contactsFile = this->contactPlan_->getContactsFile();
		configInfo->contactsFile = string("../../") + contactsFile;
		configInfo->wmKeyStart = 66000;
		configInfo->portStart = 2110;

		string command = "mkdir " + configInfo->path;
		system(command.c_str());
		chdir((configInfo->path).c_str());

		createIonstartFile(configInfo);
		createIonrcFile(configInfo);
		createIonconfigFile(configInfo);
		createIonsecrcFile(configInfo);
		createLtprcFile(configInfo);
		createBprcFile(configInfo);
		createIonipnrcFile(configInfo);

		runIonadmin((char *) "ionrc");
		runIonadmin((char *) (configInfo->contactsFile).c_str());
		runBpadmin((char *) "bprc");
		//system("./ionstart");

		if (bp_attach() < 0)
		{
			return;
		}
		cgr_start();
		bp_detach();

		delete configInfo;
		exit(0);
	}
	else
	{
		// Parent process waits here for child to terminate.
		int returnStatus;
		waitpid(pid, &returnStatus, 0);

		chdir("../");
	}
}

void RoutingCgrIon350::createIonstartFile(ConfigInfo *configInfo)
{
	string contactsFile = configInfo->contactsFile;

	ofstream file;
	file.open("ionstart");

	file << "# shell script to get node running" << endl;
	file << "#!/bin/bash" << endl;
	file << "ionadmin ionrc" << endl;
	file << "ionadmin " + contactsFile << endl;
	file << "ionsecadmin ionsecrc" << endl;
	file << "ltpadmin ltprc" << endl;
	file << "bpadmin bprc" << endl;
	file.close();

	system((string("chmod +x ") + string("ionstart")).c_str());
}

void RoutingCgrIon350::createIonrcFile(ConfigInfo *configInfo)
{
	int nodeEid = configInfo->nodeEid;
	string eid = to_string(nodeEid);

	ofstream file;
	file.open("ionrc");

	file << "1 " + eid + " ionconfig" << endl;
	//file << "s" << endl;
	//file << "m horizon +0" << endl;
	file.close();
}

void RoutingCgrIon350::createIonconfigFile(ConfigInfo *configInfo)
{
	int nodeEid = configInfo->nodeEid;
	int wmKey = configInfo->wmKeyStart + nodeEid;

	string eid = to_string(nodeEid);
	string wmKeyStr = to_string(wmKey);

	ofstream file;
	file.open("ionconfig");

	file << "wmKey " + wmKeyStr << endl;
	file << "sdrName sdr" + eid << endl;
	file << "wmSize 5000000" << endl;
	file << "configFlags 1" << endl;
	file << "heapWords 200000" << endl;
	file.close();
}

void RoutingCgrIon350::createIonsecrcFile(ConfigInfo *configInfo)
{
	ofstream file;
	file.open("ionsecrc");

	file << "1" << endl;
	file.close();
}

void RoutingCgrIon350::createLtprcFile(ConfigInfo *configInfo)
{
	int nodesNumber = configInfo->nodesNumber;
	int nodeEid = configInfo->nodeEid;
	int port = configInfo->portStart + nodeEid;
	string eid = to_string(nodeEid);

	ofstream file;

	file.open("ltprc");

	file << "1 100" << endl;

	for (int i = 1; i <= nodesNumber; i++)
	{
		if (i != nodeEid)
		{
			string neighborEid = to_string(i);
			string portToNeighbor = to_string(configInfo->portStart + i);
			file << "a span " + neighborEid + " 100 100 64000 100000 1 'udplso localhost:" + portToNeighbor + " 10000000000'" << endl;
		}
	}

	file << "s 'udplsi localhost:" + to_string(port) + "'" << endl;
	file.close();
}

void RoutingCgrIon350::createBprcFile(ConfigInfo *configInfo)
{
	int nodesNumber = configInfo->nodesNumber;
	int nodeEid = configInfo->nodeEid;
	string eid = to_string(nodeEid);

	ofstream file;
	file.open("bprc");

	file << "1" << endl;
	file << "a scheme ipn 'ipnfw' 'ipnadminep'" << endl;
	file << "a endpoint ipn:" + eid + ".0 x" << endl;
	file << "a endpoint ipn:" + eid + ".1 x" << endl;
	file << "a endpoint ipn:" + eid + ".2 x" << endl;
	file << "a endpoint ipn:" + eid + ".64 x" << endl;
	file << "a endpoint ipn:" + eid + ".65 x" << endl;
	file << "a protocol ltp 1400 100" << endl;

	file << "a induct ltp " + eid + " ltpcli" << endl;

	for (int i = 1; i <= nodesNumber; i++)
	{
		if (i != nodeEid)
		{
			file << "a outduct ltp " + to_string(i) + " ltpclo" << endl;
		}
	}

	file << "r 'ipnadmin ipnrc'" << endl;
	file << "s" << endl;
	file.close();
}

void RoutingCgrIon350::createIonipnrcFile(ConfigInfo *configInfo)
{
	int nodesNumber = configInfo->nodesNumber;
	int nodeEid = configInfo->nodeEid;
	string eid = to_string(nodeEid);

	ofstream file;
	file.open("ipnrc");

	for (int i = 1; i <= nodesNumber; i++)
	{
		if (i != nodeEid)
		{
			string neighborEid = to_string(i);
			string portToNeighbor = to_string(configInfo->portStart + i);
			file << "a plan " + neighborEid + " ltp/" + neighborEid << endl;
		}
	}

	file.close();
}

static void handleTraceState(void *data, unsigned int lineNbr, CgrTraceType traceType, va_list args)
{
	TraceState *traceState = (TraceState *) data;

	switch (traceType)
	{
	case CgrUseProximateNode:
		traceState->selectedNode = va_arg(args, uvast);
		traceState->hops = va_arg(args, PsmAddress);
		break;

	default:
		break;
	}
}

static void traceFnDefault(void *data, unsigned int lineNbr, CgrTraceType traceType, ...)
{
	va_list args;

	va_start(args, traceType);
	handleTraceState(data, lineNbr, traceType, args);
	va_end(args);
}

// Callback function used by cgr
static int getDirective(uvast nodeNbr, Object plans, Bundle *bundle, FwdDirective *directive)
{
	char *outductProto = (char *) "ltp";
	char *outductName = (char *) (to_string(nodeNbr)).c_str();

	PsmAddress vductElt;
	VOutduct *vduct;
	findOutduct(outductProto, outductName, &vduct, &vductElt);

	directive->outductElt = vduct->outductElt;

	return 1;
}

void RoutingCgrIon350::routeBundle(BundlePkt * bundlePkt, double simTime)
{
	// sets global UTC time according to the simulation offset for CGR in ION
	time_t simTimeUtc = getUtcSimulationTime(simTime);
	_globalUtcTime_ = simTimeUtc;
	// sets sdrStatus so CGR can take into account previous forwardings
	// it is used in computeArrivalTime() - computePriorClaims() methods
	_sdrStatus_ = sdr_->getSdrStatus();

	typedef struct
	{
		int neighborNode;
		int contactId;
	} CgrResult;

	int mem_id;
	mem_id = shmget(IPC_PRIVATE, sizeof(CgrResult), SHM_R | SHM_W);
	CgrResult *cgrResult;

	pid_t pid = fork();
	switch (pid)
	{
	case -1:
	{
		perror("fork");
		exit(1);
	}
	case 0:
	{
		//child
		cgrResult = (CgrResult *) shmat(mem_id, NULL, 0);
		if ((void *) -1 == (void *) cgrResult)
		{
			perror("Child cannot attach");
			exit(1);
		}
		cgrResult->contactId = -1;
		cgrResult->neighborNode = -1;

		string dir = string("ion/node") + to_string(this->eid_);
		chdir(dir.c_str());

		char cwd[1024];
		getcwd(cwd, sizeof(cwd));

		if (bp_attach() < 0)
		{
			cout << "unable to attach to ion" << endl;
			return;
		}

		uvast localNode = getOwnNodeNbr();
		uvast destNode;
		char *end;
		destNode = strtoul((to_string(bundlePkt->getDestinationEid())).c_str(), &end, 10);

		time_t dispatchOffset = 0;
		time_t expirationOffset = 100000000;
		time_t nowTime = simTimeUtc;
		time_t expirationTime = nowTime + expirationOffset;

		unsigned int bundleSize = bundlePkt->getByteLength();

		Object plans;

		Bundle bundle =
		{ };
		bundle.extendedCOS.flags = BP_BEST_EFFORT;
		bundle.payload.length = bundleSize;
		bundle.returnToSender = 0;
		bundle.clDossier.senderNodeNbr = localNode;
		bundle.expirationTime = expirationTime;
		bundle.dictionaryLength = 0;
		bundle.extensionsLength[0] = 0;
		bundle.extensionsLength[1] = 0;
		bundle.id.source.c.nodeNbr = 0;
		bundle.destination.c.nodeNbr = 0;

		CgrTrace trace;
		TraceState traceStateSt;
		traceStateSt.selectedNode = -1;
		CgrTraceFn traceFn = traceFnDefault;
		trace.fn = traceFn;
		trace.data = (void *) &traceStateSt;

		//cout<<"calling cgr_forward from node "<<localNode<<" to node "<<destNode<<endl;
		if (cgr_forward(&bundle, (Object) (&bundle), destNode, (Object) (&plans), getDirective, &trace) < 0)
		{
			cout << "unable to simulate cgr" << endl;
			return;
		}

		int neighborNodeNbr = traceStateSt.selectedNode;
		if (neighborNodeNbr != -1)
		{
			PsmPartition ionwm = getIonwm();
			IonCXref *firstContact = (IonCXref *) psp(ionwm, sm_list_data(ionwm, sm_list_first(ionwm, traceStateSt.hops)));
			cgrResult->contactId = firstContact->id;
			cgrResult->neighborNode = neighborNodeNbr;
		}

		bp_detach();
		shmdt(cgrResult);
		chdir("../../");
		exit(0);
	}
	default:
	{
		cgrResult = (CgrResult *) shmat(mem_id, NULL, 0);
		if ((void *) cgrResult == (void *) -1)
		{
			perror("Child cannot attach");
			exit(1);
		}

		// Parent process waits here for child to terminate.
		int returnStatus;
		waitpid(pid, &returnStatus, 0);

		if ((cgrResult->neighborNode != -1) && (cgrResult->contactId != -1))
		{
			//cout << "cgr enqueue to " << cgrResult->neighborNode << " in contact " << cgrResult->contactId << endl;
			cgrEnqueue(bundlePkt, cgrResult->neighborNode, cgrResult->contactId);
		}
		else
		{
			// enqueue to limbo
			cgrEnqueue(bundlePkt, 0, 0);
		}

		shmdt(cgrResult);
		if (shmctl(mem_id, IPC_RMID, 0) < 0)
		{
			perror("cannot remove shared memory");
			exit(1);
		}
	}
	}
}

void RoutingCgrIon350::cgrEnqueue(BundlePkt * bundle, int neighborNodeNbr, int contactId)
{
	bundle->setNextHopEid(neighborNodeNbr);
	sdr_->enqueueBundleToContact(bundle, contactId);
}

time_t RoutingCgrIon350::getUtcSimulationTime(double simTime)
{
	time_t utcSimTime = this->startUtcTime_ + simTime;
	return utcSimTime;
}
