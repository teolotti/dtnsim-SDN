package dtnsim.node.com;

simple Com
{
    parameters:
        @display("i=block/wtx");
                      
    gates:
        inout gateToDtn;
        input gateToAir @directIn;
}
