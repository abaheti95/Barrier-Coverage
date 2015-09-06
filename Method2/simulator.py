import math
import time

import pygame
from pygame.locals import *
from pygame.color import *
import pymunk

INFINITY = 10000000000
FORCE_THRESHOLD = 1
# Enable or disable DEBUG
DEBUG = True

#Taking sensing range as input
sensingRange = float(input("Enter the sensing range of the sensors : "))
beltWidth = float(input("Enter the beltWidth : "))
beltHeight = float(input("Enter the beltHeight: "))
numberOfSensors = int(input("Enter the number of sensors: "))

# Initializing pymunk and pygame
# Small error quantity and small pointmass radius
delta = 0.01

pygame.init()
padding = 10
max_screen_width = 1180
max_screen_height = 600
screen_width = None
screen_height = None
# calculate screen width and heigh based on the belt width and height
if (max_screen_width/max_screen_height) <= (beltWidth/beltHeight):
	# width preference
	screen_height = (max_screen_width * beltHeight / beltWidth);
	screen_width = max_screen_width;
else:
	# height preference
	screen_height = max_screen_height;
	screen_width = (max_screen_height * beltWidth / beltHeight);

pygame_screen = pygame.display.set_mode((max_screen_width + 2*padding + 100,max_screen_height + 2*padding + 100))
pygame.display.set_caption("Barrier Coverage Simulation")
pygame_clock = pygame.time.Clock()


space = pymunk.Space()
# gravity free environment
space.gravity = (0.0, 0.0)

# Global list of sensors and chain graphs
sensors = []
chain_graphs = dict()

# Right Sensor and Left Sensor are special sensors therefore they have a global reference
left_sensor = None
right_sensor = None
left_chain_graph = None
right_chain_graph = None

def get_screen_coordinates(x, y):
	scale = screen_width / beltWidth
	screen_x = int(x * scale) + padding
	screen_y = int(screen_height - y * scale) + padding
	return screen_x, screen_y

def euclidean_distance(x1, y1, x2, y2):
	change_x = x2 - x1
	change_y = y2 - y1
	distance_square = change_x*change_x + change_y*change_y
	return math.sqrt(distance_square)

class Sensor():
	"""Sensor is a class holding data regarding individual sensor"""
	# sensing range is a common variable shared across all sensors
	sensing_range = sensingRange
	def __init__(self, ID, x, y):
		self.id = ID
		self.x = x
		self.y = y
		self.init_x = x
		self.init_y = y
		self.distance = 0.0
		self.parent_chain = -1			# currently no parent chain graph
		self.dist_from_leaf = None		# will store the distance of current sensor from the leaf in a chain graph
		# Creating Sensor body in pymunk
		mass = 1
		self.radius = delta
		inertia = pymunk.moment_for_circle(mass, 0, self.radius)
		self.body = pymunk.Body(mass, inertia)
		self.body.position = self.x , self.y
		self.shape = pymunk.Circle(self.body, self.radius)
		space.add(self.body, self.shape)
	
	def update_position(self, new_pos):
		# update distance travelled by the sensor
		self.distance += euclidean_distance(self.x, self.y, new_pos[0], new_pos[1])
		# update sensor position
		self.x = new_pos[0]
		self.y = new_pos[1]

	def update_sensing_range(self, new_sensing_range):
		# update only if new sensing range is positive
		if new_sensing_range > 0.0:
			Sensor.sensing_range = new_sensing_range

	def get_screen_coordinates(self):
		return get_screen_coordinates(self.x, self.y)

	def print_sensor(self):
		print "Id : " + str(self.id),
		print "x : " + str(self.x) + " y : " + str(self.y)

	def draw_sensor(self):		
		scale = screen_width / beltWidth
		p = self.get_screen_coordinates()
		pygame.draw.circle(pygame_screen, THECOLORS["red"], p, int(sensingRange * scale), 1)
		coordinate_text = "%d(%.1f,%.1f)" % (self.id,self.x, self.y)
		coordinate_font = pygame.font.SysFont("comicsansms", 15)
		coordinate_label = coordinate_font.render(coordinate_text, 1, THECOLORS["black"])
		p = list(p)
		p[0] -= 30
		p[1] -= 25
		p = tuple(p)
		pygame_screen.blit(coordinate_label, p)
		if self.dist_from_leaf == None:
			print str(self.id)
		dist_from_leaf_text = "%d" % (self.dist_from_leaf)
		dist_from_leaf_font = pygame.font.SysFont("comicsansms", 15)
		dist_from_leaf_label = dist_from_leaf_font.render(dist_from_leaf_text, 1, THECOLORS["black"])
		p = list(p)
		p[0] += 15
		p[1] += 25
		p = tuple(p)
		pygame_screen.blit(dist_from_leaf_label, p)

	def get_distance_from_leaf(self):
		if self.dist_from_leaf == 0:
			return 0.5
		elif self.dist_from_leaf >= 3:
			return 3
		else:
			return self.dist_from_leaf

	def print_results(self):
		print str(self.id) + " :"
		print "(" + str(self.init_x) + "," + str(self.init_y) + ") -> (" + str(self.x) + "," + str(self.y) + ")"
		print "Distance Traveled = " + str(self.distance)
		print "Displacement = " + str(euclidean_distance(self.init_x, self.init_y, self.x, self.y))

