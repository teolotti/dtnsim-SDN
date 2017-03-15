package dtnsim;

import dtnsim.ion.Ion;
import dtnsim.node.Node;

network dtnsim
{
    parameters:
        int nodesNumber = default(1);

        @display("bgb=503,408");
    submodules:
        node[nodesNumber]: Node {
            parameters:
                @display("p=81,30");
        }

        ion: Ion {
            parameters:
                @display("p=347,51;is=l");
        }

    connections:
}