#include "TopologyUtils.h"

#ifdef USE_BOOST_LIBRARIES

using namespace std;

namespace topologyUtils
{
	map<double, TopologyGraph* > computeTopology(ContactPlan *contactPlan, int nodesNumber)
	{
		map<double, TopologyGraph* > topology;

		//  vector with the start times of each topology state
		vector<double> stateTimes;
		stateTimes.push_back(0);
		TopologyGraph *topologyGraph = new TopologyGraph();
		topology[0] = topologyGraph;

		vector<Contact> *contacts = contactPlan->getContacts();
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

		// filling topology with empty topologyGraphs (with vertices, without edges)
		vector<double>::iterator i1 = stateTimes.begin();
		vector<double>::iterator i2 = stateTimes.end();
		for (; i1 != i2; ++i1)
		{
			TopologyGraph *topologyGraph = new TopologyGraph();
			for (int i = 1; i <= nodesNumber; i++)
			{
				TopologyGraph::vertex_descriptor vertex = add_vertex(*topologyGraph);
				topologyGraph->operator[](vertex).eid = i;
			}

			double stateStart = *i1;
			topology[stateStart] = topologyGraph;
		}

		// traversing contacts to add corresponding edges in topology graphs
		vector<Contact>::iterator ic1 = contacts->begin();
		vector<Contact>::iterator ic2 = contacts->end();
		for (; ic1 != ic2; ++ic1)
		{
			Contact contact = *ic1;
			double contactStart = contact.getStart();
			double contactEnd = contact.getEnd();
			int contactSource = contact.getSourceEid();
			int contactDestination = contact.getDestinationEid();
			int contactId = contact.getId();

			vector<double>::iterator ii1 = stateTimes.begin();
			vector<double>::iterator ii2 = stateTimes.end();
			for (; ii1 != ii2; ++ii1)
			{
				double stateStart = *ii1;

				if (stateStart >= contactStart && stateStart < contactEnd)
				{
					TopologyGraph *topologyGraph = topology[stateStart];

					// find vertices corresponding to source and destination contact nodes
					int foundVertices = 0;
					TopologyGraph::vertex_iterator vi1, vi2;
					tie(vi1, vi2) = vertices(*topologyGraph);
					TopologyGraph::vertex_descriptor vertexSource;
					TopologyGraph::vertex_descriptor vertexDestination;
					for (; vi1 != vi2; ++vi1)
					{
						TopologyGraph::vertex_descriptor vertex = *vi1;
						if (topologyGraph->operator [](vertex).eid == contactSource)
						{
							vertexSource = vertex;
							++foundVertices;
						}
						else if (topologyGraph->operator [](vertex).eid == contactDestination)
						{
							vertexDestination = vertex;
							++foundVertices;
						}
						if(foundVertices == 2)
						{
							break;
						}
					}

					// adding edge to found vertices
					TopologyGraph::edge_descriptor edge = (add_edge(vertexSource, vertexDestination, *topologyGraph)).first;
					topologyGraph->operator [](edge).id = contactId;
				}
			}
		}

		return topology;
	}

	void printGraphs(map<double, TopologyGraph*> *topology, std::string outFileLocation)
	{
		TopologyGraph initialGraph = *(topology->begin())->second;
		int verticesNumber = num_vertices(initialGraph);

		map<double, TopologyGraph *>::iterator it1 = topology->begin();
		map<double, TopologyGraph *>::iterator it2 = topology->end();

		ofstream ofs(outFileLocation.c_str());

		ofs << "digraph G { \n\n";
		string edgeString = " -> ";

		ofs << "rank=same; " << endl;
		ofs << "ranksep=equally; " << endl;
		ofs << "nodesep=equally; " << endl;
		ofs << "\n\n";

		int k = 1;
		for (; it1 != it2; ++it1)
		{
			double state = it1->first;
			TopologyGraph graph = *(it1->second);

			ofs << "// k = " << state << endl;

			typename TopologyGraph::vertex_iterator vi, vi_end, vi2, vi2_end;
			for (tie(vi, vi_end) = vertices(graph); vi != vi_end; ++vi)
			{
				int vertexId = graph[*vi].eid;
				ofs << vertexId << "." << state << " [label=L" << vertexId << "];" << endl;
			}

			int vertexId = verticesNumber + 1;
			int stateInt = (int) state;
			string labelString = string("\"") + string("k: ") + to_string(k++) + string("\\n") + string("t: ") + to_string(stateInt) + string("\"");
			ofs << vertexId << "." << state << "[shape=box,fontsize=16,label=" << labelString << "];" << endl;

			for (tie(vi, vi_end) = vertices(graph); vi != vi_end; ++vi)
			{
				for (tie(vi2, vi2_end) = vertices(graph); vi2 != vi2_end; ++vi2)
				{
					if (vi2 != vi && *vi2 > *vi)
					{
						int v1Id = graph[*vi].eid;
						int v2Id = graph[*vi2].eid;
						ofs << v1Id << "." << state << edgeString << v2Id << "." << state << "[style=\"invis\"];" << endl;
						break;
					}
				}
			}

			int lastVertexId = verticesNumber;
			ofs << lastVertexId << "." << state << edgeString << vertexId << "." << state << "[style=\"invis\"];" << endl;

			TopologyGraph::edge_iterator ei, ei_end;
			for (tie(ei, ei_end) = edges(graph); ei != ei_end; ++ei)
			{
				int v1Id = graph[source(*ei, graph)].eid;
				int v2Id = graph[target(*ei, graph)].eid;
				ofs << v1Id << "." << state << edgeString << v2Id << "." << state << "[color=black,fontcolor=grey,penwidth=2];" << endl;
			}

			ofs << "\n";
		}

		TopologyGraph graph = *(topology->begin())->second;
		TopologyGraph::vertex_iterator vi, vi_end;
		for (tie(vi, vi_end) = vertices(graph); vi != vi_end; ++vi)
		{
			ofs << "{ rank = same; ";
			int vertexId = graph[*vi].eid;

			map<double, TopologyGraph *>::iterator iit1 = topology->begin();
			map<double, TopologyGraph *>::iterator iit2 = topology->end();
			for (; iit1 != iit2; ++iit1)
			{
				ofs << vertexId << "." << iit1->first << "; ";
			}
			ofs << "}\n";
		}

		ofs << "\n\n";
		ofs << "}" << endl;
	}

	void printGraph(TopologyGraph topologyGraph)
	{
		TopologyGraph::edge_iterator ei, ei_end;
		for (tie(ei, ei_end) = edges(topologyGraph); ei != ei_end; ++ei)
		{
			int v1Id = topologyGraph[source(*ei, topologyGraph)].eid;
			int v2Id = topologyGraph[target(*ei, topologyGraph)].eid;

			cout<<"edge = "<<topologyGraph[*ei].id << " between nodes "<<v1Id<<" and "<<v2Id<<endl;
		}
	}

}

#endif /* USE_BOOST_LIBRARIES */

