import sqlite3
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
import statistics as st

# Plot Configurations
sns.set_style("whitegrid")
#colors = ["#38B000", "#F72585", "#0D92D3"]
colors = ["#F72585", "#0D92D3"]
palette = sns.color_palette(colors)
plt.rcParams['axes.grid'] = True
plt.rcParams['font.family'] = 'monospace'

# Parameters
days = ["1", "2", "3", "4"]

# Total routing computation time at mules
def muleTime(type, distribution, scenario, size):

    data = []
    for day in days:

        if scenario == "1":
            if type == "a":
                mules = [2, 3, 4]
                dayLabels = [104, 240, 472, 898]
            else:
                mules = [2, 3, 4, 5, 6]
                dayLabels = [236, 540, 1064, 2044]

        elif scenario == "2":
            if type == "a":
                mules = [7, 8, 9]
                dayLabels = [304, 700, 1314, 2540]
            else:
                mules = [7, 8, 9, 10, 11]
                dayLabels = [550, 1268, 2400, 4668]

        else:
            if type == "a":
                mules = [23, 24, 25]
                dayLabels = [988, 2254, 4370, 8506]
            else:
                mules = [23, 24, 25, 26, 27]
                dayLabels = [1610, 3648, 7098, 13894]

        input_path = "../scenario{}/cgr/{}/{}/results/traffic-distribution={},size={},ttl=2500000-#0.sca".format(scenario, type, day, distribution, size)
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
            routingTime = 0 if (rows[0]["result"] == 0) else (rows[0]["result"]) / totalBundlesProcessedByMule
            result += routingTime

        #data.append([result, dayLabels[int(day)-1]])
        data.append([result/len(mules), dayLabels[int(day)-1]])
                
    df = pd.DataFrame(data, columns=["result", "days"])
    return df


def plotRouting(type, size):

    fig, axs = plt.subplots(1,3, sharey=True, figsize=(20, 4.5))
    if size == 64000:
        fig.suptitle("Average Total Routing Computation Time at Data Relays (64 KB Bundles)".format(type))
    else:
        fig.suptitle("Average Total Routing Computation Time at Data Relays (10 MB Bundles)".format(type))

    for id, (dist, title) in zip(range(0, 3), [("uniform", "Uniform"), ("exp", "Exponential"), ("normal", "Normal")]):

        axs[id].set_title(title)

        data1 = muleTime(type, dist, "1", size)
        data1["type"] = "scenario1"
        data2 = muleTime(type, dist, "2", size)
        data2["type"] = "scenario2"
        data3 = muleTime(type, dist, "3", size)
        data3["type"] = "scenario3"
        #data = pd.concat([data1, data2, data3.loc[data3['days'] < 8000]], axis=0)
        data = pd.concat([data1, data2, data3], axis=0)
        g = sns.scatterplot(y='result', x='days', data=data, hue="type", marker="o", s=60, ax=axs[id], palette=palette)
        g.set(yscale='log')
        axs[id].set_ylabel("[s]")
        axs[id].set_xlabel("Contacts")

        
        
        #contacts = np.linspace(1, 9000, 200)
        #y = [(x**2)*0.0000007 for x in contacts]
        if type == "a" and size == 64000:
            contacts = np.linspace(100, 9000, 200)
            y = [(x**2)*0.0000025 for x in contacts]
        elif type == "a" and size == 10000000:
            contacts = np.linspace(100, 9000, 200)
            y = [(x**3)*0.00000004 for x in contacts]
        else :
            contacts = np.linspace(1, 14500, 600)
            y = [(x**2)*0.00000012 for x in contacts]

        df = pd.DataFrame({'x':contacts, 'y':y})
        g = sns.lineplot(data=df, x='x', y='y', ax=axs[id])
        line = g.get_lines()[0]
        line.set_color('grey')
        line.set_alpha(0.5)
        
        

    handles, _ = axs[0].get_legend_handles_labels()
    fig.legend(handles, ['Scenario 1', 'Scenario 2', 'Scenario 3'], loc='lower center', ncol=3)
    axs[0].get_legend().remove()
    axs[1].get_legend().remove()
    axs[2].get_legend().remove()

    plt.subplots_adjust(left=0.05,
                    bottom=0.18,
                    right=0.95,
                    top=0.852,
                    wspace=0.07,
                    hspace=0.2)
    plt.show()