def print_chain_graph(graph):
	# print function used while debugging
	for key in graph.keys():
		print str(key) + " : ",
		print graph[key]

def sensor_distance(sensor1, sensor2):
	return euclidean_distance(sensor1.x, sensor1.y, sensor2.x, sensor2.y)

def force1(sensor1, sensor2):
	distance = sensor_distance(sensor1, sensor2)
	if distance <= 2*sensingRange:
		# two sensors are intersecting therefore no force
		return 0.0
	FORCE_FACTOR = beltWidth * beltHeight * sensingRange * sensingRange / 5
	force = FORCE_FACTOR / distance / (sensor1.get_distance_from_leaf() + sensor2.get_distance_from_leaf())
	return force

def force(sensor1, sensor2):
	# Different force models are to be applied here
	return force1(sensor1, sensor2)

def apply_flatten_force(sensor1, sensor2):
	Force = 0.10
	# apply forces on all dominant points
	distance = sensor_distance(sensor1, sensor2)
	Force_x = Force * (sensor2.x - sensor1.x) / distance
	Force_y = Force * (sensor2.y - sensor1.y) / distance
	sensor1.body.apply_force((Force_x,Force_y),(0,0))

def flatten_children(children):
	if len(children) < 2:
		if DEBUG:
			print "WTF??"
		return
	children = list(children)
	# We are provided with a set of children which are to be flattened out
	# We apply attractive forces amongst them
	for child1 in children:
		# find the closest sibling of child1
		Min = INFINITY
		closest_sibling = None
		for child2 in children:
			if child1 is not child2:
				distance = sensor_distance(sensors[child1], sensors[child2])
				if distance < Min:
					Min = distance
					closest_sibling = child2
		apply_flatten_force(sensors[child1], sensors[closest_sibling])

def joining_children(children):
	if len(children) == 0:
		if DEBUG:
			print "Joining WTF??"
		return
	children = list(children)
	# check if any childred are intersecting
	flag = False
	child1, child2 = None, None
	for i in range(len(children)):
		for j in range(i+1,len(children)):
			if sensor_distance(sensors[children[i]],sensors[children[j]]) <= 2*sensingRange:
				child1 = children[i]
				child2 = children[j]
				flag = True
				break
		if flag:
			break
	return child1, child2




