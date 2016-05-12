#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include "Coordinate.hpp"
#include "Force.hpp"
#include "Sensor.hpp"
#include "Event.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cmath>
#include <unistd.h>
#include <utility>
#include <vector>
#include <algorithm>
#include <queue>
#include <GLFW/glfw3.h>

using namespace std;

const int MAX_SENSORS = 100;
const double INFINITY_DOUBLE = 100000000.0;

const double DELTA = 0.005;		// Permissible error
const double MULTIPLICATION_FACTOR = 4.0;
#define VMAX (2.0 * sensing_range)
#define FLATTEN_FORCE_MAGNITUDE (VMAX / 2)
// Force parameters
#define SENSOR_FORCE_OFFSET (sensing_range / 2.5)
#define CHAIN_FORCE_OFFSET (sensing_range / 2)
// #define CHAIN_FORCE_OFFSET 0.0
const double STEEPNESS = 2.0;
const int MAX_ITERATIONS = 5000;

const bool DEBUG = false;

struct Simulator {
	double sensor_force_factor;
	double chain_force_factor;

	// Global Simulation Variables
	int timestamp_jump;				// next timestamp could be randomly assigned between 1-timestamp_jump
	int event_counter;				// Global event counter
	int iterations;					// Tells the number of iterations requried to complete the simulation
	int max_timestamp;				// Tells the last seen timestamp in the priority queue
	double belt_length;
	double belt_width;
	double sensing_range;
	double communication_range;
	int n_sensors;					// Total number of sensors used in the simulation
	int n_failures;
	Coordinate sensor_loc[MAX_SENSORS];
	int barrier_len;				// Number of sensors present in the barrier
	int barrier[MAX_SENSORS];		// List of sensor IDs present in the barrier
	bool on_barrier[MAX_SENSORS];	// Boolean array that tells whether a sensor is on the barrier or not
	Sensor sensors[MAX_SENSORS];	// List of all the data stored in individual sensor nodes
	int within_range[MAX_SENSORS];	// List of sensor IDs that are within the communication range of a given sensor
	// This list will change at every search query and will be shared between all functions
	vector< vector< int > > sensor_graph;	// Complete Sensor Graph

	// Event Priority Queue
	typedef pair<int, Event> P;
	struct Order {
		bool operator()(P const& a, P const& b) const {
			if(a.first == b.first) {
				// We want a priority in between events
				// First all the Barrier Connect and Branch Connect events should be scheduled
				// Then all the Flattening Events should be scheduled
				// Finally in all the Chain Maintenance events are scheduled based on some ordering
				int a_priority = value_from_type(a.second.type);
				int b_priority = value_from_type(b.second.type);
				if(a_priority == b_priority) {
					return a.second.event_number > b.second.event_number;
				}
				return a_priority > b_priority;
			}
			return a.first > b.first;
		}
	};
	priority_queue< P, vector<P>, Order> events;
	
	/****************** Simulation display code *****************/
	int display_width;
	int display_height;
	double window_width;
	double window_height;

	int num_segments;
	GLFWwindow* window;

	// FTPixmapFont* font;
	/****************** End of display code *********************/
	// Result variables
	vector<int> failed_nodes;				// List of indices of failed sensors
	double avg_distance;					// Avg distance of all the sensor distances
	double max_distance;					// Max distance of all the sensor distances

	Simulator(int = 5);
	~Simulator() {}
	// Data Handling functions
	void read_input_data();					// Reads the data from stdin
	void read_input_from_file(const char *);// Reads data from the file of provided filename
	void initialize_data();					// Initializes the various data members of simulator
	void fail_sensors(int n);				// Randomly fails n contiguous sensors from the barrier
	void initialize_display();				// Initializes the OpenGL display window
	void initialize(int n);					// Initializes data and fails n sensors
	void delete_sensor_graph();
	void delete_data();
	// Display Functions
	void line(double x1, double y1, double x2, double y2);
	void circle(double cx, double cy, double radius);
	void draw_sensor(Sensor& sensor, float r, float g, float b);
	void draw_sensors();
	void draw_link(int id1, int id2);
	void draw_links();
	void draw_sensors_and_links();
	void draw_belt();
	void refresh_display();
	void draw_current_state();
	// Other Helper functions
	inline double distance(double x1, double y1, double x2, double y2);
	inline double sensor_distance(Sensor& s1, Sensor& s2);
	int nearest_barrier_sensor_within_communication_range(int index);
	int nearest_right_barrier_sensor_within_communication_range(int index);
	int nearest_left_barrier_sensor_within_communication_range(int index);
	void search_sensors_within_communication_range(Coordinate loc);
	void search_sensors_within_communication_range(int index);
	vector<int> broken_chain_sensors(Sensor& sensor);
	void activate_flattening_sensors(Sensor& sensor, int timestamp);
	inline bool check_connected(Sensor& sensor, Sensor& dst_sensor);
	inline int random_timestamp(int timestamp);
	// Debugging Functions
	void print_sensor_locs();
	// Force Functions
	double sensor_force(double x);
	double chain_force(double x);
	void apply_sensor_force(Sensor& sensor, Sensor& dst_sensor);
	Force calculate_branch_forces(Sensor& sensor);
	Force calculate_left_right_forces(Sensor& sensor);
	void apply_chain_force(Sensor& sensor);
	// Movement Functions
	double find_k_for_violation(Sensor& current_sensor, Sensor& target_sensor, Force& force);
	void move_within_permissible_range(Sensor& sensor, Force total_force);
	// Event Hanlder functions
	void handle_sensor_failure(int timestamp, Event& e);
	void handle_failure_detection(int timestamp, Event& e);
	void handle_search_branch(int timestamp, Event& e);
	void handle_branch_connect(int timestamp, Event& e);
	void handle_barrier_connect(int timestamp, Event& e);
	void handle_chain_maintenance(int timestamp, Event& e);
	void handle_flatten_connect(int timestamp, Event& e);
	// Simulating functions
	void process_event(int timestamp, Event& e);
	bool maintain();
	// Printing and results functions
	void evaluate_results();
	void print_results();
	double get_avg_distance();
	double get_max_distance();
	int get_final_timestamp();
	int get_final_iterations();

};
// Helper functions
void draw_sensor(Sensor&, float, float, float);


#endif
