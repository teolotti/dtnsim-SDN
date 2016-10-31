package dtnsim.node.net;

simple Net
{
    parameters:
        
        @display("i=block/routing");   
                
        string contactsFile = default("contacts.txt");                   
        
    gates:
        inout gateToApp;
        inout gateToMac;
}
