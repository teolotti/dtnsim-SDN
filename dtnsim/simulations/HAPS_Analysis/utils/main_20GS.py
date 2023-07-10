name_to_id = {}

with open('contactPlan/contact_plan_name_to_id_mapping.txt', 'r') as file:
    for line in file:
        key, value = line.strip().split()
        name_to_id[key] = value

GST_to_keep = ['GST_lat-31_lon-64',
               'GST_lat025_lon055',
               'GST_lat064_lon-147',
               'GST_lat-25_lon028',
               'GST_lat021_lon-157',
               'GST_lat068_lon-133',
               'GST_lat034_lon-118',
               'GST_lat-20_lon057',
               'GST_lat064_lon-51',
               'GST_lat038_lon-04',
               'GST_lat-53_lon-70',
               'GST_lat001_lon103',
               'GST_lat078_lon014',
               'GST_lat035_lon139',
               'GST_lat069_lon018',
               'GST_lat070_lon031',
               'GST_lat-46_lon168',
               'GST_lat037_lon-25',
               'GST_lat012_lon077',
               'GST_lat-72_lon002',
               ]
HAPS_to_keep = []
LEOS_to_keep = ['LEO_ran000_tan016']
NODE_LABELS_to_keep = ['MOC'] + GST_to_keep + HAPS_to_keep + LEOS_to_keep
NODE_IDS_to_keep = []
for e in NODE_LABELS_to_keep:
    NODE_IDS_to_keep.append(name_to_id[e])

input_file = 'contactPlan/contact_plan_1d_node-ids.txt'
output_file = 'contactPlan/contact_plan_1d_node-ids_1LEO_20GST.txt'

with open(input_file, 'r') as file:
    lines = file.readlines()

filtered_lines = []

for line in lines:
    columns = line.strip().split()

    if len(columns) >= 6 and columns[4] in NODE_IDS_to_keep and columns[5] in NODE_IDS_to_keep:
        columns[6] = '1'  # data rate to 1
        line2 = ' '.join(columns)
        filtered_lines.append(line2 + '\n')

with open(output_file, 'w') as file:
    file.writelines(filtered_lines)



