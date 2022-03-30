import json
import sys

def compare_two_results(simulationpath, lowerbound):
	if lowerbound:
		with open(simulationpath + '/no_opp/jsonResults.txt') as file1:
			dict1 = json.load(file1)
	else:
		with open(simulationpath + '/opp_known/jsonResults.txt') as file1:
			dict1 = json.load(file1)
	with open(simulationpath + '/opp_not_known/jsonResults.txt') as file2:
		dict2 = json.load(file2)

	bundleIds = dict1['bundleIds']
	receivedIds1 = dict1['receivedIds']
	receivedIds2 = dict2['receivedIds']
	receivedPercent1 = len(receivedIds1) / len(bundleIds)
	receivedPercent2 = len(receivedIds2) / len(bundleIds)
	differencePercent = receivedPercent2 - receivedPercent1
	receivedTimeDifference = dict()
	delivery_counts = dict()
	
	for id_ in receivedIds2:
		if id_ in receivedIds1:
			if (dict1['bundleDeliveryTimes'][str(id_)] == 0):
				dict1['bundleDeliveryTimes'][str(id_)] = 1
			if (dict2['bundleDeliveryTimes'][str(id_)] == 0):
				dict2['bundleDeliveryTimes'][str(id_)] = 1
			if (dict1['bundleDeliveryCounts'][str(id_)] == 0):
				dict1['bundleDeliveryCounts'][str(id_)] = 1
			if (dict2['bundleDeliveryCounts'][str(id_)] == 0):
				dict2['bundleDeliveryCounts'][str(id_)] = 1
			receivedTimeDifference[id_]= dict1['bundleDeliveryTimes'][str(id_)] / dict2['bundleDeliveryTimes'][str(id_)]
			delivery_counts[id_] = dict1['bundleDeliveryCounts'][str(id_)] / dict2['bundleDeliveryCounts'][str(id_)]
	
	time_difference_sum = 0	
	delivery_difference_sum = 0
	for id_ in receivedTimeDifference:
		time_difference_sum += receivedTimeDifference[id_]
	
	for id_ in delivery_counts:
		delivery_difference_sum += delivery_counts[id_]
	
	if (len(receivedTimeDifference) == 0):
		time_difference_avg = 0
	else:
		time_difference_avg = time_difference_sum / len(receivedTimeDifference)
		
	if (len(delivery_counts) == 0):
		delivery_difference_avg = 0
	else:
		delivery_difference_avg = delivery_difference_sum / len(delivery_counts)
	
	if dict1["RUCoPCalls"] == 0:
		rucop_difference = 0
		rucop_computation_time = 0
	else:
		rucop_difference = dict1["RUCoPCalls"] / dict2["RUCoPCalls"]
		rucop_computation_time = dict1["RUCoPComputationTime"] / dict2["RUCoPComputationTime"]
	
	if (dict1["cgrCalls"] == 0):
		cgr_difference = 0
		cgr_computation_time = 0
	else:
		cgr_difference = dict1["cgrCalls"] / dict2["cgrCalls"]
		cgr_computation_time = dict1["cgrComputationTime"] - dict2["cgrComputationTime"]

	
	
	
	return (differencePercent, time_difference_avg, delivery_difference_avg, rucop_difference, 
	cgr_difference, rucop_computation_time, cgr_computation_time)
	
def compare_delivery_ratio(output, json, value1, value2):
	
	json["delivery_ratio"]["lower"] = round(value1 * 100, 2)
	if value1 < 0:
		lower_str = str(round(-value1 * 100, 2)) + "% lower"
	else:
		lower_str = str(round(value1 * 100, 2)) + "% higher"

	json["delivery_ratio"]["upper"] = round(value2 * 100, 2)
	if value2 < 0:
		upper_str = str(round(-value2 * 100, 2)) + "% lower"
	else:
		upper_str = str(round(value2 * 100, 2)) + "% higher"
	output.write("Compared to having a contact plan without any opportunistic contacts, the delivery ratio is " + lower_str + ", while it is " + upper_str + " compared to a contact plan where all contacts are scheduled already. \n")
	
	
	
