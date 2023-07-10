import sqlite3
import matplotlib.pyplot as plt
import seaborn as sns
import os
import pandas as pd
from mpl_toolkits.axes_grid1.inset_locator import zoomed_inset_axes
from mpl_toolkits.axes_grid1.inset_locator import mark_inset
from matplotlib.ticker import FixedFormatter, FixedLocator


# results["data"] = (xss, values)
def get_delivery_ratio(INPUT_PATH, scenarios, xs, repetitions):
    results = {}

    for scenario in scenarios:

        xss = []
        values = []

        for x in xs:
            for rep in repetitions:
                input_path = INPUT_PATH + scenario + "General-TTF=%ds-#%d.sca" % (x, rep)
                print(input_path)

                xss.append(x)

                conn = sqlite3.connect(input_path)
                conn.row_factory = sqlite3.Row
                cur = conn.cursor()

                cur.execute("SELECT SUM(scalarValue) AS result FROM scalar WHERE scalarName='%s'"%("appBundleSent:count"))
                rows1 = cur.fetchall()
                tx_packets = 0 if (rows1[0]["result"] == None) else rows1[0]["result"]

                cur.execute("SELECT SUM(scalarValue) AS result FROM scalar WHERE scalarName='%s'"%("appBundleReceived:count"))
                rows1 = cur.fetchall()
                rx_packets = 0 if (rows1[0]["result"] == None) else rows1[0]["result"]

                delivery_ratio = float(rx_packets) / float(tx_packets)
                values.append(delivery_ratio)

        results[scenario] = (xss, values)
    return results

# results["data"] = (xss, values)
def get_delivery_delay(INPUT_PATH, scenarios, xs, repetitions):
    results = {}

    for scenario in scenarios:

        xss = []
        values = []

        for x in xs:
            for rep in repetitions:
                input_path = INPUT_PATH + scenario + "General-TTF=%ds-#%d.sca" % (x, rep)
                print(input_path)

                xss.append(x)

                conn = sqlite3.connect(input_path)
                conn.row_factory = sqlite3.Row
                cur = conn.cursor()

                query = "SELECT AVG(scalarValue) AS result FROM scalar WHERE scalarName='%s'" % "appBundleReceivedDelay:mean"
                cur.execute(query)
                rows1 = cur.fetchall()
                mean_delay = 0 if (rows1[0]["result"] == None) else rows1[0]["result"]
                values.append(mean_delay)

        results[scenario] = (xss, values)
    return results

# results["data"] = (xss, avg_values, max_values)
def get_buffer_occupancy(INPUT_PATH, scenarios, xs, repetitions, node):
    results = {}

    for scenario in scenarios:

        xss = []
        values_avg = []
        values_max = []

        for x in xs:
            for rep in repetitions:
                input_path = INPUT_PATH + scenario + "General-TTF=%ds-#%d.sca" % (x, rep)
                print(input_path)

                xss.append(x)

                conn = sqlite3.connect(input_path)
                conn.row_factory = sqlite3.Row
                cur = conn.cursor()

                query_avg = "SELECT AVG(scalarValue) AS result FROM scalar WHERE scalarName='%s'" % "sdrBundleStored:timeavg"
                query_avg += " AND moduleName='dtnsim.node[%s].dtn'" % node
                cur.execute(query_avg)
                rows1 = cur.fetchall()
                value_avg = 0 if (rows1[0]["result"] == None) else rows1[0]["result"]
                values_avg.append(value_avg)

                query_max = "SELECT AVG(scalarValue) AS result FROM scalar WHERE scalarName='%s'" % "sdrBundleStored:max"
                query_max += " AND moduleName='dtnsim.node[%s].dtn'" % node
                cur.execute(query_max)
                rows2 = cur.fetchall()
                value_max = 0 if (rows2[0]["result"] == None) else rows2[0]["result"]
                values_max.append(value_max)

        results[scenario] = (xss, values_avg, values_max)
    return results

INPUT_PATH = os.getcwd() + "/"

scenarios = [
'../1LEO_1GS/results/',
'../1LEO_2GS/results/',
'../1LEO_5GS/results/',
'../1LEO_10GS/results/',
'../1LEO_1HAP_1GS/results/',
]

l0 = '1LEO-1GS'
l1 = '1LEO-2GS'
l2 = '1LEO-5GS'
l3 = '1LEO-10GS'
l4 = '1LEO-1HAP-1GS'

m0 = "s"
m1 = "v"
m2 = "o"
m3 = "D"
m4 = "p"

mar0 = 7
mar1 = 7
mar2 = 7
mar3 = 7
mar4 = 9

