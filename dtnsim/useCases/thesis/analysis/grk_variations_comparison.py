import sqlite3
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

# Plot Configurations
sns.set_style("whitegrid")
colors = ["#38B000", "#F72585", "#0D92D3", "#F49301"]
palette = sns.color_palette(colors)
plt.rcParams['axes.grid'] = True
plt.rcParams['font.family'] = 'monospace'

# Parameters
days = ["1", "2", "3", "4"]

'''
Comparing CGR with varations of GRK
- Average bundle delay
- Bundle delivery ratio
- Total routing computation time at relays
- Average routing statistics at relays (median over scenarios)
- Average routing table size at single relay
'''



dayLabels = {"1": "3d", "2": "7d", "3": "14d", "4": "28d"}

def bundleDelay(type, distribution, size):

    data = []

    for scenario in ["scenario2", "scenario3"]:
        for routing, routingDir in [("cgr", "cgr"), ("grk", "irr/grk/default"), ("grk_earth", "irr/grk/direct_to_earth"), ("grk_no_relay", "irr/grk/no_inter_relay")]:
            for day in ["1", "2", "3", "4"]:

                input_path = "../{}/{}/{}/{}/results/traffic-distribution={},size={},ttl=2500000-#0.sca".format(scenario, routingDir, type,day, distribution, size)
                print(input_path)
                conn = sqlite3.connect(input_path)
                conn.row_factory = sqlite3.Row
                cur = conn.cursor()
                cur.execute("SELECT AVG(scalarValue) AS result FROM scalar WHERE scalarName='{}'".format("appBundleReceivedDelay:mean"))
                rows = cur.fetchall()
                data.append([rows[0]['result'], dayLabels[day], scenario, routing])
                #data.append([rows[0]['result'], labels[routing][scenario][day], scenario, routing])
    
    df = pd.DataFrame(data, columns=["result", "days", "scenario", "routing"])                    
    #df = pd.DataFrame(data, columns=["result", "contacts", "scenario", "routing"])
    return df


def bundleDeliveryRatio(type, distribution, size):

    data = []

    for scenario in ["scenario2", "scenario3"]:
        for routing, routingDir in [("cgr", "cgr"), ("grk", "irr/grk/default"), ("grk_earth", "irr/grk/direct_to_earth"), ("grk_no_relay", "irr/grk/no_inter_relay")]:
            for day in ["1", "2", "3", "4"]:

                input_path = "../{}/{}/{}/{}/results/traffic-distribution={},size={},ttl=2500000-#0.sca".format(scenario, routingDir, type,day, distribution, size)
                conn = sqlite3.connect(input_path)
                conn.row_factory = sqlite3.Row
                cur = conn.cursor()

                cur.execute("SELECT SUM(scalarValue) AS result FROM scalar WHERE scalarName='{}'".format("appBundleReceived:count"))
                rows0 = cur.fetchall()
                successfullyDelivered = rows0[0]["result"]
                cur.execute("SELECT SUM(scalarValue) AS result FROM scalar WHERE scalarName='{}'".format("appBundleSent:count"))
                rows1 = cur.fetchall()
                overallSent = rows1[0]["result"]

                data.append([successfullyDelivered/overallSent, dayLabels[day], scenario, routing])
                #data.append([successfullyDelivered/overallSent, labels[routing][scenario][day], scenario, routing])

    df = pd.DataFrame(data, columns=["result", "days", "scenario", "routing"])                
    #df = pd.DataFrame(data, columns=["result", "contacts", "scenario", "routing"])
    return df


