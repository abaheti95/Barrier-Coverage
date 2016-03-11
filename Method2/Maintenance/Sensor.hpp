#ifndef SENSOR_HPP
#define SENSOR_HPP

// K is the number of next hop nodes that a sensor remembers
#define K 3
#define MAX_BRANCHES 10

#include <cmath>
#include <cstdlib>
#include "Coordinate.hpp"

using namespace std;

struct Sensor
{
	// Location Information
	double init_x;			// Initial x coordinate
	double init_y;			// Initial y coordinate
	double x;				// Current x coordinate. Changes Dynamically
	double y;				// Current y coordinate. Changes Dynamically
	// Siblings and adjacent nodes' information
	int n_left;				// Number of nodes to the left <= K
	int n_right;			// Number of nodes to the right <= K
	int left_nodes[K];		// Stores the IDs of the nodes to the left of current
	Coordinate left_loc[K];	// Locations of left nodes
	int right_nodes[K];		// Stores the IDs of the nodes to the right of current
	Coordinate right_loc[K];	// Locations of the right nodes
	int branches[MAX_BRANCHES];		// Stores the sensors that are braches to the current sensor except from left and right nodes which belong to the barrier
	Coordinate branches_loc[MAX_BRANCHES];
	// Self Status and Information
	bool on_barrier;		// True or false depending on whether the sensor is on the barrier or not
	bool has_branches;		// True if current sensor has any redundant branches to flatten
	bool has_failed;			// True if the sensor has failed or disabled
	double distance;		// Total distance travelled by the node

	Sensor(double = 0.0, double = 0.0);
	Sensor(const Sensor&);
	~Sensor() {}
};

struct XCoordinateComparator
{
	bool operator()(const Coordinate &left, const Coordinate &right)
	{
		return left.x < right.x;
	}
};

#endif