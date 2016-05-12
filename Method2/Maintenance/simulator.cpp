#include "simulator.hpp"

using namespace std;

const int MAX_SENSORS = 100;
const double INFINITY_DOUBLE = 100000000.0;

const double DELTA = 0.005;		// Permissible error
const double MULTIPLICATION_FACTOR = 3.0;
#define VMAX (2.0 * sensing_range)
#define FLATTEN_FORCE_MAGNITUDE (VMAX / 2)
// Force parameters
#define SENSOR_FORCE_OFFSET (sensing_range / 2.5)
#define CHAIN_FORCE_OFFSET (sensing_range / 2)
// #define CHAIN_FORCE_OFFSET 0.0
const double STEEPNESS = 2.0;
double sensor_force_factor;
double chain_force_factor;

// Global Simulation Variables
int event_counter = 1;			// Global event counter
int iterations;					// Tells the number of iterations requried to complete the simulation
int max_timestamp = 0;			// Tells the last seen timestamp in the priority queue
const int timestamp_jump = 5;	// next timestamp could be randomly assigned between 1-timestamp_jump
double belt_length;
double belt_width;
double sensing_range;
double communication_range;
int n_sensors;					// Total number of sensors used in the simulation
const int n_failures = 6;
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
int display_width = 1000;
int display_height = 800;
double window_width = 1.8f;
double window_height = 1.0f;

int num_segments = 60;
GLFWwindow* window;

FTPixmapFont* font;
/****************** End of display code *********************/

const bool DEBUG = false;

void read_input_data() {
	// The data is assumed to be coming from stdin
	// Thus simple scanf will work

	// Format of the input data is as follows
	// belt_length belt_width, sensing_range, n_sensors
	// n lines of locations of sensors as x_coordinate y_coordinate
	// Length of the barrier
	// Array of indices telling the sensors which are present on the barrier
	// Read the Sensor graph

	// NULL all the data
	for(int i = 0; i < MAX_SENSORS; i++) {
		sensor_loc[i].x = sensor_loc[i].y = 0.0;
		barrier[i] = -1;
		on_barrier[i] = false;

		sensors[i].x = sensors[i].y = sensors[i].init_x = sensors[i].init_y = 0.0;
		
		sensors[i].left_nodes.clear();
		sensors[i].right_nodes.clear();
		sensors[i].branches.clear();
		sensors[i].on_barrier = sensors[i].has_branches = false;
		sensors[i].barrier_index = -1;
	}
	
	// First read the global simulation variables
	scanf("%lf %lf %lf %d", &belt_length, &belt_width, &sensing_range, &n_sensors);
	communication_range = MULTIPLICATION_FACTOR * sensing_range;
	// Calculate the force factors beforehand
	sensor_force_factor = (VMAX - SENSOR_FORCE_OFFSET) / pow(communication_range, 2.0);
	chain_force_factor = (VMAX - CHAIN_FORCE_OFFSET) / 3.0 / pow((communication_range - 2*sensing_range), STEEPNESS);
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
	printf("Printing Graph:\n");
	for(int i = 0; i < n_sensors; i++) {
		vector<int> row;
		int n, id;
		scanf("%d", &n);
		printf("%3d: ", i);
		for(int j = 0; j < n; j++) {
			scanf("%d", &id);
			printf("%3d ", id);
			row.push_back(id);
		}
		printf("\n");
		sensor_graph.push_back(row);
	}
	// sleep(100);
}

void initialize_data() {
	// After reading the input we will initialize all the sensor data

	// Copy the locations into Sensor nodes
	for(int i = 0; i < n_sensors; i++) {
		sensors[i].id = i;
		sensors[i].init_x = sensors[i].x = sensor_loc[i].x;
		sensors[i].init_y = sensors[i].y = sensor_loc[i].y;
	}

	// Update the left and right sensors on the barrier
	// Traverse from left to right and update the sensors accordingly
	for(int i = 0; i < barrier_len; i++) {
		// Look at the K sensors on the left and right of the current sensor
		Sensor& current_node = sensors[barrier[i]];
		current_node.on_barrier = true;
		current_node.barrier_index = i;
		// Left Sensors
		for(int j = 1; j <= K; j++) {
			// Check if the desired sensor within index range
			if((i - j) >= 0) {
				current_node.right_nodes.push_back(barrier[i - j]);
			} else {
				break;
			}
		}
		// Right Sensors
		for(int j = 1; j <= K; j++) {
			// Check if the desired sensor within index range
			if((i + j) < barrier_len) {
				current_node.left_nodes.push_back(barrier[i + j]);
			} else {
				break;
			}
		}
		// Mark left and right sensors
		if(i == 0) {
			current_node.is_right_sensor = true;
		} else if(i == (barrier_len - 1)) {
			current_node.is_left_sensor = true;
		}
	}

	// Update the branches of all the sensors
	for(int i = 0; i < n_sensors; i++) {
		Sensor& current_node = sensors[i];
		int k = 0;
		for(int j = 0; j < (int)sensor_graph[i].size(); j++) {
			if((!current_node.left_nodes.empty() && sensor_graph[i][j] == current_node.left_nodes[0]) 
				|| (!current_node.right_nodes.empty() && sensor_graph[i][j] == current_node.right_nodes[0])) {
				continue;
			}
			current_node.branches.push_back(sensor_graph[i][j]);
		}
		if(k > 0) {
			current_node.has_branches = true;
		}
	}
}

void fail_sensors(int n) {
	// Number of sensors to be removed is taken as input
	int start_index = rand() % (barrier_len - n);
	for(int i = start_index; i < start_index + n; i++) {
		sensors[barrier[i]].has_failed = true;
		// Create and add event in the queue
		Event e;
		e.type = SENSOR_FAILURE;
		e.failed_node = barrier[i];
		e.failed_index = i;
		e.event_number = event_counter++;
		events.push(make_pair(0, e));
	}
}

