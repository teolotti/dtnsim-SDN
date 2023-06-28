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
                input_path = INPUT_PATH + scenario + "TTF=%ds-#%d.sca" % (x, rep)
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

INPUT_PATH = os.getcwd() + "/"

scenarios = [
'results_1sat_1GS/sat_1GS-',
'results_1sat_1HAP_1GS/sat_1HAP_1GS-',
]

l0 = 'LEO-GS'
l1 = 'LEO-HAP-GS'

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

TTF = [200, 400, 600, 800, 1000, 1200]
repetitions = list(range(0,50))
results1 = get_delivery_ratio(INPUT_PATH, scenarios, TTF, repetitions)

WIDTH = 7
HEIGHT = 5
LINEWIDTH = 0.4

import seaborn as sns
clr = sns.color_palette(None, 6)

fig, ax = plt.subplots(figsize=(WIDTH,HEIGHT))
sns.lineplot(x=results1[scenarios[0]][0], y=results1[scenarios[0]][1], color=clr[0], marker=m0, label=l0, markersize=mar0, markeredgecolor="none", ax=ax)
sns.lineplot(x=results1[scenarios[1]][0], y=results1[scenarios[1]][1], color=clr[1], marker=m1, label=l1, markersize=mar1, markeredgecolor="none", ax=ax)
ax.set_xlabel("TTF [s]", fontsize=13)
ax.set_ylabel("Delivery ratio", fontsize=13)
plt.tight_layout()
plt.grid(linewidth=LINEWIDTH, zorder=0)
ax.legend(loc='lower left', prop={'size': 12})
plt.savefig("delivery_ratio.png")
plt.cla()
plt.clf()

print("end")