def plotRoutingOne(type, size):

    dist = "uniform"

    data1 = muleTime(type, dist, "1", size)
    data1["type"] = "scenario1"
    data2 = muleTime(type, dist, "2", size)
    data2["type"] = "scenario2"
    data3 = muleTime(type, dist, "3", size)
    data3["type"] = "scenario3"
    data = pd.concat([data1, data2, data3], axis=0)
    g = sns.scatterplot(y='result', x='days', data=data, hue="type", marker="o", s=60, palette=palette)
    g.set(yscale='log')
    g.set_ylabel("[s]")
    g.set_xlabel("Contacts")

    if size == 64000:
        g.set(title="Total Routing Time at Data Relays (64 KB Bundles)".format(type))
    else:
        g.set(title="Total Routing Time at Data Relays (10 MB Bundles)".format(type))

    
    if type == "a" and size == 64000:
        contacts = np.linspace(100, 9000, 200)
        y = [(x**2)*0.0000025 for x in contacts]
    elif type == "a" and size == 10000000:
        contacts = np.linspace(100, 9000, 200)
        y = [(x**3)*0.00000004 for x in contacts]

    df = pd.DataFrame({'x':contacts, 'y':y})
    g = sns.lineplot(data=df, x='x', y='y', ax=g)
    line = g.get_lines()[0]
    line.set_color('grey')
    line.set_alpha(0.5)
        
        

    handles, _ = g.get_legend_handles_labels()
    plt.legend(handles, ['Scenario 1', 'Scenario 2', 'Scenario 3'], loc='lower center', ncol=3)
    #ax.get_legend().remove()

    plt.show()

# average route search calls (findNextBestRoute)
def muleSearchCalls(type, distribution, scenario, size):

    data = []
    for day in days:

        if scenario == "1":
            if type == "a":
                mules = [2, 3, 4]
                dayLabels = [104, 240, 472, 898]
            else:
                mules = [2, 3, 4, 5, 6]
                dayLabels = [236, 540, 1064, 2044]

        elif scenario == "2":
            if type == "a":
                mules = [7, 8, 9]
                dayLabels = [304, 700, 1314, 2540]
            else:
                mules = [7, 8, 9, 10, 11]
                dayLabels = [550, 1268, 2400, 4668]

        else:
            if type == "a":
                mules = [23, 24, 25]
                dayLabels = [988, 2254, 4370, 8506]
            else:
                mules = [23, 24, 25, 26, 27]
                dayLabels = [1610, 3648, 7098, 13894]

        input_path = "../scenario{}/cgr/{}/{}/results/traffic-distribution={},size={},ttl=2500000-#0.sca".format(scenario, type, day, distribution, size)
        conn = sqlite3.connect(input_path)
        conn.row_factory = sqlite3.Row
        cur = conn.cursor()

        totalBundlesSeen = 0
        totalRouteSearchStarts = 0
        totalRouteSearchCalls = 0
        totalYenIterations = 0
        for mule in mules:
            try:
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

            except:
                continue

        data.append([totalBundlesSeen/len(mules),
                    totalRouteSearchStarts/len(mules),
                    totalRouteSearchCalls/len(mules),
                    totalYenIterations/len(mules),
                    dayLabels[int(day)-1]])
                
    df = pd.DataFrame(data, columns=["bundles", "starts", "calls", "yens", "days"])
    return df


def plotSearchCalls(type, size):

    for dist in ["uniform", "exp", "normal"]:

        data1 = muleSearchCalls(type, dist, "1", size)
        data1["type"] = "scenario1"
        data2 = muleSearchCalls(type, dist, "2", size)
        data2["type"] = "scenario2"
        data3 = muleSearchCalls(type, dist, "3", size)
        data3["type"] = "scenario3"

        data = pd.concat([data1, data2, data3], axis=0)
        print(data)
        print("DISTRIBUTION:", dist)
        print(data.groupby(['type'])['bundles', 'starts', 'calls', 'yens'].median())




# Average route table size per mule
def muleTable(type, distribution, scenario, size):

    data = []
    for day in days:

        if scenario == "1":
            if type == "a":
                mules = [2, 3, 4]
                dayLabels = [104, 240, 472, 898]
            else:
                mules = [2, 3, 4, 5, 6]
                dayLabels = [236, 540, 1064, 2044]

        elif scenario == "2":
            if type == "a":
                mules = [7, 8, 9]
                dayLabels = [304, 700, 1314, 2540]
            else:
                mules = [7, 8, 9, 10, 11]
                dayLabels = [550, 1268, 2400, 4668]

        else:
            if type == "a":
                mules = [23, 24, 25]
                dayLabels = [988, 2254, 4370, 8506]
            else:
                mules = [23, 24, 25, 26, 27]
                dayLabels = [1610, 3648, 7098, 13894]

        input_path = "../scenario{}/cgr/{}/{}/results/traffic-distribution={},size={},ttl=2500000-#0.sca".format(scenario, type, day, distribution, size)
        conn = sqlite3.connect(input_path)
        conn.row_factory = sqlite3.Row
        cur = conn.cursor()

        result = 0
        for mule in mules:
            try:
                cur.execute("SELECT scalarValue AS result FROM scalar WHERE scalarName='{}' AND moduleName LIKE '%{}%'".format("routeTableSize:mean", mule))
            except:
                continue
            rows = cur.fetchall()
            routingTime = 0 if (rows[0]["result"] == None) else rows[0]["result"]
            result += routingTime
        #data.append([result, dayLabels[int(day)-1]])
        data.append([result/len(mules), dayLabels[int(day)-1]])
                
    df = pd.DataFrame(data, columns=["result", "days"])
    return df


