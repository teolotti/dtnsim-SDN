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
	if (par("saveTopology") || par("saveFlows") || par("saveLpFlows"))
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

	if (this->par("saveLpFlows"))
	{
		this->saveLpFlows();
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
	map<double, TopologyGraph> topology = topologyUtils::computeTopology(&this->contactPlan_, nodesNumber_);
	topologyUtils::saveGraphs(&topology, "results/topology.dot");
#endif
}

void Logger::saveFlows()
{
#ifdef USE_BOOST_LIBRARIES
	this->computeFlowIds();
	vector < string > dotColors = routerUtils::getDotColors();
	map<double, RouterGraph> flows = routerUtils::computeFlows(&this->contactPlan_, nodesNumber_, "results");
	routerUtils::saveGraphs(&this->contactPlan_, &flows, dotColors, flowIds_, "results/flows.dot");
#endif
}

void Logger::saveLpFlows()
{
#ifdef USE_BOOST_LIBRARIES
#ifdef USE_CPLEX_LIBRARY
	this->computeFlowIds();
	vector < string > dotColors = routerUtils::getDotColors();

	// traffic[k][k1][k2] generated in state k by commodity k1-k2
	map<int, map<int, map<int, double> > > traffic = getTraffics();

	Lp lp(&this->contactPlan_, nodesNumber_, traffic);

	lp.exportModel("results/lpModel");
	bool solved = lp.solve();

	if (solved)
	{
		map<double, RouterGraph> flows = lpUtils::computeFlows(&this->contactPlan_, nodesNumber_, &lp);
		routerUtils::saveGraphs(&this->contactPlan_, &flows, dotColors, flowIds_, "results/lpFlows.dot");
	}

#endif
#endif
}

map<int, map<int, map<int, double> > > Logger::getTraffics()
{
	/// traffic[k][k1][k2] tráfico generado en el estado k desde el nodo k1 hacia el nodo k2
	map<int, map<int, map<int, double> > >traffics;

	for (int i = 1; i <= nodesNumber_; i++)
	{
		App *app = check_and_cast<App *>(this->getParentModule()->getSubmodule("node", i)->getSubmodule("app"));

		int src = i;

        vector<int> bundlesNumberVec = app->getBundlesNumberVec();
        vector<int> destinationEidVec = app->getDestinationEidVec();
        vector<int> sizeVec = app->getSizeVec();
        vector<double> startVec = app->getStartVec();

		for (size_t j = 0; j<bundlesNumberVec.size(); j++)
		{
			double stateStart = this->getState(startVec.at(j));

			int dst = destinationEidVec.at(j);

			double totalSize = bundlesNumberVec.at(j) * sizeVec.at(j);

			map<int, map<int, map<int, double> > >::iterator it1 = traffics.find(stateStart);
			if(it1 != traffics.end())
			{
				map<int, map<int, double> > m1 = it1->second;
				map<int, map<int, double> > ::iterator it2 = m1.find(src);

				if(it2 != m1.end())
				{
					map<int, double> m2 = it2->second;
					map<int, double>::iterator it3 = m2.find(dst);

					if(it3 != m2.end())
					{
						m2[dst] += totalSize;
					}
					else
					{
						m2[dst] = totalSize;
					}
					m1[src] = m2;
				}
				else
				{
					map<int, double> m2;
					m2[dst] = totalSize;
					m1[src] = m2;
				}
				traffics[stateStart] = m1;
			}
			else
			{
				map<int, map<int, double> > m1;
				m1[src][dst] = totalSize;
				traffics[stateStart] = m1;
			}
		}
	}

	return traffics;
}

double Logger::getState(double trafficStart)
{
	// compute state times
	vector<double> stateTimes;
	stateTimes.push_back(0);

	vector<Contact> *contacts = contactPlan_.getContacts();
	vector<Contact>::iterator it1 = contacts->begin();
	vector<Contact>::iterator it2 = contacts->end();
	for (; it1 != it2; ++it1)
	{
		Contact contact = *it1;

		stateTimes.push_back(contact.getStart());
		stateTimes.push_back(contact.getEnd());

		std::sort(stateTimes.begin(), stateTimes.end());
		stateTimes.erase(std::unique(stateTimes.begin(), stateTimes.end()), stateTimes.end());
	}

	// get state of traffic generation
    double state = 0;
    vector<double>::iterator iit1 = stateTimes.begin();
    vector<double>::iterator iit2 = stateTimes.end();
    for (; iit1 != iit2; ++iit1)
    {
        double stateStart = *iit1;
        double stateEnd = stateTimes.back();

        ++iit1;
        if (iit1 != iit2)
        {
            stateEnd = *iit1;
        }
        --iit1;

        if (trafficStart >= stateStart && trafficStart < stateEnd)
        {
            state = stateStart;
        }
    }

    return state;
}

}

