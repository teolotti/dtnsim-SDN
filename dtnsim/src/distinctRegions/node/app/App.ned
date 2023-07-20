package src.distinctRegions.node.app;

simple App {
    
    parameters:
        @class(dtnsimdistinct::App);
        @display("i=block/telnet");
        
        
        bool enable = default(false); 				// enable traffic generation
        											// TG is based on flows (flows of bundles between Src and Dst)
        											// Each flow follows a given distribution
        
        string destinationEids = default("A1");		// comma-separated list of destination EIDs and regions per flow
        											// e.g. A1, A2, C3, ...
        
        string distribution = default("uniform1");	// distribution used for each flow
                
        string startTimes = default("0");			// comma-separated list of start times per flow (TODO times 3600)
        
        int bundleSize = default(1024);				// bundle size used for each flow
        
        int ttl = default(9000000);					// bundle TTL used for each flow
               
        
        // signals and statistics
        @signal[appBundleSent];
        @statistic[appBundleSent](title="appBundleSent"; source="appBundleSent"; record=count; interpolationmode=none);
        
        @signal[appBundleReceived];
        @statistic[appBundleReceived](title="appBundleReceived"; source="appBundleReceived"; record=count; interpolationmode=none);
        
        @signal[appBundleReceivedDelay];
        @statistic[appBundleReceivedDelay](title="appBundleReceivedDelay"; source="appBundleReceivedDelay"; record=mean,max,min,histogram; interpolationmode=none);
        
        @signal[appBundleReceivedHops];
        @statistic[appBundleReceivedHops](title="appBundleReceivedHops"; source="appBundleReceivedHops"; record=mean,max,min,histogram; interpolationmode=none);
        
        @signal[appBundleReceivedFirstHop];
        @statistic[appBundleReceivedFirstHop](title="appBundleReceivedFirstHop"; source="appBundleReceivedFirstHop"; record=mean,max,min,histogram; interpolationmode=none);
        
       	  
    gates:
        inout gateToDtn;
}

