import simulator
import random
import time
import numpy as np

# The mass simulations will work in 2 steps
# 1 - Generate a test on the basis of configuration set before
# 2 - Simulate and store results for post processing

sensingRange = 0.5
beltWidth = 10.0
beltHeight = 8.0
numberOfSensors = 13
x = []
y = []

def read_input():
	global beltWidth
	global beltHeight
	global sensingRange
	global numberOfSensors
	global x
	global y
	f = open("Maintenance/configuration.txt", 'r')
	# belt_length belt_width, sensing_range, n_sensors
	# n lines of locations of sensors as x_coordinate y_coordinate
	# Length of the barrier
	# Array of indices telling the sensors which are present on the barrier
	# Read the Sensor graph
	beltWidth = float(f.readline())
	beltHeight = float(f.readline())
	sensingRange = float(f.readline())
	numberOfSensors = int(f.readline())
	for i in range(numberOfSensors):
		x.append(float(f.readline()))
		y.append(float(f.readline()))

def main():

	global beltWidth
	global numberOfSensors
	global x
	global y

	read_input()
	print numberOfSensors
	fail_sensors_str = raw_input()
	fail_sensors_str.strip()
	fail_sensors = []
	for c in fail_sensors_str.split(" "):
		if c == '':
			continue
		fail_sensors.append(int(c))
	print fail_sensors

	for index in sorted(fail_sensors, reverse=True):
		print index
		del x[index]
		del y[index]


	simulator.init_param(sensingRange,beltWidth,beltHeight,(numberOfSensors - len(fail_sensors)),x,y)
	nm, max_dist,min_dist,max_disp,min_disp,avg_dist,avg_disp = simulator.simulate()

	print "number of sensors = " + str(numberOfSensors)
	print "sensing range = " + str(sensingRange)
	print "belt width = " + str(beltWidth)
	print "belt height = " + str(beltHeight)
	print nm
	print max_dist
	print min_dist
	print max_disp
	print min_disp
	print avg_dist
	print avg_disp
	

if __name__ == '__main__':
	main()