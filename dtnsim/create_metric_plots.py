import json
import sys
import json2latex
import math

def compute_avg_seeds(path, num_of_seeds):
	seeds_dict = dict()
	
	for num in range(0, num_of_seeds):
		with open(path + '/metrics/jsonResults_' + str(num) + '.txt') as file1:
			seeds_dict[num] = json.load(file1)
	
	result = dict()
	bundleIds = seeds_dict[0]['bundleIds']
	result["receivedIds"] = dict()
	bundles_received_count = dict()
	rucop_calls = 0 
	computation_time = 0
	delivery_times = dict()
	delivery_counts = dict()
	seed_values = dict()
	seed_values["ratio"] = dict()
	seed_values["times"] = dict()
	seed_values["counts"] = dict()
	
	delivery_ratio = 0
	
	
	for id_ in bundleIds:
		bundles_received_count[id_] = 0
		delivery_times[id_] = 0
		delivery_counts[id_] = 0
	
	for num in range(0, num_of_seeds):
		seed_values["times"][num] = dict()
		seed_values["counts"][num] = dict()
		delivery_ratio += len(seeds_dict[num]["receivedIds"]) / len(bundleIds)
		seed_values["ratio"][num] = len(seeds_dict[num]["receivedIds"]) / len(bundleIds)
		for id_ in bundleIds:
			seed_values["times"][num][id_] = -1
			seed_values["counts"][num][id_] = -1
			if id_ in seeds_dict[num]["receivedIds"]:
				if (seeds_dict[num]["bundleDeliveryTimes"][id_] == 0):
					delivery_times[id_] += 1
					seed_values["times"][num][id_] = 1
				else:
					delivery_times[id_] += seeds_dict[num]["bundleDeliveryTimes"][id_]
					seed_values["times"][num][id_] = seeds_dict[num]["bundleDeliveryTimes"][id_]
				delivery_counts[id_] += seeds_dict[num]["bundleDeliveryCounts"][id_]
				seed_values["counts"][num][id_] = seeds_dict[num]["bundleDeliveryCounts"][id_]
				bundles_received_count[id_] = bundles_received_count[id_] + 1
				result["receivedIds"][id_] = 1
		rucop_calls += seeds_dict[num]["RUCoPCalls"]
		computation_time += seeds_dict[num]["RUCoPComputationTime"]
	
	rucop_calls = rucop_calls / num_of_seeds
	computation_time = computation_time / num_of_seeds
							
	delivery_ratio = delivery_ratio / num_of_seeds
	if delivery_ratio == 0:
		delivery_ratio = ((1 / len(bundleIds)) / num_of_seeds)
		seed_values["ratio"][0] = 1 / len(bundleIds)
	
	for id_ in bundleIds:
		if bundles_received_count[id_] > 0:
			delivery_times[id_] = delivery_times[id_] / bundles_received_count[id_]
			delivery_counts[id_] = delivery_counts[id_] / bundles_received_count[id_]
	result["bundleIds"] = bundleIds
	result["delivery_ratio"] = delivery_ratio
	result["delivery_times"] = delivery_times
	result["delivery_counts"] = delivery_counts
	result["seed_values"] = seed_values
	result["calls"] = rucop_calls
	result["computation"] = computation_time
	
	return result
	
def calculate_covariance(set_one, mean_one, set_two, mean_two):
	
	if len(set_one) == 0 or len(set_one) == 1:
		return 0
	covariance = 0
	for i in range(0, len(set_one)):
		covariance += (set_one[i] - mean_one) * (set_two[i] - mean_two)
	
	return covariance / (len(set_one) - 1)
	
def calculate_standard_deviation(data_set, mean):

	if len(data_set) == 0 or len(data_set) == 1:
		return 0	
	sd = 0
	for i in range(0, len(data_set)):
		sd += (data_set[i] - mean)**2
	
	return math.sqrt(sd / (len(data_set) - 1))
	
