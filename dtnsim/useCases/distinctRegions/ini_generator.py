import json
import math

config = open("lunar_ring_road_example/configuration.json")
regions = json.load(config)["regions"] # list of all region configurations

regionNodeNumber, passagewayNodeNumber, backboneNodeNumber = 0, 0, 0

for region in regions:
    regionNodeNumber += len(region["regionNodes"])
    passagewayNodeNumber += len(region["passagewayNodes"])
    backboneNodeNumber += len(region["backboneNodes"])

def getPositions(regions):

    coordinates = {}

    middle = (500, 400)

    numberOfRegions = len(regions)

    posRadius = 300
    posAngle = (2 * 3.1415) / numberOfRegions

    for i, region in enumerate(regions):

        center = (middle[0] + (posRadius * math.cos(i * posAngle)), middle[1] + (posRadius * math.sin(i * posAngle)))

        totalNodes = len(region["regionNodes"]) + len(region["passagewayNodes"]) + len(region["backboneNodes"])
        radius = 100
        angle = (2 * 3.1415) / totalNodes

        pos = []
        for j in range(0, totalNodes):
            pos.append((center[0] + (radius * math.cos(j * angle)), center[1] + (radius * math.sin(j * angle))))

        coordinates[region["name"]] = pos

    return coordinates




# Erase current contents
open("lunar_ring_road_example/regions.ini", "w").close()

# Write new contents
with open("lunar_ring_road_example/regions.ini", "a") as sf:

    sf.write("[General]\n")
    sf.write("network = src.distinctRegions.RegionsNetwork\n")

    sf.write("RegionsNetwork.central.backboneContactPlan = \"backboneContactPlan.txt\"\n")
    sf.write("RegionsNetwork.central.regionContactPlan = \"regionContactPlan.txt\"\n")

    sf.write("\n")

    sf.write("RegionsNetwork.regionNodeNumber = {}\n".format(regionNodeNumber))
    regionNodeIndices = range(0, regionNodeNumber)

    sf.write("RegionsNetwork.passagewayNodeNumber = {}\n".format(passagewayNodeNumber))
    passagewayNodeIndices = range(0, passagewayNodeNumber)

    sf.write("RegionsNetwork.backboneNodeNumber = {}\n".format(backboneNodeNumber))
    backboneNodeIndices = range(0, backboneNodeNumber)

    sf.write("\n")

    coordinates = getPositions(regions)
    rStart, pwStart, bbStart = 0, 0, 0
    for region in regions:

        regionName = region["name"]
        positions = coordinates[regionName]
        counter = 0

        localRegionNodeNumber = len(region["regionNodes"])
        for i, regionNode in zip(regionNodeIndices[rStart:rStart+localRegionNodeNumber], region["regionNodes"]):
            sf.write("RegionsNetwork.regionNode[{}].eid = {}\n".format(i, regionNode["eid"]))
            sf.write("RegionsNetwork.regionNode[{}].region = \"{}\"\n".format(i, regionName))
            sf.write("RegionsNetwork.regionNode[{}].text = \"Region: {} EID: {}\"\n".format(i, regionName, regionNode["eid"]))
            sf.write("RegionsNetwork.regionNode[{}].posX = {}\n".format(i, positions[counter][0]))
            sf.write("RegionsNetwork.regionNode[{}].posY = {}\n".format(i, positions[counter][1]))
            counter += 1

            if ("capacity" in regionNode):

                sf.write("\n")
                sf.write("RegionsNetwork.regionNode[{}].dtn.sdrCapacity = {}\n".format(i, regionNode["capacity"]))


            if ("traffic" in regionNode):

                sf.write("\n")
                numberOfFlows = len(regionNode["traffic"])

                sf.write("RegionsNetwork.regionNode[{}].app.enable = true\n".format(i))
                sf.write("RegionsNetwork.regionNode[{}].app.numberOfFlows = {}\n".format(i, numberOfFlows))

                destinationEids, distributions, numbersOfBundles, startTimes, bundleSizes, ttls = "", "", "", "","", ""
                for j in range(0, numberOfFlows):

                    destinationEids += regionNode["traffic"][j]["destination"] + ","
                    distributions += regionNode["traffic"][j]["distribution"] + ","
                    numbersOfBundles += str(regionNode["traffic"][j]["bundles"]) + ","
                    startTimes += str(regionNode["traffic"][j]["start"]) + ","
                    bundleSizes += str(regionNode["traffic"][j]["size"]) + ","
                    ttls += str(regionNode["traffic"][j]["ttl"]) + ","
                
                sf.write("RegionsNetwork.regionNode[{}].app.destinationEids = \"{}\"\n".format(i, destinationEids))
                sf.write("RegionsNetwork.regionNode[{}].app.distributions = \"{}\"\n".format(i, distributions))
                sf.write("RegionsNetwork.regionNode[{}].app.numbersOfBundles = \"{}\"\n".format(i, numbersOfBundles))
                sf.write("RegionsNetwork.regionNode[{}].app.startTimes = \"{}\"\n".format(i, startTimes))
                sf.write("RegionsNetwork.regionNode[{}].app.bundleSizes = \"{}\"\n".format(i, bundleSizes))
                sf.write("RegionsNetwork.regionNode[{}].app.ttls = \"{}\"\n".format(i, ttls))
                
                sf.write("\n")

        rStart += localRegionNodeNumber

        localPassagewayNodeNumber = len(region["passagewayNodes"])
        for i, pwNode in zip(passagewayNodeIndices[pwStart:pwStart+localPassagewayNodeNumber], region["passagewayNodes"]):
            sf.write("RegionsNetwork.passagewayNode[{}].eid = {}\n".format(i, pwNode["eid"]))
            sf.write("RegionsNetwork.passagewayNode[{}].region = \"{}\"\n".format(i, regionName))
            sf.write("RegionsNetwork.passagewayNode[{}].text = \"Region: {} EID: {}\"\n".format(i, regionName, pwNode["eid"]))
            sf.write("RegionsNetwork.passagewayNode[{}].posX = {}\n".format(i, positions[counter][0]))
            sf.write("RegionsNetwork.passagewayNode[{}].posY = {}\n".format(i, positions[counter][1]))
            counter += 1
        pwStart += localPassagewayNodeNumber

        localBackboneNodeNumber = len(region["backboneNodes"])
        for i, bbNode in zip(backboneNodeIndices[bbStart:bbStart+localBackboneNodeNumber], region["backboneNodes"]):
            sf.write("RegionsNetwork.backboneNode[{}].eid = {}\n".format(i, bbNode["eid"]))
            sf.write("RegionsNetwork.backboneNode[{}].region = \"{}\"\n".format(i, regionName))
            sf.write("RegionsNetwork.backboneNode[{}].text = \"Region: {} EID: {}\"\n".format(i, regionName, bbNode["eid"]))
            sf.write("RegionsNetwork.backboneNode[{}].posX = {}\n".format(i, positions[counter][0]))
            sf.write("RegionsNetwork.backboneNode[{}].posY = {}\n".format(i, positions[counter][1]))
            counter += 1
        bbStart += localBackboneNodeNumber

        sf.write("\n")


# TODO
"""
outputvectormanager-class="omnetpp::envir::SqliteOutputVectorManager"
outputscalarmanager-class="omnetpp::envir::SqliteOutputScalarManager"

RegionsNetwork.regionNode[*].**.result-recording-modes = -vector
RegionsNetwork.passagewayNode[*].**.result-recording-modes = -vector
RegionsNetwork.backboneNode[*].**.result-recording-modes = -vector
"""