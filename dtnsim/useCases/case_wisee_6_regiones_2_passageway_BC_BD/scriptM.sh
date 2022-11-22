#!/bin/bash

#########################################################################

cd case0
rm -rf results
chmod +x scriptMain.sh
chmod +x script.sh
chmod +x script2.sh
./scriptMain.sh
cd ..

cd case1
rm -rf results
chmod +x scriptMain.sh
chmod +x script.sh
chmod +x script2.sh
./scriptMain.sh
cd ..

cd case2
rm -rf results
chmod +x scriptMain.sh
chmod +x script.sh
chmod +x script2.sh
./scriptMain.sh
cd ..

cd comparisonGraphs2
chmod +x script.sh
./script.sh
cd ..

: <<'END'
END
###########################################################################






