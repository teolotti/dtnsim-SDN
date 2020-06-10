package src.node.dtn;

simple Dtn
{
    parameters:

        @display("i=block/routing");

        string routing = default("direct");
        string routingType = default("none");
        int bundlesCopies = default(1);
        double sContactProb = default(1.0);
        bool printRoutingDebug = default(false);
        bool saveBundleMap = default(false);
        int sdrSize = default(0); // SDR Memory Size in Bytes (0 = infinite)
        
        // CGR Centralized parameters
        int maxRouteHops = default(-1);
        int maxRoutesWithSameDst = default(-1);
        double bfsIntervalTime = default(0);
        int bfsIntervalNum = default(1);
        
        // Custody
        int custodyReportByteSize = default(20);
        double custodyTimeout = default(100);

        // Signals and Statistics
        @signal[dtnBundleSentToCom];
        @signal[dtnBundleSentToApp];
        @signal[dtnBundleSentToAppHopCount]; // how many times was a bundle transmitted
        @signal[dtnBundleSentToAppRevisitedHops]; // how many times was a bundle transmitted from a same node?
        @signal[dtnBundleReceivedFromCom];
        @signal[dtnBundleReceivedFromApp];
        @signal[dtnBundleReRouted]; // how many times was a bundle rerouted in a same node (contact end with bundles in queue)
        @signal[sdrBundleStored];
        @signal[sdrBytesStored];
        @signal[routeCgrDijkstraCalls];
        @signal[routeCgrDijkstraLoops];
        @signal[routeCgrRouteTableEntriesCreated];
        @signal[routeCgrRouteTableEntriesExplored];
        @signal[routeCgrRouteLength];
        @signal[routeCgrTimeToComputeRoutes];

        @statistic[dtnBundleSentToCom](title="dtnBundleSentToCom"; source="dtnBundleSentToCom"; record=count; interpolationmode=none);
        @statistic[dtnBundleSentToApp](title="dtnBundleSentToApp"; source="dtnBundleSentToApp"; record=count; interpolationmode=none);
        @statistic[dtnBundleSentToAppHopCount](title="dtnBundleSentToAppHopCount"; source="dtnBundleSentToAppHopCount"; record=vector,histogram; interpolationmode=none);
        @statistic[dtnBundleSentToAppRevisitedHops](title="dtnBundleSentToAppRevisitedHops"; source="dtnBundleSentToAppRevisitedHops"; record=histogram; interpolationmode=none);
        @statistic[dtnBundleReceivedFromCom](title="dtnBundleReceivedFromCom"; source="dtnBundleReceivedFromCom"; record=count; interpolationmode=none);
        @statistic[dtnBundleReceivedFromApp](title="dtnBundleReceivedFromApp"; source="dtnBundleReceivedFromApp"; record=count; interpolationmode=none);
        @statistic[dtnBundleReRouted](title="dtnBundleReRouted"; source="dtnBundleReRouted"; record=count; interpolationmode=none);
        @statistic[sdrBundleStored](title="sdrBundleStored"; source="sdrBundleStored"; record=timeavg,max,vector; interpolationmode=none);
        @statistic[sdrBytesStored](title="sdrBytesStored"; source="sdrBytesStored"; record=timeavg,max,vector; interpolationmode=none);
        @statistic[routeCgrDijkstraCalls](title="routeCgrDijkstraCalls"; source="routeCgrDijkstraCalls"; record=sum,vector; interpolationmode=none);
        @statistic[routeCgrDijkstraLoops](title="routeCgrDijkstraLoops"; source="routeCgrDijkstraLoops"; record=sum,vector; interpolationmode=none);
        @statistic[routeCgrRouteTableEntriesCreated](title="routeCgrRouteTableEntriesCreated"; source="routeCgrRouteTableEntriesCreated"; record=max,sum,vector; interpolationmode=none);
        @statistic[routeCgrRouteTableEntriesExplored](title="routeCgrRouteTableEntriesExplored"; source="routeCgrRouteTableEntriesExplored"; record=max,sum,vector; interpolationmode=none);
        @statistic[routeCgrRouteLength](title="routeCgrRouteLength"; source="routeCgrRouteLength"; record=sum,vector,mean; interpolationmode=none);
        @statistic[routeCgrTimeToComputeRoutes](title="routeCgrTimeToComputeRoutes"; source="routeCgrTimeToComputeRoutes"; record=sum);

    gates:
        inout gateToApp;
        inout gateToCom;
}
