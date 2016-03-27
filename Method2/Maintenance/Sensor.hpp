#ifndef SENSOR_HPP
#define SENSOR_HPP

// K is the number of next hop nodes that a sensor remembers
#define K 3
#define MAX_BRANCHES 10

#include <cmath>
#include <cstdlib>
#include <map>
#include "Coordinate.hpp"
#include "Force.hpp"

using namespace std;

enum Sibling_Type {
	LEFT_SIBLING,
	RIGHT_SIBLING,
	BRANCH_SIBLING,
	NO_SIBLING
};

struct Sensor {
	int id;					// Index of this sensor in the sensors array
	// Location Information
	double init_x;			// Initial x coordinate
	double init_y;			// Initial y coordinate
	double x;				// Current x coordinate. Changes Dynamically
	double y;				// Current y coordinate. Changes Dynamically
	// Siblings and adjacent nodes' information
	vector<int> left_nodes;			// Stores the IDs of the nodes to the left of current
	vector<int> right_nodes;		// Stores the IDs of the nodes to the right of current
	vector<int> braches;			// Map of IDs and Coordinates of braches of current
	// Self Status and Information
	bool on_barrier;		// True or false depending on whether the sensor is on the barrier or not
	int barrier_index;		// Index of this sensor in the barrier array
	bool has_branches;		// True if current sensor has any redundant branches to flatten
	bool has_failed;		// True if the sensor has failed or disabled
	double distance;		// Total distance travelled by the node
	// Next Hop Information
	int dst_node;			// ID of the destination node
	int dst_barrier_index;	// Index of dst in barrier array
	int next_hop_node;		// ID of the next_hop node
	int next_hop_barrier_index;
	// Force Information
	Force sensor_force;		// Force applied due to the destination sensor
	Force chain_force;		// Force applied due to the chain connections

	Sensor(double = 0.0, double = 0.0);
	Sensor(const Sensor&);
	~Sensor() {}
	bool is_left_sibling(int id);
	bool is_right_sibling(int id);
	bool is_branch(int id);
	Sibling_Type is_adjacent(int id);
};

struct XCoordinateComparator {
	bool operator()(const Coordinate &left, const Coordinate &right) {
		return left.x < right.x;
	}
};

#endif