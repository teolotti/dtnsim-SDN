package dtnsim.node;

import dtnsim.node.fault.Fault;
import dtnsim.node.graphics.Graphics;
import dtnsim.node.app.App;
import dtnsim.node.net.Net;
import dtnsim.node.mac.Mac;

module Node
{
    parameters:
        @display("i=device/satellite");
        string icon = default("satellite");
       
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
        graphics: Graphics {
            parameters:
                @display("p=150,50");
        }
        fault: Fault {
            parameters:
                @display("p=150,120");
        }

    connections:
        app.gateToNet <--> net.gateToApp;
        net.gateToMac <--> mac.gateToNet;
}
