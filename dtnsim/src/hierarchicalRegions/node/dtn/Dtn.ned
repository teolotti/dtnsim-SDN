package src.hierarchicalRegions.node.dtn;

simple Dtn {
    
    parameters:
        @class(dtnsimhierarchical::Dtn);
        @display("i=block/routing");
        
        int sdrCapacity = default(90000);
        
    gates:
        inout gateToApp;
        inout gateToCom;
    
}