package dtnsim.node.net;

simple Net
{
    parameters:
        
        @display("i=block/routing");   
                
        string contactsFile = default("contacts.txt");     
        string routing = default("direct");              
        
    gates:
        inout gateToApp;
        inout gateToMac;
}
