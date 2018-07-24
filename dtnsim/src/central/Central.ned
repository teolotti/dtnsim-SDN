package dtnsim.central;

simple Central
{
    parameters:
        
        @class(dtnsim::Central);
        
        bool saveTopology = default(false);
        bool saveFlows = default(false);
        bool saveLpFlows = default(false);
        string contactsFile = default("contacts.txt");
        
        bool enableAvailableRoutesCalculation = default(false);
        
          
//      	Contact Failure feature:
//      	DTNSim supports 3 differents ways in order to model contact failures:
//      		1) Delete a given number of contacts, which are randomly selected. (Set parameter deleteNContacts)
//      		2) Delete contacts with a given probability of failure. 
//      		That is, any contact fails with a given probility. (Set parameter failureProbability)
//      		3) Delete contacts by id. A list of contact ids is given, and the repectives contacts are deleted. (Set contactIdsToDelete)
//      		
//      		Only one of the above ways can work each time. If more than one is selected, 
//      		DTNSim chooses one according the above list(1 is priritized respects 2 and 2 respects 3).
//      		In order to enable a contact failure model, set some of the following parameters.    
        	       
        int deleteNContacts = default(0);
        double failureProbability = default(0); //Probability of failure of any contact in the contact plan 
		string contactIdsToDelete = default(""); //Ids of contact to be deleted
		
		bool useCentrality = default(false);
		bool faultsAware = default(true);
		
		@signal[contactsNumber];
		@signal[totalRoutes];
		@signal[availableRoutes];
		@signal[pairsOfNodesWithAtLeastOneRoute];
        @statistic[contactsNumber](title="contactsNumber"; source="contactsNumber"; record=sum; interpolationmode=none);
        @statistic[totalRoutes](title="totalRoutes"; source="totalRoutes"; record=sum; interpolationmode=none);
        @statistic[availableRoutes](title="availableRoutes"; source="availableRoutes"; record=sum; interpolationmode=none);
        @statistic[pairsOfNodesWithAtLeastOneRoute](title="pairsOfNodesWithAtLeastOneRoute"; source="pairsOfNodesWithAtLeastOneRoute"; record=sum; interpolationmode=none);
        
        @display("i=device/terminal");
}