class ChainGraph():
	"""ChainGraph is the class holding data of the connected ChainGraph"""
	# This will store the the actual graph of the sensors in the form of dictionary of dictionary
	# parent dictionary is of the form
	# sensor_id : child_dictionary
	# child dictionary is of the form
	# sensor_id : pymunk_joint_ref
	def init_flatten_properties(self):
		# tells if a chain link/ sensor node is marked while flattening.. already marked nodes are not to be flattened
		self.marked = set()
		# This is applicable only for left and right chain graphs
		# Tells if a chain link / sensor node is present in the branch containing left sensor or right sensor
		self.edge_marked = set()
		# current is the currently unmarked node
		self.current = None

		if (self is left_chain_graph or self is right_chain_graph) and self != None and self.dominant != None:
			# get the boundary sensor
			boundary_sensor = None
			if self is left_chain_graph:
				boundary_sensor = left_sensor
			elif self is right_chain_graph:
				boundary_sensor = right_sensor
			
			# Pre edge mark the boundary sensor
			self.edge_marked.add(boundary_sensor)
			# Start traversing from dominant point until you reach a branching or reach boundary sensor
			self.current = self.dominant
			while True:
				if self.current == boundary_sensor:
					# reached the boundary sensor first therefore break
					break
				# Look for a node where unmarked children are more than 1 children
				children = list(set(self.graph[self.current].keys()) - self.marked)
				number_of_children = len(children)
				# start with the current unmarked node
				# if the current node has only one child(which is basically current's keys - marked set) then simply mark and jump to next node
				if number_of_children == 1:
					if children[0] == boundary_sensor:
						# only child in the boundary sensor therefore search ends here
						break
					else:
						self.marked.add(self.current)
						self.current = children[0]
				else:
					# found a node with either 0 or more than 1 unmarked children, hence break
					break
			# We have marked some sensors till current node
			# Now we mark edge sensors starting from boundary_sensor till we reach current node
			edge_sensor = boundary_sensor.id
			# if the boundary sensor has more than one sibling then we will have to flatten it separately
			self.edge_flatten_flag = False
			if len(self.graph[edge_sensor].keys()) > 1:
				self.edge_flatten_flag = True
				return
			while True:
				if edge_sensor == self.current:
					break
				# Look for a node where unmarked children are more than 1 children
				children = list(set(self.graph[edge_sensor].keys()) - self.edge_marked)
				number_of_children = len(children)
				if number_of_children == 1:
					# simply jump to next senor and update the edge_marked set
					self.edge_marked.add(edge_sensor)
					edge_sensor = children[0]
				elif number_of_children >= 2:
					# found a node with more than 1 edge unmarked children, hence break
					break

	def __init__(self, ID):
		self.id = ID
		self.graph = dict()
		# no dominant point selected yet
		self.dominant = None
		self.dominant_neighbor = None
		
		
		self.init_flatten_properties()

	def verify_edges(self):
		flag = False
		for sensor1 in self.graph.keys():
			for sensor2 in self.graph[sensor1].keys():
				if sensor_distance(sensors[sensor1], sensors[sensor2]) > 2*sensingRange:
					if DEBUG:
						print str(sensor1) + " = " + str(sensor2) + " : " + str(sensor_distance(sensors[sensor1], sensors[sensor2]))
					flag = True
		return flag

	def add_edge(self, first, second):
		if self.graph.get(first) == None:
			self.graph[first] = dict()
		if self.graph.get(second) == None:
			self.graph[second] = dict()

		if self.graph.get(first).get(second) != None:
			return
		self.graph.get(first)[second] = True
		self.graph.get(second)[first] = True

	def add_chain_edge(self, first, second):
		# initialze child dictionaries if None
		if self.graph.get(first) == None:
			self.graph[first] = dict()
		if self.graph.get(second) == None:
			self.graph[second] = dict()

		if self.graph.get(first).get(second) != None:
			return
		# create a joint between the two sensors
		chain_slide_joint_ref = pymunk.SlideJoint(sensors[first].body, sensors[second].body, (0,0), (0,0), -1, 2*sensingRange)
		space.add(chain_slide_joint_ref)
		
		self.graph[first][second] = chain_slide_joint_ref
		self.graph[second][first] = chain_slide_joint_ref

	def remove_chain_edge(self, first, second):
		chain_slide_joint_ref = self.graph.get(first).pop(second)
		self.graph.get(second).pop(first)

		# remove joint from pymunk
		space.remove(chain_slide_joint_ref)

	def calculate_distances_from_leaves(self):
		# case of single sensor chain graph
		if len(self.graph.keys()) == 1:
			sensors[self.graph.keys()[0]].dist_from_leaf = 0
			return

		visited = set()
		chain_graph_sensors_set = set(self.graph.keys())
		# mark the leafs first
		for sensor in self.graph.keys():
			if len(self.graph[sensor].keys()) == 1:
				sensors[sensor].dist_from_leaf = 0
				visited.add(sensor)
			else:
				sensors[sensor].dist_from_leaf = None

		# Update the distance from leaf for rest of the sensors
		for k in range(1,len(self.graph.keys())):
			unvisited_sensors = list(chain_graph_sensors_set - visited)
			if len(unvisited_sensors) == 0:
				break
			# This set will store all the sensors which are at a distance k from the leaf where k ranges from 0 to distance of center of graph
			sensors_at_distance = set()
			for unvisited_sensor in unvisited_sensors:
				# if any neighbor is marked then add this sensor in the set
				for neighbor in self.graph[unvisited_sensor].keys():
					if sensors[neighbor].dist_from_leaf != None:
						sensors_at_distance.add(unvisited_sensor)
						break
			# update distances of all sensors in the set sensors_at_distance
			for sensor in sensors_at_distance:
				sensors[sensor].dist_from_leaf = k
			# update the set of visited sensors
			visited = visited | sensors_at_distance

	def get_dfs_tree(self):
		dummy_graph = self.graph

		self.graph = dict()
		# perform dfs on dummy_graph and get the new final graph
		# take the leftmost sensor of the graph as the start sensor for dfs
		Min = INFINITY
		# no sensor selected yet
		start = -1
		for sensor_id in dummy_graph.keys():
			if sensors[sensor_id].x < Min:
				Min = sensors[sensor_id]
				start = sensor_id

		visited, stack = set(), [(start,-1)]
		while stack:
			stack_top = stack.pop()
			vertex = stack_top[0]
			incoming_vertex = stack_top[1]

			if vertex not in visited:
				self.graph[vertex] = dict()
				if incoming_vertex != -1:
					self.add_chain_edge(vertex, incoming_vertex)
				visited.add(vertex)
				unvisited_vertices = set(dummy_graph.get(vertex).keys()) - visited
				current_vertex_list = [vertex] * len(unvisited_vertices)
				stack_extention = zip(unvisited_vertices, current_vertex_list)
				stack.extend(stack_extention)
		# Tree formed with edges in pymunk
		# delete existing graph
		dummy_graph = None
		# Set things before the flattening begins
		self.marked = set()
		self.edge_marked = set()
		self.current = None

		# Calculate the distances from leaves after getting the dfs tree
		self.calculate_distances_from_leaves()

	# Remove edge from child1 and retain edge with child2
	def join_children(self, root, child1, child2):
		if sensors[child2] is left_sensor or sensors[child2] is right_sensor:
			# delete the edge of left or right sensor with root
			join_children(self, root, child2, child1)
			return
		# define a partial order between current, child1 and child2
		self.remove_chain_edge(self.current, child1)
		self.add_chain_edge(child1, child2)
		# Update the distance from leaves for all sensors
		self.calculate_distances_from_leaves()
		# reset forces on childs
		sensors[child1].body.reset_forces()
		sensors[child2].body.reset_forces()

	def join_children_based_on_distance(self, root, child1, child2):
		# essential the child with edge having minimum distance from the leaf is to be retained
		if sensors[child1].dist_from_leaf < sensors[child2].dist_from_leaf:
			# swap children
			child1, child2 = child2, child1
		self.join_children(root, child1, child2)

	def collapse_edge_chain(self):
		if self is left_chain_graph:
			boundary_sensor = left_sensor
		if self is right_chain_graph:
			boundary_sensor = right_sensor
		siblings = self.graph[boundary_sensor.id].keys()
		if len(siblings) <= 1:
			# undesired edge collapsed
			self.edge_flatten_flag = False
			return
		flatten_children(siblings)
		child1, child2 = joining_children(siblings)
		if child1 != None and child2 != None:
			if child1 == self.current or child2 == self.current:
				# We don't want to retain edge with self.current
				if child2 == self.current:
					# swap child1 and child2
					child1, child2 = child2, child1
				self.join_children(boundary_sensor.id, child1, child2)
			else:
				self.join_children_based_on_distance(boundary_sensor.id, child1, child2)
	# this function is called when you want to flatten the left or right chain graph i.e. the chain graphs attached to left or right boundary of belt region
	def flatten_edge(self, root):
		if self.edge_flatten_flag == True:
			self.collapse_edge_chain()
		if root == None:
			return
		# Here we want current to always be some sensor with unmarked flattenable children
		while True:
			# Current sensor's siblings can belong to 3 categories
			siblings = set(self.graph[self.current].keys())
			# 1 - Marked Siblings
			marked_siblings = siblings & self.marked
			# 2 - Edge-Marked Siblings
			edge_marked_siblings = siblings & self.edge_marked
			# 3 - Unmarked Siblings
			unmarked_siblings = siblings - marked_siblings - edge_marked_siblings

			if len(unmarked_siblings) == 0:
				# nothing to flatten. The chain graph is already flattened
				return
			elif len(unmarked_siblings) == 1 and len(edge_marked_siblings) == 0:
				if self.id == 2:
					print "previous ",
					print self.current
				# mark current and move to unmarked child
				self.marked.add(self.current)
				self.current = list(unmarked_siblings)[0]
				if self.id == 2:
					print "next ",
					print self.current
			else:
				# valid flattenable position
				break

		# There is something to flatten
		# There can be only two situation
		if len(edge_marked_siblings) == 0:
			# 1 - current sensor has only one marked sensor and rest unmarked sensors
			# Therefore, go by standard desing and join the unmarked senors if available
			flatten_children(unmarked_siblings)
			child1, child2 = joining_children(unmarked_siblings)
			if child1 != None and child2 != None:
				self.join_children_based_on_distance(self.current, child1, child2)
		else:
			# 2 - current sensor has both marked and edge marked sensors as sibling along with 0 or more unmarked siblings
			# Here again we have 2 possibilities
			if len(marked_siblings) == 0:
				# 2.1 - Doesn't have any marked sensor i.e. root node itself has branches
				# Therefore, join unmarked and edge marked siblings
				flatten_children((unmarked_siblings | edge_marked_siblings))
				child1, child2 = joining_children((unmarked_siblings | edge_marked_siblings))
				if child1 != None and child2 != None:
					# check if any of them in edge_marked_siblings
					if child1 in edge_marked_siblings or child2 in edge_marked_siblings:
						if child2 in edge_marked_siblings:
							# swap child1 and child2
							child1, child2 = child2, child1
						self.join_children(self.current, child1, child2)
						# current -- unmarked -- child1
					else:
						# join using distance from leaf
						self.join_children_based_on_distance(self.current, child1, child2)
			else:
				# 2.2 - Root node has both marked and edge marked 
				# Therefore, move the unmarked towards marked sensor
				flatten_children((marked_siblings | unmarked_siblings))
				child1, child2 = joining_children((marked_siblings | unmarked_siblings))
				if child1 != None and child2 != None:
					# check if any of them in edge_marked_siblings
					if child1 in marked_siblings or child2 in marked_siblings:
						if child2 in marked_siblings:
							# swap child1 and child2
							child1, child2 = child2, child1
						self.join_children(self.current, child1, child2)
						# marked(child1) -- unmarked -- current
						# Therefore, make unmarked child as current
						self.current = child2
					else:
						# join using distance from leaf
						self.join_children_based_on_distance(self.current, child1, child2)

	def flatten(self, root):
		if root == None:
			return
		# here we want to flatten the tree based on the root
		if self.current == None:
			self.current = root

		if (self is left_chain_graph or self is right_chain_graph) and self.dominant != None:
			self.flatten_edge(root)
			return
		

		# mark the vertices as you travel in the tree
		while True:
			children = list(set(self.graph[self.current].keys()) - self.marked)
			number_of_children = len(children)
			# star with the current unmarked node
			# if the current node has only one child(which is basically current's keys - marked set) then simply mark and jump to next node
			if number_of_children == 0:
				# nothing to flatten
				break
			elif number_of_children < 2:
				self.marked.add(self.current)
				self.current = children[0]
			else:
				# check if any childred are intersecting
				flatten_children(children)
				child1, child2 = joining_children(children)
				if child1 != None and child2 != None:
					# join children
					self.join_children_based_on_distance(self.current, child1, child2)
				break

	def draw_chain_graph(self):
		# traverse the graph and draw edges one by one
		for key in self.graph.keys():
			for sub_key in self.graph.get(key).keys():
				# draw green lines to depict joints
				p1 = sensors[key].get_screen_coordinates()
				p2 = sensors[sub_key].get_screen_coordinates()
				pygame.draw.lines(pygame_screen, THECOLORS["green"], False, [p1,p2])
		# draw all the sensors of the chain graph
		for key in self.graph.keys():
			sensors[key].draw_sensor()

	def print_graph(self):
		print "Chain Graph : " + str(self.id)
		print_chain_graph(self.graph)

	def barrier_covered(self):
		# find the leftmost and rightmost sensors of chain graph and check if it spans the belt
		if (len(self.graph.keys())*2*sensingRange + 2*delta) <= beltWidth:
			return False
		Min = INFINITY
		Max = 0.0
		for key in self.graph.keys():
			if sensors[key].x < Min:
				Min = sensors[key].x
			if sensors[key].x > Max:
				Max = sensors[key].x
		# Some error tolerance
		print "Yahan pe kuch print kyu nahi hua"
		print ((Max - Min) + 2*sensingRange + 2*delta)
		return (((Max - Min) + 2*sensingRange + 2*delta) >= beltWidth)

	def calculate_dominant_for_given_set(self,other_sensors):
		# a dictionary which will store local maximal forces for each of the sensor and ther corresponding neighbors
		local_forces = dict()
		for key in self.graph.keys():
			local_forces[key] = None
			# Maximize the local force on a sensor
			local_force = 0.0
			local_neighbor = None
			for sensor in other_sensors:
				# calculate force for each key in this graph with each of other sensor
				current_force = force(sensors[key], sensors[sensor])
				if local_force < current_force:
					local_force = current_force 
					local_neighbor = sensor
			# update local forces for current sensor
			local_forces[key] = (local_force, local_neighbor)
		# find the sensor with maximum local force
		global_force = 0.0
		dominant = None
		dominant_neighbor = None
		for sensor, value in local_forces.iteritems():
			if global_force < value[0]:
				global_force = value[0]
				dominant = sensor
				dominant_neighbor = value[1]
		return dominant, dominant_neighbor

	def calculate_dominant_point(self):
		# TODO: Include the cases for left and righ boundary chains
		if self is left_chain_graph or self is right_chain_graph:
			if self is left_chain_graph:
				other_chain_graph = right_chain_graph
			else:
				other_chain_graph = left_chain_graph
			# we calculate dominant point twice for this case
			# once without taking other_chain_graph sensors into consideration and once taking them into consideration
			other_sensors = list(set(range(numberOfSensors)) - set(self.graph.keys()))
			dominant_w, dominant_neighbor_w = self.calculate_dominant_for_given_set(other_sensors)
			other_sensors = list(set(other_sensors) - set(other_chain_graph.graph.keys()))
			dominant_wo, dominant_neighbor_wo = self.calculate_dominant_for_given_set(other_sensors)
			# if with and without other_chain_graph the dominaint points are differnet and the edge chain graphs are not expandable then keep without
			if dominant_neighbor_w != dominant_neighbor_wo and dominant_wo != None and (((len(self.graph.keys()) + len(other_chain_graph.graph.keys())) * 2.0 * sensingRange) < beltWidth):
				self.dominant, self.dominant_neighbor = dominant_wo, dominant_neighbor_wo
			else:
				self.dominant, self.dominant_neighbor = dominant_w, dominant_neighbor_w
		else:
			other_sensors = list(set(range(numberOfSensors)) - set(self.graph.keys()))
			self.dominant, self.dominant_neighbor = self.calculate_dominant_for_given_set(other_sensors)
		print self.id,
		print " : ",
		print self.dominant,
		print self.dominant_neighbor

		self.init_flatten_properties()

	def reset_forces(self):
		for key in self.graph.keys():
			sensors[key].body.reset_forces()

