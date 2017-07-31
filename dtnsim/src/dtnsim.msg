

cplusplus {{
    // Include Contact and CgrRoute structures
    #include "CgrRoute.h"
    
    typedef std::list<int> intList;
    typedef std::list<CgrRoute> cgrRouteList;
}}

struct CgrRoute;
class noncobject intList;
class noncobject cgrRouteList;

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
        
        // Source routing path
        CgrRoute cgrRoute;
        
        ////////////////////////////////////////////////
        // Bundle meta-data (set by intermediate nodes)
        ////////////////////////////////////////////////
        int senderEid;
        int nextHopEid;
        int hopCount;
        intList visitedNodes;
        
        // Probabilistic routing meta-data
        int xmitCopiesCount;
        double dlvConfidence;
        
        //Spray And Wait number of copies variable
        int bundlesCopies;
        
};

message TrafficGeneratorMsg{
    int bundlesNumber;
    int destinationEid;
    int size;
    int ttl;
    int interval;
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
};

message ForwardingMsgStart{
    int neighborEid;
    int contactId;
};


