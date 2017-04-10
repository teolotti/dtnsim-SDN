package dtnsim.node;

import dtnsim.node.graphics.Graphics;
import dtnsim.node.app.App;
import dtnsim.node.net.Net;
import dtnsim.node.mac.Mac;

module Node
{
    parameters:
        @display("i=device/satellite");
        string icon = default("satellite");
        bool enableFaults = default(false);

        int faultSeed = default(0);
        double meanTTF = default(0); // If 0 no faults enabled 
        double meanTTR = default(0); // If 0 no faults enabled

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
        graphics: Graphics {
            parameters:
                @display("p=150,50");
        }

    connections:
        app.gateToNet <--> net.gateToApp;
        net.gateToMac <--> mac.gateToNet;
}