def calculate_mean(data_set):
	
	if len(data_set) == 0:
		return 0
	mean = 0
	for i in range(0, len(data_set)):
		mean += data_set[i]
		
	return mean / len(data_set)
		

def compute_confidence_interval(set_other, set_cgr):

	result = dict()
	other_mean = calculate_mean(set_other)
	
	other_sd = calculate_standard_deviation(set_other, other_mean)
	
	cgr_mean = calculate_mean(set_cgr)

	cgr_sd = calculate_standard_deviation(set_cgr, cgr_mean)

	
	cv = calculate_covariance(set_other, other_mean, set_cgr, cgr_mean)

	if (other_mean == cgr_mean and other_sd == cgr_sd):
		confidence = other_sd
	else:
		root = math.sqrt((other_sd/other_mean)**2 + (cgr_sd/cgr_mean)**2 - 2*(cv/(cgr_mean * other_mean)))
		confidence = (other_mean / cgr_mean) * root
	
	result["mean"] = other_mean / cgr_mean
	result["confidence"] = (1.96 * confidence) / math.sqrt(len(set_other))
	
	return result
	
				
def compare_with_cgr(cgr_result, other_result, num_of_seeds):
	
	bundleIds = cgr_result['bundleIds']
	receivedIds = cgr_result["receivedIds"]

	
	delivery_ratio_result = compute_confidence_interval(other_result["seed_values"]["ratio"], cgr_result["seed_values"]["ratio"])
	delivery_ratio = delivery_ratio_result["mean"]
	delivery_ratio_ci = delivery_ratio_result["confidence"]
	other_delivery = dict()
	cgr_delivery = dict()
	other_count = dict()
	cgr_count = dict()
	bundles_arrived = dict()
	bundles_results = dict()
	bundles_results["times"] = dict()
	bundles_results["counts"] = dict()
	overall_received = 0
	
	for id_ in receivedIds:
		bundles_arrived[id_] = 0
		other_delivery[id_] = dict()
		cgr_delivery[id_] = dict()
		other_count[id_] = dict()
		cgr_count[id_] = dict()
		for num in range(0, num_of_seeds):
			if (cgr_result["seed_values"]["times"][num][id_] != -1 and other_result["seed_values"]["times"][num][id_] != -1):
				other_delivery[id_][bundles_arrived[id_]] = other_result["seed_values"]["times"][num][id_]
				cgr_delivery[id_][bundles_arrived[id_]] = cgr_result["seed_values"]["times"][num][id_]
				other_count[id_][bundles_arrived[id_]] = other_result["seed_values"]["counts"][num][id_]
				cgr_count[id_][bundles_arrived[id_]] = cgr_result["seed_values"]["counts"][num][id_]
				bundles_arrived[id_] = bundles_arrived[id_] + 1
		
		if (bundles_arrived[id_] > 0):		
			bundles_results["times"][id_] = compute_confidence_interval(other_delivery[id_], cgr_delivery[id_])
			bundles_results["counts"][id_] = compute_confidence_interval(other_count[id_], cgr_count[id_])
		
	delivery_times = 0
	delivery_times_ci = 0
	delivery_counts = 0
	delivery_counts_ci = 0
	
	for id_ in receivedIds:
		if (bundles_arrived[id_] > 0):
			delivery_times += bundles_results["times"][id_]["mean"]
			delivery_times_ci += bundles_results["times"][id_]["confidence"]
			delivery_counts += bundles_results["counts"][id_]["mean"]
			delivery_counts_ci += bundles_results["counts"][id_]["confidence"]
			overall_received +=1
		
	delivery_times = delivery_times / overall_received
	delivery_times_ci = delivery_times_ci / overall_received
	delivery_counts = delivery_counts / overall_received
	delivery_counts_ci = delivery_counts_ci / overall_received
		
				
				
	
	#delivery_ratio = other_result["delivery_ratio"] / cgr_result["delivery_ratio"]
	#compared_bundles = 0
	#delivery_times = 0
	#delivery_counts = 0
	
	#for id_ in receivedIds:
	#	if id_ in other_result["delivery_times"]:
	#		delivery_times += other_result["delivery_times"][id_] / cgr_result["delivery_times"][id_]
	#		delivery_counts += other_result["delivery_counts"][id_] / cgr_result["delivery_counts"][id_]
	#		compared_bundles += 1
		
	#if compared_bundles > 0:
	#	delivery_times = delivery_times / compared_bundles
	#	delivery_counts = delivery_counts / compared_bundles
	#else:
	#	delivery_times = 1
	#	delivery_counts = 1
		
	result = dict()
	
	result["delivery_ratio"] = delivery_ratio - 1
	result["delivery_ratio_ci"] = delivery_ratio_ci
	result["delivery_times"] = delivery_times - 1
	result["delivery_times_ci"] = delivery_times_ci
	result["delivery_counts"] = 1 - delivery_counts
	result["delivery_counts_ci"] = delivery_counts_ci
	result["calls"] = other_result["calls"]
	result["computation"] = other_result["computation"]
	
	return result
	
		
	
