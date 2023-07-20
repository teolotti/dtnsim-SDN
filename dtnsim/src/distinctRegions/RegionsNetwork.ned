package src.distinctRegions;

import src.distinctRegions.node.Node;
import src.distinctRegions.central.Central;

network RegionsNetwork {
        
    parameters:
        
        int regionNodeNumber = default(0);
        int passagewayNodeNumber = default(0);
        int backboneNodeNumber = default(0);
        
        // TODO: not applied yet, implement if more routing algorithms are added
        string intraRouting = default("yen");
        string interRouting = default("yen");
        
        // IRR variation (topology specific)
        bool staticIrr = default(false);
        
        @display("bgb=1000,800");
        
        
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
        
        backboneNode[backboneNodeNumber]: Node {
            parameters:
                type = "backbone";
                icon = "device";
                @display("i=device/device");
        }
        
        central: Central {
            parameters:
                @display("i=device/terminal");
                @display("p=40,50");
                
        }       
}
