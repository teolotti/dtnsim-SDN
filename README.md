# DTNSIM #

DTNSIM is a simulator devoted to study different aspects of Delay/Disruption Tolerant Network (DTN) such as routing, forwarding, scheduling, planning, and others. 

The simulator is implemented on the [Omnet++ framework version 5.5.1](https://omnetpp.org/) and can be conveniently utilized using the Qtenv environment and modified or extended using the Omnet++ Eclipse-based IDE. Check [this video in youtube](https://youtu.be/_5HhfNULjtk) for a quick overview of the tool.

The simulator is still under development: this is a beta version. Nonetheless, feel free to use it and test it. Our contact information is below. 

## Installation ##

* Download [Omnet++](https://omnetpp.org/omnetpp). DTNSIM was tested on version 5.5.1.
* Import the DTNSIM repository from the Omnet++ IDE (File->Import->Projects from Git->Clone URI).
* Build DTNSIM project.

* Open dtnsim_demo.ini config file from the use cases folder.
* Run the simulation. If using Omnet++ IDE, the Qtenv environment will be started. 
* Results (scalar and vectorial statistics) will be stored in the results folder.

Note: Nodes will remain static in the simulation visualization. Indeed, the dynamic of the network is captured in the "contact plans" which comprises a list of all time-bound communication opportunities between nodes. In other words, if simulating mobile network, the mobility should be captured in such contact plans and provided to DTNSIM as input.

## ION Support ##

Interplanetary Overlay Network (ION) flight code is supported in the support-ion branch. Currenly, ION 3.5.0 Contact Graph Routing library is supported by DTNSIM.

## HDTN Support ##

High-rate Delay Tolerant Network (HDTN) flight code is supported in the support-hdtn branch. To run DTNSIM using HDTN routing

* Install OMNeT++. While DtnSim is tested only for OMNeT++ 5.5.1, the HDTN support was developed and used on OMNeT++ 5.7 because the 5.5.1 version [contains a bug](https://github.com/omnetpp/omnetpp/issues/874) making it incompatible with the version of Qt packaged on the Debian 11 (bullseye) system on which this code was developed. OMNeT++ 5.7 is suggested for simulating HDTN on DTNSIM but YMMV.
* Install DTNSIM as described above. 
* Download the HDTN source code.
* Build HDTN from the source. This requires, among other dependencies, an installation of libzmq3.

Follow the [instructions from the HDTN project](https://github.com/nasa/HDTN) to build HDTN. Only the hdtn-router executable is actually needed from HDTN. If you wish to compile only this part you may replace the "make" step of HDTN build with "make hdtn-router". The "make install" step may be skipped if desired; the simulator does not assume installation but does assume that an HDTN Router has been built in a directory named "build" located in the source code root directory of HDTN.

Three paremeters specific to HDTN should be provided in any ini file written for simulating HDTN:

* dtnsim.central.hdtnSourceRoot
	* An absolute path to a directory containing the HDTN source code
* dtnsim.central.contactsFileJson
	* A JSON version of the contact plan--should be the name of a file relative to the path hdtnSourceRoot/module/scheduler/src/, which is the standard location for HDTN to keep contact plans. Any contact plans used in HDTN simulation must have JSON versions kept in that directory. This is in addition to the ION version of the contact plan that DTNSIM needs (provided as always by the dtnsim.central.contactsFile parameter)
* dtnsim.central.hdtnRoutingMode
	* One of "libcgr" or "hdtn-router". In the first mode DTNSIM nodes will directly call routing code copied from an old version of HDTN and compiled into the simulator (similar to the implementation strategy of the support-ion-350 branch). In the second mode nodes will instead run an hdtn-router executable and use ZeroMQ to listen for the route update messages that the HDTN Router publishes. The second mode is the preferred approach around which HDTN support in DTNSIM is designed. As long as the Router module in HDTN continues to respect the same command line interface and messaging protocol, "hdtn-router" mode will continue to accurately replicates HDTN's routing decisions while abstracting away its internal implementation details. Since HDTN routing algorithms and approaches are under active development and exploration, using these CLI and messaging APIs allows DTNSIM to stay up to date with HDTN without requiring maintenance every time HDTN changes. The "libcgr" mode remains available for the time being because it has proven useful for developing HDTN simulation (particularly as a target to benchmark the simulation performance of the "hdtn-router" mode against).

Some example HDTN simulation scenario are available in simulations/hdtn_demo/

## Contact Us ##

If you have any comment, suggestion, or contribution you can reach us at madoerypablo@gmail.com and juanfraire@gmail.com. For questions about HDTN contact tjr@berkeley.edu.