void initialize_display() {

	/* Initialize the library */
	if (!glfwInit()){
		return;
	}

	window = glfwCreateWindow(display_width, display_height, "Simulation", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	
	// Calculate window_width and window height based on about values
	double display_ratio = (double) display_width / (double) display_height;
	double belt_ratio = belt_length / belt_width;
	if(belt_ratio > display_ratio) {
		// length is dominant
		window_width = 1.8f;
		window_height = ((double)window_width / belt_ratio);
	} else {
		// width is dominant
		window_height = 1.0f;
		window_width = ((double)window_height * belt_ratio);
	}

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glTranslatef(-1.0f, -1.0f, 0.0f);
	glfwSwapBuffers(window);
	glfwPollEvents();

	printf("Belt Dimensions %lf x %lf\n", belt_length, belt_width);
	printf("Window dimensions %lf x %lf\n", window_width, window_height);
	// exit(1);
	// Initialize font
	/*const char* fontfile = 0;
	// font = new FTPixmapFont((const char*)fontfile);
	font = new FTGLPixmapFont((const char*)"/home/user/Arial.ttf");
	if(font->Error()) {
		fprintf(stderr, "Failed to open font");
		exit(1);
	}
	font->FaceSize(18);*/
}

void initialize(int n) {
	read_input_data();
	initialize_data();
	fail_sensors(n);
}

inline double distance(double x1, double y1, double x2, double y2) {
	double diff_x = x2 - x1;
	double diff_y = y2 - y1;
	return sqrt(diff_x * diff_x + diff_y * diff_y);
}

inline double sensor_distance(Sensor& s1, Sensor& s2) {
	return distance(s1.x, s1.y, s2.x, s2.y);
}

int nearest_barrier_sensor_within_communication_range(int index) {
	Sensor& sensor = sensors[index];
	double min = INFINITY_DOUBLE;
	int dst_id = -1;
	for(int i = 0; i < barrier_len; i++) {
		Sensor& current = sensors[barrier[i]];
		// Ignore the failed sensor in computation
		if(current.has_failed) {
			continue;
		}
		double dist = distance(sensor.x, sensor.y, current.x, current.y);
		if(dist <= communication_range) {
			if(dist < min) {
				min = dist;
				dst_id = barrier[i];
			}
		}
	}
	return dst_id;
}

int nearest_right_barrier_sensor_within_communication_range(int index) {
	Sensor& sensor = sensors[index];
	double min = INFINITY_DOUBLE;
	int dst_id = -1;
	for(int i = (sensor.barrier_index - 1); i >= 0; i--) {
		Sensor& current = sensors[barrier[i]];
		// Ignore the failed sensor in computation
		if(current.has_failed) {
			continue;
		}
		double dist = distance(sensor.x, sensor.y, current.x, current.y);
		if(dist <= communication_range) {
			if(dist < min) {
				min = dist;
				dst_id = barrier[i];
			}
		}
	}
	return dst_id;
}

int nearest_left_barrier_sensor_within_communication_range(int index) {
	Sensor& sensor = sensors[index];
	double min = INFINITY_DOUBLE;
	int dst_id = -1;
	for(int i = sensor.barrier_index + 1; i < barrier_len; i++) {
		Sensor& current = sensors[barrier[i]];
		// Ignore the failed sensor in computation
		if(current.has_failed) {
			continue;
		}
		double dist = distance(sensor.x, sensor.y, current.x, current.y);
		if(dist <= communication_range) {
			if(dist < min) {
				min = dist;
				dst_id = barrier[i];
			}
		}
	}
	return dst_id;
}

void search_sensors_within_communication_range(Coordinate loc) {
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

void search_sensors_within_communication_range(int index) {
	int k = 0;
	Sensor& sensor = sensors[index];
	for(int i = 0; i < n_sensors; i++) {
		if(i == index) {
			continue;
		}
		Sensor& current = sensors[i];
		// Ignore the failed sensors in computation
		if(current.has_failed) {
			continue;
		}
		if(distance(sensor.x, sensor.y, current.x, current.y) <= communication_range) {
			within_range[k++] = i;
		}
	}
	within_range[k] = -1;
}

inline int random_timestamp(int timestamp) {
	// generate a random number between 1 and timestamp_jump and add it to the input timestamp
	return (timestamp + (rand() % timestamp_jump) + 1);
}

void handle_sensor_failure(int timestamp, Event& e) {
	// Get the ID of the failed node and trigger the nodes adjacent to it
	search_sensors_within_communication_range(e.failed_node);
	if(DEBUG) {
		printf("communication range = %lf\n", communication_range);
		printf("Failed node: \n");
	}
	for(int i = 0; i < n_sensors; i++) {
		if(DEBUG) {
			printf("%d\n", within_range[i]);
		}
		if(within_range[i] == -1) {
			break;
		}
		// Create a Failure Detection event for all the sensors within communication range
		Event failure_detected;
		failure_detected.type = FAILURE_DETECTED;
		failure_detected.id = within_range[i];
		failure_detected.failed_node = e.failed_node;
		failure_detected.failed_index = e.failed_index;
		failure_detected.event_number = event_counter++;
		events.push(make_pair(random_timestamp(timestamp), failure_detected));
	}
}

void handle_failure_detection(int timestamp, Event& e) {
	// Check if current sensor is a sensor adjacent to failed sensor
	Sensor& sensor = sensors[e.id];
	Sibling_Type stype = sensor.is_adjacent(e.failed_node);
	if(stype == NO_SIBLING) {
		// Don't care
	} else if(stype == BRANCH_SIBLING) {
		// Search for nearest sensor within communication range which is on barrier
		int dst_sensor_id = nearest_barrier_sensor_within_communication_range(e.id);
		printf("At least yahan toh aaya hai %d\n", e.id);
		if(dst_sensor_id == -1) {
			// No barrier sensor present within the communication range
			// Enter in the Search branch state and periodically search for some destination sensor
			printf("Phir yahan bhi gaya!! %d\n", e.id);
			Event search_branch;
			search_branch.type = SEARCH_BRANCH;
			search_branch.id = sensor.id;
			events.push(make_pair(random_timestamp(timestamp), search_branch));
		} else {
			// We have a destination sensor
			// Start approaching that sensor

			Event follow_dst;
			sensor.dst_node = dst_sensor_id;
			sensor.dst_barrier_index = sensors[dst_sensor_id].barrier_index;
			follow_dst.type = BRANCH_CONNECT_TO_DST;
			follow_dst.id = e.id;
			follow_dst.dst_id = dst_sensor_id;
			follow_dst.event_number = event_counter++;
			events.push(make_pair(random_timestamp(timestamp), follow_dst));
		}
	} else if(stype == LEFT_SIBLING) {
		// Search in the left side for a barrier sensor
		int dst_sensor_id = nearest_left_barrier_sensor_within_communication_range(e.id);
		if(dst_sensor_id == -1) {
			// Gap on the left side is huge
			// Make destination as the first left node and delete that element from the list of left nodes
			sensor.dst_node = -1;
			sensor.next_hop_node = sensor.left_nodes[0];
			sensor.left_nodes.erase(sensor.left_nodes.begin());
			sensor.next_hop_barrier_index = sensors[sensor.next_hop_node].barrier_index;
			Event next_hop;
			next_hop.type = BARRIER_CONNECT_TO_DST;
			next_hop.direction = LEFT;
			next_hop.id = e.id;
			next_hop.dst_id = sensor.dst_node;
			next_hop.event_number = event_counter++;
			events.push(make_pair(random_timestamp(timestamp), next_hop));
		} else {
			// We have a destination sensor on the left
			// Hence we will put destination as that sensor
			sensor.dst_node = dst_sensor_id;
			sensor.dst_barrier_index = sensors[dst_sensor_id].barrier_index;
			Event next_hop;
			next_hop.type = BARRIER_CONNECT_TO_DST;
			next_hop.direction = LEFT;
			next_hop.id = e.id;
			next_hop.dst_id = sensor.dst_node;
			next_hop.event_number = event_counter++;
			events.push(make_pair(random_timestamp(timestamp), next_hop));
		}
	} else if(stype == RIGHT_SIBLING) {
		// Search in the right side for a barrier sensor
		int dst_sensor_id = nearest_right_barrier_sensor_within_communication_range(e.id);
		if(dst_sensor_id == -1) {
			// Gap on the right side is huge
			// Make destination as the first right node and delete that element from the list of right nodes
			sensor.dst_node = -1;
			sensor.next_hop_node = sensor.right_nodes[0];
			sensor.right_nodes.erase(sensor.right_nodes.begin());
			sensor.next_hop_barrier_index = sensors[sensor.next_hop_node].barrier_index;
			Event next_hop;
			next_hop.type = BARRIER_CONNECT_TO_DST;
			next_hop.direction = RIGHT;
			next_hop.id = e.id;
			next_hop.dst_id = sensor.dst_node;
			next_hop.event_number = event_counter++;
			events.push(make_pair(random_timestamp(timestamp), next_hop));
		} else {
			// We have a destination sensor on the right
			// Hence we will put destination as that sensor
			sensor.dst_node = dst_sensor_id;
			sensor.dst_barrier_index = sensors[dst_sensor_id].barrier_index;
			Event next_hop;
			next_hop.type = BARRIER_CONNECT_TO_DST;
			next_hop.direction = RIGHT;
			next_hop.id = e.id;
			next_hop.dst_id = sensor.dst_node;
			next_hop.event_number = event_counter++;
			events.push(make_pair(random_timestamp(timestamp), next_hop));
		}
	} else { // NO_SIBLING
		// In case of no sibling we don't create a new event.
		// When the adjacent sensor has finally connected with the new sensor, it will update the left/right nodes of this sensor
		// TODO: Decide whether to change this
	}
	// sensor.remove_sibling(e.failed_node);
}

// Some Theoretical Hypothesis
// sensor_force <=> a * x^2 + SENSOR_FORCE_OFFSET
// sensor_force = VMAX at x = R_c
// Therefore, a = (VMAX - SENSOR_FORCE_OFFSET) / R_c^2
// chain_force <=> b * (x - 2*R_s)^3 + CHAIN_FORCE_OFFSET
// chain_force > sensor_force at x = R_c
// Therefore b > (VMAX - CHAIN_FORCE_OFFSET) / (R_c - 2*R_s)^3

double sensor_force(double x) {
	x = abs(x);
	// return (x*x/(10.0 * sensing_range) + SENSOR_FORCE_OFFSET);
	return sensor_force_factor * x*x + SENSOR_FORCE_OFFSET;
}

double chain_force(double x) {
	x = abs(x);
	if(x <= 2*sensing_range) {
		return 0.0;
	}
	// return (x*x*x/(16.0 * sensing_range) + CHAIN_FORCE_OFFSET);
	double estimate_chain_force = chain_force_factor * pow(x - 2*sensing_range, STEEPNESS) + CHAIN_FORCE_OFFSET;
	if (estimate_chain_force > (x - 2*sensing_range)) {
		if(DEBUG) {
			printf("YOYOYOYOYOYOYOYOYOYOYOYOYYO\n");
		}
		return (x - 2*sensing_range);
	}
	return estimate_chain_force;
}

void apply_sensor_force(Sensor& sensor, Sensor& dst_sensor) {
	// Apply force towards the destination sensor
	double sensor_force_magnitude = sensor_force(sensor_distance(sensor, dst_sensor));
	Force f((dst_sensor.x - sensor.x), (dst_sensor.y - sensor.y), sensor_force_magnitude);
	sensor.sensor_force = f;
}

Force calculate_branch_forces(Sensor& sensor) {
	Force f;
	for(int i = 0; i < (int)sensor.branches.size(); i++) {
		Sensor& branch = sensors[sensor.branches[i]];
		if(branch.has_failed) {
			continue;
		}
		double distance = sensor_distance(sensor, branch);
		if(distance > 2*sensing_range) {
			double chain_force_magnitude = chain_force(distance);
			Force f1((branch.x - sensor.x), (branch.y - sensor.y), chain_force_magnitude);
			f += f1;
		}
	}
	return f;
}

Force calculate_left_right_forces(Sensor& sensor) {
	Force f1 = null_force;
	if(sensor.left_nodes.size() > 0) {
		Sensor& left = sensors[sensor.left_nodes[0]];
		if(left.has_failed) {
			sensor.remove_sibling(left.id);
		} else {
			double distance = sensor_distance(sensor, left);
			if(distance > 2*sensing_range) {
				double chain_force_magnitude = chain_force(distance);
				Force f((left.x - sensor.x), (left.y - sensor.y), chain_force_magnitude);
				f1 = f;
			}
		}
	}
	Force f2 = null_force;
	if(sensor.right_nodes.size() > 0) {
		Sensor& right = sensors[sensor.right_nodes[0]];
		if(right.has_failed) {
			sensor.remove_sibling(right.id);
		} else {
			double distance = sensor_distance(sensor, right);
			if(distance > 2*sensing_range) {
				double chain_force_magnitude = chain_force(distance);
				Force f((right.x - sensor.x), (right.y - sensor.y), chain_force_magnitude);
				f2 = f;
			}
		}
	}
	// If f1 and f2 are large in magnitude and opposing each other then randomly drop one force
	/*if(f1.magnitude() > DELTA && f2.magnitude() > DELTA) {
		double theta = f1.angle(f2);
		if(theta >= 135.0 && (abs(f1.magnitude() - f2.magnitude()) <= DIFFERENCE_THRESHOLD)) {
			printf("COMPARABLE!!!!!!!!!!!!!!!!!!!!!!!!!!!!! %lf %lf\n", f1.magnitude(), f2.magnitude());
			// The forces are majorly opposite
			// Randomly return one of the two
			double probability = ((double) rand() / (RAND_MAX));
			if(probability > 0.5) {
				return f1;
			} else {
				return f2;
			}
		}
	}*/
	return (f1 + f2);
}

vector<int> broken_chain_sensors(Sensor& sensor) {
	// Return the list of siblings which are violating the chain link connection condition
	vector<int> broken_siblings;
	if(sensor.left_nodes.size() > 0) {
		Sensor& left = sensors[sensor.left_nodes[0]];
		if(!left.has_failed && (sensor_distance(sensor, left) > (2*sensing_range + DELTA))) {
			broken_siblings.push_back(left.id);
		}
	}
	if(sensor.right_nodes.size() > 0) {
		Sensor& right = sensors[sensor.right_nodes[0]];
		if(!right.has_failed && (sensor_distance(sensor, right) > (2*sensing_range + DELTA))) {
			broken_siblings.push_back(right.id);
		}
	}
	for(int i = 0; i < (int)sensor.branches.size(); i++) {
		Sensor& branch = sensors[sensor.branches[i]];
		if(!branch.has_failed && (sensor_distance(sensor, branch) > (2*sensing_range + DELTA))) {
			broken_siblings.push_back(branch.id);
		}
	}
	return broken_siblings;
}

void apply_chain_force(Sensor& sensor) {
	// We need to find all the neighboring sensors who are applying force on current sensor due to chain links
	// Force acted upon the sensor based on the chain connections
	Force f;
	if(sensor.dst_node != -1) {
		// This sensor is trying to connect to some other sensor
		if(sensor.on_barrier) {
			// Apply forces due to left or right connections only
			f += calculate_left_right_forces(sensor);
		} else {
			// Apply forces from all branches
			f += calculate_branch_forces(sensor);
		}
	} else {
		// apply forces from all branches and left and right sensors
		f += calculate_left_right_forces(sensor);
		f += calculate_branch_forces(sensor);
	}
	sensor.chain_force = f;
	return;
}

bool check_connected(Sensor& sensor, Sensor& dst_sensor) {
	return (distance(sensor.x, sensor.y, dst_sensor.x, dst_sensor.y) <= (2*sensing_range));
}

void handle_search_branch(int timestamp, Event& e) {
	// Current sensor periodically searches for some neighboring destination sensor
	// If at any point in the simulation it finds such a then it will enter the BRANCH_CONNECT_TO_DST state
	printf("I'm here here here here %d\n", e.id);
	// Search for nearest sensor within communication range which is on barrier
	Sensor& sensor = sensors[e.id];
	int dst_sensor_id = nearest_barrier_sensor_within_communication_range(e.id);
	if(dst_sensor_id == -1) {
		// No barrier sensor present within the communication range
		// Enter in the Search branch state and periodically search for some destination sensor
		Event search_branch;
		search_branch.type = SEARCH_BRANCH;
		search_branch.id = sensor.id;
		events.push(make_pair(random_timestamp(timestamp), search_branch));
	} else {
		// We have a destination sensor
		// Start approaching that sensor

		Event follow_dst;
		sensor.dst_node = dst_sensor_id;
		sensor.dst_barrier_index = sensors[dst_sensor_id].barrier_index;
		follow_dst.type = BRANCH_CONNECT_TO_DST;
		follow_dst.id = e.id;
		follow_dst.dst_id = dst_sensor_id;
		follow_dst.event_number = event_counter++;
		events.push(make_pair(random_timestamp(timestamp), follow_dst));
	}
}

void handle_branch_connect(int timestamp, Event& e) {
	// This is a branch type sensor and it has to connect to its destination
	Sensor& sensor = sensors[e.id];
	Sensor& dst_sensor = sensors[sensor.dst_node];
	// Apply Sensor Force
	apply_sensor_force(sensor, dst_sensor);
	// Apply Chain Force
	apply_chain_force(sensor);
	Force total_force = sensor.chain_force + sensor.sensor_force;
	if(total_force.magnitude() > VMAX) {
		total_force = total_force / total_force.magnitude() * VMAX;
	}
	// Move in the direction of force only if its not voilating the belt conditions
	if((sensor.x + total_force.x) < 0 || (sensor.x + total_force.x) > belt_length ||
		(sensor.y + total_force.y) < 0 || (sensor.y + total_force.y) > belt_width) {
		// Don't move
	} else {
		if((sensor.is_left_sensor && (sensor.x + total_force.x - sensing_range) > 0) ||
			(sensor.is_right_sensor && (sensor.x + total_force.x + sensing_range) < belt_length)) {
			// Move in y direction only
			sensor.distance += total_force.y;
			sensor.y += total_force.y;
		} else {		
			sensor.distance += total_force.magnitude();
			sensor.x += total_force.x;
			sensor.y += total_force.y;
		}
	}
	sensor.sensor_force = null_force;
	sensor.chain_force = null_force;
	// Check if the two sensors are connected or not. If yes then create a new connection and make this sensor inactive
	if(check_connected(sensor, dst_sensor)) {
		// Create a connection between the two
		sensor.branches.push_back(dst_sensor.id);
		dst_sensor.branches.push_back(sensor.id);
		dst_sensor.has_branches = true;
		sensor.has_branches = true;
		// No need to create another event
	} else {
		// Add another BRANCH_CONNECT_TO_DST event in the priority queue
		Event follow_dst;
		follow_dst.type = BRANCH_CONNECT_TO_DST;
		follow_dst.id = e.id;
		follow_dst.dst_id = e.dst_id;
		follow_dst.event_number = event_counter++;
		events.push(make_pair(random_timestamp(timestamp), follow_dst));
	}
	// Check for any broken links here!
	// Trigger Chain Maintenace Events for the affected sensors
	sensor.sensor_force = null_force;
	vector<int> broken_siblings = broken_chain_sensors(sensor);
	if(DEBUG) {
		printf("PRINTING LIST OF BROKEN SIBLINGS for %d\n", sensor.id);
	}
	for(int i = 0; i < (int)broken_siblings.size(); i++) {
		// Create Chain Maintenance Event
		Event chain_maintenance;
		chain_maintenance.type = CHAIN_MAINTENANCE;
		chain_maintenance.id = broken_siblings[i];
		if(DEBUG) {
			printf("~~~~~~~~~~~~~~ %d ~~~~~~~~~~~~~\n", broken_siblings[i]);
		}
		chain_maintenance.event_number = event_counter++;
		events.push(make_pair(random_timestamp(timestamp), chain_maintenance));
	}
}

void handle_barrier_connect(int timestamp, Event& e) {
	// Check if current sensor has a destination sensor or not?
	Sensor& sensor = sensors[e.id];
	if(DEBUG) {
		printf("There is some destination!!!!!!!!!!!!!!!!!!!!!! %d %d\n",e.id, sensor.dst_node);
	}
	apply_chain_force(sensor);
	if(sensor.dst_node != -1) {
		// There is some destination node to reach to
		Sensor& dst_sensor = sensors[sensor.dst_node];
		// Check if destination is already connected?
		if(sensor.is_adjacent(sensor.dst_node) != NO_SIBLING) {
			if(DEBUG) {
				printf("ALREADY CONNECTED %d %d\n", sensor.id, e.dst_id);
			}
			return;
		}
		// Apply Sensor Force
		apply_sensor_force(sensor, dst_sensor);
		// Apply Chain Force
		Force total_force = sensor.chain_force + sensor.sensor_force;
		if(total_force.magnitude() > VMAX) {
			total_force = total_force / total_force.magnitude() * VMAX;
		}
		// Move in the direction of force
		if((sensor.x + total_force.x) < 0 || (sensor.x + total_force.x) > belt_length ||
			(sensor.y + total_force.y) < 0 || (sensor.y + total_force.y) > belt_width) {
			// Don't move
		} else {
			if((sensor.is_left_sensor && (sensor.x + total_force.x - sensing_range) > 0) ||
				(sensor.is_right_sensor && (sensor.x + total_force.x + sensing_range) < belt_length)) {
				// Move in y direction only
				sensor.distance += total_force.y;
				sensor.y += total_force.y;
			} else {		
				sensor.distance += total_force.magnitude();
				sensor.x += total_force.x;
				sensor.y += total_force.y;
			}
		}
		sensor.chain_force = null_force;
		// Check if the two sensors are connected or not. If yes then create a new connection and make this sensor inactive
		if(check_connected(sensor, dst_sensor)) {
			// Create a connection between the two
			dst_sensor.dst_node = sensor.id;
			if(e.direction == LEFT) {
				sensor.left_nodes.insert(sensor.left_nodes.begin(), dst_sensor.id);
				dst_sensor.right_nodes.insert(dst_sensor.right_nodes.begin(), sensor.id);
			} else if(e.direction == RIGHT) {
				sensor.right_nodes.insert(sensor.right_nodes.begin(), dst_sensor.id);
				dst_sensor.left_nodes.insert(dst_sensor.left_nodes.begin(), sensor.id);
			}
			if(DEBUG) {
				printf("SUCESSS MAXXXX!\n");
			}
			// No need to create another event
		} else {
			// Add another BRANCH_CONNECT_TO_DST event in the priority queue
			Event follow_dst;
			follow_dst.type = BARRIER_CONNECT_TO_DST;
			follow_dst.id = e.id;
			follow_dst.dst_id = e.dst_id;
			follow_dst.direction = e.direction;
			follow_dst.event_number = event_counter++;
			events.push(make_pair(random_timestamp(timestamp), follow_dst));
		}
	} else {
		// go to the next_hop node location
		Sensor& dst_sensor = sensors[sensor.next_hop_node];
		if(DEBUG) {
			printf("BLOODY BASTARD$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ %d %d\n", sensor.id, sensor.next_hop_node);
		}
		if((sensor.is_left_sensor && (dst_sensor.x - sensing_range) > 0) ||
			(sensor.is_right_sensor && (dst_sensor.x + sensing_range) < belt_length)) {
			// Move in y direction only
			// Also apply the chain forces in y direction
			if(abs(sensor.y + sensor.chain_force.y - dst_sensor.y) <= VMAX) {
				// Move to dst_sensor.y position
				sensor.distance += abs(sensor.y - dst_sensor.y);
				sensor.y = dst_sensor.y;
			} else {
				// Move with VMAX + sensor.chain_force.y towards dst_sensor.y direction
				float saved_y_location = sensor.y;
				if(dst_sensor.y > sensor.y) {
					sensor.y += VMAX + sensor.chain_force.y;
				} else {
					sensor.y -= VMAX + sensor.chain_force.y;
				}
				sensor.distance += abs(sensor.y - saved_y_location);
			}
			Event next_hop;
			next_hop.type = BARRIER_CONNECT_TO_DST;
			next_hop.direction = e.direction;
			next_hop.id = e.id;
			next_hop.dst_id = sensor.dst_node;
			next_hop.event_number = event_counter++;
			events.push(make_pair(random_timestamp(timestamp), next_hop));
		} else if(sensor_distance(sensor, dst_sensor) <= VMAX) {
			//TODO: handle chain_force in this section	
			sensor.distance += sensor_distance(sensor, dst_sensor);
			sensor.x = dst_sensor.x;
			sensor.y = dst_sensor.y;
			// Reached next hop location
			// Search for any sensor in proximity, if found update the destination sensor
			int dst_sensor_id;
			if(e.direction == LEFT) {
				dst_sensor_id = nearest_left_barrier_sensor_within_communication_range(e.id);
			} else if(e.direction == RIGHT) {
				dst_sensor_id = nearest_right_barrier_sensor_within_communication_range(e.id);
			}
			if(dst_sensor_id != -1) {
				// We have a destination sensor now

				// Also we will update the dst_sensor's dst_id
				sensors[dst_sensor_id].dst_node = sensor.id;
				// Try to connect to that
				// Unset next hop node
				sensor.next_hop_node = -1;
				sensor.next_hop_barrier_index = -1;
				// Set destination node information
				sensor.dst_node = dst_sensor_id;
				sensor.dst_barrier_index = sensors[dst_sensor_id].barrier_index;

				Event next_hop;
				next_hop.type = BARRIER_CONNECT_TO_DST;
				next_hop.direction = e.direction;
				next_hop.id = e.id;
				next_hop.dst_id = sensor.dst_node;
				next_hop.event_number = event_counter++;
				events.push(make_pair(random_timestamp(timestamp), next_hop));
			} else {
				// Else update the next hop location and create a new event
				sensor.dst_node = -1;
				if(e.direction == LEFT) {
					if(sensor.left_nodes.size() == 0) {
						// No nodes to add
						// TODO: handle this: Stuck in the middle of nowhere!!
					} else {
						// Remove first left node and set the next hop
						sensor.next_hop_node = sensor.left_nodes[0];
						sensor.left_nodes.erase(sensor.left_nodes.begin());
						sensor.next_hop_barrier_index = sensors[sensor.dst_node].barrier_index;
					}
				} else if(e.direction == RIGHT) {
					if(sensor.right_nodes.size() == 0) {
						// No nodes to add
						// TODO: handle this: Stuck in the middle of nowhere!!
					} else {
						// Remove the first right node and set the next hop
						sensor.next_hop_node = sensor.right_nodes[0];
						sensor.right_nodes.erase(sensor.right_nodes.begin());
						sensor.next_hop_barrier_index = sensors[sensor.dst_node].barrier_index;
					}
				}
				Event next_hop;
				next_hop.type = BARRIER_CONNECT_TO_DST;
				next_hop.direction = e.direction;
				next_hop.id = e.id;
				next_hop.dst_id = sensor.dst_node;
				next_hop.event_number = event_counter++;
				events.push(make_pair(random_timestamp(timestamp), next_hop));
			}
		} else {
			// Move with VMAX speed towards next hop location
			Force f(dst_sensor.x - sensor.x, dst_sensor.y - sensor.y);
			f = f / f.magnitude() * VMAX;
			f += sensor.chain_force;

			// Move in the direction of force
			if((sensor.x + f.x) < 0 || (sensor.x + f.x) > belt_length ||
				(sensor.y + f.y) < 0 || (sensor.y + f.y) > belt_width) {
				// Don't move
			} else {
				if((sensor.is_left_sensor && (sensor.x + f.x - sensing_range) > 0) ||
					(sensor.is_right_sensor && (sensor.x + f.x + sensing_range) < belt_length)) {
					// Move in y direction only
					sensor.distance += f.y;
					sensor.y += f.y;
				} else {		
					sensor.distance += VMAX;
					sensor.x += f.x;
					sensor.y += f.y;
				}
			}
			// Create another self-triggered event to keep this alive
			Event follow_dst;
			follow_dst.type = BARRIER_CONNECT_TO_DST;
			follow_dst.id = e.id;
			follow_dst.dst_id = -1;
			follow_dst.direction = e.direction;
			follow_dst.event_number = event_counter++;
			events.push(make_pair(random_timestamp(timestamp), follow_dst));
		}
	}
	// Trigger Chain Maintenace Events for the affected sensors
	sensor.sensor_force = null_force;
	vector<int> broken_siblings = broken_chain_sensors(sensor);
	if(DEBUG) {
		printf("PRINTING LIST OF BROKEN SIBLINGS for %d\n", sensor.id);
	}
	for(int i = 0; i < (int)broken_siblings.size(); i++) {
		// Create Chain Maintenance Event
		Event chain_maintenance;
		chain_maintenance.type = CHAIN_MAINTENANCE;
		chain_maintenance.id = broken_siblings[i];
		if(DEBUG) {
			printf("~~~~~~~~~~~~~~ %d ~~~~~~~~~~~~~\n", broken_siblings[i]);
		}
		chain_maintenance.event_number = event_counter++;
		events.push(make_pair(random_timestamp(timestamp), chain_maintenance));
	}
}

void activate_flattening_sensors(Sensor& sensor, int timestamp) {
	for(int i = 0; i < (int)sensor.branches.size(); i++) {
		Sensor& branch = sensors[sensor.branches[i]];
		double distance = INFINITY_DOUBLE;
		Direction direction = NO_DIRECTION;
		int dst_sensor = -1;
		if(!sensor.left_nodes.empty()) {
			Sensor& left = sensors[sensor.left_nodes[0]];
			if(!left.has_failed) {
				distance = sensor_distance(branch, left);
				dst_sensor = left.id;
				direction = LEFT;
			}
		}
		if(!sensor.right_nodes.empty()) {
			// Check if the right node is closer or not
			Sensor& right = sensors[sensor.right_nodes[0]];
			if(!right.has_failed) {
				if(distance > sensor_distance(branch, right)) {
					dst_sensor = right.id;
					direction = RIGHT;
				}
			}
		}
		if(dst_sensor != -1) {
			// Create a flatten connect event
			branch.dst_node = dst_sensor;
			Event flatten_branch;
			flatten_branch.type = FLATTEN_CONNECT_TO_DST;
			flatten_branch.id = branch.id;
			flatten_branch.dst_id = dst_sensor;
			flatten_branch.branch_id = sensor.id;
			flatten_branch.direction = direction;
			if(DEBUG) {
				printf("HOLY SHIT!!!!\nHOLY SHIT!!!!\nHOLY SHIT!!!!\nHOLY SHIT!!!!\n");
				printf("%d %d\n%d %d\n\n\n\n", sensor.id, branch.id, flatten_branch.branch_id, flatten_branch.id);
			}
			flatten_branch.event_number = event_counter++;
			events.push(make_pair(random_timestamp(timestamp), flatten_branch));
		}
	}
}

void handle_chain_maintenance(int timestamp, Event& e) {
	Sensor& sensor = sensors[e.id];
	apply_chain_force(sensor);
	if(DEBUG) {
		printf("##################Before: CHAIN FORCE magnitude: %lf\n", sensor.chain_force.magnitude());
	}
	if(sensor.chain_force.magnitude() > VMAX) {
		sensor.chain_force = sensor.chain_force / sensor.chain_force.magnitude() * VMAX;
	}
	if(DEBUG) {
		printf("###################After: CHAIN FORCE magnitude: %lf\n", sensor.chain_force.magnitude());
	}
	// Check if boundary sensor
	if((sensor.is_left_sensor && (sensor.x + sensor.chain_force.x - sensing_range) > 0) ||
		(sensor.is_right_sensor && (sensor.x + sensor.chain_force.x + sensing_range) < belt_length)) {
		// Move in y direction only
		sensor.distance += sensor.chain_force.y;
		sensor.y += sensor.chain_force.y;
	} else {
		sensor.distance += sensor.chain_force.magnitude();
		sensor.x += sensor.chain_force.x;
		sensor.y += sensor.chain_force.y;
	}
	// Trigger Chain Maintenace Events for the affected sensors
	sensor.sensor_force = null_force;
	vector<int> broken_siblings = broken_chain_sensors(sensor);
	if(DEBUG) {
		printf("PRINTING LIST OF BROKEN SIBLINGS for %d\n", sensor.id);
	}
	for(int i = 0; i < (int)broken_siblings.size(); i++) {
		// Create Chain Maintenance Event
		Event chain_maintenance;
		chain_maintenance.type = CHAIN_MAINTENANCE;
		chain_maintenance.id = broken_siblings[i];
		if(DEBUG) {
			printf("~~~~~~~~~~~~~~ %d ~~~~~~~~~~~~~\n", broken_siblings[i]);
		}
		chain_maintenance.event_number = event_counter++;
		events.push(make_pair(random_timestamp(timestamp), chain_maintenance));
	}
	// Since the current sensor has entered Chain Maintenance state, there is a scope for flattening
	// See if current sensor has any branches or not
	// If yes try to apply flattening logic on the branch sensors
	if(sensor.branches.size() > 0) {
		activate_flattening_sensors(sensor, timestamp);
	}
}

void handle_flatten_connect(int timestamp, Event& e) {
	Sensor& sensor = sensors[e.id];
	bool flag = false;
	if(sensor.dst_node != -1) {
		Sensor& dst_sensor = sensors[sensor.dst_node];
		// Check if already connect to the destination
		if(check_connected(sensor, dst_sensor)) {
			flag = true;
		}
		// Try to connect to the destination sensor
		apply_chain_force(sensor);
		Force f((dst_sensor.x - sensor.x), (dst_sensor.y - sensor.y), FLATTEN_FORCE_MAGNITUDE);
		f += sensor.chain_force;
		sensor.chain_force = null_force;
		// Normalize f if magnitude greater than VMAX
		if(f.magnitude() > VMAX) {
			f = f / f.magnitude() * VMAX;
		}
		sensor.x += f.x;
		sensor.y += f.y;
		if(check_connected(sensor, dst_sensor)) {
			flag = true;
		}

		// Check if the connection establishment is possible or not
		if(flag) {
			// Establish connection
			Sensor& original_sensor = sensors[e.branch_id];
			// Delete branch link from original sensor and current branch sensor
			if(DEBUG) {
				printf("Removing Branch %d -> %d\n", e.branch_id, sensor.id);
			}
			original_sensor.remove_branch(sensor.id);
			sensor.remove_branch(original_sensor.id);
			// Add connection to the destination sensor
			// current branch sensor will come in between destination sensor and original sensor
			if(e.direction == LEFT) {
				// destination sensor is the left sibling of the original sensor
				Sensor& branch = sensor;
				// copy the left nodes of the destination sensor to branch sensor
				branch.left_nodes = vector<int>(dst_sensor.left_nodes.begin(), dst_sensor.left_nodes.end());
				// copy the right nodes original sensor to right nodes of branch sensor
				branch.right_nodes = vector<int>(original_sensor.right_nodes.begin(), original_sensor.right_nodes.end());
				// current branch sensor will become the right node of destination sensor
				dst_sensor.right_nodes.insert(dst_sensor.right_nodes.begin(), branch.id);
				// similarly destination node will become the left node of branch sensor
				branch.left_nodes.insert(branch.left_nodes.begin(), dst_sensor.id);
				// Current branch sensor will become the left node of the original sensor
				original_sensor.left_nodes.insert(original_sensor.left_nodes.begin(), branch.id);
				// similarty original sensor will become the right node of the branch sensor
				branch.right_nodes.insert(branch.right_nodes.begin(), original_sensor.id);
				// Now branch has become a barrier sensor
				branch.on_barrier = true;

			} else if (e.direction == RIGHT) {
				// destination sensor is the right sibling of the original sensor
				Sensor& branch = sensor;
				// copy the right nodes of the destination sensor to branch sensor
				branch.right_nodes = vector<int>(dst_sensor.right_nodes.begin(), dst_sensor.right_nodes.end());
				// copy the left nodes original sensor to left nodes of branch sensor
				branch.left_nodes = vector<int>(original_sensor.left_nodes.begin(), original_sensor.left_nodes.end());
				// current branch sensor will become the left node of destination sensor
				dst_sensor.left_nodes.insert(dst_sensor.left_nodes.begin(), branch.id);
				// similarly destination node will become the right node of branch sensor
				branch.right_nodes.insert(branch.right_nodes.begin(), dst_sensor.id);
				// Current branch sensor will become the right node of the original sensor
				original_sensor.right_nodes.insert(original_sensor.right_nodes.begin(), branch.id);
				// similarty original sensor will become the left node of the branch sensor
				branch.left_nodes.insert(branch.left_nodes.begin(), original_sensor.id);
				// Now branch has become a barrier sensor
				branch.on_barrier = true;
			} else {
				// This should not happen
				printf("Error: Flatten event direction is NONE\n");
				exit(1);
			}
		} else {
			// Create the same event again
			Event new_e(e);
			new_e.event_number = event_counter++;
			events.push(make_pair(random_timestamp(timestamp), new_e));
		}
		// Activate chain maintenance here
		sensor.sensor_force = null_force;
		vector<int> broken_siblings = broken_chain_sensors(sensor);
		for(int i = 0; i < (int)broken_siblings.size(); i++) {
			// Create Chain Maintenance Event
			Event chain_maintenance;
			chain_maintenance.type = CHAIN_MAINTENANCE;
			chain_maintenance.id = broken_siblings[i];
			chain_maintenance.event_number = event_counter++;
			events.push(make_pair(random_timestamp(timestamp), chain_maintenance));
		}
	}
}

void process_event(int timestamp, Event& e) {
	// Sensor Failure
	if(e.type == SENSOR_FAILURE) {
		handle_sensor_failure(timestamp, e);
	} else if(e.type == FAILURE_DETECTED) {
		handle_failure_detection(timestamp, e);
	} else if(e.type == SEARCH_BRANCH) {
		handle_search_branch(timestamp, e);
	} else if(e.type == BRANCH_CONNECT_TO_DST) {
		handle_branch_connect(timestamp, e);
	} else if(e.type == BARRIER_CONNECT_TO_DST) {
		handle_barrier_connect(timestamp, e);
	} else if(e.type == CHAIN_MAINTENANCE) {
		handle_chain_maintenance(timestamp, e);
	} else if(e.type == FLATTEN_CONNECT_TO_DST) {
		handle_flatten_connect(timestamp, e);
	}
}


void line(double x1, double y1, double x2, double y2) {
	x1 -= 0.9f;
	y1 -= 0.9f;
	x2 -= 0.9f;
	y2 -= 0.9f;
	glBegin(GL_LINES);								// Drawing Using Triangles
		glVertex3f(x1, y1 , 0.0f);
		glVertex3f(x2, y2, 0.0f);
	glEnd();
}

void circle(double cx, double cy, double radius) {
	cx -= 0.9f;
	cy -= 0.9f;
	glBegin(GL_LINE_LOOP);
	for (int ii = 0; ii < num_segments; ii++)   {
		float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);//get the current angle 
		float x = radius * cosf(theta);//calculate the x component 
		float y = radius * sinf(theta);//calculate the y component 
		glVertex2f(x + cx, y + cy);//output vertex 
	}
	glEnd();
}
void draw_sensor(Sensor& sensor, float r, float g, float b) {
	// char sensor_id[10];
	float x = (float)((double)window_width * sensor.x / belt_length);
	float y = (float)((double)window_height * sensor.y / belt_width);
	// line(x,y, x+0.1f, y+0.1f);
	// sprintf(sensor_id, "%d", sensor.id);
	// Print sensor ID here
	// glRasterPos2f(x , y - (font->Ascender()));
	// font->Render(sensor_id);
	glColor3f(r, g, b);
	float drawing_sensing_range = (float)((double)window_width * sensing_range / belt_length);
	circle(x, y, drawing_sensing_range);
}
void draw_sensors() {
	for(int i = 0; i < n_sensors; i++) {
		Sensor& current_sensor = sensors[i];
		if(current_sensor.has_failed) {
			continue;
		}
		if(current_sensor.on_barrier) {
			// BLUE
			draw_sensor(current_sensor, 0.0f, 0.0f, 1.0f);
		} else {
			// RED
			draw_sensor(current_sensor, 1.0f, 0.0f, 0.0f);
		}
	}
}

void draw_link(int id1, int id2) {
	Sensor& sensor1 = sensors[id1];
	Sensor& sensor2 = sensors[id2];
	if(sensor2.has_failed) {
		return;
	}

	float x1 = (float)((double)window_width * sensor1.x / belt_length);
	float y1 = (float)((double)window_height * sensor1.y / belt_width);
	float x2 = (float)((double)window_width * sensor2.x / belt_length);
	float y2 = (float)((double)window_height * sensor2.y / belt_width);
	
	// GREEN
	glColor3f(0.0f, 1.0f, 0.0f);
	line(x1,y1,x2,y2);
}

void draw_links() {
	for(int i = 0; i < n_sensors; i++) {
		Sensor& current_sensor = sensors[i];
		if(current_sensor.has_failed) {
			continue;
		}
		// Draw Left Link
		if(!current_sensor.left_nodes.empty()) {
			draw_link(i, current_sensor.left_nodes[0]);
		}
		// Draw Right Link
		if(!current_sensor.right_nodes.empty()) {
			draw_link(i, current_sensor.right_nodes[0]);
		}
		// Draw Branches
		for(int j = 0; j < (int)current_sensor.branches.size(); j++) {
			draw_link(i, current_sensor.branches[j]);
		}
	}
}

void draw_sensors_and_links() {
	draw_sensors();
	draw_links();
}

void draw_belt() {
	// WHITE
	glColor3f(1.0f, 1.0f, 1.0f);
	line(0, 0, 0, window_height);
	glColor3f(1.0f, 0.0f, 0.0f);
	line(0, 0, window_width, 0);
	glColor3f(0.0f, 1.0f, 0.0f);
	line(window_width, 0, window_width, window_height);
	glColor3f(0.0f, 0.0f, 1.0f);
	line(0, window_height, window_width, window_height);

	glColor3f(1.0f, 1.0f, 0.0f);
	line(0, 0, -0.5, 0);
}

void refresh_display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();									// Reset The Current Modelview Matrix
}

