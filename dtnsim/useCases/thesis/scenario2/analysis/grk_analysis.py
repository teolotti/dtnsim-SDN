import sqlite3
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# Plot Configurations
sns.set_style("darkgrid")
colors = ["#38B000", "#F72585", "#0d92d3", "#FE9D0B", "#0A0A0A", "#8F2D56"]
palette = sns.color_palette(colors)
colors2 = ["#0d92d3", "#FE9D0B"]
palette2 = sns.color_palette(colors2)
colors3 = ["#0A0A0A", "#8F2D56"]
palette3 = sns.color_palette(colors3)
plt.rcParams['axes.grid'] = True
plt.rcParams['font.family'] = 'monospace'

# Parameters
bundleSizes = [64000, 10000000]
bundleTraffics = ["traffic1"] #, "traffic2", "traffic3"]
days = ["1", "2", "3", "4"]
dayLabels = ["3d", "7d", "14d", "28d"]
#typ = ["a", "b"]
typ = ["b"]
executions = ["inter_relay", "no_inter_relay", "no_direct_to_earth"]

# Avg. bundle delay per parameter combination
def bundleDelay(distribution):

    data = []
    for day in days:
        for t in typ:
            for ex in executions:

                # GRK
                input_path = "../irr/grk/{}/{}/{}/results/traffic1-distribution={},size=10000000,ttl=2500000-#0.sca".format(ex, t, day, distribution)
                conn = sqlite3.connect(input_path)
                conn.row_factory = sqlite3.Row
                cur = conn.cursor()
                cur.execute("SELECT AVG(scalarValue) AS result FROM scalar WHERE scalarName='{}'".format("appBundleReceivedDelay:mean"))
                rows = cur.fetchall()
                data.append([rows[0]['result'], dayLabels[int(day)-1], t, ex])
            
            # CGR
            input_path = "../cgr/{}/{}/results/traffic1-distribution={},size=10000000,ttl=2500000-#0.sca".format(t, day, distribution)
            conn = sqlite3.connect(input_path)
            conn.row_factory = sqlite3.Row
            cur = conn.cursor()
            cur.execute("SELECT AVG(scalarValue) AS result FROM scalar WHERE scalarName='{}'".format("appBundleReceivedDelay:mean"))
            rows = cur.fetchall()
            data.append([rows[0]['result'], dayLabels[int(day)-1], t, "normal"])
                
    df = pd.DataFrame(data, columns=["result", "days", "type", "execution"])
    return df

# Bundle delivery ratio per parameter combination
def bundleDelivery(distribution):

    data = []
    for day in days:
        for t in typ:
            for ex in executions:

                # GRK
                input_path = "../irr/grk/{}/{}/{}/results/traffic1-distribution={},size=10000000,ttl=2500000-#0.sca".format(ex, t, day, distribution)
                conn = sqlite3.connect(input_path)
                conn.row_factory = sqlite3.Row
                cur = conn.cursor()

                cur.execute("SELECT SUM(scalarValue) AS result FROM scalar WHERE scalarName='{}'".format("appBundleReceived:count"))
                rows0 = cur.fetchall()
                successfullyDelivered = rows0[0]["result"]
                cur.execute("SELECT SUM(scalarValue) AS result FROM scalar WHERE scalarName='{}'".format("appBundleSent:count"))
                rows1 = cur.fetchall()
                overallSent = rows1[0]["result"]

                data.append([successfullyDelivered/overallSent, day, t, ex])

            input_path = "../cgr/{}/{}/results/traffic1-distribution={},size=10000000,ttl=2500000-#0.sca".format(t, day, distribution)
            conn = sqlite3.connect(input_path)
            conn.row_factory = sqlite3.Row
            cur = conn.cursor()

            cur.execute("SELECT SUM(scalarValue) AS result FROM scalar WHERE scalarName='{}'".format("appBundleReceived:count"))
            rows0 = cur.fetchall()
            successfullyDelivered = rows0[0]["result"]
            cur.execute("SELECT SUM(scalarValue) AS result FROM scalar WHERE scalarName='{}'".format("appBundleSent:count"))
            rows1 = cur.fetchall()
            overallSent = rows1[0]["result"]

            data.append([successfullyDelivered/overallSent, day, t, "normal"])

    df = pd.DataFrame(data, columns=["result", "days", "type", "execution"])
    return df

