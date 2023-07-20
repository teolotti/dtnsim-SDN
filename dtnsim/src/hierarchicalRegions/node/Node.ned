package src.hierarchicalRegions.node;

import src.hierarchicalRegions.node.app.App;
import src.hierarchicalRegions.node.dtn.Dtn;
import src.hierarchicalRegions.node.com.Com;
import src.hierarchicalRegions.node.graphics.Graphics;

module Node {
    
    parameters:
        int eid;
        string homeRegion;
        string outerRegion = default("Z"); //TODO
        string type; // passageway node or region node
        string icon;
		
		double posX = default(200);
		double posY = default(200);
		
		@display("p=$posX,$posY");
		
		string text;
		@display("t=$text");
        
    submodules:
        app: App; 	// traffic generation
        dtn: Dtn;	// routing
        com: Com;	// forwarding
        graphics: Graphics;
        
   	connections:
         app.gateToDtn <--> dtn.gateToApp;
         dtn.gateToCom <--> com.gateToDtn;
}