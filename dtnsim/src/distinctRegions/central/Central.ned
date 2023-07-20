package src.distinctRegions.central;


simple Central {
    
    parameters:
        
        @class(dtnsimdistinct::Central);
        
        // contact plan local to the regions, one file (CSV format)
        string regionContactPlan = default("");
        
        // contact plan involving all passageways (including backbone PWs), one file (obsolete)
        string backboneContactPlan = default("");
        
        // region database file (if IRR is used)
        string regionDatabase = default("");
}