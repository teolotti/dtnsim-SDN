package dtnsim;

import dtnsim.ion.Ion;
import dtnsim.node.Node;

network dtnsim
{
    parameters:
        int nodesNumber = default(1);
        @display("bgb=503,408");
        
    submodules:
        node[nodesNumber+1]: Node {
            parameters:
                @display("p=80,50");
        }

        ion: Ion {
            parameters:
                @display("p=40,50");
        }

    connections:
}