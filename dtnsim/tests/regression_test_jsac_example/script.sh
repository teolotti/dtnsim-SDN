#!/bin/bash

# bash script that run omnet simulation for regression_test_jsac_example.ini and compare result values with original values
# and print which tests passes and which fails. If a test fails execution will stop and exit. 

# run omnet simulation
opp_runall -j4 ../../src/dtnsim regression_test_jsac_example.ini -n ../../src -u Cmdenv -c example_ss
opp_runall -j4 ../../src/dtnsim regression_test_jsac_example.ini -n ../../src -u Cmdenv -c example_aa

vec_string=".vec"
sca_string=".sca"

declare -a arr=("routingType=routeListType#3aoneBestPath,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=5-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=5-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=5-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=5-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=5-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=5-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=5-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=5-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=5-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=5-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=5-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=5-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=5-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=5-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=5-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=5-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=5-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=5-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=5-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=5-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=5-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=5-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=5-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=5-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=5-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=10-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=10-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=10-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=10-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=10-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=10-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=10-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=10-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=10-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=10-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=10-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=10-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=10-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=10-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=10-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=10-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=10-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=10-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=10-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=10-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=10-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=10-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=10-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=10-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=10-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=15-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=15-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=15-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=15-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=15-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=15-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=15-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=15-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=15-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=15-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=15-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=15-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=15-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=15-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=15-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=15-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=15-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=15-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=15-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=15-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=15-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=15-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=15-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=15-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=15-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=20-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=20-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=20-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=20-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=20-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=20-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=20-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=20-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=20-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=20-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=20-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=20-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=20-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=20-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=20-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=20-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=20-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=20-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=20-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=20-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=20-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=20-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=20-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=20-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=20-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=25-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=25-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=25-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=25-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=25-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=25-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=25-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=25-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=25-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=25-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=25-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=25-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=25-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=25-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=25-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=25-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=25-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=25-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=25-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=25-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=25-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=25-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=25-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=25-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=30-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=30-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=30-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=30-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=30-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=30-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=30-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=30-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=30-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=30-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=30-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=30-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=30-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=30-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=30-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=30-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=30-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=30-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=30-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=30-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=30-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=30-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=30-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=30-#0"
)

example_text="example_ss-"

# checking .vec files
for i in "${arr[@]}"
do
   text_string=$example_text$i$vec_string
   echo "comparing $text_string" 

   text1=originalResults_example_ss/$text_string
   text2=results/$text_string

   # compare obtained results with original results and write diffFile.txt
   sqldiff --summary $text1 $text2 > diffFile.txt

   # get lines with changes != 0
   grep -E -v '0 changes' diffFile.txt > diffFile2.txt

   # get number of different lines (must be exactly = 2) because 
   # omnet changes "run" and "runattr parameters with each run 
   # in spite of having all the same attributes
   nlines=$(wc -l < diffFile2.txt)

   if [ $nlines -ne 3 ]; then echo "TEST FAILED" && exit; else echo "TEST PASSED"; fi

   # remove output files
   rm diffFile.txt
   rm diffFile2.txt
done

# checking .sca files
for i in "${arr[@]}"
do
   text_string=$example_text$i$sca_string
   echo "comparing $text_string" 

   text1=originalResults_example_ss/$text_string
   text2=results/$text_string

   # compare obtained results with original results and write diffFile.txt
   sqldiff --summary $text1 $text2 > diffFile.txt

   # get lines with changes != 0
   grep -E -v '0 changes' diffFile.txt > diffFile2.txt

   # get number of different lines (must be exactly = 2) because 
   # omnet changes "run" and "runattr parameters with each run 
   # in spite of having all the same attributes
   nlines=$(wc -l < diffFile2.txt)

   if [ $nlines -ne 4 ]; then echo "TEST FAILED" && exit; else echo "TEST PASSED"; fi

   # remove output files
   rm diffFile.txt
   rm diffFile2.txt
done

