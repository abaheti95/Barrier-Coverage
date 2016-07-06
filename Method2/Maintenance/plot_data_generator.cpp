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
K = 2, t = 5
K = 2, t = 10
K = 2, t = 50
K = 2, t = 100
K = 5, t = 5
K = 5, t = 10
K = 5, t = 50
K = 5, t = 100*/

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

	vector<double> avg_distance2K5t;
	vector<double> avg_distance2K10t;
	vector<double> avg_distance2K50t;
	vector<double> avg_distance2K100t;
	vector<double> avg_distance5K5t;
	vector<double> avg_distance5K10t;
	vector<double> avg_distance5K50t;
	vector<double> avg_distance5K100t;
	vector<double> max_distance2K5t;
	vector<double> max_distance2K10t;
	vector<double> max_distance2K50t;
	vector<double> max_distance2K100t;
	vector<double> max_distance5K5t;
	vector<double> max_distance5K10t;
	vector<double> max_distance5K50t;
	vector<double> max_distance5K100t;
	vector<double> timestamp2K5t;
	vector<double> timestamp2K10t;
	vector<double> timestamp2K50t;
	vector<double> timestamp2K100t;
	vector<double> timestamp5K5t;
	vector<double> timestamp5K10t;
	vector<double> timestamp5K50t;
	vector<double> timestamp5K100t;
	vector<double> n_moved2K5t;
	vector<double> n_moved2K10t;
	vector<double> n_moved2K50t;
	vector<double> n_moved2K100t;
	vector<double> n_moved5K5t;
	vector<double> n_moved5K10t;
	vector<double> n_moved5K50t;
	vector<double> n_moved5K100t;

	vector<double> accuracy2K5t;
	vector<double> accuracy2K10t;
	vector<double> accuracy2K50t;
	vector<double> accuracy2K100t;
	vector<double> accuracy5K5t;
	vector<double> accuracy5K10t;
	vector<double> accuracy5K50t;
	vector<double> accuracy5K100t;

	vector<double> avg_distance2K5t_err;
	vector<double> avg_distance2K10t_err;
	vector<double> avg_distance2K50t_err;
	vector<double> avg_distance2K100t_err;
	vector<double> avg_distance5K5t_err;
	vector<double> avg_distance5K10t_err;
	vector<double> avg_distance5K50t_err;
	vector<double> avg_distance5K100t_err;
	vector<double> max_distance2K5t_err;
	vector<double> max_distance2K10t_err;
	vector<double> max_distance2K50t_err;
	vector<double> max_distance2K100t_err;
	vector<double> max_distance5K5t_err;
	vector<double> max_distance5K10t_err;
	vector<double> max_distance5K50t_err;
	vector<double> max_distance5K100t_err;
	vector<double> timestamp2K5t_err;
	vector<double> timestamp2K10t_err;
	vector<double> timestamp2K50t_err;
	vector<double> timestamp2K100t_err;
	vector<double> timestamp5K5t_err;
	vector<double> timestamp5K10t_err;
	vector<double> timestamp5K50t_err;
	vector<double> timestamp5K100t_err;
	vector<double> n_moved2K5t_err;
	vector<double> n_moved2K10t_err;
	vector<double> n_moved2K50t_err;
	vector<double> n_moved2K100t_err;
	vector<double> n_moved5K5t_err;
	vector<double> n_moved5K10t_err;
	vector<double> n_moved5K50t_err;
	vector<double> n_moved5K100t_err;


	for(int n_fail = 1; n_fail <= 5; n_fail++) {
		// printf("Number of Failed Sensors = %d\n", n_fail);
		// 1000 simulations for each configuration

		// K = 2, t = 5
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
			simulator.change_timestamp_jump(5);
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
		avg_distance2K5t.push_back(mean<double>(avg_distance));
		max_distance2K5t.push_back(mean<double>(max_distance));
		timestamp2K5t.push_back(mean<int>(timestamp));
		n_moved2K5t.push_back(mean<int>(n_moved));
		avg_distance2K5t_err.push_back(sd<double>(avg_distance, avg_distance2K5t.back()));
		max_distance2K5t_err.push_back(sd<double>(max_distance, max_distance2K5t.back()));
		timestamp2K5t_err.push_back(sd<int>(timestamp, timestamp2K5t.back()));
		n_moved2K5t_err.push_back(sd<int>(n_moved, n_moved2K5t.back()));
		accuracy2K5t.push_back(((double)(1000 - failed_simulation)) / 1000.0);

		// K = 2, t = 10
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
			simulator.change_timestamp_jump(10);
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
		avg_distance2K10t.push_back(mean<double>(avg_distance));
		max_distance2K10t.push_back(mean<double>(max_distance));
		timestamp2K10t.push_back(mean<int>(timestamp));
		n_moved2K10t.push_back(mean<int>(n_moved));
		avg_distance2K10t_err.push_back(sd<double>(avg_distance, avg_distance2K10t.back()));
		max_distance2K10t_err.push_back(sd<double>(max_distance, max_distance2K10t.back()));
		timestamp2K10t_err.push_back(sd<int>(timestamp, timestamp2K10t.back()));
		n_moved2K10t_err.push_back(sd<int>(n_moved, n_moved2K10t.back()));
		accuracy2K10t.push_back(((double)(1000 - failed_simulation)) / 1000.0);

		// K = 2, t = 50
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
			simulator.change_timestamp_jump(50);
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
		avg_distance2K50t.push_back(mean<double>(avg_distance));
		max_distance2K50t.push_back(mean<double>(max_distance));
		timestamp2K50t.push_back(mean<int>(timestamp));
		n_moved2K50t.push_back(mean<int>(n_moved));
		avg_distance2K50t_err.push_back(sd<double>(avg_distance, avg_distance2K50t.back()));
		max_distance2K50t_err.push_back(sd<double>(max_distance, max_distance2K50t.back()));
		timestamp2K50t_err.push_back(sd<int>(timestamp, timestamp2K50t.back()));
		n_moved2K50t_err.push_back(sd<int>(n_moved, n_moved2K50t.back()));
		accuracy2K50t.push_back(((double)(1000 - failed_simulation)) / 1000.0);

		// K = 2, t = 100
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
			simulator.change_timestamp_jump(100);
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
		avg_distance2K100t.push_back(mean<double>(avg_distance));
		max_distance2K100t.push_back(mean<double>(max_distance));
		timestamp2K100t.push_back(mean<int>(timestamp));
		n_moved2K100t.push_back(mean<int>(n_moved));
		avg_distance2K100t_err.push_back(sd<double>(avg_distance, avg_distance2K100t.back()));
		max_distance2K100t_err.push_back(sd<double>(max_distance, max_distance2K100t.back()));
		timestamp2K100t_err.push_back(sd<int>(timestamp, timestamp2K100t.back()));
		n_moved2K100t_err.push_back(sd<int>(n_moved, n_moved2K100t.back()));
		accuracy2K100t.push_back(((double)(1000 - failed_simulation)) / 1000.0);

		// K = 5, t = 5
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
			simulator.change_timestamp_jump(5);
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
		avg_distance5K5t.push_back(mean<double>(avg_distance));
		max_distance5K5t.push_back(mean<double>(max_distance));
		timestamp5K5t.push_back(mean<int>(timestamp));
		n_moved5K5t.push_back(mean<int>(n_moved));
		avg_distance5K5t_err.push_back(sd<double>(avg_distance, avg_distance5K5t.back()));
		max_distance5K5t_err.push_back(sd<double>(max_distance, max_distance5K5t.back()));
		timestamp5K5t_err.push_back(sd<int>(timestamp, timestamp5K5t.back()));
		n_moved5K5t_err.push_back(sd<int>(n_moved, n_moved5K5t.back()));
		accuracy5K5t.push_back(((double)(1000 - failed_simulation)) / 1000.0);

		// K = 5, t = 10
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
			simulator.change_timestamp_jump(10);
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
		avg_distance5K10t.push_back(mean<double>(avg_distance));
		max_distance5K10t.push_back(mean<double>(max_distance));
		timestamp5K10t.push_back(mean<int>(timestamp));
		n_moved5K10t.push_back(mean<int>(n_moved));
		avg_distance5K10t_err.push_back(sd<double>(avg_distance, avg_distance5K10t.back()));
		max_distance5K10t_err.push_back(sd<double>(max_distance, max_distance5K10t.back()));
		timestamp5K10t_err.push_back(sd<int>(timestamp, timestamp5K10t.back()));
		n_moved5K10t_err.push_back(sd<int>(n_moved, n_moved5K10t.back()));
		accuracy5K10t.push_back(((double)(1000 - failed_simulation)) / 1000.0);

		// K = 5, t = 50
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
			simulator.change_timestamp_jump(50);
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
		avg_distance5K50t.push_back(mean<double>(avg_distance));
		max_distance5K50t.push_back(mean<double>(max_distance));
		timestamp5K50t.push_back(mean<int>(timestamp));
		n_moved5K50t.push_back(mean<int>(n_moved));
		avg_distance5K50t_err.push_back(sd<double>(avg_distance, avg_distance5K50t.back()));
		max_distance5K50t_err.push_back(sd<double>(max_distance, max_distance5K50t.back()));
		timestamp5K50t_err.push_back(sd<int>(timestamp, timestamp5K50t.back()));
		n_moved5K50t_err.push_back(sd<int>(n_moved, n_moved5K50t.back()));
		accuracy5K50t.push_back(((double)(1000 - failed_simulation)) / 1000.0);

		// K = 5, t = 100
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
			simulator.change_timestamp_jump(100);
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
		avg_distance5K100t.push_back(mean<double>(avg_distance));
		max_distance5K100t.push_back(mean<double>(max_distance));
		timestamp5K100t.push_back(mean<int>(timestamp));
		n_moved5K100t.push_back(mean<int>(n_moved));
		avg_distance5K100t_err.push_back(sd<double>(avg_distance, avg_distance5K100t.back()));
		max_distance5K100t_err.push_back(sd<double>(max_distance, max_distance5K100t.back()));
		timestamp5K100t_err.push_back(sd<int>(timestamp, timestamp5K100t.back()));
		n_moved5K100t_err.push_back(sd<int>(n_moved, n_moved5K100t.back()));
		accuracy5K100t.push_back(((double)(1000 - failed_simulation)) / 1000.0);
	}

	// Print Results
	//K = 2, t = 5
	print_double_vect(avg_distance2K5t);
	print_double_vect(avg_distance2K5t_err);
	print_double_vect(max_distance2K5t);
	print_double_vect(max_distance2K5t_err);
	print_double_vect(timestamp2K5t);
	print_double_vect(timestamp2K5t_err);
	print_double_vect(n_moved2K5t);
	print_double_vect(n_moved2K5t_err);
	print_double_vect(accuracy2K5t);
	//K = 2, t = 10
	print_double_vect(avg_distance2K10t);
	print_double_vect(avg_distance2K10t_err);
	print_double_vect(max_distance2K10t);
	print_double_vect(max_distance2K10t_err);
	print_double_vect(timestamp2K10t);
	print_double_vect(timestamp2K10t_err);
	print_double_vect(n_moved2K10t);
	print_double_vect(n_moved2K10t_err);
	print_double_vect(accuracy2K10t);
	//K = 2, t = 50
	print_double_vect(avg_distance2K50t);
	print_double_vect(avg_distance2K50t_err);
	print_double_vect(max_distance2K50t);
	print_double_vect(max_distance2K50t_err);
	print_double_vect(timestamp2K50t);
	print_double_vect(timestamp2K50t_err);
	print_double_vect(n_moved2K50t);
	print_double_vect(n_moved2K50t_err);
	print_double_vect(accuracy2K50t);
	//K = 2, t = 100
	print_double_vect(avg_distance2K100t);
	print_double_vect(avg_distance2K100t_err);
	print_double_vect(max_distance2K100t);
	print_double_vect(max_distance2K100t_err);
	print_double_vect(timestamp2K100t);
	print_double_vect(timestamp2K100t_err);
	print_double_vect(n_moved2K100t);
	print_double_vect(n_moved2K100t_err);
	print_double_vect(accuracy2K100t);
	//K = 5, t = 5
	print_double_vect(avg_distance5K5t);
	print_double_vect(avg_distance5K5t_err);
	print_double_vect(max_distance5K5t);
	print_double_vect(max_distance5K5t_err);
	print_double_vect(timestamp5K5t);
	print_double_vect(timestamp5K5t_err);
	print_double_vect(n_moved5K5t);
	print_double_vect(n_moved5K5t_err);
	print_double_vect(accuracy5K5t);
	//K = 5, t = 10
	print_double_vect(avg_distance5K10t);
	print_double_vect(avg_distance5K10t_err);
	print_double_vect(max_distance5K10t);
	print_double_vect(max_distance5K10t_err);
	print_double_vect(timestamp5K10t);
	print_double_vect(timestamp5K10t_err);
	print_double_vect(n_moved5K10t);
	print_double_vect(n_moved5K10t_err);
	print_double_vect(accuracy5K10t);
	//K = 5, t = 50
	print_double_vect(avg_distance5K50t);
	print_double_vect(avg_distance5K50t_err);
	print_double_vect(max_distance5K50t);
	print_double_vect(max_distance5K50t_err);
	print_double_vect(timestamp5K50t);
	print_double_vect(timestamp5K50t_err);
	print_double_vect(n_moved5K50t);
	print_double_vect(n_moved5K50t_err);
	print_double_vect(accuracy5K50t);
	//K = 5, t = 100
	print_double_vect(avg_distance5K100t);
	print_double_vect(avg_distance5K100t_err);
	print_double_vect(max_distance5K100t);
	print_double_vect(max_distance5K100t_err);
	print_double_vect(timestamp5K100t);
	print_double_vect(timestamp5K100t_err);
	print_double_vect(n_moved5K100t);
	print_double_vect(n_moved5K100t_err);
	print_double_vect(accuracy5K100t);
	return 0;
}