def muleTime(type, distribution, size):

    data = []

    for scenario in ["scenario2", "scenario3"]:
        for routing, routingDir in [("cgr", "cgr"), ("grk", "irr/grk/default"), ("grk_earth", "irr/grk/direct_to_earth"), ("grk_no_relay", "irr/grk/no_inter_relay")]:
            for day in ["1", "2", "3", "4"]:

                if scenario == "scenario2":
                    if type == "a":
                        mules = [7, 8, 9]
                    else:
                        mules = [7, 8, 9, 10, 11]
                else:
                    if type == "a":
                        mules = [23, 24, 25]
                    else:
                        mules = [23, 24, 25, 26, 27]

                input_path = "../{}/{}/{}/{}/results/traffic-distribution={},size={},ttl=2500000-#0.sca".format(scenario, routingDir, type,day, distribution, size)
                conn = sqlite3.connect(input_path)
                conn.row_factory = sqlite3.Row
                cur = conn.cursor()

                result = 0
                for mule in mules:

                    cur.execute("SELECT scalarValue AS result FROM scalar WHERE scalarName='{}' AND moduleName LIKE '%{}%'".format("bundleReceivedFromCom:count", mule))
                    rows = cur.fetchall()
                    totalBundlesProcessedByMule = rows[0]["result"]

                    cur.execute("SELECT scalarValue AS result FROM scalar WHERE scalarName='{}' AND moduleName LIKE '%{}%'".format("routeExecutionTimeUs:sum", mule))
                    rows = cur.fetchall()
                    routingTime = 0 if (rows[0]["result"] == 0) else (rows[0]["result"] / totalBundlesProcessedByMule)
                    result += routingTime

                data.append([result/len(mules), dayLabels[day], scenario, routing])
                #data.append([result, labels[routing][scenario][day], scenario, routing])

    df = pd.DataFrame(data, columns=["result", "days", "scenario", "routing"])                    
    #df = pd.DataFrame(data, columns=["result", "contacts", "scenario", "routing"])
    return df


# TODO how to stats with IRR?
def muleStats(type, distribution, size):

    data = []

    for scenario in ["scenario2", "scenario3"]:
        for routing, routingDir in [("cgr", "cgr"), ("grk", "irr/grk/default"), ("grk_earth", "irr/grk/direct_to_earth"), ("grk_no_relay", "irr/grk/no_inter_relay")]:
            for day in ["1", "2", "3", "4"]:

                if scenario == "scenario2":
                    if type == "a":
                        mules = [7, 8, 9]
                    else:
                        mules = [7, 8, 9, 10, 11]
                else:
                    if type == "a":
                        mules = [23, 24, 25]
                    else:
                        mules = [23, 24, 25, 26, 27]

                input_path = "../{}/{}/{}/{}/results/traffic-distribution={},size={},ttl=2500000-#0.sca".format(scenario, routingDir, type,day, distribution, size)
                conn = sqlite3.connect(input_path)
                conn.row_factory = sqlite3.Row
                cur = conn.cursor()

                totalBundlesSeen = 0
                totalRouteSearchStarts = 0
                totalRouteSearchCalls = 0
                totalYenIterations = 0
                for mule in mules:
                    cur.execute("SELECT scalarValue AS result FROM scalar WHERE scalarName='{}' AND moduleName LIKE '%{}%'".format("bundleReceivedFromCom:count", mule))
                    rows = cur.fetchall()
                    totalBundlesSeen += rows[0]["result"]

                    cur.execute("SELECT scalarValue AS result FROM scalar WHERE scalarName='{}' AND moduleName LIKE '%{}%'".format("routeSearchStarts:sum", mule))
                    rows = cur.fetchall()
                    totalRouteSearchStarts += rows[0]["result"]

                    cur.execute("SELECT scalarValue AS result FROM scalar WHERE scalarName='{}' AND moduleName LIKE '%{}%'".format("routeSearchCalls:sum", mule))
                    rows = cur.fetchall()
                    totalRouteSearchCalls += rows[0]["result"]

                    cur.execute("SELECT scalarValue AS result FROM scalar WHERE scalarName='{}' AND moduleName LIKE '%{}%'".format("yenIterations:sum", mule))
                    rows = cur.fetchall()
                    totalYenIterations += rows[0]["result"]

                data.append([round(totalBundlesSeen/len(mules)),
                            round(totalRouteSearchStarts/len(mules)),
                            round(totalRouteSearchCalls/len(mules)),
                            round(totalYenIterations/len(mules)),
                            dayLabels[day], scenario, routing])
                
    df = pd.DataFrame(data, columns=["bundles", "starts", "calls", "yens", "days", "scenario", "routing"])
    df = df.groupby(['scenario', 'routing'])['bundles', 'starts', 'calls', 'yens', 'days'].median()
    return df


