import sqlite3
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# Plot Configurations
sns.set_style("darkgrid")
colors = ["#38B000", "#F72585"]
palette = sns.color_palette(colors)
plt.rcParams['axes.grid'] = True
plt.rcParams['font.family'] = 'monospace'

# Parameters
bundleSizes = [64000, 10000000]
days = ["1", "2", "3", "4"]
dayLabels = ["3d", "7d", "14d", "28d"]

# Avg. bundle delay per parameter combination
def bundleDelay(distribution):

    data = []
    for size in bundleSizes:
        for day in days:

            input_path = "../cgr/a/{}/results/traffic-distribution={},size={},ttl=2500000-#0.sca".format(day, distribution, size)
            conn = sqlite3.connect(input_path)
            conn.row_factory = sqlite3.Row
            cur = conn.cursor()
            cur.execute("SELECT AVG(scalarValue) AS result FROM scalar WHERE scalarName='{}'".format("appBundleReceivedDelay:mean"))
            rows = cur.fetchall()
            data.append([rows[0]['result'], dayLabels[int(day)-1], size])
                
    df = pd.DataFrame(data, columns=["result", "days", "size"])
    return df

# Bundle delivery ratio per parameter combination
def bundleDelivery(distribution):

    data = []
    for size in bundleSizes:
        for day in days:

            input_path = "../cgr/a/{}/results/traffic-distribution={},size={},ttl=2500000-#0.sca".format(day, distribution, size)
            conn = sqlite3.connect(input_path)
            conn.row_factory = sqlite3.Row
            cur = conn.cursor()

            cur.execute("SELECT SUM(scalarValue) AS result FROM scalar WHERE scalarName='{}'".format("appBundleReceived:count"))
            rows0 = cur.fetchall()
            successfullyDelivered = rows0[0]["result"]
            cur.execute("SELECT SUM(scalarValue) AS result FROM scalar WHERE scalarName='{}'".format("appBundleSent:count"))
            rows1 = cur.fetchall()
            overallSent = rows1[0]["result"]

            data.append([successfullyDelivered/overallSent, day, size])

    df = pd.DataFrame(data, columns=["result", "days", "size"])
    return df

# Bundle drops per parameter combination
def bundleDrop(distribution):

    data = []
    for size in bundleSizes:
        for day in days:

            input_path = "../cgr/a/{}/results/traffic-distribution={},size={},ttl=2500000-#0.sca".format(day, distribution, size)
            conn = sqlite3.connect(input_path)
            conn.row_factory = sqlite3.Row
            cur = conn.cursor()
            cur.execute("SELECT SUM(scalarValue) AS result FROM scalar WHERE scalarName='{}'".format("bundleDropped:sum"))
            rows = cur.fetchall()
            data.append([rows[0]["result"], day, size])

    df = pd.DataFrame(data, columns=["result", "days", "size"])
    return df


# Avg. routing complexity per bundle, averaged over all mules
def muleComplexity(distribution):

    data = []
    mules = [23, 24, 25]

    for size in bundleSizes:
        for day in days:

            input_path = "../cgr/a/{}/results/traffic-distribution={},size={},ttl=2500000-#0.sca".format(day, distribution, size)
            conn = sqlite3.connect(input_path)
            conn.row_factory = sqlite3.Row
            cur = conn.cursor()

            result = 0
            for mule in mules:

                cur.execute("SELECT scalarValue AS result FROM scalar WHERE scalarName='{}' AND moduleName LIKE '%{}%'".format("bundleReceivedFromCom:count", mule))
                rows = cur.fetchall()
                totalBundlesSeen = rows[0]["result"] #TODO

                cur.execute("SELECT scalarValue AS result FROM scalar WHERE scalarName='{}' AND moduleName LIKE '%{}%'".format("routeComplexity:sum", mule))
                rows = cur.fetchall()
                scaledComplexity = 0 if (rows[0]["result"] == 0) else rows[0]["result"] /totalBundlesSeen
                result += scaledComplexity

            data.append([result/len(mules), dayLabels[int(day)-1], size])
                
    df = pd.DataFrame(data, columns=["result", "days", "size"])
    return df

