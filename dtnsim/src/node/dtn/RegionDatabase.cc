#include <src/node/dtn/RegionDatabase.h>

RegionDatabase::~RegionDatabase()
{

}

RegionDatabase::RegionDatabase()
{

}

void RegionDatabase::parseRegionDatabaseFile(string fileName)
{
	string regionId = "";

	string fileLine = "#";
	string a;
	string command;
	ifstream file;

	file.open(fileName.c_str());

	while (getline(file, fileLine))
	{
		if (fileLine.empty())
			continue;

		if (fileLine.at(0) == '#')
			continue;

		// This seems to be a valid command line, parse it
		stringstream stringLine(fileLine);

		readLine(fileLine);
	}

	if (cin.bad())
	{
		// IO error
	}
	else if (!cin.eof())
	{
		// format error (not possible with getline but possible with operator>>)
	}
	else
	{
		// format error (not possible with getline but possible with operator>>)
		// or end of file (can't make the difference)
	}

	file.close();

	this->setRegionsFile(fileName);
	this->populateVents();
}

void RegionDatabase::readLine(const string & s)
{
	//cout<<"reading one line"<<endl;

	istringstream is(s);

	string a;
	string command;

	is >> a;
	if (a.compare("a") == 0)
	{
		is >> command;

		if ((command.compare("node") == 0))
		{
			int nodeEid;
			string regionId;
			bool rx = false;
			bool tx = false;

			is >> nodeEid >> regionId;

			string str;
			while (is >> str)
			{
				if (str == "rx")
				{
					rx = true;
				}
				if (str == "tx")
				{
					tx = true;
				}
			}

			addNode(nodeEid, regionId, rx, tx);

			return;

		}
		else
		{
			cout << "region plan warning: unknown command type in line: " << s << endl;
		}
	}
	else
	{
		cout << "region plan warning: unknown command type in line: " << s << endl;
	}

	return;
}

// Region plan population functions
void RegionDatabase::addNode(int nodeEid, string regionId, bool rx, bool tx)
{
	if (rx)
	{
		std::map<int, set<string> >::iterator it = nodesRxRegions_.find(nodeEid);

		// if a nodeEid is not in the map
		// assign region and add to the map
		if (it == nodesRxRegions_.end())
		{
			set < string > regions;
			regions.insert(regionId);
			nodesRxRegions_[nodeEid] = regions;
		}
		// else add region to existing regions of the node
		else
		{
			set < string > regions = it->second;
			regions.insert(regionId);
			it->second = regions;
		}
	}

	if (tx)
	{
		std::map<int, set<string> >::iterator it2 = nodesTxRegions_.find(nodeEid);

		// if a nodeEid is not in the map
		// assign region and add to the map
		if (it2 == nodesTxRegions_.end())
		{
			set < string > regions;
			regions.insert(regionId);
			nodesTxRegions_[nodeEid] = regions;
		}
		// else add region to existing regions of the node
		else
		{
			set < string > regions = it2->second;
			regions.insert(regionId);
			it2->second = regions;
		}
	}
}

// vents can be added from the other information.
// When a node can receive bundles in region A and transmit bundles in region B, then it has a directional vent A->B
void RegionDatabase::populateVents()
{
	for (auto it1 = nodesRxRegions_.begin(); it1 != nodesRxRegions_.end(); ++it1)
	{
		int nodeEid = it1->first;
		set < string > rxRegions = it1->second;

//		for (auto ii1 = rxRegions.begin(); ii1 != rxRegions.end(); ++ii1)
//		{
//			cout << *ii1 << " ";
//		}
//		cout << endl;

		auto it2 = nodesTxRegions_.find(nodeEid);
		if (it2 != nodesTxRegions_.end())
		{
			set < string > txRegions = it2->second;

//			for (auto ii2 = txRegions.begin(); ii2 != txRegions.end(); ++ii2)
//			{
//				cout << *ii2 << " ";
//			}
//			cout << endl;

			set < pair<string, string> > combinations = getCombinations(rxRegions, txRegions);
			for (auto it3 = combinations.begin(); it3 != combinations.end(); ++it3)
			{
				auto it4 = nodesVents_.find(nodeEid);
				if (it4 != nodesVents_.end())
				{
					it4->second.insert(*it3);
				}
				else
				{
					set < pair<string, string> > vents;
					vents.insert(*it3);
					nodesVents_[nodeEid] = vents;
				}
			}
		}
	}
}

