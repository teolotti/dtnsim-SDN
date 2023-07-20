cplusplus {{
    #include <string>
    #include <set>
}}


////////////////////
// Msg kind = BUNDLE, not RFC conform
// - created by the App module (given a traffic generation message), sent to DTN
// - APP receival: either final destination or wrong delivery
// - DTN receival: from APP -> source node
//				   from COM -> intermediate or final node
// - COM receival: from DTN -> route determined, relay to next hop
//				   from COM -> relay to DTN
//
// TODO: experiment with more fields
packet BundlePacket {
    
    //
    // Set by source node
    //
    int bundleId;
    
    int sourceEid;
    string sourceRegion;
    int destinationEid;
    string destinationRegion;
    
    simtime_t creationTimestamp;
    simtime_t ttl;
    
    // TODO critical
    // TODO source routing path (CGR)
    // TODO source routing path (IRR)
    
    //
    // Set by intermediate nodes (meta-data)
    //
    int senderEid;
   	string senderRegion;
   	
   	int nextHopEid;
   	string nextHopRegion;
   	
    int hopCount;
	int firstHopEid;
    
    // TODO hop count, visitedNodes
    // TODO QOS
    
    bool enc; // TODO, experimental  
}

////////////////////
// Msg kind = TRAFFIC_TIMER
// Msg name = trafficGenMsg
// - scheduled by and for the App module according to some distribution at App init
// - upon receival, App will send respective bundle to its DTN module
message TrafficGenMsg{
    
    int destinationEid;
    string destinationRegion;
    int bundleNumber;
    int size;
    int ttl;

};

////////////////////
// Msg kind = CONTACT_START_TIMER or CONTACT_END_TIMER
// Msg name = regionContactStart, regionContactEnd, backboneContactStart, backboneContactEnd
//
//
// - scheduled by and for the DTN module according to the respective contact plan(s),
//   signifies that a contact has started/ended and that forwarding *can* occur / *must* end
// - upon receival: start + region -> call to intra-regional routing algo
//									  and scheduling of a regionForwardingMsgStart
//					end + region -> call to intra-regional routing algo
//									and cancellation of respective regionForwardingMsgStart message
//
//					start + backbone -> call to inter-regional routing algo 
//										and scheduling of backboneForwardingMsgStart
//					end + backbone -> same but inter-regional
message ContactsMsg{
    
    int id;
    double dataRate;
    simtime_t start;
    simtime_t end;
    simtime_t duration;
    int sourceEid;
    int destinationEid;
    
};



message EndForwardingMsg{
    int neighborEid;
    int contactId;
    long bundleId;
    bool sentToDestination;
};

////////////////////
// Msg kind = FORWARDING_MSG_START
// Msg name = regionForwardingMsgStart or backboneForwardingMsgStart
//
// - scheduled by and for the DTN module whenever a contact begins,
//	 signifies that forwarding can occur at this moment
// - upon receival:
message StartForwardingMsg{
    int neighborEid;
    int contactId;
};