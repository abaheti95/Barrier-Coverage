#ifndef GEOMETRIC_GRAPH_HPP
#define GEOMETRIC_GRAPH_HPP

#include <cstdio>
#include <iostream>
#include <vector>
#include <algorithm>
#include "Coordinate.hpp"

#define DEFAULT_SENSING_RANGE 0.4
#define DEFAULT_BELT_WIDTH 10.0
#define MAX_SENSING_RANGE 2.5
#define MAX_BELT_WIDTH 100.0
#define DELTA 0.0001

using namespace std;

struct SensorChain
{
	double startX;
	double endX;
	int numberOfSensors;
	SensorChain():startX(INFINITY), endX(0.0), numberOfSensors(0) {}
	SensorChain(const SensorChain&);
	~SensorChain() {}

	bool within(const SensorChain&);
};

struct SensorChainComparator
{
	bool operator()(const SensorChain &left, const SensorChain &right)
	{
			return left.startX < right.startX;
	}
};


class GeometricGraph 
{
private:
	int _k;								// Denotes the k sensing coverage
	double _sensingRange;			// Sensing range of the sensors
	double _beltWidth;			// Width of the belt
	double _delta;				// Shift if clash of X coordinate
	int _numberOfCriticalRegions;		// Number of critical regions present in the current arrangement
	double _quality;					// Quality of the barrier as per the standard definition
	double _maxPassageDistance;			// My definition of quality
	double _verticalCenterOfMass;		// Y coordinate of center of mass
	vector<Coordinate> _coordinates;	// Vector of all the coordinates that denotes the sensors
	vector<vector<int> > _graph;		// Graph of all the connected coordinates
	vector<SensorChain> _sensorChains;

	void clearCoordinates();			// Function that empties coordinate vector
	void sortCoordinates();				// A function that sorts the coordinates based on the x coordinate
	void preprocessCoordinates();		// Function that sorts and removes conflits in x-coordinate
	void clearGraph();					// Funciton that clears the graph
	void connectCoordinates(int, int);	// Simply creates a undirected edge between the two coordinates
	void generateGraph();				// Fuction that actually generates graph
	bool BFS(vector<vector<int> >& sensorChainGraph, bool visited[], int startCoordinate, int n, SensorChain& chain);
	void calculateQuality();			// A function that calculates quality based on the standard definition
public:
	GeometricGraph(double = DEFAULT_SENSING_RANGE, double = DEFAULT_BELT_WIDTH);
	~GeometricGraph();
	void setSensingRange(double);
	void setBeltWidth(double);
	int getNumberOfCriticalRegions();
	double getQuality();
	double getNewQuality();
	double getMaxPassageDistance();
	double getVerticalCenterOfMass();
	void setCoordinates(const vector<Coordinate>&);
	vector<Coordinate>& getCoordinates();
	vector<vector<int> >& getGraph();

	void printGeometricGraph();
};

#endif
