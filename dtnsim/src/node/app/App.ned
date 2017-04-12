package dtnsim.node.app;

simple App
{
    parameters:
        @display("i=block/telnet");
        
        bool enable = default(false); // Traffic Generator Enabled
        string destinationEid = default("1"); // Generated Bundles destination EiD
        string bundlesNumber = default("1"); // Number of Bundles to Generate
        string size = default("1024"); // Generated Bundles Size in Bytes
        string start = default("0"); // tiempo de generación de mensajes
        int interval = default(0); // Bundles Generation Interval in Secs
        int ttl = default(9000000); // Generated Bundles Time to Live in Secs
        bool returnToSender = default(true);
        bool critical = default(false);
                      
    gates:
        inout gateToNet;
}
