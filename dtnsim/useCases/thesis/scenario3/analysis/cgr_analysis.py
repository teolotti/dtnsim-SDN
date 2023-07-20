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
bundleTraffics = ["traffic1"] #, "traffic2", "traffic3"]
days = ["1", "2", "3", "4"]
dayLabels = ["3d", "7d", "14d", "28d"]
typ = ["a", "b"]

# Avg. bundle delay per parameter combination
def bundleDelay(distribution):

    data = []
    for size in bundleSizes:
        for day in days:
            for t in typ:
                for traffic in bundleTraffics:

                    if day == '3':
                        if t == 'b':
                            traffic = 'traffic2'
                    
                    if day == '4':
                        if t == 'a':
                            traffic = 'traffic2'
                        else:
                            continue

                    input_path = "../cgr/{}/{}/results/{}-distribution={},size={},ttl=2500000-#0.sca".format(t, day, traffic, distribution, size)
                    conn = sqlite3.connect(input_path)
                    conn.row_factory = sqlite3.Row
                    cur = conn.cursor()
                    cur.execute("SELECT AVG(scalarValue) AS result FROM scalar WHERE scalarName='{}'".format("appBundleReceivedDelay:mean"))
                    rows = cur.fetchall()
                    data.append([rows[0]['result'], dayLabels[int(day)-1], t, size, traffic])
                
    df = pd.DataFrame(data, columns=["result", "days", "type", "size", "traffic"])
    return df

# Bundle delivery ratio per parameter combination
def bundleDelivery(distribution):

    data = []
    for size in bundleSizes:
        for day in days:
            for t in typ:

                for traffic in bundleTraffics:

                    if day == '3':
                        if t == 'b':
                            traffic = 'traffic2'

                    if day == '4':
                        if t == 'a':
                            traffic = 'traffic2'
                        else:
                            continue

                    input_path = "../cgr/{}/{}/results/{}-distribution={},size={},ttl=2500000-#0.sca".format(t, day, traffic, distribution, size)
                    conn = sqlite3.connect(input_path)
                    conn.row_factory = sqlite3.Row
                    cur = conn.cursor()

                    cur.execute("SELECT SUM(scalarValue) AS result FROM scalar WHERE scalarName='{}'".format("appBundleReceived:count"))
                    rows0 = cur.fetchall()
                    successfullyDelivered = rows0[0]["result"]
                    cur.execute("SELECT SUM(scalarValue) AS result FROM scalar WHERE scalarName='{}'".format("appBundleSent:count"))
                    rows1 = cur.fetchall()
                    overallSent = rows1[0]["result"]

                    data.append([successfullyDelivered/overallSent, day, t, size, traffic])

    df = pd.DataFrame(data, columns=["result", "days", "type", "size", "traffic"])
    return df

# Bundle drops per parameter combination
def bundleDrop(distribution):

    data = []
    for size in bundleSizes:
        for day in days:
            for t in typ:

                for traffic in bundleTraffics:

                    if day == '3':
                        if t == 'b':
                            traffic = 'traffic2'

                    if day == '4':
                        if t == 'a':
                            traffic = 'traffic2'
                        else:
                            continue

                    input_path = "../cgr/{}/{}/results/{}-distribution={},size={},ttl=2500000-#0.sca".format(t, day, traffic, distribution, size)
                    conn = sqlite3.connect(input_path)
                    conn.row_factory = sqlite3.Row
                    cur = conn.cursor()

                    cur.execute("SELECT SUM(scalarValue) AS result FROM scalar WHERE scalarName='{}'".format("bundleDropped:sum"))
                    rows = cur.fetchall()

                    data.append([rows[0]["result"], day, t, size, traffic])

    df = pd.DataFrame(data, columns=["result", "days", "type", "size", "traffic"])
    return df

# Avg. routing computation time per bundle, averaged over all mules
def muleTime(distribution):

    data = []
    for t in typ:

        if t == 'a':
            mules = [23, 24, 25]
        else:
            mules = [23, 24, 25, 26, 27]

        for size in bundleSizes:
            for day in days:
                for traffic in bundleTraffics:

                    if day == '3':
                        if t == 'b':
                            traffic = 'traffic2'

                    if day == '4':
                        if t == 'a':
                            traffic = 'traffic2'
                        else:
                            continue

                    input_path = "../cgr/{}/{}/results/{}-distribution={},size={},ttl=2500000-#0.sca".format(t, day, traffic, distribution, size)
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

                    data.append([result/len(mules), dayLabels[int(day)-1], t, size, traffic])
                
    df = pd.DataFrame(data, columns=["result", "days", "type", "size", "traffic"])
    return df

