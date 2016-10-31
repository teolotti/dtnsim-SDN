package dtnsim.node.mac;

simple Mac
{
    parameters:
        @display("i=block/wtx");
        
        int dataRate = default(1024); 
                      
    gates:
        inout gateToNet;
        input gateToAir @directIn;
}