def plotTable(type, size):

    fig, axs = plt.subplots(1,3, sharey=True, figsize=(20, 4.5))
    if size == 64000:
        fig.suptitle("Average Routing Table Size at Data Relays (64 KB Bundles)".format(type))
    else:
        fig.suptitle("Average Routing Table Size at Data Relays (10 MB Bundles)".format(type))

    for id, (dist, title) in zip(range(0, 3), [("uniform", "Uniform"), ("exp", "Exponential"), ("normal", "Normal")]):

        axs[id].set_title(title)

        data1 = muleTable(type, dist, "1", size)
        data1["type"] = "scenario1"
        data2 = muleTable(type, dist, "2", size)
        data2["type"] = "scenario2"
        data3 = muleTable(type, dist, "3", size)
        data3["type"] = "scenario3"
        data = pd.concat([data1, data2, data3], axis=0)
        g = sns.scatterplot(y='result', x='days', data=data, hue="type", marker="o", s=60, ax=axs[id], palette=palette)
        g.set(yscale='log')
        axs[id].set_ylabel("Entries")
        axs[id].set_xlabel("Contacts")

        #contacts = np.linspace(1, 9000, 200)
        #y = [(x**2)*0.0000007 for x in contacts]
        """
        if type == "a":
            contacts = np.linspace(1, 9000, 200)
            y = [(x**2)*0.0000025 for x in contacts]
        else :
            contacts = np.linspace(1, 14500, 600)
            y = [(x**2)*0.00000012 for x in contacts]

        df = pd.DataFrame({'x':contacts, 'y':y})
        g = sns.lineplot(data=df, x='x', y='y', ax=axs[id])
        line = g.get_lines()[0]
        line.set_color('grey')
        line.set_alpha(0.5)
        """

    handles, _ = axs[0].get_legend_handles_labels()
    fig.legend(handles, ['Scenario 1', 'Scenario 2', 'Scenario 3'], loc='lower center', ncol=3)
    axs[0].get_legend().remove()
    axs[1].get_legend().remove()
    axs[2].get_legend().remove()

    plt.subplots_adjust(left=0.05,
                    bottom=0.18,
                    right=0.95,
                    top=0.852,
                    wspace=0.07,
                    hspace=0.2)
    plt.show()

def plotTableOne(type, size):

    dist = "uniform"

    data1 = muleTable(type, dist, "1", size)
    data1["type"] = "scenario1"
    data2 = muleTable(type, dist, "2", size)
    data2["type"] = "scenario2"
    data3 = muleTable(type, dist, "3", size)
    data3["type"] = "scenario3"
    data = pd.concat([data1, data2, data3], axis=0)
    g = sns.scatterplot(y='result', x='days', data=data, hue="type", marker="o", s=60, palette=palette)
    g.set(yscale='log')
    g.set_ylabel("[s]")
    g.set_xlabel("Contacts")

    if size == 64000:
        g.set(title="Average Routing Table Size at Data Relay (64 KB Bundles)".format(type))
    else:
        g.set(title="Average Routing Table Size at Data Relay (10 MB Bundles)".format(type))


    handles, _ = g.get_legend_handles_labels()
    plt.legend(handles, ['Scenario 1', 'Scenario 2', 'Scenario 3'], loc='lower center', ncol=3)

    plt.show()



