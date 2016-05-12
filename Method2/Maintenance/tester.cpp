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

	// Initializing the display window
	simulator.read_input_data();
	simulator.initialize(5);

	simulator.maintain();
	simulator.print_results();
	simulator.delete_data();
	
	return 0;
}