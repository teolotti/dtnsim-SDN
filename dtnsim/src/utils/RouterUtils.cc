#include "RouterUtils.h"

#ifdef USE_BOOST_LIBRARIES

using namespace std;
using namespace boost;

namespace routerUtils
{

map<double, RouterGraph*> computeFlows(ContactPlan *contactPlan, int nodesNumber, string bundleMapsLocation)
{
    map<double, RouterGraph *> flows;

    // compute topology
    map<double, TopologyGraph*> topology = topologyUtils::computeTopology(contactPlan, nodesNumber);

    // create empty flows as a copy of topology
    map<double, TopologyGraph*>::iterator it1 = topology.begin();
    map<double, TopologyGraph*>::iterator it2 = topology.end();
    for (; it1 != it2; ++it1)
    {
        // create routeGraph and add vertices
        RouterGraph *routerGraph = new RouterGraph();
        for (int i = 1; i <= nodesNumber; i++)
        {
            RouterGraph::vertex_descriptor vertex = add_vertex(*routerGraph);
            routerGraph->operator[](vertex).eid = i;
        }

        double stateTime = it1->first;
        TopologyGraph topologyGraph = *(it1->second);

        // fill routerGraph starting from topologyGraph
        TopologyGraph::edge_iterator ei, ei_end;
        for (tie(ei, ei_end) = edges(topologyGraph); ei != ei_end; ++ei)
        {
            int edgeId = topologyGraph[*ei].id;
            int v1Id = topologyGraph[source(*ei, topologyGraph)].eid;
            int v2Id = topologyGraph[target(*ei, topologyGraph)].eid;

            // find vertices corresponding to v1Id and v2Id in RouterGraph and save them
            // in vertexSource and vertexDestination
            int foundVertices = 0;
            RouterGraph::vertex_iterator vi1, vi2;
            tie(vi1, vi2) = vertices(*routerGraph);
            RouterGraph::vertex_descriptor vertexSource;
            RouterGraph::vertex_descriptor vertexDestination;
            for (; vi1 != vi2; ++vi1)
            {
                RouterGraph::vertex_descriptor vertex = *vi1;
                if (routerGraph->operator [](vertex).eid == v1Id)
                {
                    vertexSource = vertex;
                    ++foundVertices;
                }
                else if (routerGraph->operator [](vertex).eid == v2Id)
                {
                    vertexDestination = vertex;
                    ++foundVertices;
                }
                if (foundVertices == 2)
                {
                    break;
                }
            }

            // add edge to found vertices in RouterGraph
            RouterGraph::edge_descriptor edge = (add_edge(vertexSource, vertexDestination, *routerGraph)).first;
            routerGraph->operator [](edge).id = edgeId;
            routerGraph->operator [](edge).stateCapacity = topologyGraph[*ei].stateCapacity;
        }

        flows[stateTime] = routerGraph;
    }

    // add flows
    for (int i = 1; i <= nodesNumber; i++)
    {
        string fileName = bundleMapsLocation + "/BundleMap_Node" + to_string(i) + ".csv";

        ifstream file;
        file.open(fileName.c_str());
        if (!file.is_open())
        {
            continue;
            //throw cException(("Error: wrong path to contacts file " + string(fileName)).c_str());
        }

        // read first line
        string str1;
        string str2;
        getline(file, str1, '\n');

        double simTime;
        int src, dst, tsrc, tdst;
        double bitLength;
        double durationSecs;

        // read other lines
        while (!file.eof())
        {
            getline(file, str1, '\n');

            size_t posComma = str1.find(",");
            str2 = str1.substr(0, posComma);
            if (str2.empty())
            {
                break;
            }
            simTime = stod(str2);

            str1 = str1.substr(posComma + 1, string::npos);
            posComma = str1.find(",");
            str2 = str1.substr(0, posComma);
            src = stoi(str2);

            str1 = str1.substr(posComma + 1, string::npos);
            posComma = str1.find(",");
            str2 = str1.substr(0, posComma);
            dst = stoi(str2);

            str1 = str1.substr(posComma + 1, string::npos);
            posComma = str1.find(",");
            str2 = str1.substr(0, posComma);
            tsrc = stoi(str2);

            str1 = str1.substr(posComma + 1, string::npos);
            posComma = str1.find(",");
            str2 = str1.substr(0, posComma);
            tdst = stoi(str2);

            str1 = str1.substr(posComma + 1, string::npos);
            posComma = str1.find(",");
            str2 = str1.substr(0, posComma);
            bitLength = stod(str2);

            str1 = str1.substr(posComma + 1, string::npos);
            posComma = str1.find(",");
            str2 = str1.substr(0, posComma);
            durationSecs = stod(str2);

            // get state
            double state = 0;
            map<double, TopologyGraph*>::iterator iit1 = topology.begin();
            map<double, TopologyGraph*>::iterator iit2 = topology.end();
            for (; iit1 != iit2; ++iit1)
            {
                double stateStart = iit1->first;
                double stateEnd = std::numeric_limits<double>::max();

                ++iit1;
                if (iit1 != iit2)
                {
                    stateEnd = iit1->first;
                }
                --iit1;

                if (simTime >= stateStart && simTime < stateEnd)
                {
                    state = stateStart;
                }
            }

            // get Router graph in state
            RouterGraph *rGraph = flows[state];

            // find vertices corresponding to tsrc and tdst in RouterGraph and save them
            // in vertexSourceAux and vertexDestinationAux
            int foundVerticesAux = 0;
            RouterGraph::vertex_iterator vii1, vii2;
            tie(vii1, vii2) = vertices(*rGraph);
            RouterGraph::vertex_descriptor vertexSourceAux;
            RouterGraph::vertex_descriptor vertexDestinationAux;
            for (; vii1 != vii2; ++vii1)
            {
                RouterGraph::vertex_descriptor vertex = *vii1;
                if (rGraph->operator [](vertex).eid == src)
                {
                    vertexSourceAux = vertex;
                    ++foundVerticesAux;
                }
                else if (rGraph->operator [](vertex).eid == dst)
                {
                    vertexDestinationAux = vertex;
                    ++foundVerticesAux;
                }
                if (foundVerticesAux == 2)
                {
                    break;
                }
            }

            // find corresponding edge and add flow into flows structure
            pair<RouterGraph::edge_descriptor, int> edgePair = boost::edge(vertexSourceAux, vertexDestinationAux, *rGraph);
            if (!edgePair.second)
            {
                cout << "edge not found !" << endl;
                exit(1);
            }
            else
            {
                //cout<<"edge found"<<endl;
                RouterGraph::edge_descriptor ed = edgePair.first;
                rGraph->operator [](ed).flows[tsrc][tdst] += ((double) bitLength / (double) 8);
            }
        }
        file.close();
    }

    // delete topology
    map<double, TopologyGraph*>::iterator itt1 = topology.begin();
    map<double, TopologyGraph*>::iterator itt2 = topology.end();
    for (; itt1 != itt2; ++itt1)
    {
        delete itt1->second;
    }
    topology.clear();

    return flows;
}

void printGraphs(map<double, RouterGraph*> *flows, vector<string> dotColors, map<pair<int, int>, unsigned int> flowIds, std::string outFileLocation)
{
    RouterGraph initialGraph = *(flows->begin())->second;
    int verticesNumber = num_vertices(initialGraph);

    map<double, RouterGraph *>::iterator it1 = flows->begin();
    map<double, RouterGraph *>::iterator it2 = flows->end();

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
        RouterGraph graph = *(it1->second);

        ofs << "// k = " << state << endl;

        typename RouterGraph::vertex_iterator vi, vi_end, vi2, vi2_end;
        for (tie(vi, vi_end) = vertices(graph); vi != vi_end; ++vi)
        {
            int vertexId = graph[*vi].eid;
            ofs << vertexId << "." << state << " [label=L" << vertexId << "];" << endl;
        }

        int vertexId = verticesNumber + 1;
        int stateInt = (int) state;
        string labelString = string("\"") + string("k: ") + to_string(k++) + string("\\n") + string("t: ") + to_string(stateInt) + string("\"");
        ofs << vertexId << "." << stateInt << "[shape=box,fontsize=16,label=" << labelString << "];" << endl;

        for (tie(vi, vi_end) = vertices(graph); vi != vi_end; ++vi)
        {
            for (tie(vi2, vi2_end) = vertices(graph); vi2 != vi2_end; ++vi2)
            {
                if (vi2 != vi && *vi2 > *vi)
                {
                    int v1Id = graph[*vi].eid;
                    int v2Id = graph[*vi2].eid;
                    ofs << v1Id << "." << stateInt << edgeString << v2Id << "." << stateInt << "[style=\"invis\"];" << endl;
                    break;
                }
            }
        }

        int lastVertexId = verticesNumber;
        ofs << lastVertexId << "." << stateInt << edgeString << vertexId << "." << stateInt << "[style=\"invis\"];" << endl;

        RouterGraph::edge_iterator ei, ei_end;
        for (tie(ei, ei_end) = edges(graph); ei != ei_end; ++ei)
        {
            int v1Id = graph[source(*ei, graph)].eid;
            int v2Id = graph[target(*ei, graph)].eid;
            string weight = string("id:") + to_string(graph[*ei].id) + string("\n") + to_string((int) graph[*ei].stateCapacity);
            ofs << v1Id << "." << stateInt << edgeString << v2Id << "." << stateInt << "[color=grey,fontcolor=grey,label=\"" << weight << "\",penwidth=2];" << endl;

            for (int k1 = 1; k1 <= verticesNumber; k1++)
            {
                for (int k2 = 1; k2 <= verticesNumber; k2++)
                {
                    if (graph[*ei].flows[k1][k2] != 0)
                    {
                        string flowValue = to_string((int) graph[*ei].flows[k1][k2]);
                        string edgeColor = dotColors.at(flowIds[make_pair(k1, k2)]);
                        string flowId = to_string(k1) + "-" + to_string(k2);
                        string edgeDescription = to_string(v1Id) + "." + to_string(stateInt) + edgeString + to_string(v2Id) + "." + to_string(stateInt);
                        string edgeProperties = string("[fontsize=15, penwidth=2, color=") + string("\"") + edgeColor + string("\"");
                        edgeProperties.append(string(",label=") + string("\"") + flowId + string("\\n") + flowValue + string("\"") + string("]"));
                        ofs << edgeDescription + edgeProperties << endl;
                    }
                }
            }

            ofs << "\n";
        }

    }

