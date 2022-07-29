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

## HDTN Support ##

HDTN (High-rate Delay Tolerant Network) flight code is supported in the master branch. To run DtnSim using HDTN routing you must 
1. Install OMNeT++. While DtnSim is tested only for OMNeT++ 5.5.1, the HDTN support was developed and used on OMNeT++ 5.7 because the 5.5.1 version contains a bug making it incompatible with the version of Qt packaged on the Debian 11 (bullseye) system on which this code was developed. OMNeT++ 5.7 is suggest for simulating HDTN on DtnSim but YMMV.
2. Install DtnSim as described above. 
3. Download the HDTN source code.
3. Build HDTN from the source. This requires, among other dependencies, an installation of libzmq3.

Follow the [instructions from the HDTN project](https://github.com/nasa/HDTN) for build and installation. Only the hdtn-router executable is actually needed from HDTN. If you wish to compile only this part you may replace the "make" step of HDTN build with "make hdtn-router" and skip the "make install" step.

## ION Support ##

Interplanetary Overlay Network (ION) flight code is supported in the support-ion branch. Currenly, ION 3.5.0 Contact Graph Routing library is supported by DTNSIM.

## Contact Us ##

If you have any comment, suggestion, or contribution you can reach us at madoerypablo@gmail.com and juanfraire@gmail.com.