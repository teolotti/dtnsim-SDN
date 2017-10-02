#!/bin/bash

cd 4Nodes_RandomNetwork_sprayAndWait_1x
./scriptMain.sh
cd ..

cd 4Nodes_RandomNetwork_sprayAndWait_10x
./scriptMain.sh
cd ..

cd 4Nodes_RandomNetwork_cgr1_1x
./scriptMain.sh
cd ..

cd 4Nodes_RandomNetwork_cgr1_10x
./scriptMain.sh
cd ..

: <<'END'
END



