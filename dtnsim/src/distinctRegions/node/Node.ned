package src.distinctRegions.node;

import src.distinctRegions.node.app.App;
import src.distinctRegions.node.dtn.Dtn;
import src.distinctRegions.node.com.Com;
import src.distinctRegions.node.graphics.Graphics;

module Node {
    
    parameters:
        int eid;
        string region;
        string type; // backbone node, passageway node or region node (obsolete, all nodes are of type "region")
        string icon;
		
		double posX = default(200);
		double posY = default(200);
		
		@display("p=$posX,$posY");
		
		string text;
		@display("t=$text");
        
    submodules:
        app: App; 			// traffic generation
        dtn: Dtn;			// routing
        com: Com;			// forwarding
        graphics: Graphics; // GUI
        
   	connections:
         app.gateToDtn <--> dtn.gateToApp;
         dtn.gateToCom <--> com.gateToDtn;
}