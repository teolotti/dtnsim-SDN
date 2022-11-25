import matplotlib.pyplot as plt
import ast
import os
from matplotlib import ticker

#OUTPUT
FILE_OUTPUT_PATH = os.getcwd() + "/results/executionTimeRatio.pdf"

#INPUT. Complete path will be computed as DIRECTORY/FILE_PATHS_C1[i]
DIRECTORY = os.getcwd() + "/.."
FILE_PATHS_C1 = [
"case_wisee_4_regiones_1_passageway_BC_BD/case0/results/METRIC=routeExecutionTimeUs:sum.txt",
"case_wisee_4_regiones_1_passageway_BC_BD/case1/results/METRIC=routeExecutionTimeUs:sum.txt",
"case_wisee_4_regiones_1_passageway_BC_BD/case2/results/METRIC=routeExecutionTimeUs:sum.txt",
"case_wisee_6_regiones_1_passageway_BC_BD/case1/results/METRIC=routeExecutionTimeUs:sum.txt",
"case_wisee_6_regiones_1_passageway_BC_BD/case2/results/METRIC=routeExecutionTimeUs:sum.txt",
]

#Calculate complete path
COMPLETE_PATHS_C1 = list(map(lambda x: DIRECTORY + "/" + x, FILE_PATHS_C1))

#Names of plotted functions

NAME_CURVAS_1 = ['1 Region, GFK, GRK, LRK', '4 Regions, GRK', '4 Regions, LRK', '6 Regions, GRK', '6 Regions, LRK']

#Color of ploted functions
COLOR_CURVAS_1 = ['black','blue','green','blue','green']

#Style of plotted function
STYLE_CURVAS_1 = ['-D','-o','-s','-x','-|']

X_LABEL = "Bundles Sent"
Y_LABEL = "Routing Effort Ratio"

def main():
    c1s = [getListFromFile(fname) for fname in COMPLETE_PATHS_C1]

    c1=c1s[0]
    c2=c1s[1]
    c3=c1s[2]
    c4=c1s[3]
    c5=c1s[4]
    
    c6=[]
    c7=[]
    c8=[]
    c9=[]
    c10=[]

    lines = []
    for i in zip(c1,c2,c3,c4,c5):
        c6.append((i[0][0],i[0][1] / i[0][1]))
        c7.append((i[0][0],i[1][1] / i[0][1]))
        c8.append((i[0][0],i[2][1] / i[0][1]))
        c9.append((i[0][0],i[3][1] / i[0][1]))
        c10.append((i[0][0],i[4][1] / i[0][1]))

    line_up, = plt.semilogy([x[0] for x in c6], [y[1] for y in c6], STYLE_CURVAS_1[0], color=COLOR_CURVAS_1[0], label=NAME_CURVAS_1[0],linewidth=1.0)
    lines.append(line_up)

    line_up, = plt.semilogy([x[0] for x in c7], [y[1] for y in c7], STYLE_CURVAS_1[1], color=COLOR_CURVAS_1[1], label=NAME_CURVAS_1[1],linewidth=1.0)
    lines.append(line_up)

    line_up, = plt.semilogy([x[0] for x in c8], [y[1] for y in c8], STYLE_CURVAS_1[2], color=COLOR_CURVAS_1[2], label=NAME_CURVAS_1[2],linewidth=1.0)
    lines.append(line_up)
    
    line_up, = plt.semilogy([x[0] for x in c9], [y[1] for y in c9], STYLE_CURVAS_1[3], color=COLOR_CURVAS_1[3], label=NAME_CURVAS_1[3],linewidth=1.0)
    lines.append(line_up)

    line_up, = plt.semilogy([x[0] for x in c10], [y[1] for y in c10], STYLE_CURVAS_1[4], color=COLOR_CURVAS_1[4], label=NAME_CURVAS_1[4],linewidth=1.0)
    lines.append(line_up)
        
        
    plt.legend(fontsize=8)

    #plt.xlim([0,1700])
    #plt.ylim([0.4,1.05])
    plt.xlabel(X_LABEL, fontsize=16)
    plt.ylabel(Y_LABEL, fontsize=16)
    plt.grid(color='gray', linestyle='dashed')
    
    ax = plt.gca()
    #ax.get_yaxis().get_major_formatter().set_useOffset(False)
    #ax.get_yaxis().get_major_formatter().set_scientific(False)
    #plt.tight_layout()
    #fig = plt.gcf()
    #fig.set_size_inches(10, 10)
    plt.savefig(FILE_OUTPUT_PATH)
    plt.clf()
    plt.cla()
    plt.close()
    #plt.show()

''''
Get a list from file reading first line only
'''
def getListFromFile(path):
    with open(path) as f:
        lines = f.readlines()

    return ast.literal_eval(lines[0])

main()
