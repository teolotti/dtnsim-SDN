package dtnsim.node.net;

simple Net
{
    parameters:

        @display("i=block/routing");

        string contactsFile = default("contacts.txt");

        string routing = default("direct");
        string routingType = default("none");
        bool printRoutingDebug = default(false);

        bool saveBundleMap = default(false);

        // Signals and Statistics
        @signal[netBundleSentToMac];
        @signal[netBundleSentToApp];
        @signal[netBundleSentToAppHopCount]; // how many times was a bundle transmitted
        @signal[netBundleSentToAppRevisitedHops]; // how many times was a bundle transmitted from a same node?
        @signal[netBundleReceivedFromMac];
        @signal[netBundleReceivedFromApp];
        @signal[netBundleReRouted]; // how many times was a bundle rerouted in a same node (contact end with bundles in queue)
        @signal[sdrBundleStored];
        @signal[sdrBytesStored];

        @statistic[netBundleSentToMac](title="netBundleSentToMac"; source="netBundleSentToMac"; record=count; interpolationmode=none);
        @statistic[netBundleSentToApp](title="netBundleSentToApp"; source="netBundleSentToApp"; record=count; interpolationmode=none);
        @statistic[netBundleSentToAppHopCount](title="netBundleSentToAppHopCount"; source="netBundleSentToAppHopCount"; record=vector,histogram; interpolationmode=none);
        @statistic[netBundleSentToAppRevisitedHops](title="netBundleSentToAppRevisitedHops"; source="netBundleSentToAppRevisitedHops"; record=histogram; interpolationmode=none);
        @statistic[netBundleReceivedFromMac](title="netBundleReceivedFromMac"; source="netBundleReceivedFromMac"; record=count; interpolationmode=none);
        @statistic[netBundleReceivedFromApp](title="netBundleReceivedFromApp"; source="netBundleReceivedFromApp"; record=count; interpolationmode=none);
        @statistic[netBundleReRouted](title="netBundleReRouted"; source="netBundleReRouted"; record=count; interpolationmode=none);
        @statistic[sdrBundleStored](title="sdrBundleStored"; source="sdrBundleStored"; record=timeavg,max,vector; interpolationmode=none);
        @statistic[sdrBytesStored](title="sdrBytesStored"; source="sdrBytesStored"; record=timeavg,max,vector; interpolationmode=none);

    gates:
        inout gateToApp;
        inout gateToMac;
}
