# Parses a json contact plan from the ring road paper into a region contact plan (for now only one region)
# Also creates a configuration file with all the nodes part of the CP

from os.path import exists
import json

# TODO: for now only one region!

# Create config file
config = open("configuration.json", "w")

# Create contact plan file
contactPlan = open("regionContactPlan.txt", "w")
contactPlan.write("A\n")


contacts = json.load(open("scenario-1.json"))
highestEid = 0
for contact in contacts:

    snd = contact["node1"]
    sndEid, sndName = snd.rsplit()
    sndEid = int(sndEid[1:-1])

    if (sndEid > highestEid):
        highestEid = sndEid

    rcv = contact["node2"]
    rcvEid, rcvName = rcv.rsplit()
    rcvEid = int(rcvEid[1:-1])

    if (rcvEid > highestEid):
        highestEid = rcvEid

    start = contact["start"]
    end = contact["end"]

    contactPlan.write("a contact " + str(start) + " " + str(end) + " " + str(sndEid) + " " + str(rcvEid) + " 1000\n")


# Fill config file
configSkeleton = {
    "regions" : [{
        "name" : "A",
        "regionNodes" : [],
        "passagewayNodes" : [],
        "backboneNodes" : [],
        "routing" : "cgr"
    }]
}
nodes = configSkeleton["regions"][0]["regionNodes"]
for node in range(1, highestEid+1):
    nodes.append({"eid": node})
config.write(json.dumps(configSkeleton))