# Bundle drops per parameter combination
def bundleDrop(distribution):

    data = []
    for day in days:
        for t in typ:
            for ex in executions:

                # GRK
                input_path = "../irr/grk/{}/{}/{}/results/traffic1-distribution={},size=10000000,ttl=2500000-#0.sca".format(ex, t, day, distribution)
                conn = sqlite3.connect(input_path)
                conn.row_factory = sqlite3.Row
                cur = conn.cursor()
                cur.execute("SELECT SUM(scalarValue) AS result FROM scalar WHERE scalarName='{}'".format("bundleDropped:sum"))
                rows = cur.fetchall()
                data.append([rows[0]["result"], day, t, ex])
            
            # CGR
            input_path = "../cgr/{}/{}/results/traffic1-distribution={},size=10000000,ttl=2500000-#0.sca".format(t, day, distribution)
            conn = sqlite3.connect(input_path)
            conn.row_factory = sqlite3.Row
            cur = conn.cursor()
            cur.execute("SELECT SUM(scalarValue) AS result FROM scalar WHERE scalarName='{}'".format("bundleDropped:sum"))
            rows = cur.fetchall()
            data.append([rows[0]["result"], day, t, "normal"])

    df = pd.DataFrame(data, columns=["result", "days", "type", "execution"])
    return df

# Avg. routing computation time per bundle, averaged over all mules
def muleTime(distribution):

    data = []
    for t in typ:

        if t == 'a':
            mules = [7, 8, 9]
        else:
            mules = [7, 8, 9, 10, 11]

        for day in days:
            for ex in executions:

                input_path = "../irr/grk/{}/{}/{}/results/traffic1-distribution={},size=10000000,ttl=2500000-#0.sca".format(ex, t, day, distribution)
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

                data.append([result/len(mules), dayLabels[int(day)-1], t, ex])
            
            input_path = "../cgr/{}/{}/results/traffic1-distribution={},size=10000000,ttl=2500000-#0.sca".format(t, day, distribution)
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

            data.append([result/len(mules), dayLabels[int(day)-1], t, "normal"])
                
    df = pd.DataFrame(data, columns=["result", "days", "type", "execution"])
    return df



# Avg. routing table size at mules, averaged over all mules
def muleTable(distribution):

    data = []
    for t in typ:

        if t == 'a':
            mules = [7, 8, 9]
        else:
            mules = [7, 8, 9, 10, 11]

        for day in days:
            for ex in executions:

                input_path = "../irr/grk/{}/{}/{}/results/traffic1-distribution={},size=10000000,ttl=2500000-#0.sca".format(ex, t, day, distribution)
                conn = sqlite3.connect(input_path)
                conn.row_factory = sqlite3.Row
                cur = conn.cursor()

                result = 0
                for mule in mules:
                    cur.execute("SELECT scalarValue AS result FROM scalar WHERE scalarName='{}' AND moduleName LIKE '%{}%'".format("routeTableSize:mean", mule))
                    rows = cur.fetchall()
                    result = 0 if (rows[0]["result"] == None) else rows[0]["result"]

                data.append([result/len(mules), dayLabels[int(day)-1], t, ex])

            input_path = "../cgr/{}/{}/results/traffic1-distribution={},size=10000000,ttl=2500000-#0.sca".format(t, day, distribution)
            conn = sqlite3.connect(input_path)
            conn.row_factory = sqlite3.Row
            cur = conn.cursor()

            result = 0
            for mule in mules:
                cur.execute("SELECT scalarValue AS result FROM scalar WHERE scalarName='{}' AND moduleName LIKE '%{}%'".format("routeTableSize:mean", mule))
                rows = cur.fetchall()
                result = 0 if (rows[0]["result"] == None) else rows[0]["result"]

            data.append([result/len(mules), dayLabels[int(day)-1], t, "normal"])
        
                
    df = pd.DataFrame(data, columns=["result", "days", "type", "execution"])
    return df