if DEBUG:	
	print "Sensing Range = " + str(sensingRange) + " : " + str(type(sensingRange))
	print "Belt Width = " + str(beltWidth) + " : " + str(type(beltWidth))
	print "Belt Height = " + str(beltHeight) + " : " + str(type(beltHeight))

def create_chain_graphs():
	visited_sensors = set()
	# first create global graph consisting of all chain graphs then construct the chain graphs
	sensor_graph = dict()
	for sensor in sensors:
		sensor_graph[sensor.id] = dict()
		for visited_sensor in visited_sensors:
			# check if current sensor colliding with any visited sensor
			if sensor_distance(sensor, visited_sensor) <= 2*sensingRange:
				sensor_graph[sensor.id][visited_sensor.id] = True
				sensor_graph[visited_sensor.id][sensor.id] = True
		# current sensor is now visited
		visited_sensors.add(sensor)
	
	if DEBUG:
		print "Global Chain graph for all sensors : "
		print "Sensors : "
		for sensor in sensors:
			sensor.print_sensor()
		print_chain_graph(sensor_graph)
	# create chain graphs from whole sensor graph by multiple dfs traversals
	global_visited = set()
	stack = []
	k = 0
	for sensor in sensors:
		if sensor.id not in global_visited:
			# dfs sub routine
			visited, stack = set(), [sensor.id]
			while stack:
				vertex = stack.pop()
				if vertex not in visited:
					visited.add(vertex)
					stack.extend(set(sensor_graph.get(vertex).keys()) - visited)
			# update the global visited set
			global_visited = global_visited | visited
			# all the sensors in visited set belong to Current Chain Graph
			new_chain_graph = ChainGraph(k)
			k += 1
			if DEBUG:
				print "Visited Sensors : "
				print visited
			for visited_sensor in visited:
				new_chain_graph.graph[visited_sensor] = dict()
				for key in sensor_graph.get(visited_sensor).keys():
					new_chain_graph.add_edge(visited_sensor, key)
			visited = None
			chain_graphs[new_chain_graph.id] = new_chain_graph

	# get chaingraph DFS tree from chain graph
	print "Printing chain_graphs here one by one"
	for chain_graph in chain_graphs.values():
		chain_graph.get_dfs_tree()
		chain_graph.print_graph()
	print "Print chain graphs done"

