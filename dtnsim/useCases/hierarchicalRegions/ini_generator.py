import json
import math

config = open("configuration.json")
regions = json.load(config)["regions"] # list of all region configurations

regionNodeNumber, passagewayNodeNumber = 0, 0

for region in regions:
    regionNodeNumber += len(region["regionNodes"])
    passagewayNodeNumber += len(region["passagewayNodes"])

def getPositions(regions):

    coordinates = {}

    middle = (500, 400)

    numberOfRegions = len(regions)

    posRadius = 300
    posAngle = (2 * 3.1415) / numberOfRegions

    for i, region in enumerate(regions):

        center = (middle[0] + (posRadius * math.cos(i * posAngle)), middle[1] + (posRadius * math.sin(i * posAngle)))

        totalNodes = len(region["regionNodes"]) + len(region["passagewayNodes"])
        radius = 100
        angle = (2 * 3.1415) / totalNodes

        pos = []
        for j in range(0, totalNodes):
            pos.append((center[0] + (radius * math.cos(j * angle)), center[1] + (radius * math.sin(j * angle))))

        coordinates[region["name"]] = pos

    return coordinates




# Erase current contents
open("omnetpp.ini", "w").close()

# Write new contents
with open("omnetpp.ini", "a") as sf:

    sf.write("[General]\n")
    sf.write("network = src.hierarchicalRegions.HierarchicalRegionsNetwork\n")

    sf.write("HierarchicalRegionsNetwork.central.contactPlan = \"contactPlan.txt\"\n")

    sf.write("\n")

    sf.write("HierarchicalRegionsNetwork.regionNodeNumber = {}\n".format(regionNodeNumber))
    regionNodeIndices = range(0, regionNodeNumber)

    sf.write("HierarchicalRegionsNetwork.passagewayNodeNumber = {}\n".format(passagewayNodeNumber))
    passagewayNodeIndices = range(0, passagewayNodeNumber)

    sf.write("\n")

    coordinates = getPositions(regions)
    rStart, pwStart = 0, 0
    for region in regions:

        regionName = region["name"]
        positions = coordinates[regionName]
        counter = 0

        localRegionNodeNumber = len(region["regionNodes"])
        for i, regionNode in zip(regionNodeIndices[rStart:rStart+localRegionNodeNumber], region["regionNodes"]):
            sf.write("HierarchicalRegionsNetwork.regionNode[{}].eid = {}\n".format(i, regionNode["eid"]))
            sf.write("HierarchicalRegionsNetwork.regionNode[{}].homeRegion = \"{}\"\n".format(i, regionName))
            sf.write("HierarchicalRegionsNetwork.regionNode[{}].text = \"Region: {} EID: {}\"\n".format(i, regionName, regionNode["eid"]))
            sf.write("HierarchicalRegionsNetwork.regionNode[{}].posX = {}\n".format(i, positions[counter][0]))
            sf.write("HierarchicalRegionsNetwork.regionNode[{}].posY = {}\n".format(i, positions[counter][1]))
            counter += 1

            if ("capacity" in regionNode):

                sf.write("\n")
                sf.write("HierarchicalRegionsNetwork.regionNode[{}].dtn.sdrCapacity = {}\n".format(i, regionNode["capacity"]))


            if ("traffic" in regionNode):

                sf.write("\n")
                numberOfFlows = len(regionNode["traffic"])

                sf.write("HierarchicalRegionsNetwork.regionNode[{}].app.enable = true\n".format(i))
                sf.write("HierarchicalRegionsNetwork.regionNode[{}].app.numberOfFlows = {}\n".format(i, numberOfFlows))

                destinationEids, distributions, numbersOfBundles, startTimes, bundleSizes, ttls = "", "", "", "","", ""
                for j in range(0, numberOfFlows):

                    destinationEids += regionNode["traffic"][j]["destination"] + ","
                    distributions += regionNode["traffic"][j]["distribution"] + ","
                    numbersOfBundles += str(regionNode["traffic"][j]["bundles"]) + ","
                    startTimes += str(regionNode["traffic"][j]["start"]) + ","
                    bundleSizes += str(regionNode["traffic"][j]["size"]) + ","
                    ttls += str(regionNode["traffic"][j]["ttl"]) + ","
                
                sf.write("HierarchicalRegionsNetwork.regionNode[{}].app.destinationEids = \"{}\"\n".format(i, destinationEids))
                sf.write("HierarchicalRegionsNetwork.regionNode[{}].app.distributions = \"{}\"\n".format(i, distributions))
                sf.write("HierarchicalRegionsNetwork.regionNode[{}].app.numbersOfBundles = \"{}\"\n".format(i, numbersOfBundles))
                sf.write("HierarchicalRegionsNetwork.regionNode[{}].app.startTimes = \"{}\"\n".format(i, startTimes))
                sf.write("HierarchicalRegionsNetwork.regionNode[{}].app.bundleSizes = \"{}\"\n".format(i, bundleSizes))
                sf.write("HierarchicalRegionsNetwork.regionNode[{}].app.ttls = \"{}\"\n".format(i, ttls))
                
                sf.write("\n")

        rStart += localRegionNodeNumber

        localPassagewayNodeNumber = len(region["passagewayNodes"])
        for i, pwNode in zip(passagewayNodeIndices[pwStart:pwStart+localPassagewayNodeNumber], region["passagewayNodes"]):
            sf.write("HierarchicalRegionsNetwork.passagewayNode[{}].eid = {}\n".format(i, pwNode["eid"]))
            sf.write("HierarchicalRegionsNetwork.passagewayNode[{}].homeRegion = \"{}\"\n".format(i, pwNode["home"]))
            sf.write("HierarchicalRegionsNetwork.passagewayNode[{}].outerRegion = \"{}\"\n".format(i, pwNode["outer"]))
            sf.write("HierarchicalRegionsNetwork.passagewayNode[{}].text = \"Region: {} EID: {}\"\n".format(i, regionName, pwNode["eid"]))
            sf.write("HierarchicalRegionsNetwork.passagewayNode[{}].posX = {}\n".format(i, positions[counter][0]))
            sf.write("HierarchicalRegionsNetwork.passagewayNode[{}].posY = {}\n".format(i, positions[counter][1]))
            counter += 1
        pwStart += localPassagewayNodeNumber

        sf.write("\n")


# TODO
"""
outputvectormanager-class="omnetpp::envir::SqliteOutputVectorManager"
outputscalarmanager-class="omnetpp::envir::SqliteOutputScalarManager"

HierarchicalRegionsNetwork.regionNode[*].**.result-recording-modes = -vector
HierarchicalRegionsNetwork.passagewayNode[*].**.result-recording-modes = -vector
HierarchicalRegionsNetwork.backboneNode[*].**.result-recording-modes = -vector
"""