def analyze_run(path, num_of_seeds, cgr_result):
	other_result = compute_avg_seeds(path, num_of_seeds)
	
	cgr_comparison = compare_with_cgr(cgr_result, other_result, num_of_seeds)
	
	return cgr_comparison
	
def analyze_simulation(path, pfs, num_of_seeds, num):
	cgr_results = dict()
	
	for pf in pfs:
		cgr_results[pf] = compute_avg_seeds(path + '/OCGR/pf=' + str(pf) + '/no_opp', num_of_seeds)
	algorithm_results = dict()
	algorithm_results["OCGR"] = dict()
	algorithm_results["RUCoP"] = dict()
	algorithm_results["CGR-AK"] = dict()
	algorithm_results["RUCoP-AK"] = dict()
	algorithm_results["OCGR-UCoP"] = dict()
	algorithm_results["ORUCoP"] = dict()
	algorithm_results["CGR-UCoP"] = dict()
	algorithm_results["CGR-UCoP-AK"] = dict()
	for pf in pfs:
		algorithm_results["OCGR"][pf] = analyze_run(path + '/OCGR/pf=' + str(pf) + '/opp_not_known', num_of_seeds, cgr_results[pf])
		algorithm_results["RUCoP"][pf] = analyze_run(path + '/ORUCoP/pf=' + str(pf) + '/no_opp', num_of_seeds, cgr_results[pf])
		algorithm_results["CGR-AK"][pf] = analyze_run(path + '/OCGR/pf=' + str(pf) + '/opp_known', num_of_seeds, cgr_results[pf])
		algorithm_results["RUCoP-AK"][pf] = analyze_run(path + '/ORUCoP/pf=' + str(pf) + '/opp_known', num_of_seeds, cgr_results[pf])
		algorithm_results["OCGR-UCoP"][pf] = analyze_run(path + '/OCGR-UCoP/pf=' + str(pf) + '/opp_not_known', num_of_seeds, cgr_results[pf])
		algorithm_results["ORUCoP"][pf] = analyze_run(path + '/ORUCoP/pf=' + str(pf) + '/opp_not_known', num_of_seeds, cgr_results[pf])
		algorithm_results["CGR-UCoP"][pf] = analyze_run(path + '/OCGR-UCoP/pf=' + str(pf) + '/no_opp', num_of_seeds, cgr_results[pf])
		algorithm_results["CGR-UCoP-AK"][pf] = analyze_run(path + '/OCGR-UCoP/pf=' + str(pf) + '/opp_known', num_of_seeds, cgr_results[pf])
		
		
	#print_results(algorithm_results, pfs, num)
	
	return algorithm_results
	
