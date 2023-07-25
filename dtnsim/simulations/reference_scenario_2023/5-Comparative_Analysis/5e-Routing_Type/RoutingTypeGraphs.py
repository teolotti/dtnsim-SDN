
#This python script is made to convert .sca files (results generated by dtnsim) into .png files, showing graphics on the various metrics that we have.

#This script in particular generates graph showing the influence of the routing type on the various metrics

import sqlite3
import matplotlib.pyplot as plt
import os
from matplotlib.backends.backend_pdf import PdfPages


#The input .sca files are supposed to be in /results/
INPUT_PATH = os.getcwd() + "/results/"

# Reference value. You must change it here if you changed them on dtnsim
ROUTING_REF="cgrModelRev17"
SIZE_REF="100"
CONSTELLATION_REF="Delta"

# Value that change to have the comparison
ROUTINGTYPE=["allPaths-firstEnding","allPaths-firstDepleted","oneBestPath"]

# Used to display the bar with min, max, mean, stddev
METRIC_APP=[("appBundleReceivedDelay:histogram"," Delay of received bundles"),("appBundleReceivedHops:histogram","Hops of received bundles")]

# Used to display the raw scalar as given
METRICS = [
            ("routeCgrDijkstraCalls:sum", "calls to CgrDijkstra"," "),
            ("routeCgrDijkstraLoops:sum", "Cgr Dijkstra loops"," "),
            ("routeCgrRouteTableEntriesCreated:max", "CGR table entries max"," "),
            ("routeCgrRouteTableEntriesCreated:sum", "CGR table entries sum"," "),

            ("sdrBundleStored:timeavg","Bundles stored time avg"," "),
            ("sdrBundleStored:max","Bundles stored max"," "),
        
            ("dtnBundleSentToCom:count", "bundles sent to Com"," "),
            ("dtnBundleSentToApp:count", "bundles sent to App"," "),
            ("dtnBundleReRouted:count", "blundles re-routed"," "),

            
]
'''
Function that return the mean of a list
'''
def mean(f):
    s=0
    compt=0
    for i in f:
        if (i!=None and i!=0):
            compt=compt+1
            s+=i
    if (compt==0):
        compt=1
    m=s/compt
    return m

# Initialisation
k=1


'''
Function that, for each metrics, creates a graph.png displaying the value of the current metric for each node for each constellation
'''

#Just the different font for the curves
FONT=["c-x","m-o","g-1","r-v","y-d"]

'''
part that creates a graph displaying the packet delivery ratio
'''
X=[]
g=[]
for routing in ROUTINGTYPE:
    #f is the list that is gonna store the values (y-axis)
    f=[]

    #Browse the metrics to get a sum value for each one
    #First open the corresponding database (.sca file)
    db=sqlite3.connect("%sGeneral-Routing=%s,Constellation=contactPlan#2f%s.txt,RoutingType=routeListType#3a%s,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,Size=%s-#0.sca"% (INPUT_PATH,ROUTING_REF,CONSTELLATION_REF,routing,SIZE_REF))
    db.row_factory = sqlite3.Row
    cursor=db.cursor()

    #Select with SQL the sum of all the values corresponding to our current metric
    cursor.execute("SELECT SUM(scalarValue) AS result FROM scalar WHERE scalarName='appBundleSent:count'")
    rows0 = cursor.fetchall()
    #Add the sum to the list
    f.append(rows0[0]["result"])

    #Select with SQL the sum of all the values corresponding to our current metric
    cursor.execute("SELECT SUM(scalarValue) AS result FROM scalar WHERE scalarName='appBundleReceived:count'")
    rows0 = cursor.fetchall()
    #Add the sum to the list
    f.append(rows0[0]["result"])

    #Ratio of both to have the PDR
    g.append(f[1]/f[0])
    X.append(routing)

#Display the results with a bar graphic
plt.figure(k)
plt.bar(X,g,width=1,edgecolor="white",color="#8402be")
plt.title(label="Packet Delivery Ratio (PDR)")
k=k+1

