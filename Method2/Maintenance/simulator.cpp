#include "simulator.hpp"

#define MAX_SENSORS 100

// Global Simulation Variables
double belt_lenght;
double belt_width;
double sensing_range;
double communication_range;
int n_sensors;					// Total number of sensors used in the simulation
Coordinate sensor_loc[MAX_SENSORS];
int barrier_len;				// Number of sensors present in the barrier
int barrier[MAX_SENSORS];		// List of sensor IDs present in the barrier
bool on_barrier[MAX_SENSORS];	// Boolean array that tells whether a sensor is on the barrier or not
Sensor sensors[MAX_SENSORS];	// List of all the data stored in individual sensor nodes
int within_range[MAX_SENSORS];	// List of sensor IDs that are within the communication range of a given sensor
// This list will change at every search query and will be shared between all functions
vector< vector< int > > sensor_graph;	// Complete Sensor Graph

// Event Priority Queue
struct Order {
	bool operator()(P const& a, P const& b) const {
		return a.first < b.first;
	}
};
typedef pair<int, Event> P;
priority_queue< P, vector<P>, Order> events;

void read_input_data() {
	// The data is assumed to be coming from stdin
	// Thus simple scanf will work

	// NULL all the data
	for(int i = 0; i < MAX_SENSORS; i++) {
		sensor_loc[i].x = sensor_loc[i].y = 0.0;
		barrier[i] = -1;
		on_barrier[i] = false;

		sensors[i].x = sensors[i].y = sensors[i].init_x = sensors[i].init_y = 0.0;
		for(int j = 0; j < K; j++) {
			sensors[i].left_nodes[j] = -1;
			sensors[i].right_nodes[j] = -1;
		}
		for(int j = 0; j < MAX_BRANCHES; j++) {
			sensors[i].branches[j] = 0;
		}
		sensors[i].on_barrier = sensors[i].has_branches = false;
	}
	
	// First read the global simulation variables
	scanf("%lf %lf %lf %d", &belt_lenght, &belt_width, &sensing_range, &n_sensors);
	// Read the locations of all the sensors
	for(int i = 0; i < n_sensors; i++) {
		scanf("%lf %lf", &sensor_loc[i].x, &sensor_loc[i].y);
	}
	// Read the barrier 
	scanf("%d", &barrier_len);
	for(int i = 0; i < barrier_len; i++) {
		scanf("%d", &barrier[i]);
		on_barrier[barrier[i]] = true;
	}
	// Read the Sensor Graph
	for(int i = 0; i < n_sensors; i++) {
		sensor_graph.append(new vector<int>())
		int n, id;
		scanf("%d", &n);
		for(int j = 0; j < n; j++) {
			scanf("%d", &id);
			sensor_graph[i].append(id)
		}
	}
}

void initialize_data() {
	// After reading the input we will initialize all the sensor data

	// Copy the locations into Sensor nodes
	for(int i = 0; i < n_sensors; i++) {
		sensors[i].init_x = sensors[i].x = sensor_loc[i].x;
		sensors[i].init_y = sensors[i].y = sensor_loc[i].y;
	}

	// Update the left and right sensors on the barrier
	// Traverse from left to right and update the sensors accordingly
	for(int i = 0; i < barrier_len; i++) {
		// Look at the K sensors on the left and right of the current sensor
		Sensor& current_node = sensors[barrier[i]];
		current_node.on_barrier = true;
		// Left Sensors
		current_node.n_left = 0;
		for(int j = 1; <= K; j++) {
			// Check if the desired sensor within index range
			if((i - j) >= 0) {
				current_node.left_nodes[j-1] = barrier[i - j];
				current_node.left_loc[j-1] = sensor_loc[barrier[i - j]];
				current_node.n_left++;
			}
		}
		// Right Sensors
		current_node.n_right = 0;
		for(j = 1; <= K; j++) {
			// Check if the desired sensor within index range
			if((i + j) < barrier_len) {
				current_node.left_nodes[j-1] = barrier[i + j];
				current_node.left_loc[j-1] = sensor_loc[barrier[i + j]];
				current_node.n_right++;
			}
		}
	}

	// Update the branches of all the sensors
	for(int i = 0; i < n_sensors; i++) {
		Sensor& current_node = sensors[i];
		int k = 0;
		for(int j = 0; j < (int)sensor_graph[i].size(); j++) {
			if(sensor_graph[i][j] != current_node.left_nodes[0] && sensor_graph[i][j] != current_node.right_nodes[0]) {
				current_node.branches[k] = sensor_graph[i][j];
				current_node.branches_loc[k++] = sensor_loc[sensor_graph[i][j]];
			}
		}
		if(k > 0) {
			current_node.has_branches = true;
		}
	}
}

