package dtnsim.node.mac;

simple Mac
{
    parameters:
        @display("i=block/wtx");
                      
    gates:
        inout gateToNet;
        input gateToAir @directIn;
}