declare -a arr2=("routingType=routeListType#3aoneBestPath,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=5,5,5-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=5,5,5-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=5,5,5-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=5,5,5-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=5,5,5-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=5,5,5-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=5,5,5-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=5,5,5-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=5,5,5-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=5,5,5-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=5,5,5-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=5,5,5-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=5,5,5-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=5,5,5-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=5,5,5-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=5,5,5-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=5,5,5-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=5,5,5-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=5,5,5-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=5,5,5-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=5,5,5-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=5,5,5-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=5,5,5-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=5,5,5-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=5,5,5-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=10,10,10-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=10,10,10-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=10,10,10-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=10,10,10-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=10,10,10-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=10,10,10-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=10,10,10-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=10,10,10-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=10,10,10-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=10,10,10-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=10,10,10-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=10,10,10-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=10,10,10-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=10,10,10-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=10,10,10-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=10,10,10-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=10,10,10-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=10,10,10-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=10,10,10-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=10,10,10-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=10,10,10-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=10,10,10-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=10,10,10-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=10,10,10-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=10,10,10-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=15,15,15-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=15,15,15-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=15,15,15-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=15,15,15-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=15,15,15-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=15,15,15-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=15,15,15-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=15,15,15-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=15,15,15-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=15,15,15-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=15,15,15-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=15,15,15-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=15,15,15-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=15,15,15-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=15,15,15-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=15,15,15-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=15,15,15-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=15,15,15-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=15,15,15-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=15,15,15-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=15,15,15-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=15,15,15-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=15,15,15-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=15,15,15-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=15,15,15-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=20,20,20-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=20,20,20-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=20,20,20-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=20,20,20-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=20,20,20-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=20,20,20-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=20,20,20-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=20,20,20-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=20,20,20-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=20,20,20-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=20,20,20-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=20,20,20-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=20,20,20-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=20,20,20-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=20,20,20-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=20,20,20-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=20,20,20-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=20,20,20-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=20,20,20-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=20,20,20-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=20,20,20-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=20,20,20-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=20,20,20-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=20,20,20-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=20,20,20-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=25,25,25-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=25,25,25-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=25,25,25-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=25,25,25-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=25,25,25-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=25,25,25-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=25,25,25-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=25,25,25-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=25,25,25-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=25,25,25-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=25,25,25-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=25,25,25-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=25,25,25-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=25,25,25-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=25,25,25-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=25,25,25-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=25,25,25-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=25,25,25-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=25,25,25-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=25,25,25-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=25,25,25-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=25,25,25-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=25,25,25-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=25,25,25-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=30,30,30-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=30,30,30-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=30,30,30-#0"
"routingType=routeListType#3aoneBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=30,30,30-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=30,30,30-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=30,30,30-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=30,30,30-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=30,30,30-#0"
"routingType=routeListType#3aperNeighborBestPath,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=30,30,30-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=30,30,30-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=30,30,30-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=30,30,30-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=30,30,30-#0"
"routingType=routeListType#3aallPaths-firstEnding,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=30,30,30-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=30,30,30-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=30,30,30-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=30,30,30-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=30,30,30-#0"
"routingType=routeListType#3aallPaths-firstDepleted,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=30,30,30-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aoff,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=30,30,30-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3a1stContact,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=30,30,30-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aoff,contactPlan#3alocal,bundlesNumber=30,30,30-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3alocal,bundlesNumber=30,30,30-#0"
"routingType=routeListType#3aallPaths-initial+anchor,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,bundlesNumber=30,30,30-#0"
)

example_text="example_aa-"

# checking .vec files
for i in "${arr2[@]}"
do
   text_string=$example_text$i$vec_string
   echo "comparing $text_string" 

   text1=originalResults_example_aa/$text_string
   text2=results/$text_string

   # compare obtained results with original results and write diffFile.txt
   sqldiff --summary $text1 $text2 > diffFile.txt

   # get lines with changes != 0
   grep -E -v '0 changes' diffFile.txt > diffFile2.txt

   # get number of different lines (must be exactly = 2) because 
   # omnet changes "run" and "runattr parameters with each run 
   # in spite of having all the same attributes
   nlines=$(wc -l < diffFile2.txt)

   if [ $nlines -ne 3 ]; then echo "TEST FAILED" && exit; else echo "TEST PASSED"; fi

   # remove output files
   rm diffFile.txt
   rm diffFile2.txt
done

# checking .sca files
for i in "${arr2[@]}"
do
   text_string=$example_text$i$sca_string
   echo "comparing $text_string" 

   text1=originalResults_example_aa/$text_string
   text2=results/$text_string

   # compare obtained results with original results and write diffFile.txt
   sqldiff --summary $text1 $text2 > diffFile.txt

   # get lines with changes != 0
   grep -E -v '0 changes' diffFile.txt > diffFile2.txt

   # get number of different lines (must be exactly = 2) because 
   # omnet changes "run" and "runattr parameters with each run 
   # in spite of having all the same attributes
   nlines=$(wc -l < diffFile2.txt)

   if [ $nlines -ne 4 ]; then echo "TEST FAILED" && exit; else echo "TEST PASSED"; fi

   # remove output files
   rm diffFile.txt
   rm diffFile2.txt
done

: <<'END'
END