def plotRoutingAndTable(type, size):

    fig, axs = plt.subplots(1,2, sharey=False, figsize=(10, 4.5))
    if size == 64000:
        fig.suptitle("Bundle Size = 64 KB")
    else:
        fig.suptitle("Bundle Size = 10 MB")

    dist = "uniform"

    axs[0].set_title("Total Routing Time")
    data1 = muleTime(type, dist, "1", size)
    data1["type"] = "scenario1"
    data2 = muleTime(type, dist, "2", size)
    data2["type"] = "scenario2"
    data3 = muleTime(type, dist, "3", size)
    data3["type"] = "scenario3"
    data = pd.concat([data1, data2, data3], axis=0)
    g = sns.scatterplot(y='result', x='days', data=data, hue="type", marker="o", ax=axs[0], s=60, palette=palette)
    g.set(yscale='log')
    g.set_ylabel("[s]")
    g.set_xlabel("Contacts")

    if size == 64000:
        if type == "a":
            contacts = np.linspace(100, 9000, 200)
            y = [(x**2)*0.00000037 for x in contacts]
        else:
            contacts = np.linspace(100, 14000, 500)
            y = [(x**2)*0.00000037 for x in contacts]
    else:
        if type == "a":
            contacts = np.linspace(100, 9000, 200)
            y = [(x**3)*0.00000004 for x in contacts]
        else:
            contacts = np.linspace(100, 14000, 500)
            y = [(x**3)*0.0000000015 for x in contacts]


    df = pd.DataFrame({'x':contacts, 'y':y})
    g = sns.lineplot(data=df, x='x', y='y', ax=g)
    line = g.get_lines()[0]
    line.set_color('grey')
    line.set_alpha(0.5)
        

    axs[1].set_title("Average Routing Table Size")

    data1 = muleTable(type, dist, "1", size)
    data1["type"] = "scenario1"
    data2 = muleTable(type, dist, "2", size)
    data2["type"] = "scenario2"
    data3 = muleTable(type, dist, "3", size)
    data3["type"] = "scenario3"
    data = pd.concat([data1, data2, data3], axis=0)
    g = sns.scatterplot(y='result', x='days', data=data, hue="type", marker="o", s=60, ax=axs[1], palette=palette)
    g.set(yscale='log')
    axs[1].set_ylabel("Entries")
    axs[1].set_xlabel("Contacts")


    handles, _ = axs[0].get_legend_handles_labels()
    fig.legend(handles, ['Scenario 1', 'Scenario 2', 'Scenario 3'], loc='lower center', ncol=3)
    axs[0].get_legend().remove()
    axs[1].get_legend().remove()

    plt.subplots_adjust(left=0.067,
                    bottom=0.18,
                    right=0.95,
                    top=0.852,
                    wspace=0.226,
                    hspace=0.2)
    plt.show()


def bundleHops(type, distribution, scenario, size):

    dayLabels = {"1": "3d", "2": "7d", "3": "14d", "4": "28d"}

    data = []
    for day in days:

        input_path = "../scenario{}/cgr/{}/{}/results/traffic-distribution={},size={},ttl=2500000-#0.sca".format(scenario, type, day, distribution, size)
        conn = sqlite3.connect(input_path)
        conn.row_factory = sqlite3.Row
        cur = conn.cursor()

        cur.execute("SELECT scalarValue AS result FROM scalar WHERE scalarName='{}' ORDER BY scalarValue ASC".format("appBundleReceivedHops:mean"))
        rows = cur.fetchall()
        results = []
        for row in rows:
            if (row[0] == None):
                continue
            else:
                results.append(row[0])
        result = st.median(results)
        data.append([result, dayLabels[day]])
                
    df = pd.DataFrame(data, columns=["result", "days"])
    return df

def plotHops():

    fig, axs = plt.subplots(1,2, sharey=False, figsize=(9, 4.2))

    dist = "uniform"

    axs[0].set_title("(a)", size='x-large')
    #data1 = bundleHops(type, dist, "1", "64000")
    #data1["type"] = "scenario1"
    data2 = bundleHops("a", dist, "2", "10000000")
    data2["type"] = "scenario2"
    data3 = bundleHops("a", dist, "3", "10000000")
    data3["type"] = "scenario3"
    data = pd.concat([data2, data3], axis=0)
    g = sns.scatterplot(y='result', x='days', data=data, hue="type", marker="o", ax=axs[0], s=80, palette=sns.color_palette(["#F72585", "#0D92D3"]))
    g.set(xlabel=None)
    g.set_ylabel("Number of Hops", size='x-large')
    #g.set_xlabel("Contacts")
    axs[0].xaxis.grid(False)

    axs[1].set_title("(b)", size='x-large')
    #data1 = bundleHops(type, dist, "1", "10000000")
    #data1["type"] = "scenario1"
    data2 = bundleHops("b", dist, "2", "10000000")
    data2["type"] = "scenario2"
    data3 = bundleHops("b", dist, "3", "10000000")
    data3["type"] = "scenario3"
    data = pd.concat([data2, data3], axis=0)
    g = sns.scatterplot(y='result', x='days', data=data, hue="type", marker="o", ax=axs[1], s=80, palette=sns.color_palette(["#F72585", "#0D92D3"]))
    g.set(xlabel=None)
    g.set(ylabel=None)
    g.sharey(axs[0])
    #g.set_ylabel("Hops")
    #g.set_xlabel("Contacts")
    axs[1].xaxis.grid(False)


    handles, _ = axs[0].get_legend_handles_labels()
    fig.legend(handles, ['Scenario 1', 'Scenario 2'], loc='lower center', ncol=2)
    axs[0].get_legend().remove()
    axs[1].get_legend().remove()

    plt.subplots_adjust(left=0.088,
                    bottom=0.18,
                    right=0.95,
                    top=0.852,
                    wspace=0.205,
                    hspace=0.2)
    sns.despine(bottom=True)
    plt.show()

