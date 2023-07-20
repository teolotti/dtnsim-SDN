cplusplus {{
    #include <string>
    #include <set>
}}


packet BundlePacketHIRR {
    
    int bundleId; //TODO replace with number set in flow class?
    
    int sourceEid;
    int destinationEid;
    
    simtime_t creationTimestamp;
    simtime_t ttl;
    
    int priorPassageway;
    
    int type; // 0 for data, 1 for blacklist
    int missedDestination; // in case of blacklist message
    
    int senderEid;
    int nextHopEid;
}

message TrafficGenMsgHIRR {
    
    int destinationEid;
    string destinationRegion;
    int bundleNumber;
    int size;
    int ttl;

};

message ContactsMsgHIRR {
    
    int id;
    double dataRate;
    simtime_t start;
    simtime_t end;
    simtime_t duration;
    int sourceEid;
    string sourceRegion;
    int destinationEid;
    string destinationRegion;
    
};

message EndForwardingMsgHIRR {
    int neighborEid;
    string neighborRegion;
    int contactId;
    long bundleId;
    bool sentToDestination;
};

message StartForwardingMsgHIRR {
    int neighborEid;
    string neighborRegion;
    int contactId;
}; 