    RouterGraph graphAux = *(flows->begin())->second;
    RouterGraph::vertex_iterator vii, vii_end;
    for (tie(vii, vii_end) = vertices(graphAux); vii != vii_end; ++vii)
    {
        ofs << "{ rank = same; ";
        int vertexId = graphAux[*vii].eid;

        map<double, RouterGraph *>::iterator iit1 = flows->begin();
        map<double, RouterGraph *>::iterator iit2 = flows->end();
        for (; iit1 != iit2; ++iit1)
        {
            ofs << vertexId << "." << iit1->first << "; ";
        }
        ofs << "}\n";
    }

    ofs << "\n\n";
    ofs << "}" << endl;
    ofs.close();

    //create pdf from .dot
    string command = "dot -Tpdf " + outFileLocation + " -o " + outFileLocation + string(".pdf");
    system(command.c_str());
}

void printGraph(RouterGraph routerGraph)
{
    int verticesNumber = num_vertices(routerGraph);

    RouterGraph::edge_iterator ei, ei_end;
    for (tie(ei, ei_end) = edges(routerGraph); ei != ei_end; ++ei)
    {
        int v1Id = routerGraph[source(*ei, routerGraph)].eid;
        int v2Id = routerGraph[target(*ei, routerGraph)].eid;

        cout << "edge = " << routerGraph[*ei].id << " between nodes " << v1Id << " and " << v2Id << endl;

        cout << "flows = " << endl;
        typename RouterGraph::vertex_iterator vi, vi_end, vi2, vi2_end;
        for (int k1 = 0; k1 <= verticesNumber; k1++)
        {
            for (int k2 = 0; k2 <= verticesNumber; k2++)
            {
                cout << routerGraph[*ei].flows[k1][k2] << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
}

}

#endif /* USE_BOOST_LIBRARIES */

