#include <ion/Ion.h>

Define_Module(Ion);

Ion::Ion()
{
	nodesNumber_ = 0;
	portStart_ = 0;
	wmKeyStart_ = 0;
}

Ion::~Ion()
{

}

void Ion::initialize()
{
	this->portStart_ = 2110;
	this->wmKeyStart_ = 66000;

	// erase old folders
	system("rm -f ion/ion_nodes");
	system("rm -f ion/mainScript.sh");
	string command1 = "rm -rf ion/node*";
	system(command1.c_str());

	nodesNumber_ = this->getParentModule()->par("nodesNumber");

	// fill ports for each node
	for (int i = 1; i <= nodesNumber_; i++)
	{
		ports_[i] = this->portStart_ + i;
	}

	// create a new folder for each ion node
	for (int i = 1; i <= nodesNumber_; i++)
	{
		string command2 = "mkdir ion/node" + to_string(i);
		system(command2.c_str());

		this->createIonstartFile(i);
		this->createIonstopFile(i);
		this->createIonrcFile(i);
		this->createIonconfigFile(i);
		this->createIonsecrcFile(i);
		this->createLtprcFile(i);
		this->createBprcFile(i);
		this->createIonipnrcFile(i);
	}

	IonTrafficGeneratorMsg * ionTrafficGenMsg = new IonTrafficGeneratorMsg("ionTrafficGenMsg");
	ionTrafficGenMsg->setSchedulingPriority(5);
	scheduleAt(simTime(), ionTrafficGenMsg);
}

void Ion::handleMessage(cMessage *msg)
{
	if (check_and_cast<IonTrafficGeneratorMsg *>(msg))
	{
		this->createMainScript();
	}

	delete msg;
}

void Ion::finish()
{

}

void Ion::createIonstartFile(int nodeEid)
{
	string eid = to_string(nodeEid);

	ofstream file;
	string location = "ion/node" + eid;
	file.open((location + "/ionstart").c_str());

	int omnetNode = nodeEid - 1;
	string contactsFileFullPath = this->getParentModule()->getSubmodule("node", omnetNode)->getSubmodule("net")->par("contactsFile");
	string contactsFile = "../../" + contactsFileFullPath;

	file << "# shell script to get node running" << endl;
	file << "#!/bin/bash" << endl;
	file << "ionadmin ionrc" << endl;
	file << "sleep 1" << endl;
	file << "ionadmin " + contactsFile << endl;
	file << "sleep 1" << endl;
	file << "ionsecadmin ionsecrc" << endl;
	file << "sleep 1" << endl;
	file << "ltpadmin ltprc" << endl;
	file << "sleep 1" << endl;
	file << "bpadmin bprc" << endl;
	file.close();

	system(("chmod +x " + location + "/ionstart").c_str());
}

void Ion::createIonstopFile(int nodeEid)
{
	string eid = to_string(nodeEid);

	ofstream file;
	string location = "ion/node" + eid;
	file.open((location + "/ionstop").c_str());

	file << "# shell script to remove all of my IPC keys" << endl;
	file << "#!/bin/bash" << endl;
	file << "bpadmin ." << endl;
	file << "sleep 1" << endl;
	file << "ionadmin ." << endl;
	file.close();

	system(("chmod +x " + location + "/ionstop").c_str());
}

void Ion::createIonrcFile(int nodeEid)
{
	string eid = to_string(nodeEid);

	ofstream file;
	string location = "ion/node" + eid;
	file.open((location + "/ionrc").c_str());

	file << "1 " + eid + " ionconfig" << endl;
	file << "s" << endl;
	file << "m horizon +0" << endl;
	file.close();
}

void Ion::createIonconfigFile(int nodeEid)
{
	string eid = to_string(nodeEid);
	string wmKey = to_string(this->wmKeyStart_ + nodeEid);

	ofstream file;
	string location = "ion/node" + eid;
	file.open((location + "/ionconfig").c_str());

	file << "wmKey " + wmKey << endl;
	file << "sdrName ion" + eid << endl;
	file << "wmSize 5000000" << endl;
	file << "configFlags 1" << endl;
	file << "heapWords 20000000" << endl;
	file.close();
}

void Ion::createIonsecrcFile(int nodeEid)
{
	string eid = to_string(nodeEid);

	ofstream file;
	string location = "ion/node" + eid;
	file.open((location + "/ionsecrc").c_str());

	file << "1" << endl;
	file.close();
}

void Ion::createLtprcFile(int nodeEid)
{
	string eid = to_string(nodeEid);

	ofstream file;
	string location = "ion/node" + eid;
	file.open((location + "/ltprc").c_str());

	file << "1 100" << endl;

	for (int i = 1; i <= nodesNumber_; i++)
	{
		if (i != nodeEid)
		{
			string neighborEid = to_string(i);
			string portToNeighbor = to_string(ports_[i]);
			file << "a span " + neighborEid + " 100 100 64000 100000 1 'udplso localhost:" + portToNeighbor + " 10000000000'" << endl;
		}
	}

	file << "s 'udplsi localhost:" + to_string(ports_[nodeEid]) + "'" << endl;
	file.close();
}

