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
		string routing = this->getParentModule()->getSubmodule("node", i)->getSubmodule("dtn")->par("routing");
		if (routing == "cgrIon350")
		{
			ionNodes_ = true;
			break;
		}
	}

	if (ionNodes_)
	{
		// create result folder if it doesn't exist
		struct stat st =
		{ 0 };
		if (stat("ionFiles", &st) == -1)
		{
			mkdir("ionFiles", 0700);
		}

		// erase old folders and processes
		bubble("Killing ION processes ...");
		system("rm -rf ../src/ion/ion_nodes");
		string command1 = "rm -rf ../src/ion/node*";
		system(command1.c_str());
		system("chmod +x ../src/ion/killm");
		system("../src/ion/killm");
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
		system("../src/ion/killm");
	}
}
