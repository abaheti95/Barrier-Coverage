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
numberOfSensors = 16
x = []
y = []

def generate_input():
	global x
	global y
	# divide number of sensors and belt region in 3 parts
	# left sensors
	x = []
	y = []
	current_time = time.time()
	fraction_time = current_time - int(current_time)
	nano_seconds = int(fraction_time * 1000000000)
	random.seed(nano_seconds)
	for i in range(0,int(numberOfSensors/3)):
		x.append(random.uniform(beltWidth/numberOfSensors * i,beltWidth/numberOfSensors * (i+1))) 
		y.append(random.uniform(0.2,2.8))

	for i in range(int(numberOfSensors/3),int(2*numberOfSensors/3)):
		x.append(random.uniform(beltWidth/numberOfSensors * i,beltWidth/numberOfSensors * (i+1))) 
		y.append(random.uniform(beltHeight-2.8,beltHeight-0.2))

	for i in range(int(2*numberOfSensors/3),int(numberOfSensors)):
		x.append(random.uniform(beltWidth/numberOfSensors * i,beltWidth/numberOfSensors * (i+1))) 
		y.append(random.uniform(0.2,2.8))

	print numberOfSensors
	print x
	print len(x)

def generate_random_input():
	global x
	global y
	# divide number of sensors and belt region in 3 parts
	# left sensors
	x = []
	y = []
	current_time = time.time()
	fraction_time = current_time - int(current_time)
	nano_seconds = int(fraction_time * 1000000000)
	random.seed(nano_seconds)
	for i in range(numberOfSensors):
		x.append(random.uniform(0.0,beltWidth)) 
		y.append(random.uniform(0.0,beltHeight))

def main():

	global beltWidth
	global numberOfSensors
	# run 10 phases each time increasing the multiplier by 1
	initial_beltWidth = beltWidth
	initial_numberOfSensors = numberOfSensors

	number_of_sensors_moved = []
	max_distance = []
	min_distance = []
	max_displacement = []
	min_displacement = []
	avg_distance = []
	avg_displacement = []
	
	multiplier = 5
	number_of_simulations = 10

	beltWidth = initial_beltWidth * multiplier
	numberOfSensors = initial_numberOfSensors * multiplier
	for i in range(number_of_simulations):
		generate_random_input()
		simulator.init_param(sensingRange,beltWidth,beltHeight,numberOfSensors,x,y)
		simulator.simulate()
		nm, max_dist,min_dist,max_disp,min_disp,avg_dist,avg_disp = simulator.print_results()
		number_of_sensors_moved.append(nm)
		max_distance.append(max_dist)
		max_displacement.append(max_disp)
		min_distance.append(min_dist)
		min_displacement.append(min_disp)
		avg_distance.append(avg_dist)
		avg_displacement.append(avg_disp)


	print "number of sensors = " + str(numberOfSensors)
	print "sensing range = " + str(sensingRange)
	print "belt width = " + str(beltWidth)
	print "belt height = " + str(beltHeight)
	print number_of_sensors_moved
	print max_distance
	print min_distance
	print max_displacement
	print min_displacement
	print avg_distance
	print avg_displacement
	
	nm = np.array(number_of_sensors_moved)
	max_dist = np.array(max_distance)
	min_dist = np.array(min_distance)
	max_disp = np.array(max_displacement)
	min_disp = np.array(min_displacement)
	avg_dist = np.array(avg_distance)
	avg_disp = np.array(avg_displacement)

	print np.mean(nm)
	print np.mean(max_dist)
	print np.mean(min_dist)
	print np.mean(max_disp)
	print np.mean(min_disp)
	print np.mean(avg_dist)
	print np.mean(avg_disp)	

	print np.std(nm)
	print np.std(max_dist)
	print np.std(min_dist)
	print np.std(max_disp)
	print np.std(min_disp)
	print np.std(avg_dist)
	print np.std(avg_disp)

if __name__ == '__main__':
	main()