void fail_sensors(int n) {
	// Number of sensors to be removed is taken as input
	start_index = rand() % (barrier_len - n);
	for(int i = start_index; i < start_index + n; i++) {
		sensors[barrier[i]].has_failed = true;
		// Create and add event in the queue
		Event e;
		e.type = SENSOR_FAILURE;
		e.failed_node = barrier[i];
		e.failed_index = i;
		events.push(make_pair(0, e));
	}
}

void initialize(int n) {
	read_input_data();
	initialize_data();
	fail_sensors(n);
}

double distance(x1, y1, x2, y2) {
	diff_x = x2 - x1;
	diff_y = y2 - y1;
	return sqrt(diff_x * diff_x + diff_y * diff_y);
}

void seach_sensors_within_communication_range(Coordinate loc) {
	int k = 0;
	for(int i = 0; i < n_sensors; i++) {
		Sensor& current = sensors[i];
		// Ignore the failed sensors in computation
		if(current.has_failed) {
			continue;
		}
		if(distance(loc.x, loc.y, current.x, current.y) <= communication_range) {
			within_range[k++] = i;
		}
	}
	within_range[k] = -1;
}

void seach_sensors_within_communication_range(int index) {
	int k = 0;
	Sensor& sensor = sensors[index];
	for(int i = 0; i < n_sensors; i++) {
		if(i == index) {
			continue;
		}
		// Ignore the failed sensors in computation
		if(current.has_failed) {
			continue;
		}
		Sensor& current = sensors[i];
		if(distance(sensor.x, sensor.y, current.x, current.y) <= communication_range) {
			within_range[k++] = i;
		}
	}
	within_range[k] = -1;
}

void handle_sensor_failure(timestamp, Event& e) {
	// Get the ID of the failed node and trigger the nodes adjacent to it
	seach_sensors_within_communication_range(e.failed_node);
	for(int i = 0; i < n_sensors; i++) {
		if(within_range[i] == -1) {
			break;
		}
		// Create a Failure Detection event for all the sensors within communication range
		Event failure_detected;
		failure_detected.type = FAILURE_DETECTED;
		failure_detected.id = within_range[i];
		events.push(make_pair(timestamp + 1, failure_detected))
	}
}

void process_event(int timestamp, Event& e) {
	// Sensor Failure
	if(e.type == SENSOR_FAILURE) {
		handle_sensor_failure(timestamp, e);
	} else if(e.type == FAILURE_DETECTED) {
		//TODO: Idhar ka logic likhna hai!!
	} 
}

void maintain() {
	while(!events.empty()) {
		// pull out one entry
		P p = events.top();
		int timestamp = p.first;
		Event e = p.second;
		events.pop();
		// Event Processing
		process_event(timestamp, e);
	}
}
void delete_sensor_graph() {
	for(int i = 0; i < n_sensors; i++) {
		delete sensor_graph[i];
	}
}

void delete_data() {
	delete_sensor_graph();
}

int main()
{
	srand(time(NULL))
	return 0;
}
