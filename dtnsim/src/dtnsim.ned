package dtnsim;

import dtnsim.node.Node;

network dtnsim
{
    parameters:
        int nodesNumber = default(1);
        @display("bgb=280,181");
    submodules:
        node[nodesNumber]: Node {
            parameters:
                @display("p=80,60,ri");
        }
    connections:
}
