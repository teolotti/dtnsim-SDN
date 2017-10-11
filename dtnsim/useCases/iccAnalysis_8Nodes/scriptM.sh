#!/bin/bash

#########################################################################

cd RandomNetwork_cgr1_1x
./scriptMain.sh
cd ..

cd RandomNetwork_cgr1_10x
./scriptMain.sh
cd ..

cd RandomNetwork_cgr3_1x
./scriptMain.sh
cd ..

cd RandomNetwork_cgr3_10x
./scriptMain.sh
cd ..

cd RandomNetwork_sprayAndWait_1x
./scriptMain.sh
cd ..

cd RandomNetwork_sprayAndWait_10x
./scriptMain.sh
cd ..

cd RandomNetwork_sprayAndWait_3copies_1x
./scriptMain.sh
cd ..

cd RandomNetwork_sprayAndWait_3copies_10x
./scriptMain.sh
cd ..

cd RandomNetwork_sprayAndWait_4copies_1x
./scriptMain.sh
cd ..

cd RandomNetwork_sprayAndWait_4copies_10x
./scriptMain.sh
cd ..

cd RandomNetwork_epidemic_1x
./scriptMain.sh
cd ..

cd RandomNetwork_epidemic_10x
./scriptMain.sh
cd ..

: <<'END'
END
###########################################################################

cd cgr-spray_1x
./script2.sh
cd ..

cd cgr-spray_10x
./script2.sh
cd ..