def bundleFirst(distribution, scenario, type):

    data = []

    northMules = [25, 26]
    circMule = 24
    southMules = [27, 28]
    if scenario == "2":
        north = [3]         #[4, 6, 8, 10, 14, 18, 19, 20]
        circ = [5, 6]       #[7, 8, 11, 12, 15, 16]
        south = [4]         #[5, 9, 13, 17, 21, 22, 23]
    else:
        north = [3, 5, 7, 9, 13, 17, 18, 19]
        circ = [6, 10, 11, 14, 15]
        south = [4, 8, 12, 16, 20, 21, 22]

    totalBundlesSentByNorth = 0
    northToNorthBundles = 0
    northToCircBundles = 0
    northToSouthBundles = 0
    northToEarthBundles = 0
    for size in ["64000", "10000000"]:
        for day in days:

            input_path = "../scenario{}/cgr/{}/{}/results/traffic-distribution={},size={},ttl=2500000-#0.sca".format(scenario, type, day, distribution, size)
            conn = sqlite3.connect(input_path)
            conn.row_factory = sqlite3.Row
            cur = conn.cursor()

            for northLS in north:

                cur.execute("SELECT scalarValue FROM scalar WHERE scalarName ='{}' AND moduleName LIKE '%{}%'".format("appBundleSent:count", northLS))
                totalBundlesSentByNorth += cur.fetchall()[0][0]

                cur.execute("SELECT scalarValue FROM scalar WHERE scalarName ='{}' AND moduleName LIKE '%{}%'".format("bundleDropped:sum", northLS))
                totalBundlesSentByNorth -= cur.fetchall()[0][0]

                cur.execute("SELECT statId FROM statistic WHERE statName ='{}' AND moduleName LIKE '%{}%'".format("firstHop:histogram", northLS))
                histogramId = cur.fetchall()[0][0]

                cur.execute("SELECT lowerEdge, binValue FROM histogramBin WHERE statId='{}'".format(histogramId))
                rows = cur.fetchall()
                for row in rows:
                    node = row["lowerEdge"]
                    if row["lowerEdge"] == float("-inf") or row["lowerEdge"] == float("inf"):
                        continue
                    else:
                        node = int(row["lowerEdge"])
                    if node in northMules:
                        northToNorthBundles += row["binValue"]
                    elif node == circMule:
                        northToCircBundles += row["binValue"]
                    elif node in southMules:
                        northToSouthBundles += row["binValue"]
                    else:
                        northToEarthBundles += row["binValue"]

    """
    data.append([
        northToEarthBundles/totalBundlesSentByNorth, 
        northToNorthBundles/totalBundlesSentByNorth,
        northToCircBundles/totalBundlesSentByNorth,
        northToSouthBundles/totalBundlesSentByNorth,
        "north"])
    """
    total = northToEarthBundles + northToNorthBundles + northToCircBundles + northToSouthBundles
    if total > 0:
        data.append([
            northToEarthBundles/total, 
            northToNorthBundles/total,
            northToCircBundles/total,
            northToSouthBundles/total,
            "north"])

    totalBundlesSentByCirc = 0
    circToNorthBundles = 0
    circToCircBundles = 0
    circToSouthBundles = 0
    circToEarthBundles = 0
    for size in ["64000", "10000000"]:
        for day in days:

            input_path = "../scenario{}/cgr/{}/{}/results/traffic-distribution={},size={},ttl=2500000-#0.sca".format(scenario, type, day, distribution, size)
            conn = sqlite3.connect(input_path)
            conn.row_factory = sqlite3.Row
            cur = conn.cursor()

            for circLS in circ:

                cur.execute("SELECT scalarValue FROM scalar WHERE scalarName ='{}' AND moduleName LIKE '%{}%'".format("appBundleSent:count", circLS))
                rows = cur.fetchall()
                if len(rows) == 0:
                    continue
                result = rows[0]["scalarValue"]
                totalBundlesSentByCirc += result

                cur.execute("SELECT scalarValue FROM scalar WHERE scalarName ='{}' AND moduleName LIKE '%{}%'".format("bundleDropped:sum", circLS))
                totalBundlesSentByCirc -= cur.fetchall()[0][0]

                cur.execute("SELECT statId FROM statistic WHERE statName ='{}' AND moduleName LIKE '%{}%'".format("firstHop:histogram", circLS))
                histogramId = cur.fetchall()[0][0]

                cur.execute("SELECT lowerEdge, binValue FROM histogramBin WHERE statId='{}'".format(histogramId))
                rows = cur.fetchall()
                for row in rows:
                    node = row["lowerEdge"]
                    if row["lowerEdge"] == float("-inf") or row["lowerEdge"] == float("inf"):
                        continue
                    else:
                        node = int(row["lowerEdge"])
                    if node in northMules:
                        circToNorthBundles += row["binValue"]
                    elif node == circMule:
                        circToCircBundles += row["binValue"]
                    elif node in southMules:
                        circToSouthBundles += row["binValue"]
                    else:
                        circToEarthBundles += row["binValue"]

    """                    
    if (totalBundlesSentByCirc > 0):
        data.append([
            circToEarthBundles/totalBundlesSentByCirc, 
            circToNorthBundles/totalBundlesSentByCirc,
            circToCircBundles/totalBundlesSentByCirc,
            circToSouthBundles/totalBundlesSentByCirc,
            "circ"])
    """
    total = circToEarthBundles + circToNorthBundles + circToCircBundles + circToSouthBundles
    if total > 0:
        data.append([
                circToEarthBundles/total, 
                circToNorthBundles/total,
                circToCircBundles/total,
                circToSouthBundles/total,
                "circ"])

    totalBundlesSentBySouth = 0
    southToNorthBundles = 0
    southToCircBundles = 0
    southToSouthBundles = 0
    southToEarthBundles = 0
    for size in ["64000", "10000000"]:
        for day in days:

            input_path = "../scenario{}/cgr/{}/{}/results/traffic-distribution={},size={},ttl=2500000-#0.sca".format(scenario, type, day, distribution, size)
            conn = sqlite3.connect(input_path)
            conn.row_factory = sqlite3.Row
            cur = conn.cursor()

            for southLS in south:

                cur.execute("SELECT scalarValue FROM scalar WHERE scalarName ='{}' AND moduleName LIKE '%{}%'".format("appBundleSent:count", southLS))
                totalBundlesSentBySouth += cur.fetchall()[0][0]

                cur.execute("SELECT scalarValue FROM scalar WHERE scalarName ='{}' AND moduleName LIKE '%{}%'".format("bundleDropped:sum", southLS))
                totalBundlesSentBySouth -= cur.fetchall()[0][0]

                cur.execute("SELECT statId FROM statistic WHERE statName ='{}' AND moduleName LIKE '%{}%'".format("firstHop:histogram", southLS))
                histogramId = cur.fetchall()[0][0]

                cur.execute("SELECT lowerEdge, binValue FROM histogramBin WHERE statId='{}'".format(histogramId))
                rows = cur.fetchall()
                for row in rows:
                    node = row["lowerEdge"]
                    if row["lowerEdge"] == float("-inf") or row["lowerEdge"] == float("inf"):
                        continue
                    else:
                        node = int(row["lowerEdge"])
                    if node in northMules:
                        southToNorthBundles += row["binValue"]
                    elif node == circMule:
                        southToCircBundles += row["binValue"]
                    elif node in southMules:
                        southToSouthBundles += row["binValue"]
                    else:
                        circToEarthBundles += row["binValue"]
    """
    if (totalBundlesSentBySouth > 0):
        data.append([
            southToEarthBundles/totalBundlesSentBySouth, 
            southToNorthBundles/totalBundlesSentBySouth,
            southToCircBundles/totalBundlesSentBySouth,
            southToSouthBundles/totalBundlesSentBySouth,
            "south"])
    """
    total = southToEarthBundles + southToNorthBundles + southToCircBundles + southToSouthBundles
    if total > 0:
        data.append([
                southToEarthBundles/total, 
                southToNorthBundles/total,
                southToCircBundles/total,
                southToSouthBundles/total,
                "south"])
   
    df = pd.DataFrame(data, columns=["earth", "north", "circ", "south", "region"])
    return df

