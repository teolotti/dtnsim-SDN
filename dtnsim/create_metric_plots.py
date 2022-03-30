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
	result["receivedIds"] = []
	bundles_received_count = dict()
	delivery_times = dict()
	delivery_counts = dict()
	seed_values = dict()
	seed_values["ratio"] = dict()
	seed_values["times"] = dict()
	seed_values["counts"] = dict()
	
	delivery_ratio = 0
	
	
	for id_ in bundleIds:
		bundles_received_count[id_] = 0
		seed_values["times"][id_] = dict()
		seed_values["counts"][id_] = dict()
		delivery_times[id_] = 0
		delivery_counts[id_] = 0
	
	for num in range(0, num_of_seeds):
		delivery_ratio += len(seeds_dict[num]["receivedIds"]) / len(bundleIds)
		seed_values["ratio"][num] = len(seeds_dict[num]["receivedIds"]) / len(bundleIds)
		for id_ in bundleIds:
			if id_ in seeds_dict[num]["receivedIds"]:
				if (seeds_dict[num]["bundleDeliveryTimes"][id_] == 0):
					delivery_times[id_] += 1
					seed_values["times"][id_][bundles_received_count[id_]] = 1
				else:
					delivery_times[id_] += seeds_dict[num]["bundleDeliveryTimes"][id_]
					seed_values["times"][id_][bundles_received_count[id_]] = seeds_dict[num]["bundleDeliveryTimes"][id_]
				delivery_counts[id_] += seeds_dict[num]["bundleDeliveryCounts"][id_]
				seed_values["counts"][id_][bundles_received_count[id_]] = seeds_dict[num]["bundleDeliveryCounts"][id_]
				bundles_received_count[id_] = bundles_received_count[id_] + 1
				result["receivedIds"].append(id_)
				
	means_dict = dict()	
	means_dict["times"] = dict()
	means_dict["counts"] = dict()				
	delivery_ratio = delivery_ratio / num_of_seeds
	means_dict["ratio"] = delivery_ratio
	if delivery_ratio == 0:
		delivery_ratio = ((1 / len(bundleIds)) / num_of_seeds) / 2
	
	for id_ in bundleIds:
		if bundles_received_count[id_] > 0:
			delivery_times[id_] = delivery_times[id_] / bundles_received_count[id_]
			delivery_counts[id_] = delivery_counts[id_] / bundles_received_count[id_]
			means_dict["times"][id_] = delivery_times[id_]
			means_dict["counts"][id_] = delivery_counts[id_]
			
	margin_of_errors = compute_confidence_intervals(seed_values, means_dict)
	print(margin_of_errors["ratio"])
	result["bundleIds"] = bundleIds
	result["delivery_ratio"] = delivery_ratio
	result["delivery_times"] = delivery_times
	result["delivery_counts"] = delivery_counts
	
	return result


def compute_confidence_intervals(seed_values, means):
	sd_dict = compute_standard_deviations(seed_values, means)
	
	result_dict = dict()
	ratio_ci = (1.96 * sd_dict["ratio"]) / math.sqrt(len(seed_values["ratio"]))
	result_dict["ratio"] = ratio_ci
	times_ci = 0
	counts_ci = 0
	for id_ in sd_dict["times"]:
		times_ci += (1.96 * sd_dict["times"][id_]) / math.sqrt(len(seed_values["times"][id_]))
		counts_ci += (1.96 * sd_dict["times"][id_]) / math.sqrt(len(seed_values["counts"][id_]))
	
	times_ci = times_ci / len(seed_values["times"])
	counts_ci = counts_ci / len(seed_values["counts"])
	
	result_dict["times"] = times_ci
	result_dict["counts"] = counts_ci
	
	return result_dict
	
	
def compute_standard_deviations(seed_values, means):
	result_dict = dict()
	result_dict["times"] = dict()
	result_dict["counts"] = dict()
	
	result_dict["ratio"] = compute_standard_deviation(seed_values["ratio"], means["ratio"])
	
	
	for id_ in means["times"]:
		result_dict["times"][id_] = compute_standard_deviation(seed_values["times"][id_], means["times"][id_])
		result_dict["counts"][id_] = compute_standard_deviation(seed_values["counts"][id_], means["counts"][id_])

	
	return result_dict
		
def compute_standard_deviation(values, mean):
	
	value_sum = 0
	for i in range(0, len(values)):
		value_sum += (values[i] - mean) * (values[i] - mean)
	
	value_sum = value_sum / len(values)
	
	return math.sqrt(value_sum)	
	
				
def compare_with_cgr(cgr_result, other_result):
	
	bundleIds = cgr_result['bundleIds']
	receivedIds = cgr_result["receivedIds"]
	
	delivery_ratio = other_result["delivery_ratio"] / cgr_result["delivery_ratio"]
	compared_bundles = 0
	delivery_times = 0
	delivery_counts = 0
	
	for id_ in receivedIds:
		if id_ in other_result["delivery_times"]:
			delivery_times += other_result["delivery_times"][id_] / cgr_result["delivery_times"][id_]
			delivery_counts += other_result["delivery_counts"][id_] / cgr_result["delivery_counts"][id_]
			compared_bundles += 1
		
	if compared_bundles > 0:
		delivery_times = delivery_times / compared_bundles
		delivery_counts = delivery_counts / compared_bundles
	else:
		delivery_times = 1
		delivery_counts = 1
		
	result = dict()
	
	result["delivery_ratio"] = delivery_ratio - 1
	result["delivery_times"] = delivery_times - 1
	result["delivery_counts"] = 1 - delivery_counts
	
	return result
	
		
	
def analyze_run(path, num_of_seeds, cgr_result):
	other_result = compute_avg_seeds(path, num_of_seeds)
	
	cgr_comparison = compare_with_cgr(cgr_result, other_result)
	
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
	
	for i in range(1, 9):
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
		
		final_string += "\n"
		
	with open("plots/sim_" + str(num) + "_" + metric, "w") as output:
		output.write(final_string)
		
def print_metric_graph_specific(results, metric, num):
	final_string = "x "
	
	for i in range(1, 9):
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
	
def main():
	num_of_sims = 10
	path = str(sys.argv[1])
	result = dict()
	
	for sim in range(1, num_of_sims + 1):
		if sim < 7:
			result[sim] = analyze_simulation(path + "/simulation_" + str(sim), [0.2, 0.35, 0.5, 0.65, 0.8], 30, sim)
		else:
			result[sim] = analyze_simulation(path + "/simulation_" + str(sim), [-1], 30, sim)
			
	for i in range(0, 5):
		results = combine_result(result[2*i+1], result[2*i+2])
		print_results(results, [0.2, 0.35, 0.5, 0.65, 0.8], 2*i+1)
	
if __name__ == "__main__":
    main()	
		
	
	