def compare_delivery_times(output, json, value1, value2):
	if value1 >= 1:
		lower_str = str(round((1 - 1/value1) * 100, 2)) + "% less"
		json["delivery_times"]["lower"] = -round((1 - 1/value1) * 100, 2)
	elif value1 > 0:
		lower_str = str(round((1/value1 - 1) * 100, 2)) + "% higher"
		json["delivery_times"]["lower"] = round((1/value1 - 1) * 100, 2)
	else:
		lower_str = "0% faster"
		json["delivery_times"]["lower"] = 0
	if value2 >= 1:
		upper_str = str(round((1 - 1/value2) * 100, 2)) + "% less"
		json["delivery_times"]["upper"] = -round((1 - 1/value2) * 100, 2)
	elif value2 > 0:
		upper_str = str(round((1/value2 - 1) * 100, 2)) + "% higher"
		json["delivery_times"]["upper"] = round((1/value2 - 1) * 100, 2)
	else:
		upper_str = "0% faster"
		json["delivery_times"]["upper"] = 0
	
	output.write("Compared to having a contact plan without any opportunistic contacts, the delivery times were " + lower_str + ", while they were " + upper_str + " compared to a contact plan where all contacts are scheduled already. \n")
	
	
	
def compare_delivery_counts(output, json, value1, value2):
	if value1 >= 1:
		lower_str = str(round((1 - 1/value1) * 100, 2)) + "% less often"
		json["delivery_counts"]["lower"] = -round((1 - 1/value1) * 100, 2)
	elif value1 > 0:
		lower_str = str(round((1/value1 - 1) * 100, 2)) + "% more often"
		json["delivery_counts"]["lower"] = round((1/value1 - 1) * 100, 2)
	else:
		lower_str = "0 % more often"
		json["delivery_counts"]["lower"] = 0
		
	if value2 >= 1:
		upper_str = str(round((1 - 1/value2) * 100, 2)) + "% less often"
		json["delivery_counts"]["upper"] = -round((1 - 1/value2) * 100, 2)
	elif value2 > 0:
		upper_str = str(round((1/value2 - 1) * 100, 2)) + "% more often"
		json["delivery_counts"]["upper"] = round((1/value2 - 1) * 100, 2)
	else:
		upper_str = "0% more often"
		json["delivery_counts"]["upper"] = 0
		
	output.write("Compared to having a contact plan without any opportunistic contacts, the bundles were sent " + lower_str + ", while they were sent " + upper_str + " compared to a contact plan where all contacts are scheduled already. \n")
	
def compare_computation_metrics(output, json, algorithm, count1, count2, time1, time2):
	if count1 >= 1:
		lower_str_count = str(round((1 - (1/count1)) * 100, 2)) + "% less often"
		json[algorithm + "_count"]["lower"] = -round((1 - (1/count1)) * 100, 2)
	elif count1 > 0:
		lower_str_count = str(round((1/count1 - 1) * 100, 2)) + "% more often"
		json[algorithm + "_count"]["lower"] = round((1/count1 - 1) * 100, 2)
	else:
		lower_str_count = "0% more often"
		json[algorithm + "_count"]["lower"] = 0
		
	if count2 >= 1:
		upper_str_count = str(round((1 - (1/count2)) * 100, 2)) + "% less often"
		json[algorithm + "_count"]["upper"] = -round((1 - (1/count2)) * 100, 2)
	elif count2 > 0:
		upper_str_count = str(round((1/count2 - 1) * 100, 2)) + "% more often"
		json[algorithm + "_count"]["upper"] = round((1/count2 - 1) * 100, 2)
	else:
		upper_str_count = "0% more often"
		json[algorithm + "_count"]["upper"] = 0
	
	if time1 >= 1:
		lower_str_time = str(round((1 - 1/time1) * 100, 2)) + "% less time"
		json[algorithm + "_time"]["lower"] = -round((1 - 1/time1) * 100, 2)
	elif time1 > 0:
		lower_str_time = str(round((1/time1 - 1) * 100, 2)) + "% more time"
		json[algorithm + "_time"]["lower"] = round((1/time1 - 1) * 100, 2)
	else:
		lower_str_time = "0% more often"
		json[algorithm + "_time"]["lower"] = 0
	
	if time2 >= 1:
		upper_str_time = str(round((1- 1/time2) * 100, 2)) + "% less time"
		json[algorithm + "_time"]["upper"] = -round((1 - 1/time2) * 100, 2)
	elif time2 > 0:
		upper_str_time = str(round((1/time2 - 1) * 100, 2)) + "% more time"
		json[algorithm + "_time"]["upper"] = round((1/time2 - 1) * 100, 2)
	else:
		upper_str_time = "0% more often"
		json[algorithm + "_time"]["upper"] = 0
		
	output.write("Compared to having a contact plan without any opportunistic contacts, "+ algorithm + " was called " + lower_str_count + ", which overall took " + lower_str_time + ", while " + algorithm + " was called " + upper_str_count + ", which took " + upper_str_time + ", compared to a contact plan where all contacts are scheduled already. \n")
		
	
