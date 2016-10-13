// New plot data generator to generate results for fixed t and varing k from 1-5
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include "Simulator.hpp"

using namespace std;

/*We want to generate following plots
Number of Sensors moved Vs number of sensors failed
Avg distance Vs Number of Sensors failed
Accuracy Vs Number of Sensors failed
Timestamps Vs Number of Sensors failed

We want this two plots for following scenarios
t = 5
K = 1
K = 2
K = 3
K = 4
K = 5*/
const int fixed_t = 5;
const int BUFFER_LEN = 200;
char filename[BUFFER_LEN];

template<typename T>
double mean(vector<T>& v) {
	double avg = 0.0;
	for(int i = 0; i < (int)v.size(); i++) {
		avg += (double)v[i];
	}
	return (avg/v.size());
}
template<typename T>
double sd(vector<T>& v, double avg) {
	double sum = 0.0;
	for(int i = 0; i < (int)v.size(); i++) {
		sum += ((double)v[i] - avg)*((double)v[i] - avg);
	}
	return sqrt(sum/v.size());
}

template double mean<double>(vector<double>& v);
template double mean<int>(vector<int>& v);
template double sd<double>(vector<double>& v, double avg);
template double sd<int>(vector<int>& v, double avg);

void print_int_vect(vector<int>& v) {
	for(int i = 0; i < (int)v.size(); i++) {
		printf("%d\n", v[i]);
	}
}
void print_double_vect(vector<double>& v) {
	for(int i = 0; i < (int)v.size(); i++) {
		printf("%lf\n", v[i]);
	}
}

