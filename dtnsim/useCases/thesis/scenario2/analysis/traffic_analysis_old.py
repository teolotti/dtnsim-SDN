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


# result = percentage of total bundles sent in that config with that number of hops
def numberOfHops(distribution, size, typ):

    data = []
    for day in days:

        totalReceivedBundles = 0
        statIds = []
        input_path = "../cgr/{}/{}/results/traffic1-distribution={},size={},ttl=2500000-#0.sca".format(typ, day, distribution, size)

        conn = sqlite3.connect(input_path)
        conn.row_factory = sqlite3.Row
        cur = conn.cursor()

        cur.execute("SELECT statId, statCount FROM statistic WHERE statName='appBundleReceivedHops:histogram'")
        rows = cur.fetchall()
        for row in rows:
            totalReceivedBundles += row['statCount']
            statIds.append(row['statId'])

        for id in statIds:
            cur.execute("SELECT lowerEdge, binValue FROM histogramBin WHERE statId='{}'".format(id))
            rows = cur.fetchall()
            for row in rows:
                if row[0] != float('-inf'):
                    data.append([row[1], row[0], dayLabels[int(day)-1], size, totalReceivedBundles])
                
    df = pd.DataFrame(data, columns=["result", "hops", "days", "size", "total"])
    return df


def plotHops(distribution, size, typ):
    df = numberOfHops(distribution, size, typ)
    dfFinal = pd.DataFrame()
    for day in dayLabels:

        total = df.loc[df['days'] == day]['total'].iloc[0]
        dff = df.loc[df['days'] == day].groupby(['hops']).result.sum()
        print(dff)
        dfff = dff.apply(lambda x: 0 if (x == 0) else 100 / total * x).to_frame().reset_index()
        dfff['days'] = day
        dfFinal = dfFinal.append(dfff)

    dfFinal['hops'] = dfFinal['hops'].astype(int)
    #dfFinal = dfFinal.drop(dfFinal[(dfFinal.hops == 0)| (dfFinal.hops == 5) | (dfFinal.hops == 6)].index)
    sns.barplot(data=dfFinal, x='hops', y='result', hue='days', palette=palette)
    plt.show()

#plotHops("dist0", "64000", "a")
plotHops("dist0", "10000000", "a")




