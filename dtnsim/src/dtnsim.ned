package dtnsim;

import dtnsim.node.Node;

network dtnsim
{
    parameters:
        int nodesNumber = default(1);
        @display("bgb=493.95,238.65001");
    submodules:
        node[nodesNumber]: Node {
            parameters:
                @display("p=81.4,20.35,ri");
        }

    connections:
}