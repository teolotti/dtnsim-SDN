#include "Logger.h"

Define_Module (dtnsim::Logger);

namespace dtnsim
{

Logger::Logger()
{

}

Logger::~Logger()
{
}

void Logger::initialize()
{
    if (par("saveTopology") || par("saveFlows"))
    {
        string contactsFile = par("contactsFile");
        contactPlan_.parseContactPlanFile(contactsFile);
        nodesNumber_ = this->getParentModule()->par("nodesNumber");

        // check that contactPlan passed to Logger be the same as the one passed to the other nodes
        for (int i = 1; i <= nodesNumber_; i++)
        {
            Net *net = check_and_cast<Net *>(this->getParentModule()->getSubmodule("node", i)->getSubmodule("net"));
            string contactsFileNi = net->par("contactsFile");

            if (contactsFile != contactsFileNi)
            {
                string str1 = "Contacts File passed to Logger: " + contactsFile;
                string str2 = "is different from Contacts File passed to node " + to_string(i) + ": " + contactsFileNi;
                throw cException(("Error: " + str1 + " " + str2).c_str());
            }
        }
    }
}

void Logger::finish()
{
    if (this->par("saveTopology"))
    {
        this->saveTopology();
    }

    if (this->par("saveFlows"))
    {
        this->saveFlows();
    }
}

void Logger::computeFlowIds()
{
    int flowId = 0;

    for (int i = 1; i <= nodesNumber_; i++)
    {
        App *app = check_and_cast<App *>(this->getParentModule()->getSubmodule("node", i)->getSubmodule("app"));

        int src = i;
        vector<int> dsts = app->getDestinationEidVec();

        vector<int>::iterator it1 = dsts.begin();
        vector<int>::iterator it2 = dsts.end();
        for (; it1 != it2; ++it1)
        {
            int dst = *it1;
            pair<int, int> pSrcDst(src, dst);

            map<pair<int, int>, unsigned int>::iterator iit = flowIds_.find(pSrcDst);
            if (iit == flowIds_.end())
            {
                flowIds_[pSrcDst] = flowId++;
            }
        }
    }
}

void Logger::saveTopology()
{
#ifdef USE_BOOST_LIBRARIES
    map<double, TopologyGraph *> topology = topologyUtils::computeTopology(&this->contactPlan_, nodesNumber_);
    topologyUtils::printGraphs(&topology, "results/topology.dot");

    map<double, TopologyGraph*>::iterator it1 = topology.begin();
    map<double, TopologyGraph*>::iterator it2 = topology.end();
    for (; it1 != it2; ++it1)
    {
        delete it1->second;
    }

    topology.clear();
#endif
}

void Logger::saveFlows()
{
#ifdef USE_BOOST_LIBRARIES
    this->computeFlowIds();
    vector < string > dotColors = routerUtils::getDotColors();
    map<double, RouterGraph *> flows = routerUtils::computeFlows(&this->contactPlan_, nodesNumber_, "results");
    routerUtils::printGraphs(&flows, dotColors, flowIds_, "results/flows.dot");
#endif
}

}

