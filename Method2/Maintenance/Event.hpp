#ifndef EVENT_HPP
#define EVENT_HPP

#include <cstdio>

using namespace std;
enum Type {
	SENSOR_FAILURE,				// Some sensor fails
	FAILURE_DETECTED,			// Some sensor detects a failure within its communication range
	BRANCH_CONNECT_TO_DST,		// Tell the branch sensor to connect to the nearest sensor on the barrier
	BARRIER_CONNECT_TO_DST,		// Tell the barrier sensor to connect to the nearest sensor on the barrier
	CHAIN_MAINTENANCE,			// Tells the sensor to maintain the chain state
	NO_TYPE
};

enum Direction {
	LEFT,
	RIGHT,
	NO_DIRECTION
};

struct Event {
	// This is an event information bundled up in a class
	Type type;
	int failed_node;			// In case of Node Failure Event, tells which node has failed
	int failed_index;			// Index of id of failed node in the barrier array
	int id;						// ID of the concerned sensor node
	int dst_id;					// ID of the destination sensor. The sensor to which the current sensor has to connect.
	Direction direction;		// Tells which side of the current sensor is broken

	Event();					// Constructor
	Event(const Event&);		// Copy Constructor
	~Event() {}					// Destructor
	void print();
};


#endif