void Ion::createBprcFile(int nodeEid)
{
	string eid = to_string(nodeEid);

	ofstream file;
	string location = "ion/node" + eid;
	file.open((location + "/bprc").c_str());

	string port = to_string(ports_[nodeEid]);

	file << "1" << endl;
	file << "a scheme ipn 'ipnfw' 'ipnadminep'" << endl;
	file << "a endpoint ipn:" + eid + ".0 x" << endl;
	file << "a endpoint ipn:" + eid + ".1 x" << endl;
	file << "a endpoint ipn:" + eid + ".2 x" << endl;
	file << "a endpoint ipn:" + eid + ".64 x" << endl;
	file << "a endpoint ipn:" + eid + ".65 x" << endl;
	file << "a protocol ltp 1400 100" << endl;

	file << "a induct ltp " + eid + " ltpcli" << endl;

	for (int i = 1; i <= nodesNumber_; i++)
	{
		if (i != nodeEid)
		{
			file << "a outduct ltp "+ to_string(i) + " ltpclo" << endl;
		}
	}

	file << "r 'ipnadmin ipnrc'" << endl;
	file << "s" << endl;
	file.close();
}

void Ion::createIonipnrcFile(int nodeEid)
{
	string eid = to_string(nodeEid);

	ofstream file;
	string location = "ion/node" + eid;
	file.open((location + "/ipnrc").c_str());

	for (int i = 1; i <= nodesNumber_; i++)
	{
		if (i != nodeEid)
		{
			string neighborEid = to_string(i);
			string portToNeighbor = to_string(ports_[i]);
			file << "a plan " + neighborEid + " ltp/" + neighborEid << endl;
		}
	}

	file.close();
}

void Ion::createMainScript()
{
	ofstream file;
	string location = "ion/";
	file.open((location + "/mainScript").c_str());

	file << "killm" << endl;
	file << "rm -f ion_nodes" << endl;
	file << "sleep 1" << endl;
	file << "export ION_NODE_LIST_DIR=$PWD" << endl;
	file << endl;

	file << "echo \"staring nodes\"" << endl;
	// initialize ION nodes
	for (int i = 1; i <= nodesNumber_; i++)
	{
		string eid = to_string(i);
		file << "cd node" + eid << endl;
		file << "./ionstart" << endl;
		file << "cd .." << endl;
	}

	file << endl;

	// sum all omnet nodes generated traffic and add bpcounter application to the corresponding ION nodes

	// map (eid -> bundlesToReceive)
	map<int, int> bundlesToReceive;

	for (int j = 1; j <= nodesNumber_; j++)
	{
		int bundlesNumber = 0;
		for (int i = 1; i <= nodesNumber_; i++)
		{
			int omnetNode = i - 1;
			cModule *module = this->getParentModule()->getSubmodule("node", omnetNode)->getSubmodule("app");
			App *app = check_and_cast<App *>(module);

			if (!app->getBundlesNumberVec().empty())
			{
				if (app->getDestinationEidVec().at(0) == j)
				{
					bundlesNumber += app->getBundlesNumberVec().at(0);
				}
			}
		}

		string eid = to_string(j);

		if (bundlesNumber > 0)
		{
			file << "cd node" + eid << endl;
			file << "bpcounter ipn:" + eid + ".1 " + to_string(bundlesNumber) + " &" << endl;
			file << "cd .." << endl;
		}
	}

	file << endl;

	// generate traffic between ION nodes according to omnet nodes traffic
	for (int i = 1; i <= nodesNumber_; i++)
	{
		string eid = to_string(i);
		int omnetNode = i - 1;
		cModule *module = this->getParentModule()->getSubmodule("node", omnetNode)->getSubmodule("app");
		App *app = check_and_cast<App *>(module);

		if (!app->getBundlesNumberVec().empty())
		{
			file << "cd node" + eid << endl;

			string bundlesNumber = to_string(app->getBundlesNumberVec().at(0));
			string destinationEid = to_string(app->getDestinationEidVec().at(0));
			string size = to_string(app->getSizeVec().at(0));

			// todo see how can we generate traffic at specific times != t=0

			file << "bpdriver " + bundlesNumber + " ipn:" + eid + ".1 " + "ipn:" + destinationEid + ".1 " + "-" + size + " &" << endl;

			file << "cd .." << endl;
		}
	}

	file << endl;
	string seconds = "60";
	file << "echo \"waiting + " + seconds + " seconds\" " << endl;
	file << "sleep " + seconds << endl;
	file << endl;

	file << "echo \"stopping nodes\"" << endl;
	// stop ION nodes
	for (int i = 1; i <= nodesNumber_; i++)
	{
		string eid = to_string(i);
		file << "cd node" + eid << endl;
		file << "./ionstop &" << endl;
		file << "cd .." << endl;
	}

	file << "sleep " + 5 << endl;
	file << "killm" << endl;
	file << "echo \"ION terminated\"" << endl;

	file.close();

	system(("chmod +x " + location + "mainScript").c_str());
}