# Plots
def plotDelay():
    fig, axs = plt.subplots(1,3, sharey=True, figsize=(20, 5))
    fig.suptitle("Scenario 2: Average Bundle Delay")

    axs[0].set_title("every 10 seconds")
    data = bundleDelay("dist0")
    sns.lineplot(data=data, x="days", y="result", hue="execution", style="type", ax=axs[0], markers=["o", "s"], palette=palette)
    axs[0].set_ylabel("[s]")
    axs[0].set(xlabel=None)

    axs[1].set_title("every 60 seconds")
    data = bundleDelay("dist1")
    sns.lineplot(data=data, x="days", y="result", hue="execution", style="type", ax=axs[1], markers=["o", "s"], palette=palette)
    axs[1].set_ylabel("[s]")
    axs[1].set(xlabel=None)

    axs[2].set_title("every 30 minutes")
    data = bundleDelay("dist4")
    sns.lineplot(data=data, x="days", y="result", hue="execution", style="type", ax=axs[2], markers=["o", "s"], palette=palette)
    axs[2].set_ylabel("[s]")
    axs[2].set(xlabel=None)

    handles, labels = axs[0].get_legend_handles_labels()
    fig.legend(handles, labels, loc='lower center')
    #fig.legend(handles, ['Bundle Size', '64 KB', '10 MB', 'Nr. of Relays', '3', '5'], loc='lower center', ncol=6)
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
    fig.suptitle("Scenario 2: Bundle Delivery Ratio")

    axs[0].set_title("every 10 seconds")
    data = bundleDelivery("dist0")
    sns.lineplot(data=data, x="days", y="result", hue="execution", style="type", ax=axs[0], markers=["o", "s"], palette=palette)
    axs[0].set_ylabel("ratio")
    axs[0].set(xlabel=None)

    axs[1].set_title("every 60 seconds")
    data = bundleDelivery("dist1")
    sns.lineplot(data=data, x="days", y="result", hue="execution", style="type", ax=axs[1], markers=["o", "s"], palette=palette)
    axs[1].set_ylabel("ratio")
    axs[1].set(xlabel=None)

    axs[2].set_title("every 30 minutes")
    data = bundleDelivery("dist4")
    sns.lineplot(data=data, x="days", y="result", hue="execution", style="type", ax=axs[2], markers=["o", "s"], palette=palette)
    axs[2].set_ylabel("ratio")
    axs[2].set(xlabel=None)

    handles, labels = axs[0].get_legend_handles_labels()
    fig.legend(handles, labels, loc='lower center')
    #fig.legend(handles, ['Bundle Size', '64 KB', '10 MB', 'Nr. of Relays', '3', '5'], loc='lower center', ncol=6)
    axs[0].get_legend().remove()
    axs[1].get_legend().remove()
    axs[2].get_legend().remove()

    plt.show()

def plotDrop():
    fig, axs = plt.subplots(1,3, sharey=False, figsize=(20, 5))
    fig.suptitle("Scenario 2: Bundle Drops")

    axs[0].set_title("every 10 seconds")
    data = bundleDrop("dist0")
    sns.lineplot(data=data, x="days", y="result", hue="execution", style="type", ax=axs[0], markers=["o", "s"], palette=palette)
    axs[0].set_ylabel("drops")
    axs[0].set(xlabel=None)

    axs[1].set_title("every 60 seconds")
    data = bundleDrop("dist1")
    sns.lineplot(data=data, x="days", y="result", hue="execution", style="type", ax=axs[1], markers=["o", "s"], palette=palette)
    axs[1].set_ylabel("drops")
    axs[1].set(xlabel=None)

    axs[2].set_title("every 30 minutes")
    data = bundleDrop("dist4")
    sns.lineplot(data=data, x="days", y="result", hue="execution", style="type", ax=axs[2], markers=["o", "s"], palette=palette)
    axs[2].set_ylabel("drops")
    axs[2].set(xlabel=None)

    handles, labels = axs[0].get_legend_handles_labels()
    fig.legend(handles, labels, loc='lower center')
    #fig.legend(handles, ['Bundle Size', '64 KB', '10 MB', 'Nr. of Relays', '3', '5'], loc='lower center', ncol=6)
    axs[0].get_legend().remove()
    axs[1].get_legend().remove()
    axs[2].get_legend().remove()

    plt.show()