def print_results(output, json,  low_comp, upper_comp):
	json["delivery_ratio"] = dict()
	json["delivery_times"] = dict()
	json["delivery_counts"] = dict()
	json["ORUCoP_count"] = dict()
	json["ORUCoP_time"] = dict()
	json["OCGR-UCoP_count"] = dict()
	json["OCGR-UCoP_time"] = dict()
	
	output.write("--------------------------------DELIVERY RATIO METRICS-------------------------------- \n")
	compare_delivery_ratio(output, json, low_comp[0], upper_comp[0])
	output.write("--------------------------------DELIVERY TIME METRICS-------------------------------- \n")
	compare_delivery_times(output, json, low_comp[1], upper_comp[1])
	output.write("--------------------------------ENERGY CONSUMPTION METRICS-------------------------------- \n")
	compare_delivery_counts(output, json, low_comp[2], upper_comp[2])
	
	output.write("--------------------------------COMPUTATION METRICS-------------------------------- \n")
	compare_computation_metrics(output, json, "ORUCoP", low_comp[3], upper_comp[3], low_comp[5], upper_comp[5])
	compare_computation_metrics(output, json, "OCGR-UCoP", low_comp[4], upper_comp[4], low_comp[6], upper_comp[6])

def print_traffic(path, output):
	with open(path + '/opp_not_known/jsonResults.txt') as file_:
		dict_ = json.load(file_)	
		
	bundleIds = dict_['bundleIds']
	i = 1
	
	output.write("--------------------------------TRAFFIC MODEL-------------------------------- \n")
	for id_ in bundleIds:
		informations = id_.split(":")
		output.write("Bundle " + str(i) + " was sent from node " + informations[0] + " at simulation time " + informations[2] + "s, having node " + informations[1] + " as destination. \n")
		i = i + 1

def analyze_complete_run(simulationpath):
	low_comp = compare_two_results(simulationpath, True)
	upper_comp = compare_two_results(simulationpath, False)
	jsondict = dict()
	
	with open(simulationpath + '/analysis_of_the_run.txt', 'w') as output:
		with open(simulationpath + '/analysis_of_the_run.json', 'w') as joutput:
			print_results(output, jsondict, low_comp, upper_comp)
			joutput.write(json.dumps(jsondict))
	
	return (low_comp, upper_comp)
	
def calculate_mean_of_results(results):
	
	sums = [0,0,0,0,0,0,0]
	for result in results.values():
		sums[0] = result[0] + sums[0]
		sums[1] = result[1] + sums[1]
		sums[2] = result[2] + sums[2]
		sums[3] = result[3] + sums[3]
		sums[4] = result[4] + sums[4]
		sums[5] = result[5] + sums[5]
		sums[6] = result[6] + sums[6]
	
	sums[0] = sums[0] / len(results)
	sums[1] = sums[1] / len(results)
	sums[2] = sums[2] / len(results)
	sums[3] = sums[3] / len(results)
	sums[4] = sums[4] / len(results)
	sums[5] = sums[5] / len(results)
	sums[6] = sums[6] / len(results)
	
	return (sums[0], sums[1], sums[2], sums[3], sums[4], sums[5], sums[6])
	