def plotFirst():

    fig = plt.figure(figsize=(15, 7))
    subfigs = fig.subfigures(1, 2)
    axs1 = subfigs[0].subplots(3,1, sharex=True)
    subfigs[0].suptitle("Three Relays")
    axs2 = subfigs[1].subplots(3,1, sharex=True)
    subfigs[1].suptitle("Five Relays")

    ax1 = axs1[0]
    ax3 = axs1[1]
    ax5 = axs1[2]

    ax2 = axs2[0]
    ax4 = axs2[1]
    ax6 = axs2[2]
    
    # 3 RELAYS (a)
    data2 = bundleFirst("uniform", "2", "a")
    data2["type"] = "scenario2"
    data3 = bundleFirst("uniform", "3", "a")
    data3["type"] = "scenario3"
    data = pd.concat([data2, data3], axis=0)
    data = pd.melt(data, id_vars=['type', 'region'], var_name='relay', value_name='result')

    sns.barplot(data=data.loc[data["region"] == "north"], x='result', y='relay', hue='type', ax=ax1, palette=sns.color_palette(["#F72585", "#0D92D3"]))
    ax1.set_ylabel("North")
    ax1.set(xlabel=None)
    sns.barplot(data=data.loc[data["region"] == "circ"], x='result', y='relay', hue='type', ax=ax3, palette=sns.color_palette(["#F72585", "#0D92D3"]))
    ax3.set(xlabel=None)
    ax3.set_ylabel("Center")
    sns.barplot(data=data.loc[data["region"] == "south"], x='result', y='relay', hue='type', ax=ax5, palette=sns.color_palette(["#F72585", "#0D92D3"]))
    ax5.set_ylabel("South")
    ax5.set_xlabel("Ratio")

    # 5 RELAYS (b)
    data2 = bundleFirst("uniform", "2", "b")
    data2["type"] = "scenario2"
    data3 = bundleFirst("uniform", "3", "b")
    data3["type"] = "scenario3"
    data = pd.concat([data2, data3], axis=0)
    data = pd.melt(data, id_vars=['type', 'region'], var_name='relay', value_name='result')

    sns.barplot(data=data.loc[data["region"] == "north"], x='result', y='relay', hue='type', ax=ax2, palette=sns.color_palette(["#F72585", "#0D92D3"]))
    ax2.set(ylabel=None, yticklabels=[])
    ax2.set(xlabel=None)
    sns.barplot(data=data.loc[data["region"] == "circ"], x='result', y='relay', hue='type', ax=ax4, palette=sns.color_palette(["#F72585", "#0D92D3"]))
    ax4.set(ylabel=None, yticklabels=[])
    ax4.set(xlabel=None)
    sns.barplot(data=data.loc[data["region"] == "south"], x='result', y='relay', hue='type', ax=ax6, palette=sns.color_palette(["#F72585", "#0D92D3"]))
    ax6.set(ylabel=None, yticklabels=[])
    ax6.set_xlabel("Ratio")

    handles, _ = ax1.get_legend_handles_labels()
    subfigs[0].legend(handles, ['Scenario 2', 'Scenario 3'], loc='lower center', ncol=2)
    ax1.get_legend().remove()
    ax2.get_legend().remove()
    ax3.get_legend().remove()
    ax4.get_legend().remove()
    ax5.get_legend().remove()
    ax6.get_legend().remove()

    plt.subplots_adjust(left=0.152,
                    bottom=0.126,
                    right=0.95,
                    top=0.929,
                    wspace=0.04,
                    hspace=0.076)
    plt.show()



