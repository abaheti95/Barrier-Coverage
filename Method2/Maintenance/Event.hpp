#ifndef EVENT_HPP
#define EVENT_HPP

using namespace std;
enum Type {
	SENSOR_FAILURE,
	FAILURE_DETECTED
};

struct Event {
	// This is an event information bundled up in a class
	Type type;
	int failed_node;			// In case of Node Failure Event, tells which node has failed
	int failed_index;			// Index of id of failed node in the barrier array
	int id;						// ID of the concerned sensor node
};


#endif