'''
The part that generates the graphes with infos such as min and max for concerned metrics
'''
for metric in METRIC_APP:
    X=[]
    means=[]
    stddev=[]
    mins=[]
    maxs=[]
    for routing in ROUTINGTYPE :
        #First open the corresponding database (.sca file)
        db=sqlite3.connect("%sGeneral-Routing=%s,Constellation=contactPlan#2f%s.txt,RoutingType=routeListType#3a%s,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,Size=%s-#0.sca"% (INPUT_PATH,ROUTING_REF,CONSTELLATION_REF,routing,SIZE_REF))
        db.row_factory = sqlite3.Row
        cursor=db.cursor()
    
        meanstmp=[]
        stddevtmp=[]
        minstmp=[]
        maxstmp=[]
        cursor.execute("SELECT statMean,statStddev,statMin,statMax FROM statistic WHERE statName='%s'"%(metric[0]))
        compt=0
        for r in cursor.fetchall():
            if (r[0])!=None:
                meanstmp.append(r[0])
                stddevtmp.append(r[1])
                minstmp.append(r[2])
                maxstmp.append(r[3])
            compt=compt+1
        X.append(routing)
        mins.append(mean(minstmp))
        maxs.append(mean(maxstmp))
        means.append(mean(meanstmp))
        stddev.append(mean(stddevtmp))
    
    plt.figure(k)
    plt.plot(X,means) 
    plt.xlabel("routing type")
    if "Delay" in metric[0]:
        plt.ylabel("time (s)")
    
    plt.errorbar(X, means, [[b_elt - a_elt for a_elt, b_elt in zip(mins, means)],[b_elt - a_elt for a_elt, b_elt in zip(means, maxs)]],
                fmt='.k', ecolor='gray', lw=2)
    plt.errorbar(X, means, stddev, fmt='dk', ecolor='green',lw=5)
    #plt.xlim(111,124)
    plt.title("%s"%(metric[1]))
    k=k+1

#Browse the metrics to create a graph for each one 
for metric in METRICS : 

    j=0
    plt.figure(k)

    #Browse the constellations to have a curve for each one 
    for routing in ROUTINGTYPE:

        #f is used to store metric values for each constellation (y-axis)
        f=[[],[],[],[],[]]

        #Open the corresponding database (.sca file)
        db=sqlite3.connect("%sGeneral-Routing=%s,Constellation=contactPlan#2f%s.txt,RoutingType=routeListType#3a%s,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,Size=%s-#0.sca"% (INPUT_PATH,ROUTING_REF,CONSTELLATION_REF,routing,SIZE_REF))
        db.row_factory = sqlite3.Row
        cursor=db.cursor()

        #Select with SQL the values corresponding to our current metric
        cursor.execute("SELECT scalarValue AS result FROM scalar WHERE scalarName='%s'" % (metric[0]))
        rows0 = cursor.fetchall()

        #Add the values to the list
        for i in range(len(rows0)):
            f[j].append(rows0[i]["result"])  

        #Create a list correponding to nodes number (x-axis)      
        X=[i for i in range(len(rows0))]
        X.append(123)
        f[j].append(mean(f[j]))

        #Display the curve corresponding to the current constellation on the graph
        plt.plot(X,f[j],"%s"%(FONT[j]),label="Constellation=%s"%(routing))
        j=j+1

    #Once each constellation is done, we can save the figure, and close it to go to the next metric
    plt.title(label="%s"%(metric[1]))
    plt.legend()
    plt.grid()
    plt.xlabel("node Id")
    plt.ylabel("%s"%(metric[2]))
    k=k+1

'''
Function that, for each metric, creates a graph showing the average value of this metric for each constellation
'''

#Browse the metrics to create a graph for each one 
for metric in METRICS : 
    #X is gonna store the constellations (x-axis)
    X=[]

    #f is gonna store the mean values (y-axis)
    f=[]

    #Browse the constellations to have a mean value for each one
    for routing in ROUTINGTYPE:
    
        X.append(routing)

        #g is an intermediate list for storing values before averaging them  
        g=[]

        #Open the corresponding database (.sca file)
        db=sqlite3.connect("%sGeneral-Routing=%s,Constellation=contactPlan#2f%s.txt,RoutingType=routeListType#3a%s,volumeAware#3aallContacts,extensionBlock#3aon,contactPlan#3aglobal,Size=%s-#0.sca"% (INPUT_PATH,ROUTING_REF,CONSTELLATION_REF,routing,SIZE_REF))
        db.row_factory = sqlite3.Row
        cursor=db.cursor()

        #Select with SQL the values corresponding to our current metric
        cursor.execute("SELECT scalarValue AS result FROM scalar WHERE scalarName='%s'" % (metric[0]))
        rows0 = cursor.fetchall()

        #Add the values to the intermediate list
        for i in range(len(rows0)):
            g.append(rows0[i]["result"])

        #Add the mean of the intermediate list to the final list
        f.append(mean(g))

    #Display the result, save the figure and close it
    plt.figure(k)
    plt.bar(X,f,width=0.8,color="#8402be")
    plt.title(label="%s"%(metric[1]))
    plt.xlabel("Routing Type")
    plt.ylabel("%s"%(metric[2]))
    k=k+1

pdf=PdfPages("figures.pdf")
fig_num=plt.get_fignums()
figs=[plt.figure(n) for n in fig_num]
for fig in figs:
    fig.savefig(pdf,format='pdf')
pdf.close()

