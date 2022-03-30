#!venv/bin/python3.6

from brufn.network import *
from brufn.brufspark import BRUFSpark
from pyspark import SparkContext, SparkConf
from brufn.experiment_generator import generate_bash_script, generate_omnet_ini_file, BRUF_2

DTNSIM_PATH = '/home/simon/git/bachelorthesis/dtnsim/src' # Indicate the full path to DTNSIM src folder
net = Net.get_net_from_file('net_two_path_two_ts.py')
sources = [0]; target=3; num_of_copies=2; working_dir='working_dir';
os.makedirs(working_dir)
pf_rng = []
bruf = BRUFSpark(net, sources, target, num_of_copies, pf_rng, working_dir)
conf = SparkConf().setAppName("BRUF-Spark")
conf = (conf.setMaster('local[2]')
        .set('spark.executor.memory', '2G')
        .set('spark.driver.memory', '4G')
        .set('spark.driver.maxResultSize', '8G'))
sc = SparkContext(conf=conf)
bruf.compute_bruf(sc)
sc.stop()

routing_files_path = os.path.join(working_dir, 'routing_files/'); os.makedirs(routing_files_path, exist_ok=True)
bruf.generate_mc_to_dtnsim_all_sources_all_pf(routing_files_path)
ts_duration=10; capacity=100; cp_path=os.path.join('working_dir','cp.txt')
net.print_dtnsim_cp_to_file(ts_duration, capacity, cp_path)
ini_path = os.path.join('working_dir', 'run.ini')
traffic = {1: [4]}
generate_omnet_ini_file(net.num_of_nodes, traffic, BRUF_2, ini_path, 'cp.txt', frouting_path='routing_files/', ts_duration=ts_duration, repeats=100)
generate_bash_script('run.ini', os.path.join('working_dir', 'run_simulation.sh'), DTNSIM_PATH)