def analyze_simulation(simulationspath, failure_prob = []):
	lower_results = dict()
	upper_results = dict()
	jsondict = dict()
	for prob in failure_prob:
		result = analyze_complete_run(simulationspath + "/pf=" + str(prob))
		lower_results[prob] = result[0]
		upper_results[prob] = result[1]
	
	results_mean_low = calculate_mean_of_results(lower_results)
	results_mean_upper = calculate_mean_of_results(upper_results)
	
	with open(simulationspath + '/analysis_of_the_simulations.txt', 'w') as output:
		with open(simulationspath + '/analysis_of_the_simulation.json', 'w') as joutput:
			print_traffic(simulationspath + "/pf=" + str(failure_prob[0]), output)
			print_results(output, jsondict, results_mean_low, results_mean_upper)
			joutput.write(json.dumps(jsondict))
	
	return (results_mean_low, results_mean_upper)
	
	
def analyze_algorithm(path, num_of_sims):
	lower_results = dict()
	upper_results = dict()
	jsondict = dict()
	for num in range(1, num_of_sims + 1):
		result = analyze_simulation(path + "/" + "simulation_" + str(num), [0.2, 0.5])
		lower_results[num] = result[0]
		upper_results[num] = result[1]
		
	results_mean_low = calculate_mean_of_results(lower_results)
	results_mean_upper = calculate_mean_of_results(upper_results)
	
	with open(path + "/analysis_of_the_algorithm_performance.txt", "w") as output:
		with open(path + '/analysis_of_the_algorithm_performance.json', "w") as joutput:
			print_results(output, jsondict, results_mean_low, results_mean_upper)
			joutput.write(json.dumps(jsondict))
	return jsondict
	
def print_algorithm_result(output, json, result_low, result_upper):
	output.write("--------------------------------DELIVERY RATIO METRICS-------------------------------- \n")
	json["delivery_ratio"]["lower"] = result_low[0]
	if result_low[0] < 0:
		output.write("Compared with a contact plan without opportunistic contacts, OCGR-UCoP does have a " + str(-result_low[0]) + "% lower delivery ratio than ORUCoP. \n")
	else:
		output.write("Compared with a contact plan without opportunistic contacts, OCGR-UCoP does have a " + str(result_low[0]) + "% higher delivery ratio than ORUCoP. \n")
	json["delivery_ratio"]["upper"] = result_upper[0]
	if result_upper[0] < 0:
		output.write("On the other hand, compared with a contact plan where all contacts are known beforehand, OCGR-UCoP has a " + str(-result_upper[0]) + "% lower delivery ratio than ORUCoP. \n")
	else:
		output.write("On the other hand, compared with a contact plan where all contacts are known beforehand, OCGR-UCoP has a " + str(result_upper[0]) + "% higher delivery ratio than ORUCoP. \n")
	output.write("--------------------------------DELIVERY TIMES METRICS-------------------------------- \n")
	json["delivery_times"]["lower"] = result_low[1]
	if result_low[1] < 0:
		output.write("Compared with a contact plan without opportunistic contacts, OCGR-UCoP's delivery times are " + str(-result_low[1]) + "% lower than ORUCoP's. \n")
	else:
		output.write("Compared with a contact plan without opportunistic contacts, OCGR-UCoP's delivery times are " + str(result_low[1]) + "% higher than ORUCoP's. \n")
	json["delivery_times"]["upper"] = result_upper[1]
	if result_upper[1] < 0:
		output.write("On the other hand, compared with a contact plan where all contacts are known beforehand, OCGR-UCoP's delivery times are " + str(-result_upper[1]) + "% lower than ORUCoP's. \n")
	else:
		output.write("On the other hand, compared with a contact plan where all contacts are known beforehand, OCGR-UCoP's delivery times are " + str(result_upper[1]) + "% higher than ORUCoP's. \n")
	
	output.write("--------------------------------ENERGY CONSUMPTION METRICS-------------------------------- \n")
	json["delivery_counts"]["lower"] = result_low[2]
	if result_low[2] < 0:
		output.write("Compared with a contact plan without opportunistic contacts, OCGR-UCoP does have " + str(-result_low[2]) + "% lower delivery counts than ORUCoP. \n")
	else:
		output.write("Compared with a contact plan without opportunistic contacts, OCGR-UCoP does have " + str(result_low[2]) + "% higher delivery counts than ORUCoP. \n")
	json["delivery_counts"]["upper"] = result_upper[2]
	if result_upper[2] < 0:
		output.write("On the other hand, compared with a contact plan where all contacts are known beforehand, OCGR-UCoP has " + str(-result_upper[2]) + "% lower delivery counts than ORUCoP. \n")
	else:
		output.write("On the other hand, compared with a contact plan where all contacts are known beforehand, OCGR-UCoP has " + str(result_upper[2]) + "% higher delivery counts than ORUCoP. \n")
		
	output.write("-------------------------------- RUCoP COMPUTATION METRICS-------------------------------- \n")
	json["ORUCoP_count"]["lower"] = result_low[3]
	if result_low[3] < 0:
		output.write("Compared with a contact plan without opportunistic contacts, OCGR-UCoP does need " + str(-result_low[3]) + "% less RUCoP calls than ORUCoP. \n")
	else:
		output.write("Compared with a contact plan without opportunistic contacts, OCGR-UCoP does need " + str(result_low[3]) + "% more RUCoP calls than ORUCoP. \n")
	json["ORUCoP_count"]["upper"] = result_upper[3]
	if result_upper[3] < 0:
		output.write("On the other hand, compared with a contact plan where all contacts are known beforehand, OCGR-UCoP does need " + str(-result_upper[3]) + "% less RUCoP calls than ORUCoP. \n")
	else:
		output.write("On the other hand, compared with a contact plan where all contacts are known beforehand, OCGR-UCoP does need " + str(result_upper[3]) + "% more RUCoP calls than ORUCoP. \n")

	json["ORUCoP_time"]["lower"] = result_low[4]
	if result_low[4] < 0:
		output.write("Compared with a contact plan without opportunistic contacts, OCGR-UCoP's RUCoP calls need " + str(-result_low[4]) + "% less time than ORUCoP's. \n")
	else:
		output.write("Compared with a contact plan without opportunistic contacts, OCGR-UCoP's RUCoP calls need " + str(result_low[4]) + "% more time than ORUCoP's. \n")
	json["ORUCoP_time"]["upper"] = result_upper[4]
	if result_upper[4] < 0:
		output.write("On the other hand, compared with a contact plan where all contacts are known beforehand, OCGR-UCoP's RUCoP calls need " + str(-result_upper[4]) + "% less time than ORUCoP's. \n")
	else:
		output.write("On the other hand, compared with a contact plan where all contacts are known beforehand, OCGR-UCoP's RUCoP calls need " + str(result_upper[4]) + "% more time than ORUCoP's. \n")

		
	
	
