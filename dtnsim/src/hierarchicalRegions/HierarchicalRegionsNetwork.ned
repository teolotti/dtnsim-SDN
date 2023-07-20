package src.hierarchicalRegions;

import src.hierarchicalRegions.node.Node;
import src.hierarchicalRegions.central.Central;

network HierarchicalRegionsNetwork {
    
    parameters:
        int regionNodeNumber = default(0);
        int passagewayNodeNumber = default(0);
        
        @display("bgp=1000,800");
        
    submodules:
        
        regionNode[regionNodeNumber]: Node {
            parameters:
                type = "region";
                icon = "satellite";
                @display("i=device/satellite");
        }
        
        passagewayNode[passagewayNodeNumber]: Node {
            parameters:
                type = "passageway";
                icon = "antennatower";
                @display("i=device/antennatower");
        }
        
        central: Central {
            parameters:
                @display("i=device/terminal");
                @display("p=40,50");
                
        }  
}