set<pair<string, string> > RegionDatabase::getCombinations(set<string> s1, set<string> s2)
{
	set < pair<string, string> > combinations;

	for (auto it1 = s1.begin(); it1 != s1.end(); ++it1)
	{
		string rxRegion = *it1;

		for (auto it2 = s2.begin(); it2 != s2.end(); ++it2)
		{
			string txRegion = *it2;

			if (rxRegion != txRegion)
			{
				combinations.insert(make_pair(rxRegion, txRegion));
			}
		}
	}

	return combinations;
}

// debug function
void RegionDatabase::printRegionDatabase()
{
	cout << "RX Regions:" << endl;
	for (map<int, set<string> >::iterator it1 = nodesRxRegions_.begin(); it1 != nodesRxRegions_.end(); ++it1)
	{
		int nodeEid = it1->first;
		set < string > regions = it1->second;

		cout << nodeEid << " ";

		for (auto it = regions.begin(); it != regions.end(); ++it)
		{
			cout << *it << " ";
		}
		cout << endl;
	}

	cout << "TX Regions:" << endl;
	for (map<int, set<string> >::iterator it1 = nodesTxRegions_.begin(); it1 != nodesTxRegions_.end(); ++it1)
	{
		int nodeEid = it1->first;
		set < string > regions = it1->second;

		cout << nodeEid << " ";

		for (auto it = regions.begin(); it != regions.end(); ++it)
		{
			cout << *it << " ";
		}
		cout << endl;
	}

	cout << "vents:" << endl;
	for (auto it1 = nodesVents_.begin(); it1 != nodesVents_.end(); ++it1)
	{
		int nodeEid = it1->first;
		set < pair<string, string> > vents = it1->second;

		cout << nodeEid << " ";

		for (auto it = vents.begin(); it != vents.end(); ++it)
		{
			cout << "(" << it->first << " -> " << it->second << ") , ";
		}
		cout << endl;
	}
}

void RegionDatabase::setRegionsFile(string regionsFile)
{
	regionsFile_ = regionsFile;
}

int RegionDatabase::getNodesRxRegionSize()
{
	return nodesRxRegions_.size();
}

int RegionDatabase::getNodesTxRegionSize()
{
	return nodesTxRegions_.size();
}

set<string> RegionDatabase::getRxRegionIds(int nodeEid)
{
	set < string > emptySet;
	auto it = nodesRxRegions_.find(nodeEid);
	if (it != nodesRxRegions_.end())
	{
		return it->second;
	}

	return emptySet;
}

set<string> RegionDatabase::getTxRegionIds(int nodeEid)
{
	set < string > emptySet;
	auto it = nodesTxRegions_.find(nodeEid);
	if (it != nodesTxRegions_.end())
	{
		return it->second;
	}

	return emptySet;
}

map<int, set<string> > &RegionDatabase::getNodesRxRegions()
{
	return nodesRxRegions_;
}

map<int, set<string> > &RegionDatabase::getNodesTxRegions()
{
	return nodesTxRegions_;
}

set<string> RegionDatabase::getRegions(int nodeEid)
{
	set<string> allRegions;

	auto it1 = nodesRxRegions_.find(nodeEid);
	if(it1 != nodesRxRegions_.end())
	{
		set<string> sx = it1->second;
		for(auto iit = sx.begin(); iit != sx.end(); ++iit)
		{
			allRegions.insert(*iit);
		}
	}
	auto it2 = nodesTxRegions_.find(nodeEid);
	if(it2 != nodesTxRegions_.end())
	{
		set<string> sx = it2->second;
		for(auto iit = sx.begin(); iit != sx.end(); ++iit)
		{
			allRegions.insert(*iit);
		}
	}

	return allRegions;
}

map<int, set<pair<string, string> > > &RegionDatabase::getNodesVents()
{
	return nodesVents_;
}

int RegionDatabase::getVentsNumber()
{
	int ventsNumber = 0;

	for(auto it = nodesVents_.begin(); it != nodesVents_.end(); ++it)
	{
		ventsNumber += it->second.size();
	}

	return ventsNumber;
}