def muleTable(type, distribution, size):

    data = []

    for scenario in ["scenario2", "scenario3"]:
        for routing, routingDir in [("cgr", "cgr"), ("grk", "irr/grk/default"), ("grk_earth", "irr/grk/direct_to_earth"), ("grk_no_relay", "irr/grk/no_inter_relay")]:
            for day in ["1", "2", "3", "4"]:

                if scenario == "scenario2":
                    if type == "a":
                        mules = [7, 8, 9]
                    else:
                        mules = [7, 8, 9, 10, 11]
                else:
                    if type == "a":
                        mules = [23, 24, 25]
                    else:
                        mules = [23, 24, 25, 26, 27]

                input_path = "../{}/{}/{}/{}/results/traffic-distribution={},size={},ttl=2500000-#0.sca".format(scenario, routingDir, type,day, distribution, size)
                conn = sqlite3.connect(input_path)
                conn.row_factory = sqlite3.Row
                cur = conn.cursor()

                result = 0
                for mule in mules:
                    cur.execute("SELECT scalarValue AS result FROM scalar WHERE scalarName='{}' AND moduleName LIKE '%{}%'".format("routeTableSize:mean", mule))
                    rows = cur.fetchall()
                    tableSize = 0 if (rows[0]["result"] == None) else rows[0]["result"]
                    result += tableSize

                data.append([result/len(mules), dayLabels[day], scenario, routing])
                #data.append([result/len(mules), labels[routing][scenario][day], scenario, routing])

    df = pd.DataFrame(data, columns=["result", "days", "scenario", "routing"])                    
    #df = pd.DataFrame(data, columns=["result", "contacts", "scenario", "routing"])
    return df




def plotBundle(t):

    fig, axs = plt.subplots(1,4, sharey=False, figsize=(18, 4.2))

    # Scenario 2
    axs[0].set_title("(a.1)", size='x-large')
    delay = bundleDelay(t, "uniform", "10000000")
    g = sns.scatterplot(y='result', x='days', data=delay.loc[delay["scenario"] == "scenario2"], style='routing', hue='routing', markers=['o', 'X', 'D', 'P'], s=80, ax=axs[0], palette=palette)
    g.set_ylabel("Avg. Bundle Delay [s]", size='x-large')
    g.xaxis.grid(False)
    g.set(xlabel=None)

    axs[1].set_title("(a.2)", size='x-large')
    ratio = bundleDeliveryRatio(t, "uniform", "10000000")
    g = sns.scatterplot(y='result', x='days', data=ratio.loc[ratio["scenario"] == "scenario2"], hue='routing', style='routing', markers=['o', 'X', 'D', 'P'], s=80, ax=axs[1], palette=palette)
    g.set_ylabel("Bundle Delivery Ratio", size='x-large')
    g.xaxis.grid(False)
    g.set(xlabel=None)


    # Scenario 3
    axs[2].set_title("(b.1)", size='x-large')
    g = sns.scatterplot(y='result', x='days', data=delay.loc[delay["scenario"] == "scenario3"], style='routing', hue='routing', markers=['o', 'X', 'D', 'P'], s=80, ax=axs[2], palette=palette)
    g.set_ylabel("Avg. Bundle Delay [s]", size='x-large')
    g.xaxis.grid(False)
    g.sharey(axs[0])
    g.set(xlabel=None)

    axs[3].set_title("(b.2)", size='x-large')
    g = sns.scatterplot(y='result', x='days', data=ratio.loc[ratio["scenario"] == "scenario3"], hue='routing', style='routing', markers=['o', 'X', 'D', 'P'], s=80, ax=axs[3], palette=palette)
    g.set_ylabel("Bundle Delivery Ratio", size='x-large')
    g.xaxis.grid(False)
    g.sharey(axs[1])
    g.set(xlabel=None)

    handles, _ = axs[0].get_legend_handles_labels()
    fig.legend(handles, ['CGR', 'GRK', 'GRK (direct to Earth)', 'GRK (no inter-relay)'], loc='lower center', ncol=4, fontsize='x-large')
    axs[0].get_legend().remove()
    axs[1].get_legend().remove()
    axs[2].get_legend().remove()
    axs[3].get_legend().remove()

    plt.subplots_adjust(left=0.045,
                    bottom=0.236,
                    right=0.974,
                    top=0.93,
                    wspace=0.37,
                    hspace=0.2)
    sns.despine(bottom=True)
    plt.show()