# Avg. routing computation time per bundle, per mule, just 10MB
def muleTimeIdv(distribution):

    data = []
    for t in typ:

        if t == 'a':
            mules = [23, 24, 25]
        else:
            mules = [23, 24, 25, 26, 27]

        for day in days:
            for traffic in bundleTraffics:

                if day == '3':
                        if t == 'b':
                            traffic = 'traffic2'

                if day == '4':
                    if t == 'a':
                        traffic = 'traffic2'
                    else:
                        continue

                input_path = "../cgr/{}/{}/results/{}-distribution={},size=10000000,ttl=2500000-#0.sca".format(t, day, traffic, distribution)
                conn = sqlite3.connect(input_path)
                conn.row_factory = sqlite3.Row
                cur = conn.cursor()

                for mule in mules:
                    cur.execute("SELECT scalarValue AS result FROM scalar WHERE scalarName='{}' AND moduleName LIKE '%{}%'".format("bundleReceivedFromCom:count", mule))
                    rows = cur.fetchall()
                    totalBundlesSeen = rows[0]["result"]

                    cur.execute("SELECT scalarValue AS result FROM scalar WHERE scalarName='{}' AND moduleName LIKE '%{}%'".format("routeExecutionTimeUs:sum", mule))
                    rows = cur.fetchall()
                    routingTime = 0 if (rows[0]["result"] == 0) else (rows[0]["result"] / 1000000)/totalBundlesSeen

                    data.append([routingTime, dayLabels[int(day)-1], t, traffic, mule])
                
    df = pd.DataFrame(data, columns=["result", "days", "type", "traffic", "mule"])
    return df

# Avg. routing table size at mules, averaged over all mules
def muleTable(distribution):

    data = []
    for t in typ:

        if t == 'a':
            mules = [23, 24, 25]
        else:
            mules = [23, 24, 25, 26, 27]

        for size in bundleSizes:
            for day in days:
                for traffic in bundleTraffics:

                    if day == '3':
                        if t == 'b':
                            traffic = 'traffic2'

                    if day == '4':
                        if t == 'a':
                            traffic = 'traffic2'
                        else:
                            continue

                    input_path = "../cgr/{}/{}/results/{}-distribution={},size={},ttl=2500000-#0.sca".format(t, day, traffic, distribution, size)
                    conn = sqlite3.connect(input_path)
                    conn.row_factory = sqlite3.Row
                    cur = conn.cursor()

                    result = 0
                    for mule in mules:
                        cur.execute("SELECT scalarValue AS result FROM scalar WHERE scalarName='{}' AND moduleName LIKE '%{}%'".format("routeTableSize:mean", mule))
                        rows = cur.fetchall()
                        result = 0 if (rows[0]["result"] == None) else rows[0]["result"]

                    data.append([result/len(mules), dayLabels[int(day)-1], t, size, traffic])
                
    df = pd.DataFrame(data, columns=["result", "days", "type", "size", "traffic"])
    return df

# Plots
def plotDelay():
    fig, axs = plt.subplots(1,3, sharey=True, figsize=(20, 5))
    fig.suptitle("Scenario 3: Average Bundle Delay")

    axs[0].set_title("every 10 seconds")
    dataDelay = bundleDelay("dist0")
    sns.lineplot(data=dataDelay, x="days", y="result", hue="size", style="type", ax=axs[0], markers=["o", "s"], palette=palette, errorbar=None)
    axs[0].set_ylabel("[s]")
    axs[0].set(xlabel=None)

    axs[1].set_title("every 60 seconds")
    dataDelay = bundleDelay("dist1")
    sns.lineplot(data=dataDelay, x="days", y="result", hue="size", style="type", ax=axs[1], markers=["o", "s"], palette=palette, errorbar=None)
    axs[0].set_ylabel("[s]")
    axs[1].set(xlabel=None)

    axs[2].set_title("every 30 minutes")
    dataDelay = bundleDelay("dist4")
    sns.lineplot(data=dataDelay, x="days", y="result", hue="size", style="type", ax=axs[2], markers=["o", "s"], palette=palette, errorbar=None)
    axs[0].set_ylabel("[s]")
    axs[2].set(xlabel=None)

    handles, labels = axs[0].get_legend_handles_labels()
    fig.legend(handles, ['Bundle Size', '64 KB', '10 MB', 'Nr. of Relays', '3', '5'], loc='lower center', ncol=6)
    axs[0].get_legend().remove()
    axs[1].get_legend().remove()
    axs[2].get_legend().remove()

    plt.subplots_adjust(left=0.05,
                    bottom=0.148,
                    right=0.95,
                    top=0.852,
                    wspace=0.083,
                    hspace=0.2)

    plt.show()

