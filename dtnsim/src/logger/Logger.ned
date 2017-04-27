package dtnsim.logger;

///Módulo Logger. Módulo de manejo de estadísticas.
simple Logger
{
    parameters:
        
        @class(dtnsim::Logger);
        
        bool saveTopology = default(false);
        bool saveFlows = default(false);
        bool saveLpFlows = default(false);
        string contactsFile = default("contacts.txt");
        
        @display("i=device/terminal");
}
