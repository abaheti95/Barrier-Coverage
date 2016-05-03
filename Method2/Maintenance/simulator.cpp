#include "simulator.hpp"

using namespace std;

const int MAX_SENSORS = 100;
const double INFINITY_DOUBLE = 100000000.0;

#define VMAX (2.0 * sensing_range)
#define SENSOR_FORCE_OFFSET (sensing_range / 2.5)
#define CHAIN_FORCE_OFFSET (sensing_range / 5)

// Global Simulation Variables
double belt_length;
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
typedef pair<int, Event> P;
struct Order {
	bool operator()(P const& a, P const& b) const {
		return a.first > b.first;
	}
};
priority_queue< P, vector<P>, Order> events;

const Force null_force(0.0,0.0);



/****************** Simulation display code *****************/
int display_width = 1000;
int display_height = 800;
double window_width = 1.8f;
double window_height = 1.0f;

int num_segments = 60;
GLFWwindow* window;

FTPixmapFont* font;
/****************** End of display code *********************/

const bool DEBUG = true;
const double MULTIPLICATION_FACTOR = 3.0;

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
		vector<int> row;
		int n, id;
		scanf("%d", &n);
		for(int j = 0; j < n; j++) {
			scanf("%d", &id);
			row.push_back(id);
		}
		sensor_graph.push_back(row);
	}
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
				current_node.left_nodes.push_back(barrier[i - j]);
			} else {
				break;
			}
		}
		// Right Sensors
		for(int j = 1; j <= K; j++) {
			// Check if the desired sensor within index range
			if((i + j) < barrier_len) {
				current_node.right_nodes.push_back(barrier[i + j]);
			} else {
				break;
			}
		}
	}

	// Update the branches of all the sensors
	for(int i = 0; i < n_sensors; i++) {
		Sensor& current_node = sensors[i];
		int k = 0;
		for(int j = 0; j < (int)sensor_graph[i].size(); j++) {
			if(!current_node.left_nodes.empty() && sensor_graph[i][j] != current_node.left_nodes[0] 
				&& !current_node.right_nodes.empty() && sensor_graph[i][j] != current_node.right_nodes[0]) {
				current_node.branches.push_back(sensor_graph[i][j]);
			}
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
		window_height = (int)((double)window_width / belt_ratio);
	} else {
		// width is dominant
		window_height = 1.0;
		window_width = (int)((double)window_height * belt_ratio);
	}

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glTranslatef(-1.0f, -1.0f, 0.0f);
	glfwSwapBuffers(window);
	glfwPollEvents();

	printf("Belt Dimensions %lf x %lf\n", belt_length, belt_width);
	printf("Window dimensions %lf x %lf\n", window_width, window_height);

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

int nearest_left_barrier_sensor_within_communication_range(int index) {
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

int nearest_right_barrier_sensor_within_communication_range(int index) {
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

void handle_sensor_failure(int timestamp, Event& e) {
	// Get the ID of the failed node and trigger the nodes adjacent to it
	search_sensors_within_communication_range(e.failed_node);
	printf("communication range = %lf\n", communication_range);
	printf("Failed node: \n");
	for(int i = 0; i < n_sensors; i++) {
		printf("%d\n", within_range[i]);
		if(within_range[i] == -1) {
			break;
		}
		// Create a Failure Detection event for all the sensors within communication range
		Event failure_detected;
		failure_detected.type = FAILURE_DETECTED;
		failure_detected.id = within_range[i];
		failure_detected.failed_node = e.failed_node;
		failure_detected.failed_index = e.failed_index;
		events.push(make_pair(timestamp + 1, failure_detected));
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
		if(dst_sensor_id == -1) {
			// No barrier sensor present within the communication range
			// TODO: Waiting for someone to wake me up
		} else {
			// We have a destination sensor
			// Start approaching that sensor

			Event follow_dst;
			sensor.dst_node = dst_sensor_id;
			sensor.dst_barrier_index = sensors[dst_sensor_id].barrier_index;
			follow_dst.type = BRANCH_CONNECT_TO_DST;
			follow_dst.id = e.id;
			follow_dst.dst_id = dst_sensor_id;
			events.push(make_pair(timestamp + 1, follow_dst));
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
			events.push(make_pair(timestamp + 1, next_hop));
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
			events.push(make_pair(timestamp + 1, next_hop));
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
			events.push(make_pair(timestamp + 1, next_hop));
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
			events.push(make_pair(timestamp + 1, next_hop));
		}
	} else { // NO_SIBLING
		// In case of no sibling we don't create a new event.
		// When the adjacent sensor has finally connected with the new sensor, it will update the left/right nodes of this sensor
		// TODO: Decide whether to change this
	}
	sensor.remove_sibling(e.failed_node);
}

double sensor_force(double x) {
	if(x < 0) {
		return -1.0 * sensor_force(-x);
	}
	x -= 2*sensing_range;
	if(x <= 0) {
		return 0.0;
	}
	return (x*x/(10.0 * sensing_range) + SENSOR_FORCE_OFFSET);
}

double chain_force(double x) {
	if(x < 0) {
		return -1.0 * chain_force(-x);
	}
	x -= 2*sensing_range;
	if(x <= 0) {
		return 0.0;
	}
	return (x*x*x/(16.0 * sensing_range) + CHAIN_FORCE_OFFSET);
}

void apply_sensor_force(Sensor& sensor, Sensor& dst_sensor) {
	// Apply force towards the destination sensor
	sensor.sensor_force.x = sensor_force(dst_sensor.x - sensor.x);
	sensor.sensor_force.y = sensor_force(dst_sensor.y - sensor.y);
}

Force calculate_branch_forces(Sensor& sensor) {
	Force f;
	for(int i = 0; i < (int)sensor.branches.size(); i++) {
		Sensor& branch = sensors[sensor.branches[i]];
		f.x += sensor_force(branch.x - sensor.x);
		f.y += sensor_force(branch.y - sensor.y);
	}
	return f;
}

Force calculate_left_right_forces(Sensor& sensor) {
	Force f;
	if(sensor.left_nodes.size() > 0) {
		Sensor& left = sensors[sensor.left_nodes[0]];
		f.x += sensor_force(left.x - sensor.x);
		f.y += sensor_force(left.y - sensor.y);
	} else if(sensor.right_nodes.size() > 0) {
		Sensor& right = sensors[sensor.right_nodes[0]];
		f.x += sensor_force(right.x - sensor.x);
		f.y += sensor_force(right.y - sensor.y);
	}
	return f;
}

void apply_chain_force(Sensor& sensor) {
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
}

bool check_connected(Sensor& sensor, Sensor& dst_sensor) {
	return (distance(sensor.x, sensor.y, dst_sensor.x, dst_sensor.y) <= (2*sensing_range));
}

void handle_branch_connect(int timestamp, Event& e) {
	// This is a branch type sensor and it has to connect to its destination
	Sensor& sensor = sensors[e.id];
	Sensor& dst_sensor = sensors[e.dst_id];
	// Apply Sensor Force
	apply_sensor_force(sensor, dst_sensor);
	// Apply Chain Force
	apply_chain_force(sensor);
	Force total_force = sensor.chain_force + sensor.sensor_force;
	if(total_force.magnitude() > VMAX) {
		total_force = total_force / total_force.magnitude() * VMAX;
	}
	// Move in the direction of force
	sensor.distance += total_force.magnitude();
	sensor.x += total_force.x;
	sensor.y += total_force.y;
	sensor.sensor_force = null_force;
	sensor.chain_force = null_force;
	// Check if the two sensors are connected or not. If yes then create a new connection and make this sensor inactive
	if(check_connected(sensor, dst_sensor)) {
		// Create a connection between the two
		sensor.branches.push_back(dst_sensor.id);
		dst_sensor.branches.push_back(sensor.id);
		// No need to create another event
	} else {
		// Add another BRANCH_CONNECT_TO_DST event in the priority queue
		Event follow_dst;
		follow_dst.type = BRANCH_CONNECT_TO_DST;
		follow_dst.id = e.id;
		follow_dst.dst_id = e.dst_id;
		events.push(make_pair(timestamp + 1, follow_dst));
	}
}

//TODO: Check conditions if the other sensor has already made a connection
void handle_barrier_connect(int timestamp, Event& e) {
	// Check if current sensor has a destination sensor or not?
	Sensor& sensor = sensors[e.id];
	if(sensor.dst_node != -1) {
		// There is some destination node to reach to
		Sensor& dst_sensor = sensors[e.dst_id];
		// Apply Sensor Force
		apply_sensor_force(sensor, dst_sensor);
		// Apply Chain Force
		apply_chain_force(sensor);
		Force total_force = sensor.chain_force + sensor.sensor_force;
		if(total_force.magnitude() > VMAX) {
			total_force = total_force / total_force.magnitude() * VMAX;
		}
		// Move in the direction of force
		sensor.distance += total_force.magnitude();
		sensor.x += total_force.x;
		sensor.y += total_force.y;
		sensor.sensor_force = null_force;
		sensor.chain_force = null_force;
		// Check if the two sensors are connected or not. If yes then create a new connection and make this sensor inactive
		if(check_connected(sensor, dst_sensor)) {
			// Create a connection between the two
			sensor.branches.push_back(dst_sensor.id);
			dst_sensor.branches.push_back(sensor.id);
			// No need to create another event
		} else {
			// Add another BRANCH_CONNECT_TO_DST event in the priority queue
			Event follow_dst;
			follow_dst.type = BRANCH_CONNECT_TO_DST;
			follow_dst.id = e.id;
			follow_dst.dst_id = e.dst_id;
			events.push(make_pair(timestamp + 1, follow_dst));
		}
	} else {
		// go to the next_hop node location
		Sensor& dst_sensor = sensors[sensor.next_hop_node];
		if(sensor_distance(sensor, dst_sensor) <= VMAX) {
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
				events.push(make_pair(timestamp + 1, next_hop));
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
				events.push(make_pair(timestamp + 1, next_hop));
			}
		} else {
			// Move with VMAX speed towards next hop location
			Force f(dst_sensor.x - sensor.x, dst_sensor.y - sensor.y);
			f = f / f.magnitude() * VMAX;
			sensor.distance += VMAX;
			sensor.x += f.x;
			sensor.y += f.y;
			// Create another self-triggered event to keep this alive
			Event follow_dst;
			follow_dst.type = BRANCH_CONNECT_TO_DST;
			follow_dst.id = e.id;
			follow_dst.dst_id = e.dst_id;
			events.push(make_pair(timestamp + 1, follow_dst));
		}
	}
}

void process_event(int timestamp, Event& e) {
	// Sensor Failure
	if(e.type == SENSOR_FAILURE) {
		handle_sensor_failure(timestamp, e);
	} else if(e.type == FAILURE_DETECTED) {
		handle_failure_detection(timestamp, e);
	} else if(e.type == BRANCH_CONNECT_TO_DST) {
		handle_branch_connect(timestamp, e);
	} else if(e.type == BARRIER_CONNECT_TO_DST) {
		handle_barrier_connect(timestamp, e);
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

void draw_sensors() {
	float drawing_sensing_range = (float)((double)window_width * sensing_range / belt_length);
	char sensor_id[10];
	for(int i = 0; i < n_sensors; i++) {
		Sensor& current_sensor = sensors[i];
		if(current_sensor.has_failed) {
			continue;
		}
		float x = (float)((double)window_width * current_sensor.x / belt_length);
		float y = (float)((double)window_height * current_sensor.y / belt_width);
		// line(x,y, x+0.1f, y+0.1f);
		sprintf(sensor_id, "%d", current_sensor.id);
		// Print sensor ID here
		// glRasterPos2f(x , y - (font->Ascender()));
		// font->Render(sensor_id);
		if(current_sensor.on_barrier) {
			// BLUE
			glColor3f(0.0f, 0.0f, 1.0f);
		} else {
			// RED
			glColor3f(1.0f, 0.0f, 0.0f);
		}
		circle(x, y, drawing_sensing_range);
	}
}

void draw_link(int id1, int id2) {
	Sensor& sensor1 = sensors[id1];
	Sensor& sensor2 = sensors[id2];

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
			draw_link(i, current_sensor.branches[i]);
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
	printf("!!!!!!!!!!!!!!!!!Drawing current state!!!!!!!!!!!!!!!!!!!!!\n");
	draw_belt();
	draw_sensors_and_links();
	
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void print_sensor_locs() {
	printf("All Sensor Locations:\n");
	for(int i = 0; i < n_sensors; i++) {
		Sensor& current_sensor = sensors[i];
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
	while(!events.empty()) {
		// pull out one entry
		P p = events.top();
		int timestamp = p.first;
		Event e = p.second;
		events.pop();
		// Event Processing
		process_event(timestamp, e);
		if(DEBUG) {
			printf("Current Size of the queue: %d\n", (int)events.size());
			printf("timestamp: %d\n", timestamp);
			e.print();
			print_sensor_locs();
		}
		printf("refreshing display\n");
		draw_current_state();
		printf("Refreshing done!!\n");
		sleep(1);
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

int main(int argc, char *argv[])
{
	// srand(time(NULL));

	// Initializing the display window
	initialize(3);
	if(DEBUG) {
		printf("Initializing Display now\n");
	}
	
	initialize_display();

	if(DEBUG) {
		printf("Initializing Done\n");
	}
	maintain();

	delete_data();
	
	return 0;
}