# Avg. routing computation time per bundle, averaged over all mules
def muleTime(distribution):

    data = []
    mules = [23, 24, 25]

    for size in bundleSizes:
        for day in days:

            input_path = "../cgr/a/{}/results/traffic-distribution={},size={},ttl=2500000-#0.sca".format(day, distribution, size)
            conn = sqlite3.connect(input_path)
            conn.row_factory = sqlite3.Row
            cur = conn.cursor()

            result = 0
            for mule in mules:
                cur.execute("SELECT scalarValue AS result FROM scalar WHERE scalarName='{}' AND moduleName LIKE '%{}%'".format("bundleReceivedFromCom:count", mule))
                rows = cur.fetchall()
                totalBundlesSeen = rows[0]["result"]

                cur.execute("SELECT scalarValue AS result FROM scalar WHERE scalarName='{}' AND moduleName LIKE '%{}%'".format("routeExecutionTimeUs:sum", mule))
                rows = cur.fetchall()
                routingTime = 0 if (rows[0]["result"] == 0) else (rows[0]["result"] / 1000000)/totalBundlesSeen
                result += routingTime

            data.append([result/len(mules), dayLabels[int(day)-1], size])
                
    df = pd.DataFrame(data, columns=["result", "days", "size"])
    return df


# Avg. routing table size at mules, averaged over all mules
def muleTable(distribution):

    data = []
    mules = [23, 24, 25]

    for size in bundleSizes:
        for day in days:

            input_path = "../cgr/a/{}/results/traffic-distribution={},size={},ttl=2500000-#0.sca".format(day, distribution, size)
            conn = sqlite3.connect(input_path)
            conn.row_factory = sqlite3.Row
            cur = conn.cursor()

            result = 0
            for mule in mules:
                cur.execute("SELECT scalarValue AS result FROM scalar WHERE scalarName='{}' AND moduleName LIKE '%{}%'".format("routeTableSize:mean", mule))
                rows = cur.fetchall()
                result = 0 if (rows[0]["result"] == None) else rows[0]["result"]

            data.append([result/len(mules), dayLabels[int(day)-1], size])
                
    df = pd.DataFrame(data, columns=["result", "days", "size"])
    return df

# Plots
def plotDelay():
    fig, axs = plt.subplots(1,5, sharey=True, figsize=(20, 5))
    fig.suptitle("Scenario 3: Average Bundle Delay")

    axs[0].set_title("every 10 seconds")
    data = bundleDelay("dist0")
    sns.lineplot(data=data, x="days", y="result", hue="size", ax=axs[0], markers=["o", "s"], palette=palette, errorbar=None)
    axs[0].set_ylabel("[s]")
    axs[0].set(xlabel=None)

    axs[1].set_title("every 60 seconds")
    data = bundleDelay("dist1")
    sns.lineplot(data=data, x="days", y="result", hue="size", ax=axs[1], markers=["o", "s"], palette=palette, errorbar=None)
    #axs[1].set_ylabel("[s]")
    axs[1].set(ylabel=None)
    axs[1].set(xlabel=None)

    axs[2].set_title("every 5 minutes")
    data = bundleDelay("dist2")
    sns.lineplot(data=data, x="days", y="result", hue="size", ax=axs[2], markers=["o", "s"], palette=palette, errorbar=None)
    #axs[1].set_ylabel("[s]")
    axs[2].set(ylabel=None)
    axs[2].set(xlabel=None)

    axs[3].set_title("every 15 minutes")
    data = bundleDelay("dist3")
    sns.lineplot(data=data, x="days", y="result", hue="size", ax=axs[3], markers=["o", "s"], palette=palette, errorbar=None)
    #axs[1].set_ylabel("[s]")
    axs[3].set(ylabel=None)
    axs[3].set(xlabel=None)

    axs[4].set_title("every 30 minutes")
    data = bundleDelay("dist4")
    sns.lineplot(data=data, x="days", y="result", hue="size", ax=axs[4], markers=["o", "s"], palette=palette, errorbar=None)
    #axs[2].set_ylabel("[s]")
    axs[4].set(ylabel=None)
    axs[4].set(xlabel=None)

    handles, labels = axs[0].get_legend_handles_labels()
    #fig.legend(handles, ['Bundle Size', '64 KB', '10 MB', 'Nr. of Relays', '3', '5'], loc='lower center', ncol=6)
    #axs[0].get_legend().remove()
    axs[1].get_legend().remove()
    axs[2].get_legend().remove()

    """
    plt.subplots_adjust(left=0.05,
                    bottom=0.148,
                    right=0.95,
                    top=0.852,
                    wspace=0.2,
                    hspace=0.2)
    """
    plt.show()