def read_sensor_input():
	sensor_coordinates = []
	for i in range(numberOfSensors):
		x = float(input())
		y = float(input())
		sensor_coordinates.append((x,y))
	sensor_coordinates.sort(key = lambda tup: tup[0]) # in place sorting
	# create sensors from sorted coordinates
	for i in range(numberOfSensors):
		sensor = Sensor(ID = i, x =  sensor_coordinates[i][0], y =  sensor_coordinates[i][1])
		sensors.append(sensor)

	create_chain_graphs()

def draw_belt():
	# pygame considers inverse of y coordinates therefore base is actually the top in game screen
	# left
	p1 = (padding, padding)
	p2 = (padding, padding + int(screen_height))
	pygame.draw.lines(pygame_screen, THECOLORS["black"], False, [p1,p2])
	# base
	p1 = (padding, padding)
	p2 = (padding + int(screen_width), padding)
	pygame.draw.lines(pygame_screen, THECOLORS["gray"], False, [p1,p2])
	# right
	p1 = (padding + int(screen_width), padding)
	p2 = (padding + int(screen_width), padding + int(screen_height))
	pygame.draw.lines(pygame_screen, THECOLORS["black"], False, [p1,p2])
	# top
	p1 = (padding, padding + int(screen_height))
	p2 = (padding + int(screen_width), padding + int(screen_height))
	pygame.draw.lines(pygame_screen, THECOLORS["gray"], False, [p1,p2])

