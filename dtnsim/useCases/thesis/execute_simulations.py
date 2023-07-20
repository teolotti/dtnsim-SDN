import os

baseDir = '~/dtnsim/dtnsim/useCases/thesis/scenario'
runOption = ' -r "\$size==10000000"'

for t in ["a", "b"]:
    for scenario in ["3"]:
        for days in ["1", "2", "3", "4"]:

            #runDir = baseDir + scenario + '/cgr/' + t + '/' + days
            #runDir = baseDir + scenario + '/irr/grk/default/' + t + '/' + days
            #runDir = baseDir + scenario + '/irr/grk/direct_to_earth/' + t + '/' + days
            #runDir = baseDir + scenario + '/irr/grk/no_inter_relay/' + t + '/' + days
            #runDir = baseDir + scenario + '/irr/lrk/default/' + t + '/' + days
            runDir = baseDir + scenario + '/irr/lrk/no_inter_relay/' + t + '/' + days 

            #runCommand = 'opp_runall -j8 ../../../../../../dtnsim -m -u Cmdenv -c traffic -n ../../../../../.. cgr.ini' #+ runOption
            #runCommand = 'opp_runall -j8 ../../../../../../../../dtnsim -m -u Cmdenv -c traffic -n ../../../../../../../.. grk.ini' + runOption
            runCommand = 'opp_runall -j8 ../../../../../../../../dtnsim -m -u Cmdenv -c traffic -n ../../../../../../../.. lrk.ini' + runOption
            print("DIR:", runDir)
            command = 'cd ' + runDir + ' && ' + runCommand 
            os.system(command)

#os.system('cd scenario1/analysis && python3 cgr_analysis_2.py')


