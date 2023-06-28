#!/bin/bash

opp_runall -j4 ../../dtnsim omnetpp.ini -n ../../src -u Cmdenv -c sat_1GS

opp_runall -j4 ../../dtnsim omnetpp.ini -n ../../src -u Cmdenv -c sat_1HAP_1GS

: <<'END'
END