def plotDelivery():
    fig, axs = plt.subplots(1,3, sharey=True, figsize=(20, 5))
    fig.suptitle("Scenario 3: Bundle Delivery Ratio")

    axs[0].set_title("every 10 seconds")
    data = bundleDelivery("dist0")
    sns.lineplot(data=data, x="days", y="result", hue="size", ax=axs[0], markers=["o", "s"], palette=palette)
    axs[0].set_ylabel("ratio")
    axs[0].set(xlabel=None)

    axs[1].set_title("every 60 seconds")
    data = bundleDelivery("dist1")
    sns.lineplot(data=data, x="days", y="result", hue="size", ax=axs[1], markers=["o", "s"], palette=palette)
    axs[0].set_ylabel("ratio")
    axs[1].set(xlabel=None)

    axs[2].set_title("every 30 minutes")
    data = bundleDelivery("dist4")
    sns.lineplot(data=data, x="days", y="result", hue="size", ax=axs[2], markers=["o", "s"], palette=palette)
    axs[0].set_ylabel("ratio")
    axs[2].set(xlabel=None)

    handles, labels = axs[0].get_legend_handles_labels()
    #fig.legend(handles, ['Bundle Size', '64 KB', '10 MB', 'Nr. of Relays', '3', '5'], loc='lower center', ncol=6)
    #axs[0].get_legend().remove()
    axs[1].get_legend().remove()
    axs[2].get_legend().remove()

    plt.subplots_adjust(left=0.05,
                    bottom=0.148,
                    right=0.95,
                    top=0.852,
                    wspace=0.083,
                    hspace=0.2)

    plt.show()

def plotDrop():
    fig, axs = plt.subplots(1,3, sharey=False, figsize=(20, 5))
    fig.suptitle("Scenario 3: Bundle Drops")

    axs[0].set_title("every 10 seconds")
    data = bundleDrop("dist0")
    sns.lineplot(data=data, x="days", y="result", hue="size", ax=axs[0], markers=["o", "s"], palette=palette)
    axs[0].set_ylabel("drops")
    axs[0].set(xlabel=None)

    axs[1].set_title("every 60 seconds")
    data = bundleDrop("dist1")
    sns.lineplot(data=data, x="days", y="result", hue="size", ax=axs[1], markers=["o", "s"], palette=palette)
    axs[0].set_ylabel("drops")
    axs[1].set(xlabel=None)

    axs[2].set_title("every 30 minutes")
    data = bundleDrop("dist4")
    sns.lineplot(data=data, x="days", y="result", hue="size", ax=axs[2], markers=["o", "s"], palette=palette)
    axs[0].set_ylabel("drops")
    axs[2].set(xlabel=None)

    handles, labels = axs[0].get_legend_handles_labels()
    #fig.legend(handles, ['Bundle Size', '64 KB', '10 MB', 'Nr. of Relays', '3', '5'], loc='lower center', ncol=6)
    #axs[0].get_legend().remove()
    axs[1].get_legend().remove()
    axs[2].get_legend().remove()

    plt.show()

def plotComplexity():
    fig, axs = plt.subplots(1,3, sharey=False, figsize=(20, 5))
    fig.suptitle("Scenario 3: Average Routing Complexity at Data Relays")

    axs[0].set_title("every 10 seconds")
    data = muleComplexity("dist0")
    print(data)
    sns.lineplot(data=data, x="days", y="result", hue="size", ax=axs[0], markers=["o", "s"], palette=palette, errorbar=None)
    axs[0].set_ylabel("todo")
    axs[0].set(xlabel=None)

    axs[1].set_title("every 60 seconds")
    data = muleComplexity("dist1")
    sns.lineplot(data=data, x="days", y="result", hue="size", ax=axs[1], markers=["o", "s"], palette=palette, errorbar=None)
    #axs[1].set_ylabel("[s]")
    axs[1].set(ylabel=None)
    axs[1].set(xlabel=None)

    axs[2].set_title("every 30 minutes")
    data = muleComplexity("dist4")
    sns.lineplot(data=data, x="days", y="result", hue="size", ax=axs[2], markers=["o", "s"], palette=palette, errorbar=None)
    #axs[2].set_ylabel("[s]")
    axs[2].set(ylabel=None)
    axs[2].set(xlabel=None)

    handles, labels = axs[0].get_legend_handles_labels()
    #fig.legend(handles, ['Bundle Size', '64 KB', '10 MB', 'Nr. of Relays', '3', '5'], loc='lower center', ncol=6)
    #axs[0].get_legend().remove()
    axs[1].get_legend().remove()
    axs[2].get_legend().remove()

    plt.subplots_adjust(left=0.05,
                    bottom=0.148,
                    right=0.95,
                    top=0.852,
                    wspace=0.2,
                    hspace=0.2)
    plt.show()

