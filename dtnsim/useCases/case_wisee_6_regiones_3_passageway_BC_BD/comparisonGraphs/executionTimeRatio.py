import matplotlib.pyplot as plt
import ast
import os

#OUTPUT
FILE_OUTPUT_PATH = os.getcwd() + "/results/executionTimeRatio.pdf"

#INPUT. Complete path will be computed as DIRECTORY/FILE_PATHS_C1[i]
DIRECTORY = os.getcwd() + "/.."
FILE_PATHS_C1 = [
"case1/results/METRIC=routeExecutionTimeUs:sum.txt",
"case2/results/METRIC=routeExecutionTimeUs:sum.txt",
]

#Calculate complete path
COMPLETE_PATHS_C1 = list(map(lambda x: DIRECTORY + "/" + x, FILE_PATHS_C1))

#Names of plotted functions

NAME_CURVAS_1 = ["$(IRR + CGR) / CGR$"]

#Color of ploted functions
COLOR_CURVAS_1 = ['green']

#Style of plotted function
STYLE_CURVAS_1 = ['--s']

X_LABEL = "Bundles Sent"
Y_LABEL = "Execution Time Ratio"

def main():
    c1s = [getListFromFile(fname) for fname in COMPLETE_PATHS_C1]

    c1=c1s[0]
    c2=c1s[1]
    c3=[]
    for i in zip(c1,c2):
        c3.append((i[0][0],i[1][1] / i[0][1]))


    plt.plot([x[0] for x in c3], [y[1] for y in c3], STYLE_CURVAS_1[0], color=COLOR_CURVAS_1[0], label=NAME_CURVAS_1[0],linewidth=1.0)

    #plt.xlim([0.2,0.9])
    plt.ylim([0.0,1.00])
    plt.xlabel(X_LABEL, fontsize=16)
    plt.ylabel(Y_LABEL, fontsize=16)
    plt.grid(color='gray', linestyle='dashed')
    ax = plt.gca()
    ax.get_yaxis().get_major_formatter().set_useOffset(False)
    ax.get_yaxis().get_major_formatter().set_scientific(False)
    plt.tight_layout()
    plt.legend()
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