def plotDelivery():
    fig, axs = plt.subplots(1,3, sharey=True, figsize=(20, 5))
    fig.suptitle("Scenario 3: Bundle Delivery Ratio")

    axs[0].set_title("every 10 seconds")
    data = bundleDelivery("dist0")
    sns.lineplot(data=data, x="days", y="result", hue="size", style="type", ax=axs[0], markers=["o", "s"], palette=palette)
    axs[0].set_ylabel("ratio")
    axs[0].set(xlabel=None)

    axs[1].set_title("every 60 seconds")
    data = bundleDelivery("dist1")
    sns.lineplot(data=data, x="days", y="result", hue="size", style="type", ax=axs[1], markers=["o", "s"], palette=palette)
    axs[0].set_ylabel("ratio")
    axs[1].set(xlabel=None)

    axs[2].set_title("every 30 minutes")
    data = bundleDelivery("dist4")
    sns.lineplot(data=data, x="days", y="result", hue="size", style="type", ax=axs[2], markers=["o", "s"], palette=palette)
    axs[0].set_ylabel("ratio")
    axs[2].set(xlabel=None)

    handles, labels = axs[0].get_legend_handles_labels()
    fig.legend(handles, ['Bundle Size', '64 KB', '10 MB', 'Nr. of Relays', '3', '5'], loc='lower center', ncol=6)
    axs[0].get_legend().remove()
    axs[1].get_legend().remove()
    axs[2].get_legend().remove()

    plt.show()

def plotDrop():
    fig, axs = plt.subplots(1,3, sharey=False, figsize=(20, 5))
    fig.suptitle("Scenario 3: Bundle Drops")

    axs[0].set_title("every 10 seconds")
    data = bundleDrop("dist0")
    sns.lineplot(data=data, x="days", y="result", hue="size", style="type", ax=axs[0], markers=["o", "s"], palette=palette)
    axs[0].set_ylabel("drops")
    axs[0].set(xlabel=None)

    axs[1].set_title("every 60 seconds")
    data = bundleDrop("dist1")
    sns.lineplot(data=data, x="days", y="result", hue="size", style="type", ax=axs[1], markers=["o", "s"], palette=palette)
    axs[0].set_ylabel("drops")
    axs[1].set(xlabel=None)

    axs[2].set_title("every 30 minutes")
    data = bundleDrop("dist4")
    sns.lineplot(data=data, x="days", y="result", hue="size", style="type", ax=axs[2], markers=["o", "s"], palette=palette)
    axs[0].set_ylabel("drops")
    axs[2].set(xlabel=None)

    handles, labels = axs[0].get_legend_handles_labels()
    fig.legend(handles, ['Bundle Size', '64 KB', '10 MB', 'Nr. of Relays', '3', '5'], loc='lower center', ncol=6)
    axs[0].get_legend().remove()
    axs[1].get_legend().remove()
    axs[2].get_legend().remove()

    plt.show()

def plotRouting():
    fig, axs = plt.subplots(1,3, sharey=False, figsize=(20, 5))
    fig.suptitle("Scenario 3: Average Routing Computation Time per Bundle at Data Relays")

    axs[0].set_title("every 10 seconds")
    data = muleTime("dist0")
    sns.lineplot(data=data, x="days", y="result", hue="size", style="type", ax=axs[0], markers=["o", "s"], palette=palette, errorbar=None)
    axs[0].set_ylabel("[s]")
    axs[0].set(xlabel=None)

    axs[1].set_title("every 60 seconds")
    data = muleTime("dist1")
    sns.lineplot(data=data, x="days", y="result", hue="size", style="type", ax=axs[1], markers=["o", "s"], palette=palette, errorbar=None)
    #axs[1].set_ylabel("[s]")
    axs[1].set(ylabel=None)
    axs[1].set(xlabel=None)

    axs[2].set_title("every 30 minutes")
    data = muleTime("dist4")
    sns.lineplot(data=data, x="days", y="result", hue="size", style="type", ax=axs[2], markers=["o", "s"], palette=palette, errorbar=None)
    #axs[2].set_ylabel("[s]")
    axs[2].set(ylabel=None)
    axs[2].set(xlabel=None)

    handles, labels = axs[0].get_legend_handles_labels()
    fig.legend(handles, ['Bundle Size', '64 KB', '10 MB', 'Nr. of Relays', '3', '5'], loc='lower center', ncol=6)
    axs[0].get_legend().remove()
    axs[1].get_legend().remove()
    axs[2].get_legend().remove()

    plt.subplots_adjust(left=0.05,
                    bottom=0.148,
                    right=0.95,
                    top=0.852,
                    wspace=0.2,
                    hspace=0.2)

    plt.show()

