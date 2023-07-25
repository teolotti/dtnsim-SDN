#This python script is made to convert .sca files (results generated by dtnsim) into .png files, showing graphics on the various metrics that we have.

#This script in particular generates graph showing the influence of both the bundle size and the contellation on the various metrics

import sqlite3
import matplotlib.pyplot as plt
import os
from matplotlib.backends.backend_pdf import PdfPages



#The input .sca files are supposed to be in /results/
INPUT_PATH = os.getcwd() + "/results/"

# Reference value. You must change it here if you changed them on dtnsim
ROUTING_REF="cgrModelRev17"
SIZE_REF="100"

# Value that change to have the comparison
CONSTELLATION=["Delta","Star"]
INTERVAL=["3600","360","36"]
NUMBER=["10","100","1000"]


# A list of all metrics available. You can remove as many as you want
METRICS = [
            ("appBundleSent:count","bundles sent"," "),
            ("appBundleReceived:count", "bundles received count"," "),
            ("appBundleReceivedDelay:mean", "bundles received delay mean","t(s)"),
            ("appBundleReceivedDelay:max", "bundles received delay max","t(s)"),
            ("appBundleReceivedDelay:min", "bundles received delay min","t(s)"),
            ("appBundleReceivedHops:mean", " Average hops of received bundles"," "),
            ("appBundleReceivedHops:max", " max hops of received bundles"," "),
            ("appBundleReceivedHops:min", " min hops of received bundles"," "),

            ("routeCgrDijkstraCalls:sum", "calls to CgrDijkstra"," "),
            ("routeCgrDijkstraLoops:sum", "Cgr Dijkstra loops"," "),
            ("routeCgrRouteTableEntriesCreated:max", "CGR table entries max"," "),
            ("routeCgrRouteTableEntriesCreated:sum", "CGR table entries sum"," "),

            ("sdrBundleStored:timeavg","Bundles stored time avg"," "),
            ("sdrBundleStored:max","Bundles stored max"," "),
        
            ("dtnBundleSentToCom:count", "bundles sent to Com"," "),
            ("dtnBundleSentToApp:count", "bundles sends to app"," "),
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


'''
Function that, for each metric, creates a graph showing the average value of this metric for each size
'''
FONT=["c-x","m-o","g-1","r-v","y-d","b-2"]

fig=0
#Browse the metrics to create a graph for each one 
for metric in METRICS : 
    plt.figure(fig)
    j=0
    for constellation in CONSTELLATION:
        #f is gonna store the mean values (y-axis)
        f=[]
        #X is gonna store the sizes (x-axis)
        X=[]
    #Browse the sizes to have a mean value for each one
        for k in range(len(INTERVAL)):
    
            X.append(INTERVAL[k])

            #g is an intermediate list for storing values before averaging them  
            g=[]
            
            #Open the corresponding database (.sca file)
            db=sqlite3.connect("%sGeneral-Routing=%s,Constellation=contactPlan#2f%s.txt,Size=%s,Interval=%s,Number=%s-#0.sca"% (INPUT_PATH,ROUTING_REF,constellation,SIZE_REF,INTERVAL[k],NUMBER[k]))
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
        
        plt.plot(X,f,"%s"%(FONT[j]),label="Constellation=%s"%(constellation))
        j=j+1
    #Display the result, save the figure and close it
    plt.title(label="%s"%(metric[1]))
    plt.legend()
    plt.xlabel("Interval")
    plt.ylabel("%s"%(metric[2]))
    fig=fig+1

pdf=PdfPages("Rate-Constelaltion.pdf")
fig_num=plt.get_fignums()
figs=[plt.figure(n) for n in fig_num]
for fig in figs:
    fig.savefig(pdf,format='pdf')
pdf.close()





