#include <cstdio>
#include <cstdlib>
#include <vector>
#include "Simulator.hpp"

using namespace std;

int main(int argc, char *argv[])
{
	srand(time(NULL));

	// Create a simulator object
	Simulator simulator;

	simulator.read_input_from_file("configuration.txt");

	simulator.initialize(5);
	simulator.change_K(4);
	simulator.maintain();
	simulator.evaluate_results();
	simulator.print_results();

	simulator.initialize(2);
	simulator.change_K(4);
	simulator.maintain();
	simulator.evaluate_results();
	simulator.print_results();

	simulator.delete_data();	
	return 0;
}