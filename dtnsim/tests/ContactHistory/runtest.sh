#! /bin/sh
#
# usage: runtest [<testfile>...]
# without args, runs all *.test files in the current directory
#

MAKE="make MODE=debug"
DBG_SUFFIX="_dbg"

TESTFILES=$*
if [ "x$TESTFILES" = "x" ]; then TESTFILES='*.test'; fi
if [ ! -d work ];  then mkdir work; fi
opp_test gen $OPT -v $TESTFILES || exit 1
echo



(cd work; opp_makemake -f --deep -ldtnsim$DBG_SUFFIX -L../../.. -P . --no-deep-includes -I../../..; $MAKE) || exit 1
echo
opp_test run -p work$DBG_SUFFIX $OPT -v $TESTFILES || exit 1
echo
echo Results can be found in ./work
