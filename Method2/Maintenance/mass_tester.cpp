#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include "Simulator.hpp"

using namespace std;

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
	vector<double> avg_distance5;
	vector<double> max_distance5;
	vector<int> timestamp5;
	vector<int> iterations5;
	int failed_test5 = 0;
	vector<double> avg_distance2;
	vector<double> max_distance2;
	vector<int> timestamp2;
	vector<int> iterations2;
	int failed_test2 = 0;

	srand(time(NULL));

	// Create a simulator object

	// We have 10 configurations files
	// For each file we will run 10 test cases for both 5 and 2 failed sensors separately
	for(int i = 1; i <= 10; i++) {
		sprintf(filename, "Configurations/configuration%d.txt", i);
		Simulator simulator;
		simulator.read_input_from_file(filename);
		for(int j = 0; j < 100; j++) {
			simulator.initialize(5);
			if(simulator.maintain()) {
				simulator.evaluate_results();
				avg_distance5.push_back(simulator.get_avg_distance());
				max_distance5.push_back(simulator.get_max_distance());
				timestamp5.push_back(simulator.get_final_timestamp());
				iterations5.push_back(simulator.get_final_iterations());
			} else {
				failed_test5++;
			}
		}

		for(int j = 0; j < 100; j++) {
			simulator.initialize(2);
			if(simulator.maintain()) {
				simulator.evaluate_results();
				avg_distance2.push_back(simulator.get_avg_distance());
				max_distance2.push_back(simulator.get_max_distance());
				timestamp2.push_back(simulator.get_final_timestamp());
				iterations2.push_back(simulator.get_final_iterations());
			} else {
				failed_test2++;
			}
		}
		printf("Failed5: %d, Failed2: %d\n", failed_test5, failed_test2);
		simulator.delete_data();
	}
	printf("Printing Final Results\n");
	printf("Results for 5\n");
	double mean_avg_distance5 = mean<double>(avg_distance5);
	double mean_max_distance5 = mean<double>(max_distance5);
	double mean_timestamp5 = mean<int>(timestamp5);
	double mean_iterations5 = mean<int>(iterations5);
	double sd_avg_distance5 = sd<double>(avg_distance5, mean_avg_distance5);
	double sd_max_distance5 = sd<double>(max_distance5, mean_max_distance5);
	double sd_timestamp5 = sd<int>(timestamp5, mean_timestamp5);
	double sd_iterations5 = sd<int>(iterations5, mean_iterations5);
	printf("%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n", mean_avg_distance5, mean_max_distance5, mean_timestamp5, mean_iterations5, sd_avg_distance5, sd_max_distance5, sd_timestamp5, sd_iterations5);
	printf("Failed Tests: %d\n", failed_test5);
	// printf("Size of the avg_distance5: %d\n", (int)avg_distance5.size());
	// print_double_vect(avg_distance5);
	// printf("Size of the max_distance5: %d\n", (int)max_distance5.size());
	// print_double_vect(max_distance5);
	// printf("Size of the timestamp5: %d\n", (int)timestamp5.size());
	// print_int_vect(timestamp5);
	// printf("Size of the iterations5: %d\n", (int)iterations5.size());
	// print_int_vect(iterations5);


	printf("\n\n\n\nResults for 2\n");
	double mean_avg_distance2 = mean<double>(avg_distance2);
	double mean_max_distance2 = mean<double>(max_distance2);
	double mean_timestamp2 = mean<int>(timestamp2);
	double mean_iterations2 = mean<int>(iterations2);
	double sd_avg_distance2 = sd<double>(avg_distance2, mean_avg_distance2);
	double sd_max_distance2 = sd<double>(max_distance2, mean_max_distance2);
	double sd_timestamp2 = sd<int>(timestamp2, mean_timestamp2);
	double sd_iterations2 = sd<int>(iterations2, mean_iterations2);
	printf("%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n%lf\n", mean_avg_distance2, mean_max_distance2, mean_timestamp2, mean_iterations2, sd_avg_distance2, sd_max_distance2, sd_timestamp2, sd_iterations2);
	printf("Failed Tests: %d\n", failed_test2);
	// printf("Size of the avg_distance2: %d\n", (int)avg_distance2.size());
	// print_double_vect(avg_distance2);
	// printf("Size of the max_distance2: %d\n", (int)max_distance2.size());
	// print_double_vect(max_distance2);
	// printf("Size of the timestamp2: %d\n", (int)timestamp2.size());
	// print_int_vect(timestamp2);
	// printf("Size of the iterations2: %d\n", (int)iterations2.size());
	// print_int_vect(iterations2);


	// simulator.delete_data();	
	return 0;
}