def init_screen():
	pygame_screen.fill(THECOLORS["white"])
	draw_belt()

def apply_drag():
	drag_factor = 2
	# for all sensors apply air drag in the direction opposite to their velocities
	for sensor in sensors:
		air_drag = ((-1*sensor.body.velocity[0]/drag_factor), (-1*sensor.body.velocity[1]/drag_factor))
		# if sensor.id == 10:
		# 	print air_drag
		sensor.body.apply_impulse(((-1*sensor.body.velocity[0]/drag_factor), (-1*sensor.body.velocity[1]/drag_factor)), (0,0))
		# if sensor.id == 10:
		# 	print sensor.body.velocity
		# 	print ((-1*sensor.body.velocity[0]/drag_factor), (-1*sensor.body.velocity[1]/drag_factor))
		# 	print air_drag

def update_sensor_positions():
	# update positions after applying forces
	for sensor in sensors:
		sensor.update_position(sensor.body.position)

def get_left_sensor():
	global left_sensor
	global left_chain_graph
	
	# get the leftmost sensor and corresponding chain graph
	Min = INFINITY
	for sensor in sensors:
		# get min
		if sensor.x < Min:
			Min = sensor.x
			left_sensor = sensor
	# get chain graph which contains the leftmost sensor
	for chain_graph in chain_graphs.values():
		if chain_graph.graph.get(left_sensor.id) != None:
			left_chain_graph = chain_graph
			break
	return left_sensor, left_chain_graph

