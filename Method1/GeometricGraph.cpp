#include "GeometricGraph.hpp"
#include <queue>

using namespace std;

SensorChain::SensorChain(const SensorChain& copySensorChain)
{
	startX = copySensorChain.startX;
	endX = copySensorChain.endX;
	numberOfSensors = copySensorChain.numberOfSensors;
}

bool SensorChain::within(const SensorChain& innerSensorChain)
{
	return ((startX <= innerSensorChain.startX) && (innerSensorChain.endX <= endX));
}

/****** CLASS GEOMETRIC GRAPH *****/
/*Constructors and Destructors*/
// Paramterized Constructor
GeometricGraph::GeometricGraph(double sensingRange, double beltWidth)
:_sensingRange(sensingRange),_beltWidth(beltWidth)
{
	_numberOfCriticalRegions = 0;
	_quality = 0.0;
	_maxPassageDistance = 0.0;
	_verticalCenterOfMass = 0.0;
	_delta = DELTA;
}
// Destructor
GeometricGraph::~GeometricGraph()
{
	clearGraph();
	clearCoordinates();
}

/*Private utility functions*/
void GeometricGraph::clearCoordinates()
{
	_coordinates.clear();
}

void GeometricGraph::sortCoordinates()
{
	// Simply sort the coordinates based on the x coordinates
	sort(_coordinates.begin(), _coordinates.end(), XCoordinateComparator());
}

void GeometricGraph::preprocessCoordinates()
{
	int i,j;				// Interating Variable
	double previousX = 0.0;
	// remove duplicates and sort
	sortCoordinates();
	for(i = 0; i < (int)_coordinates.size(); i++)
	{
		j = i;
		while(j < (int)_coordinates.size() && previousX == _coordinates[j].x)
		{
			//shift current coordinate by delta amount
			_coordinates[j++].x += _delta;
		}
		previousX = _coordinates[i].x;
	}
	// TODO: handle edge case on the right side
}

void GeometricGraph::clearGraph()
{
	int i;				// Interating variables
	// clear existing graph if not empty
	for(i = 0; i < (int)_graph.size(); i++)
	{
		_graph[i].clear();
	}
}

void GeometricGraph::connectCoordinates(int first, int second)
{
	_graph[first].push_back(second);
	_graph[second].push_back(first);
}

// We also calculate the vertical center of mass in this function
void GeometricGraph::generateGraph()
{
	double diameter = 2.0 * _sensingRange;
	int i,j;			// Iterating variables
	int n = _coordinates.size();	// number of coordinates

	// Clear existing graph if present
	clearGraph();
	//Traverse the coordinate and generate fresh graph
	_verticalCenterOfMass = 0.0;
	for(i = 0; i < n-1; i++)
	{
		_verticalCenterOfMass += _coordinates[i].y;
		// Directly connect with the next vertex without any checking
		connectCoordinates(i,i+1);
		for(j = i+2; j < n; j++)
		{
			if( coordinateDistance(_coordinates[i],_coordinates[j]) <= diameter )
			{
				connectCoordinates(i,j);
			}
			else if((_coordinates[j].x - _coordinates[i].x) > diameter)
			{
				// early termination
				break;
			}
		}
	}
	// Add y coordinate of the last coordinate
	_verticalCenterOfMass += _coordinates[_coordinates.size() - 1].y;
	// Take average to get the vertical center of mass
	_verticalCenterOfMass /= (double)_coordinates.size();
}


bool GeometricGraph::BFS(vector<vector<int> >& sensorChainGraph, bool visited[], int startCoordinate, int n, SensorChain& chain)
{
	int i;					// Iterating variable
	queue<int> q;
	if(visited[startCoordinate])
	{
		return false;
	}
	q.push(startCoordinate);
	while(!q.empty())
	{
		int current = q.front();
		q.pop();
		//Update SensorChain Data
		visited[current] = true;
		chain.startX = min(_coordinates[current].x, chain.startX);
		chain.endX = max(_coordinates[current].x, chain.endX);
		chain.numberOfSensors++;
		for(i = 0; i < (int)sensorChainGraph[current].size(); i++)
		{
			if(!visited[sensorChainGraph[current][i]])
			{
				q.push(sensorChainGraph[current][i]);
			}
		}
	}
	return true;
}

void GeometricGraph::printGeometricGraph()
{
	int i,j;
	printf("Original Graph\n");
	for(i = 0; i < (int)_graph.size(); i++)
	{
		printf("%d : ",i);
		for(j = 0; j < (int)_graph[i].size(); j++)
		{
			printf("%d ",_graph[i][j]);
		}
		printf("\n");
	}
}

