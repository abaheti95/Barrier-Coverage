#include "Sensor.hpp"
Sensor::Sensor(double x, double y) {
	this->id = -1;
	this->init_x = x;
	this->init_y = y;
	this->x = x;
	this->y = y;
	this->left_nodes = vector<int>();
	this->right_nodes = vector<int>();
	this->has_branches = false;
	this->branches = vector<int>();
	this->on_barrier = false;
	this->barrier_index = -1;
	this->has_failed = false;
	this->distance = 0.0;
	this->dst_node = -1;
	this->dst_barrier_index = -1;
}

Sensor::~Sensor() {
	left_nodes.clear();
	right_nodes.clear();
	branches.clear();
	// delete left_nodes;
	// delete right_nodes;
	// delete branches;
}

bool Sensor::is_left_sibling(int id) {
	if(this->left_nodes.size() != 0) {
		return this->left_nodes[0] == id;
	}
	return false;
}

bool Sensor::is_right_sibling(int id) {
	if(this->right_nodes.size() != 0) {
		return this->right_nodes[0] == id;
	}
	return false;
}

bool Sensor::is_branch(int id) {
	for(int i = 0; i < int(this->branches.size()); i++) {
		if(this->branches[i] == id) {
			return true;
		}
	}
	return false;
}

Sibling_Type Sensor::is_adjacent(int id) {
	// Check if the given id is adjacent to current sensor or not
	if(this->is_left_sibling(id)) {
		return LEFT_SIBLING;
	} else if(this->is_right_sibling(id)) {
		return RIGHT_SIBLING;
	} else if(this->is_branch(id)) {
		return BRANCH_SIBLING;
	}
	return NO_SIBLING;
}

void Sensor::remove_sibling(int id) {
	left_nodes.erase(std::remove(left_nodes.begin(), left_nodes.end(), id), left_nodes.end());
	right_nodes.erase(std::remove(right_nodes.begin(), right_nodes.end(), id), right_nodes.end());
	branches.erase(std::remove(branches.begin(), branches.end(), id), branches.end());
}
