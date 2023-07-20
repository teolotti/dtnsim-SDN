package src.hierarchicalRegions.node.com;

simple Com {
    
    parameters:
        @class(dtnsimhierarchical::Com);
        @display("i=block/wtx");
        
    gates:
        inout gateToDtn;
        input gateToAir @directIn;
}