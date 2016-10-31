package dtnsim.node;

import dtnsim.node.app.App;
import dtnsim.node.net.Net;
import dtnsim.node.mac.Mac;

module Node
{
    parameters:
        @display("i=device/satellite;bgb=150,250");
        
    gates:


    submodules:
        app: App {
            parameters:
            	@display("p=75,50");  
        }
        net: Net {
            parameters:
            	@display("p=75,120");
        }
        mac: Mac {
            parameters:
                @display("p=75,190");
        }

    connections:
        app.gateToNet <--> net.gateToApp;
        net.gateToMac <--> mac.gateToNet;
}
