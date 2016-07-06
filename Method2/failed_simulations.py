import simulator
import random
import time
import numpy as np
from random import randint

# For each configuration make 10 simluations for particular fail_size

sensingRange = 0.5
belt_length = 10.0
belt_width = 8.0
numberOfSensors = 13
x = []
y = []
barrier = []

def read_from_file(filename):
	# Format of the input data is as follows
	# belt_length belt_width, sensing_range, n_sensors
	# n lines of locations of sensors as x_coordinate y_coordinate
	# Length of the barrier
	# Array of indices telling the sensors which are present on the barrier
	# Read the Sensor graph

	global belt_length
	global belt_width
	global sensingRange
	global numberOfSensors
	global x
	global y
	global barrier
	x = []
	y = []
	barrier = []
	with open(filename, 'r') as f:
		belt_length = float(f.readline())
		belt_width = float(f.readline())
		sensingRange = float(f.readline())
		numberOfSensors = int(f.readline())
		n = numberOfSensors
		for i in range(n):
			x.append(float(f.readline()))
			y.append(float(f.readline()))
		n_barrier = int(f.readline())
		for i in range(n_barrier):
			barrier.append(int(f.readline()))

def fail_sensors(n_fail):
	x_clone = x[:]
	y_clone = y[:]
	start_index = randint(0,(len(barrier) - n_fail - 1))
	# delete n_fail sensors from start_index
	sensors_to_be_failed = []
	for i in range(start_index, start_index + n_fail):
		sensors_to_be_failed.append(barrier[i])
	# Fail sensors now
	for i in sorted(sensors_to_be_failed, reverse=True):
		del x_clone[i]
		del y_clone[i]
	return sensingRange,belt_length,belt_width,(numberOfSensors - n_fail), x_clone, y_clone

def print_list(l):
	for element in l:
		print element

def main():
	plot_avg_dist = []
	plot_avg_disp = []
	plot_max_dist = []
	plot_max_disp = []
	plot_avg_dist_err = []
	plot_avg_disp_err = []
	plot_max_dist_err = []
	plot_max_disp_err = []

	for n_fail in range(1,6):
		# We will run 100 simulations for every number of failed sensors
		avg_distance = []
		avg_displacement = []
		max_distance = []
		max_displacement = []
		for file_no in range(1,11):
			filename = "Maintenance/Configurations/configuration%d.txt" % file_no
			read_from_file(filename)
			for i in range(10):
				simulator.init_param(*fail_sensors(n_fail))
				nm, max_dist,min_dist,max_disp,min_disp,avg_dist,avg_disp = simulator.simulate()
				avg_distance.append(avg_dist)
				avg_displacement.append(avg_disp)
				max_distance.append(max_dist)
				max_displacement.append(max_disp)

		np_avg_distance = np.array(avg_distance)
		np_avg_displacement = np.array(avg_displacement)
		np_max_distance = np.array(max_distance)
		np_max_displacement = np.array(max_displacement)

		plot_avg_dist.append(np.mean(np_avg_distance))
		plot_avg_disp.append(np.mean(np_avg_displacement))
		plot_max_dist.append(np.mean(np_max_distance))
		plot_max_disp.append(np.mean(np_max_displacement))
		plot_avg_dist_err.append(np.std(np_avg_distance))
		plot_avg_disp_err.append(np.std(np_avg_displacement))
		plot_max_dist_err.append(np.std(np_max_distance))
		plot_max_disp_err.append(np.std(np_max_displacement))

	#Print plot data

	print_list(plot_avg_dist)
	print_list(plot_avg_disp)
	print_list(plot_max_dist)
	print_list(plot_max_disp)
	print_list(plot_avg_dist_err)
	print_list(plot_avg_disp_err)
	print_list(plot_max_dist_err)
	print_list(plot_max_disp_err)

if __name__ == '__main__':
	main()
