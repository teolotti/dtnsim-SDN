package src.distinctRegions.node.com;

simple Com {
    
    parameters:
        @class(dtnsimdistinct::Com);
        @display("i=block/wtx");
        
        // TODO packetloss
        
    gates:
        inout gateToDtn;
        input gateToAir @directIn;
}