def compare_algorithms(output, joutput, ocgr_ucop, rucop):
	jsondict = dict()
	jsondict["delivery_ratio"] = dict()
	jsondict["delivery_times"] = dict()
	jsondict["delivery_counts"] = dict()
	jsondict["ORUCoP_count"] = dict()
	jsondict["ORUCoP_time"] = dict()
	jsondict["OCGR-UCoP_count"] = dict()
	jsondict["OCGR-UCoP_time"] = dict()
	result_low = [0,0,0,0,0,0,0]
	result_low[0] = round(ocgr_ucop["delivery_ratio"]["lower"] - rucop["delivery_ratio"]["lower"], 2)
	result_low[1] = round(ocgr_ucop["delivery_times"]["lower"] - rucop["delivery_times"]["lower"], 2)
	result_low[2] = round(ocgr_ucop["delivery_counts"]["lower"] - rucop["delivery_counts"]["lower"], 2)
	result_low[3] = round(ocgr_ucop["ORUCoP_count"]["lower"] - rucop["ORUCoP_count"]["lower"], 2)
	result_low[4] = round(ocgr_ucop["ORUCoP_time"]["lower"] - rucop["ORUCoP_time"]["lower"], 2)
	result_low[5] = round(ocgr_ucop["OCGR-UCoP_count"]["lower"] - rucop["OCGR-UCoP_count"]["lower"], 2)
	result_low[6] = round(ocgr_ucop["OCGR-UCoP_time"]["lower"] - rucop["OCGR-UCoP_time"]["lower"], 2)
	
	result_upper = [0,0,0,0,0,0,0]
	result_upper[0] = round(ocgr_ucop["delivery_ratio"]["upper"] - rucop["delivery_ratio"]["upper"], 2)
	result_upper[1] = round(ocgr_ucop["delivery_times"]["upper"] - rucop["delivery_times"]["upper"], 2)
	result_upper[2] = round(ocgr_ucop["delivery_counts"]["upper"] - rucop["delivery_counts"]["upper"], 2)
	result_upper[3] = round(ocgr_ucop["ORUCoP_count"]["upper"] - rucop["ORUCoP_count"]["upper"], 2)
	result_upper[4] = round(ocgr_ucop["ORUCoP_time"]["upper"] - rucop["ORUCoP_time"]["upper"], 2)
	result_upper[5] = round(ocgr_ucop["OCGR-UCoP_count"]["upper"] - rucop["OCGR-UCoP_count"]["upper"], 2)
	result_upper[6] = round(ocgr_ucop["OCGR-UCoP_time"]["upper"] - rucop["OCGR-UCoP_time"]["upper"], 2)
	
	print_algorithm_result(output, jsondict, result_low, result_upper)
	joutput.write(json.dumps(jsondict))
		
