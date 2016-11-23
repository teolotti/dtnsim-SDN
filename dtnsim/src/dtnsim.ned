package dtnsim;

import dtnsim.node.Node;

network dtnsim
{
    parameters:
        int nodesNumber = default(1);
        
    submodules:
        node[nodesNumber]: Node {
            parameters:
                @display("");
        }

    connections:
}