import sqlite3
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# Plot Configurations
sns.set_style("darkgrid")
colors = ["#38B000", "#F72585", "#0d92d3", "#FE9D0B"]
palette = sns.color_palette(colors)
plt.rcParams['axes.grid'] = True
plt.rcParams['font.family'] = 'monospace'

# Parameters
bundleSizes = [64000, 10000000]
bundleTraffics = ["traffic1"] #, "traffic2", "traffic3"]
days = ["1", "2", "3", "4"]
dayLabels = ["3d", "7d", "14d", "28d"]
typ = ["a", "b"]



#
def meanNumberOfHops(distribution):

    data = []
    for size in bundleSizes:
        for day in days:
            for t in typ:
                for traffic in bundleTraffics:

                    input_path = "../cgr/{}/{}/results/{}-distribution={},size={},ttl=2500000-#0.sca".format(t, day, traffic, distribution, size)
                    conn = sqlite3.connect(input_path)
                    conn.row_factory = sqlite3.Row
                    cur = conn.cursor()

                    cur.execute("SELECT AVG(statMean) AS result FROM statistic WHERE statName='appBundleReceivedHops:histogram'")
                    rows = cur.fetchall()
                    data.append([rows[0]['result'], dayLabels[int(day)-1], t, size, traffic])

                
    df = pd.DataFrame(data, columns=["result", "days", "type", "size", "traffic"])
    return df


# Plots
def plotHops():
    fig, axs = plt.subplots(1,3, sharey=True, figsize=(20, 5))
    fig.suptitle("Scenario 2: Average Number of Hops per Bundle")

    axs[0].set_title("every 10 seconds")
    data = meanNumberOfHops('dist0')
    sns.lineplot(data=data, x="days", y="result", hue="size", style="type", ax=axs[0], markers=["o", "s"], palette=palette, errorbar=None)
    axs[0].set_ylabel("Hops")
    axs[0].set(xlabel=None)

    axs[1].set_title("every 60 seconds")
    data = meanNumberOfHops('dist1')
    sns.lineplot(data=data, x="days", y="result", hue="size", style="type", ax=axs[1], markers=["o", "s"], palette=palette, errorbar=None)
    axs[0].set_ylabel("Hops")
    axs[1].set(xlabel=None)

    axs[2].set_title("every 30 minutes")
    data = meanNumberOfHops('dist4')
    sns.lineplot(data=data, x="days", y="result", hue="size", style="type", ax=axs[2], markers=["o", "s"], palette=palette, errorbar=None)
    axs[0].set_ylabel("Hops")
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


plotHops()