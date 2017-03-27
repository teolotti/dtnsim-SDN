package dtnsim.node.net;

simple Net
{
    parameters:
        
        @display("i=block/routing");   
                
        string contactsFile = default("contacts.txt");     
        string routing = default("direct");    
        
        bool saveBundleMap=default(false); 
        bool generateOutputGraph=default(false);        
        
    gates:
        inout gateToApp;
        inout gateToMac;
}