TTF = [10] + list(range(20,100,20)) + list(range(100,1100,100))
#TTF = list(range(100,1100,100))
repetitions = list(range(0,50))
#results1 = get_delivery_ratio(INPUT_PATH, scenarios, TTF, repetitions)
#results2 = get_delivery_delay(INPUT_PATH, scenarios, TTF, repetitions)
results3 = get_buffer_occupancy(INPUT_PATH, ['../1LEO_1HAP_1GS/results/'], TTF, repetitions, "11")

WIDTH = 7
HEIGHT = 5
LINEWIDTH = 0.4

import seaborn as sns
clr = sns.color_palette(None, 6)
clr[3], clr[4] = clr[4], clr[3]
clr[1], clr[2] = clr[2], clr[1]
clr[0], clr[1] = clr[1], clr[0]

# fig, ax = plt.subplots(figsize=(WIDTH,HEIGHT))
# sns.lineplot(x=results1[scenarios[0]][0], y=results1[scenarios[0]][1], color=clr[0], marker=m0, label=l0, markersize=mar0, markeredgecolor="none", ax=ax)
# sns.lineplot(x=results1[scenarios[1]][0], y=results1[scenarios[1]][1], color=clr[1], marker=m1, label=l1, markersize=mar1, markeredgecolor="none", ax=ax)
# sns.lineplot(x=results1[scenarios[2]][0], y=results1[scenarios[2]][1], color=clr[2], marker=m2, label=l2, markersize=mar2, markeredgecolor="none", ax=ax)
# sns.lineplot(x=results1[scenarios[3]][0], y=results1[scenarios[3]][1], color=clr[3], marker=m3, label=l3, markersize=mar3, markeredgecolor="none", ax=ax)
# sns.lineplot(x=results1[scenarios[4]][0], y=results1[scenarios[4]][1], color=clr[4], marker=m4, label=l4, markersize=mar4, markeredgecolor="none", ax=ax)
# ax.set_xlabel("TTF [s]", fontsize=13)
# ax.set_ylabel("Delivery ratio", fontsize=13)
# plt.tight_layout()
# plt.grid(linewidth=LINEWIDTH, zorder=0)
# ax.legend(loc='lower right', prop={'size': 12})
# plt.savefig("delivery_ratio.png")
# plt.cla()
# plt.clf()
#
# fig, ax = plt.subplots(figsize=(WIDTH,HEIGHT))
# sns.lineplot(x=results2[scenarios[0]][0], y=results2[scenarios[0]][1], color=clr[0], marker=m0, label=l0, markersize=mar0, markeredgecolor="none", ax=ax)
# sns.lineplot(x=results2[scenarios[1]][0], y=results2[scenarios[1]][1], color=clr[1], marker=m1, label=l1, markersize=mar1, markeredgecolor="none", ax=ax)
# sns.lineplot(x=results2[scenarios[2]][0], y=results2[scenarios[2]][1], color=clr[2], marker=m2, label=l2, markersize=mar2, markeredgecolor="none", ax=ax)
# sns.lineplot(x=results2[scenarios[3]][0], y=results2[scenarios[3]][1], color=clr[3], marker=m3, label=l3, markersize=mar3, markeredgecolor="none", ax=ax)
# sns.lineplot(x=results2[scenarios[4]][0], y=results2[scenarios[4]][1], color=clr[4], marker=m4, label=l4, markersize=mar4, markeredgecolor="none", ax=ax)
# ax.set_xlabel("TTF [s]", fontsize=13)
# ax.set_ylabel("Mean Delivery Delay [s]", fontsize=13)
# plt.tight_layout()
# plt.grid(linewidth=LINEWIDTH, zorder=0)
# ax.legend(loc='lower right', prop={'size': 12})
# plt.savefig("delivery_delay.png")
# plt.cla()
# plt.clf()

fig, ax = plt.subplots(figsize=(WIDTH,HEIGHT))
sns.lineplot(x=results3[scenarios[4]][0], y=results3[scenarios[4]][2], color=clr[4], marker=m4, label="max", markersize=mar4, markeredgecolor="none", ax=ax)
sns.lineplot(x=results3[scenarios[4]][0], y=results3[scenarios[4]][1], color=clr[4], marker=m4, linestyle='--', label="mean", markersize=mar4, markeredgecolor="none", ax=ax)
ax.set_xlabel("TTF [s]", fontsize=13)
ax.set_ylabel("Buffer Occupancy [packets]", fontsize=13)
plt.tight_layout()
plt.grid(linewidth=LINEWIDTH, zorder=0)
ax.legend(loc='upper right', prop={'size': 12})
plt.savefig("buffer_occupancy.png")
plt.cla()
plt.clf()

print("end")