def plotTable():
    fig, axs = plt.subplots(1,3, sharey=True, figsize=(20, 5))
    fig.suptitle("Scenario 3: Average Routing Table Size at Data Relays")

    axs[0].set_title("every 10 seconds")
    data = muleTable("dist0")
    sns.lineplot(data=data, x="days", y="result", hue="size", style="type", ax=axs[0], markers=["o", "s"], palette=palette, errorbar=None)
    axs[0].set_ylabel("Table Entries")
    axs[0].set(xlabel=None)

    axs[1].set_title("every 60 seconds")
    data = muleTable("dist0")
    sns.lineplot(data=data, x="days", y="result", hue="size", style="type", ax=axs[1], markers=["o", "s"], palette=palette, errorbar=None)
    axs[1].set(xlabel=None)

    axs[2].set_title("every 30 minutes")
    data = muleTable("dist0")
    sns.lineplot(data=data, x="days", y="result", hue="size", style="type", ax=axs[2], markers=["o", "s"], palette=palette, errorbar=None)
    axs[2].set(xlabel=None)

    handles, labels = axs[0].get_legend_handles_labels()
    fig.legend(handles, ['Bundle Size', '64 KB', '10 MB', 'Nr. of Relays', '3', '5'], loc='lower center', ncol=6)
    axs[0].get_legend().remove()
    axs[1].get_legend().remove()
    axs[2].get_legend().remove()

    plt.subplots_adjust(left=0.05,
                    bottom=0.148,
                    right=0.95,
                    top=0.852,
                    wspace=0.083,
                    hspace=0.2)

    plt.show()

def plotRoutingPerMule():
    fig, axs = plt.subplots(1,3, sharey=False, figsize=(20, 5))
    fig.suptitle("Scenario 3: Average Routing Computation Time per Bundle at Data Relays - Comparison")

    axs[0].set_title("every 10 seconds")
    data = muleTimeIdv("dist0")
    sns.barplot(data=data.loc[data["days"] == "28d"], x="mule", y="result", hue="type", ax=axs[0], errorbar=None)
    sns.barplot(data=data.loc[data["days"] == "14d"], x="mule", y="result", hue="type", ax=axs[0], errorbar=None)
    sns.barplot(data=data.loc[data["days"] == "7d"], x="mule", y="result", hue="type", ax=axs[0], errorbar=None)
    sns.barplot(data=data.loc[data["days"] == "3d"], x="mule", y="result", hue="type", ax=axs[0], errorbar=None)
    axs[0].set_ylabel("[s]")
    axs[0].set(xlabel=None)

    axs[1].set_title("every 60 seconds")
    data = muleTime("dist1")
    sns.lineplot(data=data, x="days", y="result", hue="size", style="type", ax=axs[1], markers=["o", "s"], palette=palette)
    #axs[1].set_ylabel("[s]")
    axs[1].set(ylabel=None)
    axs[1].set(xlabel=None)

    axs[2].set_title("every 30 minutes")
    data = muleTime("dist4")
    sns.lineplot(data=data, x="days", y="result", hue="size", style="type", ax=axs[2], markers=["o", "s"], palette=palette)
    #axs[2].set_ylabel("[s]")
    axs[2].set(ylabel=None)
    axs[2].set(xlabel=None)

    handles, labels = axs[0].get_legend_handles_labels()
    fig.legend(handles, labels)
    #['Bundle Size', '64 KB', '10 MB', 'Nr. of Relays', '3', '5'], loc='lower center', ncol=6)
    axs[0].get_legend().remove()
    axs[1].get_legend().remove()
    axs[2].get_legend().remove()

    plt.show()

plotDelay()
plotDelivery()
plotDrop()
plotRouting()
plotTable()

#plotRoutingPerMule()