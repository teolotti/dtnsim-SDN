package src.distinctRegions.node.dtn;

simple Dtn {
    
    parameters:
        
        @class(dtnsimdistinct::Dtn);
        @display("i=block/routing");
        
        int sdrCapacity = default(900000);
        
        @signal[routeExecutionTimeUs];
        @statistic[routeExecutionTimeUs](title="routeExecutionTimeUs"; source="routeExecutionTimeUs"; record=mean,sum,vector; interpolationmode=none);
        
        @signal[sdrBundleStored];
        @signal[sdrBytesStored];
        @statistic[sdrBundleStored](title="sdrBundleStored"; source="sdrBundleStored"; record=timeavg,max,vector; interpolationmode=none);
        @statistic[sdrBytesStored](title="sdrBytesStored"; source="sdrBytesStored"; record=timeavg,max,vector; interpolationmode=none);
        
        @signal[routeCgrRouteTableEntriesCreated];
        @signal[routeCgrRouteTableEntriesExplored];
        @statistic[routeCgrRouteTableEntriesCreated](title="routeCgrRouteTableEntriesCreated"; source="routeCgrRouteTableEntriesCreated"; record=mean,sum,vector; interpolationmode=none);
        @statistic[routeCgrRouteTableEntriesExplored](title="routeCgrRouteTableEntriesExplored"; source="routeCgrRouteTableEntriesExplored"; record=mean,sum,vector; interpolationmode=none);
        
        @signal[bundleDropped];
        @statistic[bundleDropped](title="bundleDropped"; source="bundleDropped"; record=sum,vector; interpolationmode=none);
        
        @signal[bundleReceivedFromCom];
        @statistic[bundleReceivedFromCom](title="bundleReceivedFromCom"; source="bundleReceivedFromCom"; record=count; interpolationmode=none);
        
        @signal[routeComplexity];
        @statistic[routeComplexity](title="routeComplexity"; source="routeComplexity"; record=sum,vector; interpolationmode=none);
        
        @signal[routeTableSize];
        @statistic[routeTableSize](title="routeTableSize"; source="routeTableSize"; record=mean,sum,vector; interpolationmode=none);
        
        @signal[yenIterations];
        @statistic[yenIterations](title="yenIterations"; source="yenIterations"; record=mean,sum,vector; interpolationmode=none);
        
      	@signal[routeSearchCalls];
        @statistic[routeSearchCalls](title="routeSearchCalls"; source="routeSearchCalls"; record=mean,sum,vector; interpolationmode=none);
        
        @signal[routeSearchStarts];
        @statistic[routeSearchStarts](title="routeSearchStarts"; source="routeSearchStarts"; record=mean,sum,vector; interpolationmode=none);
        
        @signal[firstHop];
        @statistic[firstHop](title="firstHop"; source="firstHop"; record=histogram; interpolationmode=none);
        
        
    gates:
        inout gateToApp;
        inout gateToCom;
}