def plotRouting():
    fig, axs = plt.subplots(1,3, sharey=False, figsize=(20, 5))
    fig.suptitle("Scenario 2: Average Routing Computation Time per Bundle at Data Relays")

    axs[0].set_title("every 10 seconds")
    data = muleTime("dist0")
    sns.lineplot(data=data, x="days", y="result", hue="execution", style="type", ax=axs[0], markers=["o", "s"], palette=palette)
    axs[0].set_ylabel("[s]")
    axs[0].set(xlabel=None)

    axs[1].set_title("every 60 seconds")
    data = muleTime("dist1")
    sns.lineplot(data=data, x="days", y="result", hue="execution", style="type", ax=axs[1], markers=["o", "s"], palette=palette)
    #axs[1].set_ylabel("[s]")
    axs[1].set(ylabel=None)
    axs[1].set(xlabel=None)

    axs[2].set_title("every 30 minutes")
    data = muleTime("dist4")
    sns.lineplot(data=data, x="days", y="result", hue="execution", style="type", ax=axs[2], markers=["o", "s"], palette=palette)
    #axs[2].set_ylabel("[s]")
    axs[2].set(ylabel=None)
    axs[2].set(xlabel=None)

    handles, labels = axs[0].get_legend_handles_labels()
    fig.legend(handles, labels, loc='lower center')
    #fig.legend(handles, ['Bundle Size', '64 KB', '10 MB', 'Nr. of Relays', '3', '5'], loc='lower center', ncol=6)
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
    fig.suptitle("Scenario 2: Average Routing Table Size at Data Relays")

    axs[0].set_title("every 10 seconds")
    data = muleTable("dist0")
    sns.lineplot(data=data, x="days", y="result", hue="execution", style="type", ax=axs[0], markers=["o", "s"], palette=palette)
    axs[0].set_ylabel("Table Entries")
    axs[0].set(xlabel=None)

    axs[1].set_title("every 60 seconds")
    data = muleTable("dist0")
    sns.lineplot(data=data, x="days", y="result", hue="execution", style="type", ax=axs[1], markers=["o", "s"], palette=palette)
    axs[1].set(xlabel=None)

    axs[2].set_title("every 30 minutes")
    data = muleTable("dist0")
    sns.lineplot(data=data, x="days", y="result", hue="execution", style="type", ax=axs[2], markers=["o", "s"], palette=palette)
    axs[2].set(xlabel=None)

    handles, labels = axs[0].get_legend_handles_labels()
    fig.legend(handles, labels, loc='lower center')
    #fig.legend(handles, ['Bundle Size', '64 KB', '10 MB', 'Nr. of Relays', '3', '5'], loc='lower center', ncol=6)
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

def plotSummary():
    fig, axs = plt.subplots(1,3, sharey=False, figsize=(20, 5))
    fig.suptitle("Scenario 2: Impact of Contact Plan Reduction")

    axs[0].set_title("Avg. Routing Computation Time / Bundle at Data Relays")
    data = muleTime("dist0")
    sns.lineplot(data=data, x="days", y="result", hue="execution", style="type", ax=axs[0], markers=["o", "s"], palette=palette)
    axs[0].set_ylabel("[s]")
    axs[0].set(xlabel=None)

    axs[1].set_title("Avg. Routing Table Size at Data Relays")
    data = muleTable("dist0")
    sns.lineplot(data=data, x="days", y="result", hue="execution", style="type", ax=axs[1], markers=["o", "s"], palette=palette)
    axs[1].set_ylabel("Table Entries")
    axs[1].set(xlabel=None)

    axs[2].set_title("Avg. Bundle Delay")
    data = bundleDelay("dist0")
    sns.lineplot(data=data, x="days", y="result", hue="execution", style="type", ax=axs[2], markers=["o", "s"], palette=palette)
    axs[2].set_ylabel("[s]")
    axs[2].set(xlabel=None)

    deliveryRatio = bundleDelivery("dist0")
    deliveryRatio = deliveryRatio.groupby(["execution"]).mean()
    deliveryRatio = deliveryRatio.apply(lambda x: x * 100)
    dr = deliveryRatio.round(2)['result'].tolist()
    dr = [str(x) + "%" for x in dr]
    dr.insert(0, 'Delivery')

    handles, labels = axs[0].get_legend_handles_labels()
    fig.legend(handles, ['Reduction', 'Lunar Stations', 'Inter Relay', 'Direct to Earth', 'None', 'Relays', '5'], loc='lower center', ncol=7)
    axs[0].get_legend().remove()
    axs[1].get_legend().remove()

    handles, labels = axs[2].get_legend_handles_labels()
    axs[2].legend(handles[:-2], dr)

    plt.subplots_adjust(left=0.05,
                    bottom=0.148,
                    right=0.95,
                    top=0.852,
                    wspace=0.176,
                    hspace=0.2)

    plt.show()


'''
plotDelay()
plotDelivery()
plotDrop()
plotRouting()
plotTable()
'''

plotSummary()