def get_right_sensor():
	global right_sensor
	global right_chain_graph
	
	# get the leftmost sensor and corresponding chain graph
	Max = 0.0
	for sensor in sensors:
		# get max
		if sensor.x > Max:
			Max = sensor.x
			right_sensor = sensor
	# get chain graph which contains the leftmost sensor
	for chain_graph in chain_graphs.values():
		if chain_graph.graph.get(right_sensor.id) != None:
			right_chain_graph = chain_graph
			break
	return right_sensor, right_chain_graph
	
left_boundary = None
right_boundary = None
left_groove = None
right_groove = None
def init_simulation():
	global left_boundary
	global right_boundary
	# activate all the bodies
	for sensor in sensors:
		sensor.body.activate()
	# Create stationary bodies for left and right boundaries of belt region
	left_boundary = pymunk.Body()			# static left boundary
	left_boundary.position = (0, beltHeight/2)
	right_boundary = pymunk.Body()
	right_boundary.position = (beltWidth, beltHeight/2)
	left_boundary.activate()
	right_boundary.activate()

def apply_forces_phasel(left_sensor):
	# apply impulse on left_sensor towards left
	left_sensor.body.apply_force((-0.5,0.0), (0,0))
	apply_drag()
	space.step(1/100.0)
	update_sensor_positions()

def phasel():
	global left_groove
	left_sensor, left_chain_graph = get_left_sensor()
	# left_sensor, left_chain_graph = get_right_sensor()
	# Join some chain graph with the left boundary
	running = True
	while running:
		for event in pygame.event.get():
			if event.type == QUIT:
				running = False
			elif event.type == KEYDOWN and event.key == K_ESCAPE:
				running = False

		init_screen()
		apply_forces_phasel(left_sensor)
		left_chain_graph.flatten(left_sensor.id)
		space.step(1/100.0)
		for chain_graph in chain_graphs.values():
			chain_graph.draw_chain_graph()
		pygame.display.flip()
		pygame_clock.tick(50)

		# Check if rightmost sensor touching rightmost boundary
		if (left_sensor.x - sensingRange) <= 0.0:
			left_sensor.body.reset_forces()
			break
	# Create Groove joint in between right sensor and right boundary
	print left_boundary
	left_groove = pymunk.GrooveJoint(left_boundary, left_sensor.body, (0.0, -(beltHeight/2)), (0.0, beltHeight/2), (-sensingRange, 0))
	space.add(left_groove)
	# re-initialize left chain graph for flattening
	left_chain_graph.init_flatten_properties()

def apply_forces_phaser(right_sensor):
	# apply impulse on left_sensor towards left
	right_sensor.body.apply_force((0.5,0), (0,0))
	apply_drag()
	space.step(1/100.0)
	update_sensor_positions()

def phaser():
	global right_groove
	right_sensor, right_chain_graph = get_right_sensor()
	# Join some chain graph with the right boundary
	running = True
	while running:
		for event in pygame.event.get():
			if event.type == QUIT:
				running = False
			elif event.type == KEYDOWN and event.key == K_ESCAPE:
				running = False

		init_screen()
		apply_forces_phaser(right_sensor)
		right_chain_graph.flatten(right_sensor.id)
		for chain_graph in chain_graphs.values():
			chain_graph.draw_chain_graph()
		pygame.display.flip()
		pygame_clock.tick(50)
		# time.sleep(0.5)

		# Check if rightmost sensor touching rightmost boundary
		if (right_sensor.x + sensingRange) >= beltWidth:
			right_sensor.body.reset_forces()
			break
	# Create Groove joint in between right sensor and right boundary
	right_groove = pymunk.GrooveJoint(right_boundary, right_sensor.body, (0, -(beltHeight/2)), (0, beltHeight/2), (sensingRange,0))
	space.add(right_groove)
	# re-initialize right chain graph for flattening
	right_chain_graph.init_flatten_properties()

def barrier_covered():
	print "Yahan bhi aaya hai"
	for chain_graph in chain_graphs.values():
		if chain_graph.barrier_covered():
			return True
	return False

