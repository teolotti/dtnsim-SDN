

cplusplus {{
    #include "src/node/dtn/routing/CgrRoute.h"
    #include "src/node/dtn/routing/SdnRoute.h"
    typedef std::list<int> intList;
}}

class CgrRoute
{
    @existingClass;
}

class SdnRoute
{
    @existingClass;
}

class intList
{
    @existingClass;
}

//struct CgrRoute;
//struct SdnRoute;
//class noncobject cgrRouteList;

packet BundlePkt{
		////////////////////////////////////////////////    
    	// Bundle Protocol fields (set by source node)
    	////////////////////////////////////////////////
    	long bundleId;
    	int sourceEid;
        int destinationEid;
        bool critical;
        simtime_t creationTimestamp;
        simtime_t ttl;
        bool returnToSender;
        bool custodyTransferRequested;
        
        // Source routing path
        CgrRoute cgrRoute;
        
        // Bundle is a report
        bool bundleIsCustodyReport;
        bool custodyAccepted;
        long custodyBundleId;
        
        ////////////////////////////////////////////////
        // Bundle meta-data (set by intermediate nodes)
        ////////////////////////////////////////////////
        int custodianEid;
        int senderEid;
        int nextHopEid;
        int hopCount;
        intList visitedNodes;
        
        //SDN enabled
	    bool SDNenabled;
	    SdnRoute sdnRoute;
	
	    //Control Bundle
	    bool control;
	    //firstEid == destionationEid for the control bundle
	    int targetEid;  //this id the destinationEid of the data bundle
	    int bundleByteLength;
	    int controlBundleNumber;
        
        // Probabilistic routing meta-data
        int xmitCopiesCount;
        double dlvConfidence;
        
        //Spray And Wait number of copies variable
        int bundlesCopies;
        
        //Proactive CGR Routing specific use variable 
        // 0 to route by delivery time
        // 1 to route by hops
        // 2 to route by hops and delivery time (making a copy)
        int qos;
        
};

message TrafficGeneratorMsg{
    int bundlesNumber;
    int destinationEid;
    int size;
    double ttl;
    double interval;
    
     //Control Section

    int targetEid;
    int bundlesNumberControl;
    int sizeControl;
};

message ContactMsg{
    int id;
    double dataRate;
    simtime_t start;
    simtime_t end;
    simtime_t duration;
    int sourceEid;
    int destinationEid;
};

message ForwardingMsgEnd{
    int neighborEid;
    int contactId;
    long bundleId;
    bool sentToDestination;
};

message ForwardingMsgStart{
    int neighborEid;
    int contactId;
};

message CustodyTimout{
    int bundleId;
};