void GeometricGraph::calculateQuality()
{
	int i,j;			// Iterating variables
	bool visited[_graph.size()];
	for(i = 0; i < (int)_graph.size(); i++)
	{
		visited[i] = false;
	}
	vector<vector<int> > sensorChainGraph;
	sensorChainGraph.resize(_graph.size());
	// Generate dummy graph from the orignal fully connected graph
	// The dummy graph will only contain edges which are smaller than the diameter
	double diameter = 2.0 * _sensingRange;
	//printGeometricGraph();
	//printf("DummyGraph\n");
	for(i = 0; i < (int)_graph.size(); i++)
	{
		visited[i] = true;
		//printf("%d : ",i);
		for(j = 0; j < (int)_graph[i].size(); j++)
		{
			if(visited[_graph[i][j]])
			{
				continue;
			}
			// printf("Distance between %d %d = %lf\n", i,_graph[i][j],coordinateDistance(_coordinates[i],_coordinates[_graph[i][j]]));
			if(coordinateDistance(_coordinates[i],_coordinates[_graph[i][j]]) <= diameter)
			{
				sensorChainGraph[i].push_back(_graph[i][j]);
				sensorChainGraph[_graph[i][j]].push_back(i);
				//printf("%d ",_graph[i][j]);
			}
		}
		//printf("\n");
	}
	// Generate Segments from components of sensor chain graph
	// For this we will perform multiple BFS on the graph
	for(i = 0; i < (int)_graph.size(); i++)
	{
		visited[i] = false;
	}
	_sensorChains.clear();
	for(i = 0; i < (int)_graph.size(); i++)
	{
		SensorChain temp;
		if(BFS(sensorChainGraph, visited, i, _graph.size(), temp))
		{
			// if any sensorChain's area is already covered by another sensorChain then simple reject this sensorChain
			bool flag = true;
			for(j = 0; j < (int)_sensorChains.size(); j++)
			{
				if(_sensorChains[i].within(temp))
				{
					flag = false;
				}
			}
			if(flag)
			{
				_sensorChains.push_back(temp);
			}
		}
	}
	// Process the sensorChiain list and extract information
	// Sort sensor chains according to startX values
	sort(_sensorChains.begin(), _sensorChains.end(), SensorChainComparator());
	bool weakBarrierCovered = true;
	double minimumCriticalRegion = INFINITY;
	int numberOfCriticalRegions = 0;
	// First sensorChain should be overlapping with left edge
	if((_sensorChains[0].startX - _sensingRange) <= 0.0)
	{
		weakBarrierCovered = false;
	}
	for(i = 0; i < (int)_sensorChains.size() - 1; i++)
	{
		double overlap = _sensorChains[i].endX - _sensorChains[i+1].startX;
		if(overlap < 0.0)
		{
			weakBarrierCovered = false;
		}
		else
		{
			minimumCriticalRegion = min(minimumCriticalRegion, overlap);
			numberOfCriticalRegions++;
		}
	}
	// Last sensorChain should be overlapping with the right edge
	int lastSensor = _sensorChains.size() - 1;
	if((_sensorChains[lastSensor].endX + _sensingRange) < _beltWidth)
	{
		weakBarrierCovered = false;
	}
	//printf("Number of SensorChains = %ld\n", _sensorChains.size());
	if(weakBarrierCovered)
	{
		_quality = minimumCriticalRegion;
	}
	else if(_sensorChains.size() == 1)
	{
		// full barrier covered
		_quality = _beltWidth;
	}
	else
	{
		_quality = -1.0;
	}
	_numberOfCriticalRegions = numberOfCriticalRegions;
}

/*Remaining Getter Setter Functions*/
void GeometricGraph::setSensingRange(double sensingRange)
{
	if(sensingRange > 0.0 && _sensingRange <= MAX_SENSING_RANGE)
	{
		_sensingRange = sensingRange;
	}
	else
	{
		_sensingRange = DEFAULT_SENSING_RANGE;
	}
}

void GeometricGraph::setBeltWidth(double beltWidth)
{
	if(beltWidth > 0.0 && beltWidth <= MAX_BELT_WIDTH)
	{
		_beltWidth = beltWidth;
	}
	else
	{
		_beltWidth = DEFAULT_BELT_WIDTH;
	}
}

int GeometricGraph::getNumberOfCriticalRegions()
{ return _numberOfCriticalRegions; }

double GeometricGraph::getQuality()
{ return _quality; }

double GeometricGraph::getNewQuality()
{
	// Has to be changed according to new schemes
	return getMaxPassageDistance();
}

double GeometricGraph::getMaxPassageDistance()
{ return _maxPassageDistance; }

double GeometricGraph::getVerticalCenterOfMass()
{ return _verticalCenterOfMass; }

void GeometricGraph::setCoordinates(const vector<Coordinate>& inputCoordinates)
{
	//Initialze the Coordinates proper input present
	if(inputCoordinates.size() > 0)
	{
		_coordinates.assign(inputCoordinates.begin(), inputCoordinates.end());
		_graph.resize(_coordinates.size(), vector<int>());
		preprocessCoordinates();
	}
	// Generate Graph
	generateGraph();
	calculateQuality();
}

vector<Coordinate>& GeometricGraph::getCoordinates()
{ return _coordinates; }

vector<vector<int> >& GeometricGraph::getGraph()
{ return _graph; }
