# DTNSIM #

This document describes DTNSIM, a simulator devoted to study different aspects of Delay/Disruption Tolerant Network (DTN) such as routing, forwarding, scheduling, planning, and others. The simulator is implemented on the [Omnet++ framework version 5.1](https://omnetpp.org/) and can be conveniently utilized using the Tkenv environment and modified or extended using the Omnet++ Eclipse-based IDE. Check [this video in youtube](https://youtu.be/_5HhfNULjtk) for a quick overview of the tool.

The simulator is still under development: this is a beta version. Nonetheless, feel free to use it and test it. Our contact information is below. 

## Installation ##

### Basic Installation ###

* Download [Omnet++](https://omnetpp.org/omnetpp). DTNSIM was tested on version 5.1.
* Import the DTNSIM repository from the Omnet++ IDE (File->Import->Projects from Git->Clone URI).
* Build DTNSIM project.

### Opportunistic simulations ###

DTNSIM has been updated in order to support opportunistic simulations with the model of OCGR. They can be scheduled just as regular contacts in the contact plan .txt file using the keyword "ocontact" (and "orange" for the respective ranges). In this context, there are three possible modes available, each associated with an integer:

* Mode 0: Opportunistic contacts are ignored completely. 
* Mode 1: Opportunistic contacts are simulated and are discovered dynamically by the nodes. This is the realistic use-case of opportunistic simulations.
* Mode 2: Opportunistic contacts are known by the nodes in advance, which is an unrealistic assumptions, but useful for upperbounds in a simulation context.

### Combining opportunistic and uncertain routing ###

DTNSIM features two new routing additions, ORUCoP and OCGR-UCoP, that combine opportunistic and uncertain routing approaches. As of now, the current approach includes regular calls to the Python3 implementation of L-RUCoP. This requires some setup in order to enable this call. The most convenient way is to use a virtual environment provided by the authors of RUCoP (https://www.notion.so/fraverta/Routing-Under-Uncertain-Contact-Plans-070f323ee0c745f5b283fa0af198e81b). 
Here a brief overview of the most important steps:
First you need the current implementation of RUCoP:

* mkdir libs &&
* cd libs &&
* git clone -b v-spark https://nandoraverta@bitbucket.org/nandoraverta/bruf-withcopies19.git &&
* cd ..

When this step is accomplished, all that is left to do is to install this libraries in a virtual environment:

* virtualenv venv -p /usr/bin/python3.6 &&
* source venv/bin/activate &&
* cd libs/bruf-withcopies19/dist/ &&
* tar -xvf brufn-0.0.2.tar.gz &&
* cd brufn-0.0.2 &&
* python setup.py install &&
* cd ../../../..

The required scripts to execute and parse L-RUCoP are already in this repo, being run_ibruf.py for ORUCoP and run_cgrbruf.py for OCGR-UCoP. Both are shell scripts working with the python3 interpreter from the virtualenv. All that is left to do in order to make them work is to provide the correct path to the python3.6 version at the top of the file. This will be then in the form path/to/virtualenv/bin/python3.6. 
When this is done, everything should be working, as the new routing schemes can be executed just as the other schemes. What might be noteworthy in this context, is the execution of the simulation scripts in simulations/opportunistic_uncertain_/simulation_x. As these script are essentially python scripts, one needs to execute them from within the virtual environment.
Still, it might happen, that one of the libraries of the virtualenv (PySpark) overlaps with an existing python installation on the machine, leading to an error because of the mismatch. In this case, the following commands will help:

* export PYSPARK_PYTHON=path/to/virtualenv/bin/python
* export PYSPARK_DRIVER_PYTHON=path/to/virtualenv/bin/python


### Topology Outputs (optional) ###

* Download [Boost](http://www.boost.org/users/download) and extract it. DTNSIM was tested on version 1.63.0, but should work in newer versions as well.
* Uncomment Boost corresponding line (INCLUDE_PATH) in src/makefrag file and edit according to your installation path.
* Uncomment line "//#define USE_BOOST_LIBRARIES 1" in src/Config.h: 
* Build DTNSIM project.

Note: DTNSIM can be used without boost libraries but output topology graphs will not be generated.

### Linear Programming Model (optional) ###

* Download [IBM ILOG CPLEX Optimization Studio](https://www.ibm.com/developerworks/downloads/ws/ilogcplex/) and install it.
* Uncomment cplex and concert corresponding lines (INCLUDE_PATH, LIBS, LDFLAGS and COPTS) in src/makefrag file and edit according to your installation path.
* Uncomment line "//#define USE_CPLEX_LIBRARY 1" in src/Config.h: 
* Build DTNSIM project.

Note: DTNSIM can be used without Cplex library but the linear programming model output will not be available.

## Utilization ##

* Open dtnsim_demo.ini config file from the use cases folder. Familiarize with the simulation parameters which are explained in the same file.
* Run the simulation. If using Omnet++ IDE, the Tkenv environment will be started. 
* Results (scalar and vectorial statistics) will be stored in the results folder.
* Explore more complex scenarios from the tutorial folder in the use cases folder.

Note: Nodes will remain static in the simulation visualization. Indeed, the dynamic of the network is captured in the "contact plans" which comprises a list of all time-bound communication opportunities between nodes. In other words, if simulating mobile network, the mobility should be captured in such contact plans and provided to DTNSIM as input.

## Contact Us ##

If you have any comment, suggestion, or contribution you can reach us at madoerypablo@gmail.com, juanfraire@gmail.com or s8sirink@stud.uni-saarland.de.