void draw_current_state() {
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	

	refresh_display();
	draw_belt();
	draw_sensors_and_links();
	
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void print_sensor_locs() {
	printf("All Sensor Locations:\n");
	for(int i = 0; i < n_sensors; i++) {
		Sensor& current_sensor = sensors[i];
		if(current_sensor.is_left_sensor) {
			printf("Left Sensor:\n");
		} else if(current_sensor.is_right_sensor) {
			printf("Right Sensor:\n");
		}
		if(current_sensor.has_failed) {
			printf("**");
		}
		printf("%3d: %5.3lf, %5.3lf\n", current_sensor.id, current_sensor.x, current_sensor.y);
		printf("left:");
		for(int j = 0; j < (int)current_sensor.left_nodes.size(); j++) {
			printf(" %d", current_sensor.left_nodes[j]);
		}
		printf(" right:");
		for(int j = 0; j < (int)current_sensor.right_nodes.size(); j++) {
			printf(" %d", current_sensor.right_nodes[j]);
		}
		printf(" branches:");
		for(int j = 0; j < (int)current_sensor.branches.size(); j++) {
			printf(" %d", current_sensor.branches[j]);
		}
		if(current_sensor.dst_node != -1) {
			printf("\nDestination Node: %d", current_sensor.dst_node);
		}
		printf("\n");
	}
	printf("\n\n");
}

void maintain() {
	iterations = 0;
	while(!events.empty()) {
		// pull out one entry
		P p = events.top();
		int timestamp = p.first;
		max_timestamp = max(max_timestamp, timestamp);
		Event e = p.second;
		events.pop();
		// Event Processing
		// If the chain maintenance for current sensor is already handled for this timestamp then simply continue
		if(e.type == CHAIN_MAINTENANCE) {
			Sensor& sensor = sensors[e.id];
			// printf("id %d\n", e.id);
			if(sensor.timestamp != timestamp) {
				sensor.timestamp = timestamp;
			}
			else {
				continue;
			}
		}
		process_event(timestamp, e);
		// printf("timestamp: %5d : ", timestamp);
		// e.print();
		if(DEBUG) {
			// printf("Current Size of the queue: %d\n", (int)events.size());
			print_sensor_locs();
		}
		draw_current_state();		
		// usleep(1);
		usleep(100000);
		iterations++;
	}
}

void delete_sensor_graph() {
	for(int i = 0; i < n_sensors; i++) {
		sensor_graph[i].clear();
	}
}

void delete_data() {
	delete_sensor_graph();
}

void print_results() {
	printf("\n\n\n\n\nPRINTING FINAL RESULTS:\n");
	printf("Belt Dimensions: %lf * %lf\n", belt_length, belt_width);
	printf("Number of Sensors: %d\n", n_sensors);
	printf("Number of Failures: %d\n", n_failures);
	printf("Failed Sensors: ");
	for(int i = 0; i < n_sensors; i++) {
		if(sensors[i].has_failed) {
			printf("%3d ", i);
		}
	}
	printf("\n");
	printf("Sensing Range: %lf\n", sensing_range);
	printf("Communication Range: %lf\n", communication_range);
	printf("Simulation Iterations: %d\n", iterations);
	printf("Simulation Timestamps: %d\n", max_timestamp);
	printf("Sensor final locations and distances:\n");
	double avg_distance = 0.0;
	for(int i = 0; i < n_sensors; i++) {
		if(sensors[i].has_failed) {
			continue;
		}
		Sensor& sensor = sensors[i];
		printf("%3d: %5.3lf %5.3lf: %5.3lf\n", sensor.id, sensor.x, sensor.y, sensor.distance);
		avg_distance += sensor.distance;
	}
	avg_distance /= (double)(n_sensors - n_failures);
	printf("Average Distance Moved: %lf\n", avg_distance);
}

int main(int argc, char *argv[])
{
	srand(time(NULL));

	// Initializing the display window
	initialize(n_failures);
	if(DEBUG) {
		printf("Initializing Display now\n");
	}
	
	initialize_display();

	if(DEBUG) {
		printf("Initializing Done\n");
	}
	maintain();
	print_results();
	delete_data();
	
	return 0;
}