def merge_chain_graphs(chain_graph1_id, chain_graph2_id):
	global left_chain_graph
	global right_chain_graph
	# chain graph 1 or 2 maybe left or right chain graphs. Hence, we need to update after merge

	chain_graph1 = chain_graphs[chain_graph1_id]
	chain_graph2 = chain_graphs[chain_graph2_id]
	if chain_graph2 is left_chain_graph:
		left_chain_graph = chain_graph1
	elif chain_graph2 is right_chain_graph:
		right_chain_graph = chain_graph1
	# copy the chain graph2 in chain graph1
	for sensor, neighbor_sensors in chain_graph2.graph.iteritems():
		chain_graph1.graph[sensor] = neighbor_sensors
	# add one edge from dominant to dominant neighbor
	chain_graph1.add_chain_edge(chain_graph1.dominant, chain_graph1.dominant_neighbor)
	chain_graph1.calculate_distances_from_leaves()

	# remove chain_graph2 from global dictionary
	chain_graph2 = None
	del chain_graphs[chain_graph2_id]

def dominant_points_meet():
	# check if any two dominant points meet
	for Id, chain_graph in chain_graphs.iteritems():
		if chain_graph.dominant == None:
			return False
		if sensor_distance(sensors[chain_graph.dominant], sensors[chain_graph.dominant_neighbor]) <= 2*sensingRange:
			# find the chain_graph in which dominant neighbor belongs
			chain_graph_neighbor = None
			for Id1, chain_graph1 in chain_graphs.iteritems():
				if chain_graph1 != chain_graph and chain_graph.dominant_neighbor in chain_graph1.graph.keys():
					chain_graph_neighbor = Id1
					# merge current chain graphs and its neighbor then break
					print str(Id) + " : " + str(Id1)
					merge_chain_graphs(Id, Id1)
					return True
	return False

# denotes if some force greater than force Threshold
force_flag = False
def apply_forces():
	global force_flag
	# apply forces on all dominant points
	if len(chain_graphs) == 1:
		return
	for chain_graph in chain_graphs.values():
		if chain_graph.dominant == None:
			print "chain graph = " + str(chain_graph.id)
		Force = force(sensors[chain_graph.dominant], sensors[chain_graph.dominant_neighbor])
		if Force > FORCE_THRESHOLD:
			force_flag = True
			# print Force
			Force = FORCE_THRESHOLD
		distance = sensor_distance(sensors[chain_graph.dominant], sensors[chain_graph.dominant_neighbor])
		Force_x = Force * (sensors[chain_graph.dominant_neighbor].x - sensors[chain_graph.dominant].x) / distance
		Force_y = Force * (sensors[chain_graph.dominant_neighbor].y - sensors[chain_graph.dominant].y) / distance
		sensors[chain_graph.dominant].body.apply_force((Force_x,Force_y),(0,0))

counter = 0
def stop_sensors():
	global counter
	global force_flag
	if force_flag:
		counter += 1
		if counter%80 == 0:
			force_flag = False
			for chain_graph in chain_graphs.values():
				chain_graph.reset_forces()

def print_results():
	# verify edges
	print "List of sensor links which are violating the conditions :"
	for chain_graph in chain_graphs.values():
		chain_graph.verify_edges()

	distance = 0.0
	displacement = 0.0
	print "Printing Results:"
	for sensor in sensors:
		sensor.print_results()
		distance += sensor.distance
		displacement += euclidean_distance(sensor.init_x, sensor.init_y, sensor.x, sensor.y)

	avg_distance = distance / numberOfSensors
	avg_displacement = displacement / numberOfSensors
	print "\n\nAvg Distance : " + str(avg_distance)
	print "Avg Displacement : " + str(avg_displacement)


def simulate():
	read_sensor_input()
	init_simulation()

	phasel()
	print "l done"
	phaser()
	print "Yahan toh aaya hai!!"
	while not barrier_covered():
		# get the domianant points for each chain_graph
		for chain_graph in chain_graphs.values():
			chain_graph.calculate_dominant_point()
			chain_graph.reset_forces()
		# time.sleep(10)
		# keep simulating unless any two dominant points meet
		running = True
		while running:
			for event in pygame.event.get():
				if event.type == QUIT:
					running = False
				elif event.type == KEYDOWN and event.key == K_ESCAPE:
					running = False
			
			init_screen()
			apply_forces()
			for chain_graph in chain_graphs.values():
				# flatten and draw chain_graphs
				chain_graph.flatten(chain_graph.dominant)
				chain_graph.draw_chain_graph()
			apply_drag()
			update_sensor_positions()
			space.step(1/100.0)
			stop_sensors()
			pygame.display.flip()
			pygame_clock.tick(50)
			# time.sleep(0.5)
			flag = False
			while dominant_points_meet():
				flag = True
			if flag:
				break

	print_results()

def main():
	# we make a console program which continuously takes input from the user
	# Program will start with a setup screen where the user will place sensors by mouse clicks
	# on pressing 1 will start the simulation. Simulation automatically stops after 1 min
	# after simulation is done the user log inputs and outputs or reject them
	simulate()
	# time.sleep(100)

if __name__ == '__main__':
	main()
