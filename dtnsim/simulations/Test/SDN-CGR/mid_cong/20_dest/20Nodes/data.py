import sqlite3
import sys


# Specify the path to the database file
# partendo dal file attuale vai alla cartella results e poi prendi il file General-#0.sca
# il file General-#0.sca è il file che contiene i dati di simulazione
#
def jain_index(occupancy_list):
    # Calcola la somma delle occupazioni medie
    sum_x = sum(occupancy_list)

    # Calcola la somma dei quadrati delle occupazioni medie
    sum_x_squared = sum(x ** 2 for x in occupancy_list)

    # Calcola il Jain's fairness index
    j_index = (sum_x ** 2) / (len(occupancy_list) * sum_x_squared)

    return j_index


def main():
    database_path = "results/General-#0.sca"

    # Connect to the database
    conn = sqlite3.connect(database_path)

    # Perform database operations here...
    # execute sql query to get nodesNumber
    conn.row_factory = sqlite3.Row

    cur = conn.cursor()
    cur.execute("SELECT * FROM runConfig WHERE configKey='dtnsim.nodesNumber'")  # SELECT * FROM runparam WHERE
    # parName='dtnsim.nodesNumber'
    rows0 = cur.fetchall()
    nodesNumber = rows0[0]["configValue"]

    # save stats in dictionaries
    # statsApp[statName][nodeNum] = [min, max, mean, std deviation]
    # statsDtn[statName][nodeNum] = [min, max, mean, std deviation]
    statsApp = {}
    statsDtn = {}

    cur.execute("SELECT * FROM statistic")
    rows = cur.fetchall()

    moduleNames = [row["moduleName"] for row in rows]
    statNames = [row["statName"] for row in rows]
    statIds = [row["statId"] for row in rows]
    statMins = [row["statMin"] for row in rows]
    statMaxs = [row["statMax"] for row in rows]
    statMeans = [row["statMean"] for row in rows]
    statStds = [row["statStddev"] for row in rows]

    for i in range(0, len(statIds)):

        moduleName = moduleNames[i]
        nodeNum = int(''.join(ele for ele in moduleName if ele.isdigit()))

        if (nodeNum != 0):
            values = [statMins[i], statMaxs[i], statMeans[i], statStds[i]]

            if (moduleName.find(".app") != -1):
                if (statNames[i] in statsApp.keys()):
                    d = statsApp[statNames[i]]
                    d[nodeNum] = values
                    statsApp[statNames[i]] = d
                else:
                    d = {}
                    d[nodeNum] = values
                    statsApp[statNames[i]] = d

            if (moduleName.find(".dtn") != -1):
                if (statNames[i] in statsDtn.keys()):
                    d = statsDtn[statNames[i]]
                    d[nodeNum] = values
                    statsDtn[statNames[i]] = d
                else:
                    d = {}
                    d[nodeNum] = values
                    statsDtn[statNames[i]] = d

    # Close the connection
    conn.close()

    # Salva su un file .txt la media di ogni media di ogni nodo per gli statName appBundleReceivedHops:histogram e appBundleReceivedDelay:histogram
    f = open("data.txt", "w")
    for statName in statsApp.keys():
        if statName == "appBundleReceivedHops:histogram":
            count = 0
            sum = 0
            for nodeNum in statsApp[statName].keys():
                # somma e poi calcola la media
                if nodeNum > 1:
                    values = statsApp[statName][nodeNum]
                    mean = values[2]
                    if mean is not None:
                        count += 1
                        sum += mean
            f.write("Hop mean: " + str(sum / count) + "\n")

        if statName == "appBundleReceivedDelay:histogram":
            count = 0
            sum = 0
            for nodeNum in statsApp[statName].keys():
                # somma e poi calcola la media
                if nodeNum > 1:
                    values = statsApp[statName][nodeNum]
                    mean = values[2]
                    if mean is not None:
                        count += 1
                        sum += mean
            f.write("Delay mean: " + str(sum / count) + "\n")

    f.close()

    #Salva sullo stesso file il minimo hop totale e il massimo hop totale
    # statsApp[statName][nodeNum] = [min, max, mean, std deviation]
    # statsDtn[statName][nodeNum] = [min, max, mean, std deviation]
    f = open("data.txt", "a")
    minHop = sys.maxsize
    maxHop = 0
    for statName in statsApp.keys():
        if statName == "appBundleReceivedHops:histogram":
            for nodeNum in statsApp[statName].keys():
                if nodeNum > 1:
                    values = statsApp[statName][nodeNum]
                    min = values[0]
                    max = values[1]
                    if min is not None:
                        if min < minHop:
                            minHop = min
                    if max is not None:
                        if max > maxHop:
                            maxHop = max
    f.write("Min hop: " + str(minHop) + "\n")
    f.write("Max hop: " + str(maxHop) + "\n")

    #Salva sullo stesso file il minimo delay totale e il massimo delay totale
    # statsApp[statName][nodeNum] = [min, max, mean, std deviation]
    # statsDtn[statName][nodeNum] = [min, max, mean, std deviation]
    f = open("data.txt", "a")
    minDelay = sys.maxsize
    maxDelay = 0
    for statName in statsApp.keys():
        if statName == "appBundleReceivedDelay:histogram":
            for nodeNum in statsApp[statName].keys():
                if nodeNum > 1:
                    values = statsApp[statName][nodeNum]
                    min = values[0]
                    max = values[1]
                    if min is not None:
                        if min < minDelay:
                            minDelay = min
                    if max is not None:
                        if max > maxDelay:
                            maxDelay = max
    f.write("Min delay: " + str(minDelay) + "\n")
    f.write("Max delay: " + str(maxDelay) + "\n")


    # apri una nuova connessione al database per il file General-#0.sca e tabella scalar
    conn = sqlite3.connect(database_path)
    conn.row_factory = sqlite3.Row
    cur = conn.cursor()
    cur.execute("SELECT * FROM scalar")
    rows = cur.fetchall()

    # salva nello stesso file .txt il jain index di ogni media di ogni nodo per lo statName sdrBundleStored:timeavg
    f = open("data.txt", "a")
    mean_list = []
    for i in range(0, len(rows)):
        moduleName = rows[i]["moduleName"]
        digit_string = ''.join(ele for ele in moduleName if ele.isdigit())
        nodeNum = int(digit_string) if digit_string else 0
        #escludi nodo 21 se i nodi sono 21 e 51 se i nodi sono 51
        if (int(nodesNumber) == 21 and nodeNum != 21) or (int(nodesNumber) == 51 and nodeNum != 51) or int(nodesNumber) == 20 or int(nodesNumber) == 50:
            scalarName = rows[i]["scalarName"]
            if scalarName == "sdrBytesStored:timeavg":
                # salva in una lista tutte le medie
                mean = rows[i]["scalarValue"]
                if mean is not None:
                    mean_list.append(mean)

    f.write("Bytes Stored Jain's Fairness Index: " + str(jain_index(mean_list)))
    f.close()


if __name__ == "__main__":
    main()
