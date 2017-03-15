#include <ion/Ion.h>

Define_Module(Ion);

Ion::Ion()
{

}

Ion::~Ion()
{

}

void Ion::initialize()
{
	int nodesNumber = this->getParentModule()->par("nodesNumber");

	ionNodes_ = false;
	for (int i = 0; i < nodesNumber; i++)
	{
		string routing = this->getParentModule()->getSubmodule("node", i)->getSubmodule("net")->par("routing");
		if (routing == "cgrIon350")
		{
			ionNodes_ = true;
			break;
		}
	}

	if (ionNodes_)
	{
		// erase old folders and processes
		bubble("Killing ION processes ...");
		system("rm -rf ion/ion_nodes");
		string command1 = "rm -rf ion/node*";
		system(command1.c_str());
		system("chmod +x ./ion/killm");
		system("./ion/killm");
	}
}

void Ion::handleMessage(cMessage *msg)
{

}

void Ion::finish()
{
	if (ionNodes_)
	{
		bubble("Killing ION processes ...");
		system("./ion/killm");
	}
}
