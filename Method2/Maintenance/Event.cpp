#include "Event.hpp"

Event::Event() {
	type = NO_TYPE;
	failed_node = -1;
	failed_index = -1;
	id = -1;
	branch_id = -1;
	dst_id = -1;
	direction = NO_DIRECTION;
	event_number = 0;
}

Event::Event(const Event& e) {
	this->type = e.type;
	this->failed_node = e.failed_node;
	this->failed_index = e.failed_index;
	this->id = e.id;
	this->dst_id = e.dst_id;
	this->branch_id = e.branch_id;
	this->direction = e.direction;
	this->event_number = e.event_number;
}

void Event::print() {
	switch(type) {
		case SENSOR_FAILURE:
			printf("Event: Sensor Failure");
			break;
		case FAILURE_DETECTED:
			printf("Event: FAILURE_DETECTED");
			break;
		case SEARCH_BRANCH:
			printf("Event: Branch sensor searching for some sensor to connect");
			break;
		case BRANCH_CONNECT_TO_DST:
			printf("Event: Branch Connecting to Destination");
			break;
		case BARRIER_CONNECT_TO_DST:
			printf("Event: Barrier Connecting to Destination");
			break;
		case CHAIN_MAINTENANCE:
			printf("Event: Chain Maintenance");
			break;
		case FLATTEN_CONNECT_TO_DST:
			printf("Event: Flatten Branch Connecting to Destination");
			break;
		case NO_TYPE:
			printf("Event: No type");
			break;
	}
	printf(": Event number: %d\n", event_number);
	printf("Id: %3d, Dst_id: %3d, branch_id: %3d\n", id, dst_id, branch_id);
	// printf("Failed node: %d, failed_index: %d, id: %d, dst id: %d, direction:", failed_node, failed_index, id, dst_id);
	/*switch(direction) {
		case LEFT:
			printf("LEFT\n");
			break;
		case RIGHT:
			printf("RIGHT\n");
			break;
		case NO_DIRECTION:
			printf("NONE\n");
	}*/
}

int value_from_type(Type type) {
	// Returns the priority of the event;
	switch(type) {
		case SENSOR_FAILURE:
			return 1;
		case FAILURE_DETECTED:
			return 2;
		case BARRIER_CONNECT_TO_DST:
			return 3;
		case BRANCH_CONNECT_TO_DST:
			return 4;
		case FLATTEN_CONNECT_TO_DST:
			return 5;
		case CHAIN_MAINTENANCE:
			return 6;
		case NO_TYPE:
			return 7;
		default:
			return 8;
	}
}