def compute_avg_seeds(path, num_of_seeds):
	seeds_dict = dict()
	
	for num in range(0, num_of_seeds):
		with open(simulationpath + '/metrics/jsonResults_' + str(num) + '.txt') as file1:
			seeds_dict[num] = json.load(file1)
	
	result = dict()
	bundleIds = seeds_dict[0]['bundleIds']
	bundles_received_count = dict()
	delivery_times = dict()
	delivery_counts = dict()
	
	
	delivery_ratio = 0
	
	for id_ in bundleIds:
		bundles_received_count[id_] = 0
	
	for num in range(0, num_of_seeds):
		delivery_ratio += len(seeds_dict[num]["receivedIds"] / len(bundleIds)
		
		for id_ in bundleIds:
			if id_ in seeds_dict[num]["receivedIds"]:
				delivery_times[id_] += seeds_dict[num]["bundleDeliveryTimes"][id_]
				delivery_counts[id_] += seeds_dict[num]["bundleDeliveryCounts"][id_]
				bundles_received_count[id_]++
				
						
	delivery_ratio = delivery_ratio / num_of_seeds
	
	for id_ in bundleIds:
		if bundles_received_count[id_] > 0:
			delivery_times[id_] = delivery_times[id_] / bundles_received_count[id_]
			delivery_counts[id_] = delivery_counts[id_] / bundles_received_count[id_]
	
	result["delivery_ratio"] = delivery_ratio
	result["delivery_times"] = delivery_times
	result["delivery_counts"] = delivery_counts
	
	return result
	
def compare_with_cgr(cgr_result, other_result):
	
	bundleIds = cgr_result['bundleIds']
	receivedIds = cgr_Result["receivedIds"]
	
	delivery_ratio = other_result["delivery_ratio"] / cgr_result["delivery_ratio"]
	compared_bundles = 0
	delivery_times = 0
	delivery_counts = 0
	
	for id_ in receivedIds:
		if id_ in other_result["delivery_times"]:
			delivery_times += other_result["delivery_times"][id_] / cgr_result["delivery_times"][id_]
			delivery_counts += other_result["delivery_counts"][id_] / cgr_result["delivery_counts"][id_]
			compared_bundles++
		
	if compared_bundles > 0:
		delivery_times = delivery_times / compared_bundles
		delivery_counts = delivery_counts / compared_bundles
	else:
		delivery_times = 0
		delivery_counts = 0
		
	result = dict()
	
	result["delivery_ratio"] = delivery_ratio - 1
	result["delivery_times"] = delivery_times - 1
	result["delivery_counts"] = delivery_counts - 1
	
	return result
	
		
		
def main():
	num_of_sims = 6
	path = str(sys.argv[1])
	ocgr_ucop = analyze_algorithm(path + "/OCGR-UCoP", num_of_sims)
	orucop = analyze_algorithm(path + "/ORUCoP", num_of_sims)
	cgr = analyze_algorithm(path + "/CGR", num_of_sims)
	ocgr = analyze_algorithm(path+ "/OCGR", num_of_sims)
	
	
	with open(path + "/algorithm_comparison.txt", "w") as output:
		with open(path + "/algorithm_comparison.json", "w") as joutput:
			compare_algorithms(output, joutput, ocgr_ucop, orucop)
	
	
if __name__ == "__main__":
    main()		

	