def print_metric_graph_non_specific(results, metric, pfs, num):
	final_string = "x "
	
	for i in range(1, 17):
		final_string += "y" + str(i) + " "
		
	final_string += "\n"
	
	for pf in pfs:
		if pf == 0.2:
			final_string += "1 "
		if pf == 0.35:
			final_string += "2 "
		if pf == 0.5:
			final_string += "3 "
		if pf == 0.65:
			final_string += "4 "
		if pf == 0.8:
			final_string += "5 "
			
		final_string += str(round(results["OCGR"][pf][metric] * 100, 2)) + " "
		final_string += str(round(results["CGR-AK"][pf][metric] * 100, 2))  + " "
		final_string += str(round(results["RUCoP"][pf][metric] * 100, 2))  + " "
		final_string += str(round(results["ORUCoP"][pf][metric] * 100, 2))  + " "
		final_string += str(round(results["RUCoP-AK"][pf][metric] * 100, 2))  + " "
		final_string += str(round(results["CGR-UCoP"][pf][metric] * 100, 2))  + " "
		final_string += str(round(results["OCGR-UCoP"][pf][metric] * 100, 2))  + " "
		final_string += str(round(results["CGR-UCoP-AK"][pf][metric] * 100, 2) ) + " "
		final_string += str(round(results["OCGR"][pf][metric + "_ci"] * 100, 2)) + " "
		final_string += str(round(results["CGR-AK"][pf][metric + "_ci"] * 100, 2))  + " "
		final_string += str(round(results["RUCoP"][pf][metric + "_ci"] * 100, 2))  + " "
		final_string += str(round(results["ORUCoP"][pf][metric + "_ci"] * 100, 2))  + " "
		final_string += str(round(results["RUCoP-AK"][pf][metric + "_ci"] * 100, 2))  + " "
		final_string += str(round(results["CGR-UCoP"][pf][metric + "_ci"] * 100, 2))  + " "
		final_string += str(round(results["OCGR-UCoP"][pf][metric + "_ci"] * 100, 2))  + " "
		final_string += str(round(results["CGR-UCoP-AK"][pf][metric + "_ci"] * 100, 2) ) + " "
		
		final_string += "\n"
		
	with open("plots/sim_" + str(num) + "_" + metric, "w") as output:
		output.write(final_string)
		
def print_metric_graph_specific(results, metric, num):
	final_string = "x "
	
	for i in range(1, 17):
		final_string += "y" + str(i) + " "
		
	final_string += "\n"
	
	for i in range(0, 2):
		if i == 0:
			final_string += "0 "
		else:
			final_string += "2 "
			
		final_string += str(round(results["OCGR"][-1][metric] * 100, 2)) + " "
		final_string += str(round(results["CGR-AK"][-1][metric] * 100, 2))  + " "
		final_string += str(round(results["RUCoP"][-1][metric] * 100, 2))  + " "
		final_string += str(round(results["ORUCoP"][-1][metric] * 100, 2))  + " "
		final_string += str(round(results["RUCoP-AK"][-1][metric] * 100, 2))  + " "
		final_string += str(round(results["CGR-UCoP"][-1][metric] * 100, 2))  + " "
		final_string += str(round(results["OCGR-UCoP"][-1][metric] * 100, 2))  + " "
		final_string += str(round(results["CGR-UCoP-AK"][-1][metric] * 100, 2) ) + " "
		final_string += str(round(results["OCGR"][-1][metric + "_ci"] * 100, 2)) + " "
		final_string += str(round(results["CGR-AK"][-1][metric + "_ci"] * 100, 2))  + " "
		final_string += str(round(results["RUCoP"][-1][metric + "_ci"] * 100, 2))  + " "
		final_string += str(round(results["ORUCoP"][-1][metric + "_ci"] * 100, 2))  + " "
		final_string += str(round(results["RUCoP-AK"][-1][metric + "_ci"] * 100, 2))  + " "
		final_string += str(round(results["CGR-UCoP"][-1][metric + "_ci"] * 100, 2))  + " "
		final_string += str(round(results["OCGR-UCoP"][-1][metric + "_ci"] * 100, 2))  + " "
		final_string += str(round(results["CGR-UCoP-AK"][-1][metric + "_ci"] * 100, 2) ) + " "
	
		final_string += "\n"
	
	with open("plots/sim_" + str(num) + "_" + metric, "w") as output:
		output.write(final_string)
	
