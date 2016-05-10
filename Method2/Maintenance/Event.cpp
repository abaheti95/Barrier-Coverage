#include "Event.hpp"

Event::Event() {
	type = NO_TYPE;
	failed_node = -1;
	failed_index = -1;
	id = -1;
	dst_id = -1;
	direction = NO_DIRECTION;
}

Event::Event(const Event& e) {
	this->type = e.type;
	this->failed_node = e.failed_node;
	this->failed_index = e.failed_index;
	this->id = e.id;
	this->dst_id = e.dst_id;
	this->direction = e.direction;
}

void Event::print() {
	switch(type) {
		case SENSOR_FAILURE:
			printf("Event: Sensor Failure\n");
			break;
		case FAILURE_DETECTED:
			printf("Event: FAILURE_DETECTED\n");
			break;
		case BRANCH_CONNECT_TO_DST:
			printf("Event: Branch Connecting to Destination\n");
			break;
		case BARRIER_CONNECT_TO_DST:
			printf("Event: Barrier Connecting to Destination\n");
			break;
		case CHAIN_MAINTENANCE:
			printf("Event: Chain Maintenance\n");
			break;
		case NO_TYPE:
			printf("Event: No type\n");
			break;
	}
	printf("Failed node: %d, failed_index: %d, id: %d, dst id: %d, direction:", failed_node, failed_index, id, dst_id);
	switch(direction) {
		case LEFT:
			printf("LEFT\n");
			break;
		case RIGHT:
			printf("RIGHT\n");
			break;
		case NO_DIRECTION:
			printf("NONE\n");
	}
}