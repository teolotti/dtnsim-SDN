

cplusplus {{
    typedef std::list<int> List;
}}
class noncobject List;

packet BundlePkt{
    	// Things that are set once
    	int sourceEid;
        int destinationEid;
        bool returnToSender;
        bool critical;
        simtime_t creationTimestamp;
        simtime_t ttl;
        
        // Things that changes on each hop
        int senderEid;
        int nextHopEid;
        int hopCount;
        int xmitCopiesCount;
        double dlvConfidence;
        List originalRoute;
        List takenRoute;
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