int main(int argc, char *argv[])
{
	vector<double> avg_distance;
	vector<double> max_distance;
	vector<int> timestamp;
	vector<int> n_moved;
	int failed_simulation;

	vector<double> avg_distance1K;
	vector<double> avg_distance2K;
	vector<double> avg_distance3K;
	vector<double> avg_distance4K;
	vector<double> avg_distance5K;

	vector<double> max_distance1K;
	vector<double> max_distance2K;
	vector<double> max_distance3K;
	vector<double> max_distance4K;
	vector<double> max_distance5K;

	vector<double> timestamp1K;
	vector<double> timestamp2K;
	vector<double> timestamp3K;
	vector<double> timestamp4K;
	vector<double> timestamp5K;

	vector<double> n_moved1K;
	vector<double> n_moved2K;
	vector<double> n_moved3K;
	vector<double> n_moved4K;
	vector<double> n_moved5K;

	vector<double> accuracy1K;
	vector<double> accuracy2K;
	vector<double> accuracy3K;
	vector<double> accuracy4K;
	vector<double> accuracy5K;

	vector<double> avg_distance1K_err;
	vector<double> avg_distance2K_err;
	vector<double> avg_distance3K_err;
	vector<double> avg_distance4K_err;
	vector<double> avg_distance5K_err;

	vector<double> max_distance1K_err;
	vector<double> max_distance2K_err;
	vector<double> max_distance3K_err;
	vector<double> max_distance4K_err;
	vector<double> max_distance5K_err;

	vector<double> timestamp1K_err;
	vector<double> timestamp2K_err;
	vector<double> timestamp3K_err;
	vector<double> timestamp4K_err;
	vector<double> timestamp5K_err;

	vector<double> n_moved1K_err;
	vector<double> n_moved2K_err;
	vector<double> n_moved3K_err;
	vector<double> n_moved4K_err;
	vector<double> n_moved5K_err;

	for(int n_fail = 1; n_fail <= 5; n_fail++) {
		// printf("Number of Failed Sensors = %d\n", n_fail);
		// 1000 simulations for each configuration

		// K = 1
		avg_distance.clear();
		max_distance.clear();
		timestamp.clear();
		n_moved.clear();
		failed_simulation = 0;
		for(int i = 1; i <= 10; i++) {
			sprintf(filename, "Configurations/configuration%d.txt", i);
			Simulator simulator;
			simulator.read_input_from_file(filename);
			simulator.change_K(1);
			simulator.change_timestamp_jump(fixed_t);
			for(int j = 0; j < 100; j++) {
				simulator.initialize(n_fail);
				if(simulator.maintain()) {
					simulator.evaluate_results();
					avg_distance.push_back(simulator.get_avg_distance());
					max_distance.push_back(simulator.get_max_distance());
					timestamp.push_back(simulator.get_final_timestamp());
					n_moved.push_back(simulator.get_no_of_sensors_moved());
				} else {
					failed_simulation++;
				}
			}
			simulator.delete_data();
		}
		avg_distance1K.push_back(mean<double>(avg_distance));
		max_distance1K.push_back(mean<double>(max_distance));
		timestamp1K.push_back(mean<int>(timestamp));
		n_moved1K.push_back(mean<int>(n_moved));
		avg_distance1K_err.push_back(sd<double>(avg_distance, avg_distance1K.back()));
		max_distance1K_err.push_back(sd<double>(max_distance, max_distance1K.back()));
		timestamp1K_err.push_back(sd<int>(timestamp, timestamp1K.back()));
		n_moved1K_err.push_back(sd<int>(n_moved, n_moved1K.back()));
		accuracy1K.push_back(((double)(1000 - failed_simulation)) / 1000.0);

		// K = 2
		avg_distance.clear();
		max_distance.clear();
		timestamp.clear();
		n_moved.clear();
		failed_simulation = 0;
		for(int i = 1; i <= 10; i++) {
			sprintf(filename, "Configurations/configuration%d.txt", i);
			Simulator simulator;
			simulator.read_input_from_file(filename);
			simulator.change_K(2);
			simulator.change_timestamp_jump(fixed_t);
			for(int j = 0; j < 100; j++) {
				simulator.initialize(n_fail);
				if(simulator.maintain()) {
					simulator.evaluate_results();
					avg_distance.push_back(simulator.get_avg_distance());
					max_distance.push_back(simulator.get_max_distance());
					timestamp.push_back(simulator.get_final_timestamp());
					n_moved.push_back(simulator.get_no_of_sensors_moved());
				} else {
					failed_simulation++;
				}
			}
			simulator.delete_data();
		}
		avg_distance2K.push_back(mean<double>(avg_distance));
		max_distance2K.push_back(mean<double>(max_distance));
		timestamp2K.push_back(mean<int>(timestamp));
		n_moved2K.push_back(mean<int>(n_moved));
		avg_distance2K_err.push_back(sd<double>(avg_distance, avg_distance2K.back()));
		max_distance2K_err.push_back(sd<double>(max_distance, max_distance2K.back()));
		timestamp2K_err.push_back(sd<int>(timestamp, timestamp2K.back()));
		n_moved2K_err.push_back(sd<int>(n_moved, n_moved2K.back()));
		accuracy2K.push_back(((double)(1000 - failed_simulation)) / 1000.0);

		// K = 3
		avg_distance.clear();
		max_distance.clear();
		timestamp.clear();
		n_moved.clear();
		failed_simulation = 0;
		for(int i = 1; i <= 10; i++) {
			sprintf(filename, "Configurations/configuration%d.txt", i);
			Simulator simulator;
			simulator.read_input_from_file(filename);
			simulator.change_K(3);
			simulator.change_timestamp_jump(fixed_t);
			for(int j = 0; j < 100; j++) {
				simulator.initialize(n_fail);
				if(simulator.maintain()) {
					simulator.evaluate_results();
					avg_distance.push_back(simulator.get_avg_distance());
					max_distance.push_back(simulator.get_max_distance());
					timestamp.push_back(simulator.get_final_timestamp());
					n_moved.push_back(simulator.get_no_of_sensors_moved());
				} else {
					failed_simulation++;
				}
			}
			simulator.delete_data();
		}
		avg_distance3K.push_back(mean<double>(avg_distance));
		max_distance3K.push_back(mean<double>(max_distance));
		timestamp3K.push_back(mean<int>(timestamp));
		n_moved3K.push_back(mean<int>(n_moved));
		avg_distance3K_err.push_back(sd<double>(avg_distance, avg_distance3K.back()));
		max_distance3K_err.push_back(sd<double>(max_distance, max_distance3K.back()));
		timestamp3K_err.push_back(sd<int>(timestamp, timestamp3K.back()));
		n_moved3K_err.push_back(sd<int>(n_moved, n_moved3K.back()));
		accuracy3K.push_back(((double)(1000 - failed_simulation)) / 1000.0);

		// K = 4
		avg_distance.clear();
		max_distance.clear();
		timestamp.clear();
		n_moved.clear();
		failed_simulation = 0;
		for(int i = 1; i <= 10; i++) {
			sprintf(filename, "Configurations/configuration%d.txt", i);
			Simulator simulator;
			simulator.read_input_from_file(filename);
			simulator.change_K(4);
			simulator.change_timestamp_jump(fixed_t);
			for(int j = 0; j < 100; j++) {
				simulator.initialize(n_fail);
				if(simulator.maintain()) {
					simulator.evaluate_results();
					avg_distance.push_back(simulator.get_avg_distance());
					max_distance.push_back(simulator.get_max_distance());
					timestamp.push_back(simulator.get_final_timestamp());
					n_moved.push_back(simulator.get_no_of_sensors_moved());
				} else {
					failed_simulation++;
				}
			}
			simulator.delete_data();
		}
		avg_distance4K.push_back(mean<double>(avg_distance));
		max_distance4K.push_back(mean<double>(max_distance));
		timestamp4K.push_back(mean<int>(timestamp));
		n_moved4K.push_back(mean<int>(n_moved));
		avg_distance4K_err.push_back(sd<double>(avg_distance, avg_distance4K.back()));
		max_distance4K_err.push_back(sd<double>(max_distance, max_distance4K.back()));
		timestamp4K_err.push_back(sd<int>(timestamp, timestamp4K.back()));
		n_moved4K_err.push_back(sd<int>(n_moved, n_moved4K.back()));
		accuracy4K.push_back(((double)(1000 - failed_simulation)) / 1000.0);

		// K = 5
		avg_distance.clear();
		max_distance.clear();
		timestamp.clear();
		n_moved.clear();
		failed_simulation = 0;
		for(int i = 1; i <= 10; i++) {
			sprintf(filename, "Configurations/configuration%d.txt", i);
			Simulator simulator;
			simulator.read_input_from_file(filename);
			simulator.change_K(5);
			simulator.change_timestamp_jump(fixed_t);
			for(int j = 0; j < 100; j++) {
				simulator.initialize(n_fail);
				if(simulator.maintain()) {
					simulator.evaluate_results();
					avg_distance.push_back(simulator.get_avg_distance());
					max_distance.push_back(simulator.get_max_distance());
					timestamp.push_back(simulator.get_final_timestamp());
					n_moved.push_back(simulator.get_no_of_sensors_moved());
				} else {
					failed_simulation++;
				}
			}
			simulator.delete_data();
		}
		avg_distance5K.push_back(mean<double>(avg_distance));
		max_distance5K.push_back(mean<double>(max_distance));
		timestamp5K.push_back(mean<int>(timestamp));
		n_moved5K.push_back(mean<int>(n_moved));
		avg_distance5K_err.push_back(sd<double>(avg_distance, avg_distance5K.back()));
		max_distance5K_err.push_back(sd<double>(max_distance, max_distance5K.back()));
		timestamp5K_err.push_back(sd<int>(timestamp, timestamp5K.back()));
		n_moved5K_err.push_back(sd<int>(n_moved, n_moved5K.back()));
		accuracy5K.push_back(((double)(1000 - failed_simulation)) / 1000.0);
	}

	// Print Results
	//K = 1
	print_double_vect(avg_distance1K);
	print_double_vect(avg_distance1K_err);
	print_double_vect(max_distance1K);
	print_double_vect(max_distance1K_err);
	print_double_vect(timestamp1K);
	print_double_vect(timestamp1K_err);
	print_double_vect(n_moved1K);
	print_double_vect(n_moved1K_err);
	print_double_vect(accuracy1K);
	//K = 2
	print_double_vect(avg_distance2K);
	print_double_vect(avg_distance2K_err);
	print_double_vect(max_distance2K);
	print_double_vect(max_distance2K_err);
	print_double_vect(timestamp2K);
	print_double_vect(timestamp2K_err);
	print_double_vect(n_moved2K);
	print_double_vect(n_moved2K_err);
	print_double_vect(accuracy2K);
	//K = 3
	print_double_vect(avg_distance3K);
	print_double_vect(avg_distance3K_err);
	print_double_vect(max_distance3K);
	print_double_vect(max_distance3K_err);
	print_double_vect(timestamp3K);
	print_double_vect(timestamp3K_err);
	print_double_vect(n_moved3K);
	print_double_vect(n_moved3K_err);
	print_double_vect(accuracy3K);
	//K = 4
	print_double_vect(avg_distance4K);
	print_double_vect(avg_distance4K_err);
	print_double_vect(max_distance4K);
	print_double_vect(max_distance4K_err);
	print_double_vect(timestamp4K);
	print_double_vect(timestamp4K_err);
	print_double_vect(n_moved4K);
	print_double_vect(n_moved4K_err);
	print_double_vect(accuracy4K);
	//K = 5
	print_double_vect(avg_distance5K);
	print_double_vect(avg_distance5K_err);
	print_double_vect(max_distance5K);
	print_double_vect(max_distance5K_err);
	print_double_vect(timestamp5K);
	print_double_vect(timestamp5K_err);
	print_double_vect(n_moved5K);
	print_double_vect(n_moved5K_err);
	print_double_vect(accuracy5K);
	return 0;
}
