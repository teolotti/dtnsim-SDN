import pandas as pd
def countContacts(fileName):

    data = {"B": 0, "C": 0, "D": 0, "E": 0}
    cp = pd.read_csv(fileName)
    for idx, row in cp.iterrows():
        region = row['Region']
        data[region]+=1
    
    for region in data:
        print("Region", region, "has", data[region], "contacts")


for type in ["a", "b"]:
    print("TYPE:", type)
    for day in ["1", "2", "3", "4"]:
        print("DAY:", day)
        fileName = '{}/{}/scenario2_{}_{}_adapted.csv'.format(type, day, type, day)
        countContacts(fileName)