def print_results(results, pfs, num):
	
	if (num < 7):
		print_metric_graph_non_specific(results, "delivery_ratio", pfs, num)
		print_metric_graph_non_specific(results, "delivery_times", pfs, num)
		print_metric_graph_non_specific(results, "delivery_counts", pfs, num)
	else:
		print_metric_graph_specific(results, "delivery_ratio", num)
		print_metric_graph_specific(results, "delivery_times", num)
		print_metric_graph_specific(results, "delivery_counts", num)
	
	
def combine_result(result_1, result_2):
	final_result = dict()
	
	for algorithm in result_1:
		final_result[algorithm] = dict()
		for pf in result_1[algorithm]:
			final_result[algorithm][pf] = dict()
			for metric in result_1[algorithm][pf]:
				final_result[algorithm][pf][metric] = (result_1[algorithm][pf][metric] + result_2[algorithm][pf][metric]) / 2
	
	return final_result
	
def generate_overall_means(final_results):
	
	overall_means = dict()
	for algorithm in final_results[0]:
		overall_means[algorithm] = dict()
		overall_means[algorithm]["delivery_ratio"] = 0
		overall_means[algorithm]["delivery_times"] = 0
		overall_means[algorithm]["delivery_counts"] = 0
		overall_means[algorithm]["delivery_ratio_ci"] = 0
		overall_means[algorithm]["delivery_times_ci"] = 0
		overall_means[algorithm]["delivery_counts_ci"] = 0
		overall_means[algorithm]["calls"] = 0
		overall_means[algorithm]["computation"] = 0
		
	
	for num in final_results:
		for algorithm in final_results[num]:
			for pf in final_results[num][algorithm]:
				for metric in final_results[num][algorithm][pf]:
					overall_means[algorithm][metric] += final_results[num][algorithm][pf][metric]
					
					
	for algorithm in overall_means:
		overall_means[algorithm]["delivery_ratio"] = overall_means[algorithm]["delivery_ratio"] / 17
		overall_means[algorithm]["delivery_times"] = overall_means[algorithm]["delivery_times"] / 17
		overall_means[algorithm]["delivery_counts"] = overall_means[algorithm]["delivery_counts"] / 17
		overall_means[algorithm]["calls"] = overall_means[algorithm]["calls"] / 17
		overall_means[algorithm]["computation"] = overall_means[algorithm]["computation"] / 17
				
	print_overall_means(overall_means)
	
def print_overall_means(overall_means):
	
	with open("plots/means", "w") as output:
		for algorithm in overall_means:
			output.write(algorithm + ": " + str(round(overall_means[algorithm]["delivery_ratio"]*100,2)) + " " + str(round(overall_means[algorithm]["delivery_times"]*100,2)) + " " +  str(round(overall_means[algorithm]["delivery_counts"]*100,2)) + " " + str(overall_means[algorithm]["calls"]) + " " + str(overall_means[algorithm]["computation"]) + "\n")
 				
	
def main():
	num_of_sims = 10
	path = str(sys.argv[1])
	result = dict()
	final_results = dict()
	
	for sim in range(1, num_of_sims + 1):
		if sim < 7:
			result[sim] = analyze_simulation(path + "/simulation_" + str(sim), [0.2, 0.35, 0.5, 0.65, 0.8], 30, sim)
		else:
			result[sim] = analyze_simulation(path + "/simulation_" + str(sim), [-1], 30, sim)
			
	for i in range(0, 5):
		final_results[i] = combine_result(result[2*i+1], result[2*i+2])
		print_results(final_results[i], [0.2, 0.35, 0.5, 0.65, 0.8], 2*i+1)
	
	generate_overall_means(final_results)
	
if __name__ == "__main__":
    main()	
		
	
	