def plotRoutingAndTable2(t):

    # Figure (routing time and table size 64 KB | routing time and table size 10 MB)
    fig, axs = plt.subplots(1,4, sharey=False, figsize=(18, 4.2))

    # AXS[0] Routing Time 64KB
    axs[0].set_title("(a.1)", size='x-large')
    #data1 = muleTime(t, "uniform", "1", "64000")
    #data1["type"] = "scenario1"
    data2 = muleTime(t, "uniform", "2", "64000")
    data2["type"] = "scenario2"
    data3 = muleTime(t, "uniform", "3", "64000")
    data3["type"] = "scenario3"
    data = pd.concat([data2, data3], axis=0)
    g = sns.scatterplot(y='result', x='days', data=data, hue="type", marker="o", ax=axs[0], s=70, palette=palette)
    g.set(yscale='log')
    g.set_ylabel("Routing Time / Bundle [μs]", size='x-large')
    g.set_xlabel("Contacts")
    g.xaxis.grid(False)

    
    # AXS[1] Routing Table 64KB
    axs[1].set_title("(a.2)", size='x-large')
    #data1 = muleTable(t, "uniform", "1", "64000")
    #data1["type"] = "scenario1"
    data2 = muleTable(t, "uniform", "2", "64000")
    data2["type"] = "scenario2"
    data3 = muleTable(t, "uniform", "3", "64000")
    data3["type"] = "scenario3"
    data = pd.concat([data2, data3], axis=0)
    #g = sns.scatterplot(y='result', x='days', data=data, hue="type", marker="o", s=30, ax=axs[1], palette=palette)
    g.set(yscale='log')
    g = sns.barplot(y='result', x='days', data=data, hue="type", ax=axs[1], palette=palette)
    #g.set_xticks(range(0, 12))
    #if t=="a":
        #g.set_xticklabels(['104','','','','','','','','','','','8506'])
    #else:
        #g.set_xticklabels(['236','','','','','','','','','','','13894'])
    axs[1].set_ylabel("Number of Routes", size='x-large')
    axs[1].set_xlabel("Contacts")
    g.xaxis.grid(False)
    g.sharey(axs[3])


    # AXS[2] Routing Time 10MB
    axs[2].set_title("(b.1)", size='x-large')
    #data1 = muleTime(t, "uniform", "1", "10000000")
    #data1["type"] = "scenario1"
    data2 = muleTime(t, "uniform", "2", "10000000")
    data2["type"] = "scenario2"
    data3 = muleTime(t, "uniform", "3", "10000000")
    data3["type"] = "scenario3"
    data = pd.concat([data2, data3], axis=0)
    g = sns.scatterplot(y='result', x='days', data=data, hue="type", marker="o", ax=axs[2], s=70, palette=palette)
    g.set(yscale='symlog')
    g.set_ylabel("Routing Time / Bundle [μs]", size='x-large')
    g.set_xlabel("Contacts")
    g.xaxis.grid(False)
    g.sharey(axs[0])

    # AXS[3] Routing Table 10MB
    axs[3].set_title("(b.2)", size='x-large')
    #data1 = muleTable(t, "uniform", "1", "10000000")
    #data1["type"] = "scenario1"
    data2 = muleTable(t, "uniform", "2", "10000000")
    data2["type"] = "scenario2"
    data3 = muleTable(t, "uniform", "3", "10000000")
    data3["type"] = "scenario3"
    data = pd.concat([data2, data3], axis=0)
    #g = sns.scatterplot(y='result', x='days', data=data, hue="type", marker="o", s=30, ax=axs[3], palette=palette)
    g.set(yscale='log')
    g = sns.barplot(y='result', x='days', data=data, hue="type", ax=axs[3], palette=palette)
    g.set(yscale='symlog')
    #g.set_xticks(range(0, 12))
    #if t=="a":
        #g.set_xticklabels(['104','','','','','','','','','','','8506'])
    #else:
        #g.set_xticklabels(['236','','','','','','','','','','','13894'])
    axs[3].set_ylabel("Number of Routes", size='x-large')
    axs[3].set_xlabel("Contacts")
    g.xaxis.grid(False)


    handles, _ = axs[0].get_legend_handles_labels()
    fig.legend(handles, ['Scenario 1', 'Scenario 2'], loc='lower center', ncol=2, fontsize='x-large')
    axs[0].get_legend().remove()
    axs[1].get_legend().remove()
    axs[2].get_legend().remove()
    axs[3].get_legend().remove()
    sns.despine()


    plt.subplots_adjust(left=0.045,
                    bottom=0.236,
                    right=0.974,
                    top=0.93,
                    wspace=0.257,
                    hspace=0.2)
    plt.show()