def plotRouting(t):

    fig, axs = plt.subplots(1,4, sharey=False, figsize=(18, 4.2))

    # Scenario 2
    axs[0].set_title("(a.1)", size='x-large')
    time = muleTime(t, "uniform", "10000000")
    g = sns.lineplot(y='result', x='days', data=time.loc[time["scenario"] == "scenario2"], style='routing', hue='routing', markers=['o', 'X', 'D', 'P'], markersize=8, ax=axs[0], palette=palette)
    g.set(yscale='log', xlabel=None)
    g.set_ylabel("Routing Time / Bundle [μs]", size='x-large')
    g.xaxis.grid(False)
    g.sharey(axs[2])


    axs[1].set_title("(a.2)", size='x-large')
    table = muleTable(t, "uniform", "10000000")
    g = sns.scatterplot(y='result', x='days', data=table.loc[table["scenario"] == "scenario2"], hue='routing', style='routing', markers=['o', 'X', 'D', 'P'], s=80, ax=axs[1], palette=palette)
    g.set(yscale='log')
    g.set(xlabel=None)
    g.set_ylabel("Number of Routes", size='x-large')
    g.xaxis.grid(False)
    g.sharey(axs[3])


    # Scenario 3
    axs[2].set_title("(b.1)", size='x-large')
    g = sns.lineplot(y='result', x='days', data=time.loc[time["scenario"] == "scenario3"], style='routing', hue='routing', markers=['o', 'X', 'D', 'P'], markersize=8, ax=axs[2], palette=palette)
    g.set(yscale='log', xlabel=None)
    g.set_ylabel("Routing Time / Bundle [μs]", size='x-large')
    g.xaxis.grid(False)
    #g.sharey(axs[0])

    axs[3].set_title("(b.2)", size='x-large')
    g = sns.scatterplot(y='result', x='days', data=table.loc[table["scenario"] == "scenario3"], hue='routing', style='routing', markers=['o', 's', 'D', 'P'], s=80, ax=axs[3], palette=palette)
    g.set(yscale='log', xlabel=None)
    g.set_ylabel("Number of Routes", size='x-large')
    g.xaxis.grid(False)

    handles, _ = axs[0].get_legend_handles_labels()
    fig.legend(handles, ['CGR', 'GRK', 'GRK (direct to Earth)', 'GRK (no inter-relay)'], loc='lower center', ncol=4, fontsize='x-large')
    axs[0].get_legend().remove()
    axs[1].get_legend().remove()
    axs[2].get_legend().remove()
    axs[3].get_legend().remove()

    plt.subplots_adjust(left=0.045,
                    bottom=0.236,
                    right=0.974,
                    top=0.93,
                    wspace=0.25,
                    hspace=0.2)

    sns.despine(bottom=True)
    plt.show()


for t in ["a", "b"]:
    plotBundle(t)
    plotRouting(t)
    print(muleStats(t, "uniform", "10000000"))

