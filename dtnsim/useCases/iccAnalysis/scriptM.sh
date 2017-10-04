#!/bin/bash

cd limited_Buffers_4Nodes_RandomNetwork_cgr1_1x
./scriptMain.sh
cd ..

cd limited_Buffers_4Nodes_RandomNetwork_cgr1_10x
./scriptMain.sh
cd ..

cd limited_Buffers_4Nodes_RandomNetwork_cgr3_1x
./scriptMain.sh
cd ..

cd limited_Buffers_4Nodes_RandomNetwork_cgr3_10x
./scriptMain.sh
cd ..

cd limited_Buffers_4Nodes_RandomNetwork_sprayAndWait_1x
./scriptMain.sh
cd ..

cd limited_Buffers_4Nodes_RandomNetwork_sprayAndWait_10x
./scriptMain.sh
cd ..

cd limited_Buffers_4Nodes_RandomNetwork_sprayAndWait_3copies_1x
./scriptMain.sh
cd ..

cd limited_Buffers_4Nodes_RandomNetwork_sprayAndWait_3copies_10x
./scriptMain.sh
cd ..

#########################################################################

cd 4Nodes_RandomNetwork_cgr1_1x
./scriptMain.sh
cd ..

cd 4Nodes_RandomNetwork_cgr1_10x
./scriptMain.sh
cd ..

cd 4Nodes_RandomNetwork_cgr3_1x
./scriptMain.sh
cd ..

cd 4Nodes_RandomNetwork_cgr3_10x
./scriptMain.sh
cd ..

cd 4Nodes_RandomNetwork_sprayAndWait_1x
./scriptMain.sh
cd ..

cd 4Nodes_RandomNetwork_sprayAndWait_10x
./scriptMain.sh
cd ..

cd 4Nodes_RandomNetwork_sprayAndWait_3copies_1x
./scriptMain.sh
cd ..

cd 4Nodes_RandomNetwork_sprayAndWait_3copies_10x
./scriptMain.sh
cd ..

###########################################################################

cd cgr-spray_1x
./script2.sh
cd ..

cd cgr-spray_10x
./script2.sh
cd ..

cd limited_Buffers_cgr-spray_1x
./script2.sh
cd ..

cd limited_Buffers_cgr-spray_10x
./script2.sh
cd ..

: <<'END'
END



