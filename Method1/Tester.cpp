#include <cstdio>
#include "Simulator.hpp"

using namespace std;

int main(int argc, char const *argv[])
{
	Simulator simulator;
	int n;
	while(true)
	{
		printf("Enter the number of sensors \n");
		scanf("%d",&n);
		if(n <= 0 || n > 1000)
		{
			break;
		}
		simulator.setSensorCount(n);
		simulator.simulate();
	}
	// simulator.setSensorCount(30);
	return 0;
}


