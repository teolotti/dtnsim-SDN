

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
        
        // Probabilistic routing meta-data
        int xmitCopiesCount;
        double dlvConfidence;
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

message ForwardingMsg{
    int neighborEid;
    int contactId;
};


