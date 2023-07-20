package src.hierarchicalRegions.node.app;

simple App {
    
    parameters:
        @class(dtnsimhierarchical::App);
        @display("i=block/telnet");
        
        
        bool enable = default(false); 				// enable traffic generation
        											// TG is based on flows (flows of bundles between Src and Dst)
        											// Each flow follows a given distribution
        
        int numberOfFlows = default(0); 			// number of flows starting at this node
        
        string destinationEids = default("A1");		// comma-separated list of destination EIDs and regions per flow
        											// e.g. A1, A2, C3, ...
        
        string distributions = default("uniform1");	// comma-separated list of packet distributions per flow
        
        string numbersOfBundles = default("1");		// comma-separated list of number of packets per flow
        
        string startTimes = default("0");			// comma-separated list of start times per flow
        
        string bundleSizes = default("1024");		// comma-separated list of bundle sizes per flow
        											// the bundles of a single flow all have the same size TODO
        
        string ttls = default("9000000");			// comma-separated list of TTL values for all bundles in a given flow
               
        // string priorities; TODO critical etc... other parameters
        
        // signals and statistics
        @signal[appBundleSent];
        @statistic[appBundleSent](title="appBundleSent"; source="appBundleSent"; record=count; interpolationmode=none);
        
        @signal[appBundleReceived];
        @statistic[appBundleReceived](title="appBundleReceived"; source="appBundleReceived"; record=count; interpolationmode=none);
        
        @signal[appBundleReceivedDelay];
        @statistic[appBundleReceivedDelay](title="appBundleReceivedDelay"; source="appBundleReceivedDelay"; record=mean,max,min,histogram; interpolationmode=none);
        
        // TODO: hops
       	  
    gates:
        inout gateToDtn;
}