def plotRoutingPresentation(t):

    fig, axs = plt.subplots(1,2, sharey=False, figsize=(15, 5))

    # AXS[0] Routing Time 64KB
    #axs[0].set_title("(a.1)", size='x-large')
    #data1 = muleTime(t, "uniform", "1", "64000")
    #data1["type"] = "scenario1"
    #data2 = muleTime(t, "uniform", "2", "64000")
    #data2["type"] = "scenario2"
    data3 = muleTime(t, "uniform", "3", "64000")
    data3["type"] = "scenario3"
    g = sns.scatterplot(y='result', x='days', data=data3, marker="o", ax=axs[0], s=90, palette=palette)
    g.set(yscale='log')
    g.set(ylabel=None)
    g.set(xlabel=None, xticklabels=[])
    g.xaxis.grid(False)
    g.yaxis.grid(False)
    g.yaxis.tick_right()
    g.spines['right'].set_linewidth(1.5)
    g.spines['bottom'].set_linewidth(1.5)


    # AXS[2] Routing Time 10MB
    g = sns.scatterplot(y='result', x='days', data=data3, marker="o", ax=axs[1], s=90, palette=palette)
    data4 = muleTime(t, "uniform", "3", "10000000")
    data4["type"] = "scenario3"
    g = sns.scatterplot(y='result', x='days', data=data4, hue="type", marker="o", ax=axs[1], s=70, palette=palette)
    g.set(yscale='symlog')
    g.xaxis.grid(False)
    g.yaxis.grid(False)
    g.sharey(axs[0])
    g.set(ylabel=None)
    g.set(xlabel=None, xticklabels=[])
    g.yaxis.tick_right()
    g.spines['right'].set_linewidth(1.5)
    g.spines['bottom'].set_linewidth(1.5)
    


    #axs[0].get_legend().remove()
    axs[1].get_legend().remove()
    sns.despine(right=False, left=True)
    plt.show()

#plotRoutingAndTable2("a")
plotRoutingAndTable2("b")

#plotHops()


"""
print("A 64")
plotSearchCalls("a", 64000)
print("A 10")
plotSearchCalls("a", 10000000)
"""
#plotRoutingAndTable("a", 64000)
#plotRoutingAndTable("a", 10000000)

"""
print("B 64")
plotSearchCalls("b", 64000)
print("B 10")
plotSearchCalls("b", 10000000)

plotRoutingAndTable("b", 64000)
plotRoutingAndTable("b", 10000000)
"""

#plotFirst("a")
#plotFirst("b")

"""
print(bundleFirst("uniform", "2", "a"))
print(bundleFirst("uniform", "2", "b"))

print(bundleFirst("uniform", "3", "a"))
print(bundleFirst("uniform", "3", "b"))
"""

#plotFirst()
#plotHops()


plotRoutingPresentation("b")