#include <cstdio>
#include "Simulator.hpp"
#include <fstream>

using namespace std;

void saveDataIntoFile(Simulator &simulator, double avgAverageSensorMovement, double avgIterationCount, int numberOfSimulations)
{
	ofstream outFile;
	outFile.open("Simulations_Log.txt", ios::app);
	if(outFile.is_open())
	{
		// Write data here
		outFile << "Number of Simulations : " << numberOfSimulations << endl;
		outFile << "Number of Sensors : " << simulator.getSensorCount() << endl;
		outFile << "Belt Dimensions : (" << simulator.getBeltWidth() << "," << simulator.getBeltHeight() << ")" << endl;
		outFile << "Average Avg. Sensor Movement : " << avgAverageSensorMovement << endl;
		outFile << "Average Iteration Count : " << avgIterationCount << endl;
		outFile << endl;
	}
	outFile.close();
}

int main()
{
	double avgAverageSensorMovement = 0.0;
	double avgIterationCount = 0;
	/*Ask Number of Sensors
	Belt Dimensions*/
	Simulator simulator;
	simulator.setViewerMode(false);
	simulator.saveImages(false);
	int n,count,i;
	printf("Enter the number of Sensors for this simulation\n");
	scanf("%d",&n);
	printf("Enter the number of times you want to carry out this simulation:\n");
	scanf("%d",&count);
	simulator.setSensorCount(n);
	for(i = 0; i < count; i++)
	{
		simulator.randomizeSensors();
		simulator.simulate();
		avgAverageSensorMovement += simulator.getAverageSensorMovement();
		avgIterationCount += simulator.getNumberOfIterations();
		printf("%d : Avg Movement = %lf Iteration = %d\n", simulator.getSimulationCount(), simulator.getAverageSensorMovement(), simulator.getNumberOfIterations());
	}
	//Normalize data
	avgAverageSensorMovement /= (double)count;
	avgIterationCount /= (double)count;
	saveDataIntoFile(simulator,avgAverageSensorMovement,avgIterationCount,count);
	return 0;
}