def plotRouting():
    fig, axs = plt.subplots(1,5, sharey=True, figsize=(20, 4))
    fig.suptitle("Scenario 3: Average Routing Computation Time per Bundle at Data Relays")

    axs[0].set_title("every 10 seconds")
    data = muleTime("dist0")
    g = sns.lineplot(data=data, x="days", y="result", hue="size", ax=axs[0], markers=["o", "s"], palette=palette, errorbar=None)
    g.set(yscale='log')
    axs[0].set_ylabel("[s]")
    axs[0].set(xlabel=None)

    axs[1].set_title("every 60 seconds")
    data = muleTime("dist1")
    g = sns.lineplot(data=data, x="days", y="result", hue="size", ax=axs[1], markers=["o", "s"], palette=palette, errorbar=None)
    g.set(yscale='log')
    #axs[1].set_ylabel("[s]")
    axs[1].set(ylabel=None)
    axs[1].set(xlabel=None)

    axs[2].set_title("every 5 minutes")
    data = muleTime("dist2")
    g = sns.lineplot(data=data, x="days", y="result", hue="size", ax=axs[2], markers=["o", "s"], palette=palette, errorbar=None)
    g.set(yscale='log')
    #axs[1].set_ylabel("[s]")
    axs[2].set(ylabel=None)
    axs[2].set(xlabel=None)

    axs[3].set_title("every 15 minutes")
    data = muleTime("dist3")
    g = sns.lineplot(data=data, x="days", y="result", hue="size", ax=axs[3], markers=["o", "s"], palette=palette, errorbar=None)
    g.set(yscale='log')
    #axs[1].set_ylabel("[s]")
    axs[3].set(ylabel=None)
    axs[3].set(xlabel=None)

    axs[4].set_title("every 30 minutes")
    data = muleTime("dist4")
    g = sns.lineplot(data=data, x="days", y="result", hue="size", ax=axs[4], markers=["o", "s"], palette=palette, errorbar=None)
    g.set(yscale='log')
    #axs[2].set_ylabel("[s]")
    axs[4].set(ylabel=None)
    axs[4].set(xlabel=None)

    handles, labels = axs[0].get_legend_handles_labels()
    fig.legend(handles, ['64 KB', '10 MB'], loc='lower center', ncol=2)
    axs[0].get_legend().remove()
    axs[1].get_legend().remove()
    axs[2].get_legend().remove()
    axs[3].get_legend().remove()
    axs[4].get_legend().remove()

    plt.subplots_adjust(left=0.05,
                    bottom=0.171,
                    right=0.95,
                    top=0.838,
                    wspace=0.07,
                    hspace=0.2)
    plt.show()

def plotTable():
    fig, axs = plt.subplots(1,3, sharey=True, figsize=(20, 5))
    fig.suptitle("Scenario 3: Average Routing Table Size at Data Relays")

    axs[0].set_title("every 10 seconds")
    data = muleTable("dist0")
    sns.lineplot(data=data, x="days", y="result", hue="size", ax=axs[0], markers=["o", "s"], palette=palette, errorbar=None)
    axs[0].set_ylabel("Table Entries")
    axs[0].set(xlabel=None)

    axs[1].set_title("every 60 seconds")
    data = muleTable("dist0")
    sns.lineplot(data=data, x="days", y="result", hue="size", ax=axs[1], markers=["o", "s"], palette=palette, errorbar=None)
    axs[1].set(xlabel=None)

    axs[2].set_title("every 30 minutes")
    data = muleTable("dist0")
    sns.lineplot(data=data, x="days", y="result", hue="size", ax=axs[2], markers=["o", "s"], palette=palette, errorbar=None)
    axs[2].set(xlabel=None)

    handles, labels = axs[0].get_legend_handles_labels()
    #fig.legend(handles, ['Bundle Size', '64 KB', '10 MB', 'Nr. of Relays', '3', '5'], loc='lower center', ncol=6)
    #axs[0].get_legend().remove()
    axs[1].get_legend().remove()
    axs[2].get_legend().remove()

    plt.subplots_adjust(left=0.05,
                    bottom=0.148,
                    right=0.95,
                    top=0.852,
                    wspace=0.083,
                    hspace=0.2)

    plt.show()


#plotDelay()
#plotDelivery()
#plotDrop()
#plotComplexity()
plotRouting()
#plotTable()