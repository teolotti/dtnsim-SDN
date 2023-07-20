package src.node.app;

simple App
{
    parameters:
        @display("i=block/telnet");
        
        bool enable = default(false); 				// enable traffic generation
        											// TG is based on flows (flows of bundles between Src and Dst)
        											// Each flow follows a given distribution
        
        string destinationEids = default("1");		// comma-separated list of destination EIDs per flow
        
        string distribution = default("uniform1");	// distribution used for each flow
                
        string startTimes = default("0");			// comma-separated list of start times per flow (TODO times 3600)
        
        int bundleSize = default(1024);				// bundle size used for each flow
        
        int ttl = default(9000000);					// bundle TTL used for each flow
        
        // Signals and Statistics
        @signal[appBundleSent];
        @signal[appBundleReceived];
        @signal[appBundleReceivedHops];
        @signal[appBundleReceivedDelay];
        
        @statistic[appBundleSent](title="appBundleSent"; source="appBundleSent"; record=count; interpolationmode=none);
        @statistic[appBundleReceived](title="appBundleReceived"; source="appBundleReceived"; record=count; interpolationmode=none);
        @statistic[appBundleReceivedHops](title="appBundleReceivedHops"; source="appBundleReceivedHops"; record=mean,max,min,histogram; interpolationmode=none);
        @statistic[appBundleReceivedDelay](title="appBundleReceivedDelay"; source="appBundleReceivedDelay"; record=mean,max,min,histogram; interpolationmode=none);
                   
                      
    gates:
        inout gateToDtn;
}
