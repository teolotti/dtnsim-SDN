import sqlite3
#import matplotlib.pyplot as plt
from functools import reduce
import sys
import os

REPS = 1

INPUT_PATH = os.getcwd() + "/results"
OUTPUT_PATH = os.getcwd() + "/results"

traffic_vector = [100,600,1100,1600,2100,2600]

# [(metric, x-axis label)]
METRICS = [("appBundleReceived:count", "Delivered Bundles"), ("deliveryRatio", "Delivery Ratio"),
           ("appBundleReceivedDelay:mean", "Mean Delay per Bundle"),
           ("appBundleReceivedHops:mean", "Mean Hops per Bundle"),
           ("routeCgrDijkstraCalls:sum", "Dijkstra Calls"),("routeDijkstraCallsXnodes:sum", "Dijkstra Calls x Nodes"),("routeExecutionTimeUs:sum", "Execution Time"),("dtnBundleReRouted:count", "Bundle ReRouted")]


def main():
    for metric in METRICS:
        f_avg_by_rep = []

        if (metric[0] == "deliveryRatio"):
            f_avg_by_rep = deliveryRatioAverage(
                "%s/dtnsim" % (INPUT_PATH))
        elif ((metric[0] == "appBundleReceivedDelay:mean") or (metric[0] == "appBundleReceivedHops:mean")):
            f_avg_by_rep = avAv2("%s/dtnsim" % (INPUT_PATH), metric[0])
        else:
            f_avg_by_rep = sumAv("%s/dtnsim" % (INPUT_PATH), metric[0])

        # save function
        text_file = open("%s/METRIC=%s.txt" % (OUTPUT_PATH, metric[0]), "w")
        text_file.write(str(f_avg_by_rep))
        text_file.close()


def sumAv(input_path, metric):

    # here we save the computed means for each traffic
    graph_data = []
    for d in traffic_vector:
        #here we save values to compute the mean of REPS repetitions
        graph_data_aux = []

        for i in range(0,REPS):
            metricResult = 0

            # Connect to database
            print("%s-bundlesNumber=%s-#%d.sca" % (input_path, d, i))
            conn = sqlite3.connect("%s-bundlesNumber=%s-#%d.sca" % (input_path, d, i))
            conn.row_factory = sqlite3.Row
            cur = conn.cursor()

            # execute sql query to get bundles received by all nodes
            cur.execute("SELECT SUM(scalarValue) AS result FROM scalar WHERE scalarName='%s'" % (metric))
            rows0 = cur.fetchall()
            metricResult += 0 if (rows0[0]["result"] == None) else rows0[0]["result"]

            cur.execute("SELECT SUM(scalarValue) AS result FROM scalar WHERE scalarName='%s'" % ("appBundleSent:count"))
            rows1 = cur.fetchall()
            tx_packet = 0 if (rows1[0]["result"] == None) else rows1[0]["result"]

            graph_data_aux.append((tx_packet, metricResult))

        graph_data.append(tuple(map(lambda y: sum(y) / float(len(y)), zip(*graph_data_aux))))

    return graph_data


def deliveryRatioAverage(input_path):
    graph_data = []
    for d in traffic_vector:
        #here we save values to compute the mean of REPS repetitions
        graph_data_aux = []

        for i in range(0,REPS):

            metricResult = 0

            # Connect to database
            print("%s-bundlesNumber=%s-#%d.sca" % (input_path, d, i))
            conn = sqlite3.connect("%s-bundlesNumber=%s-#%d.sca" % (input_path, d, i))
            conn.row_factory = sqlite3.Row
            cur = conn.cursor()

            # execute sql query to get bundles received by all nodes
            cur.execute("SELECT SUM(scalarValue) AS result FROM scalar WHERE scalarName='%s'" % ("appBundleReceived:count"))
            rows0 = cur.fetchall()
            rx_packet = 0 if (rows0[0]["result"] == None) else rows0[0]["result"]

            cur.execute("SELECT SUM(scalarValue) AS result FROM scalar WHERE scalarName='%s'" % ("appBundleSent:count"))
            rows1 = cur.fetchall()
            tx_packet = 0 if (rows1[0]["result"] == None) else rows1[0]["result"]

            metricResult += float(rx_packet) / float(tx_packet)

            graph_data_aux.append((tx_packet, metricResult))

        graph_data.append(tuple(map(lambda y: sum(y) / float(len(y)), zip(*graph_data_aux))))

    return graph_data


def avAv2(input_path, metric):
    graph_data = []
    for d in traffic_vector:
        #here we save values to compute the mean of REPS repetitions
        graph_data_aux = []

        for i in range(0,REPS):

            metricResult = 0

            # Connect to database
            print("%s-bundlesNumber=%s-#%d.sca" % (input_path, d, i))
            conn = sqlite3.connect("%s-bundlesNumber=%s-#%d.sca" % (input_path, d, i))
            conn.row_factory = sqlite3.Row
            cur = conn.cursor()

            # execute sql query to get bundles received by all nodes
            cur.execute("SELECT AVG(scalarValue) AS result FROM scalar WHERE scalarName='%s'" % (metric))
            rows0 = cur.fetchall()

            metricResult += 0 if (rows0[0]["result"] == None) else rows0[0]["result"]

            cur.execute("SELECT SUM(scalarValue) AS result FROM scalar WHERE scalarName='%s'" % ("appBundleSent:count"))
            rows1 = cur.fetchall()
            tx_packet = 0 if (rows1[0]["result"] == None) else rows1[0]["result"]

            graph_data_aux.append((tx_packet, metricResult))

        graph_data.append(tuple(map(lambda y: sum(y) / float(len(y)), zip(*graph_data_aux))))

    return graph_data


main()
