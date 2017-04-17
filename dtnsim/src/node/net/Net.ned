package dtnsim.node.net;

simple Net
{
    parameters:
        
        @display("i=block/routing");   
                
        string contactsFile = default("contacts.txt");
             
        string routing = default("direct");
        string routingType = default("none");        
        bool printRoutingDebug = default(false);
        
        bool saveBundleMap=default(false); 
        bool generateTopologyOutput=default(false);        
        
    gates:
        inout gateToApp;
        inout gateToMac;
}
