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

	maintenance_number_of_sensors_moved = []
	maintenance_max_distance = []
	maintenance_min_distance = []
	maintenance_max_displacement = []
	maintenance_min_displacement = []
	maintenance_avg_distance = []
	maintenance_avg_displacement = []
	
	multiplier = 5
	number_of_simulations = 5

	beltWidth = initial_beltWidth * multiplier
	numberOfSensors = initial_numberOfSensors * multiplier
	# no_of_sensors_to_delete = (initial_numberOfSensors - 10) * 3
	no_of_sensors_to_delete = 7
	for i in range(number_of_simulations):
		generate_random_input()
		simulator.init_param(sensingRange,beltWidth,beltHeight,numberOfSensors,x,y)
		nm, max_dist,min_dist,max_disp,min_disp,avg_dist,avg_disp = simulator.simulate()
		number_of_sensors_moved.append(nm)
		max_distance.append(max_dist)
		max_displacement.append(max_disp)
		min_distance.append(min_dist)
		min_displacement.append(min_disp)
		avg_distance.append(avg_dist)
		avg_displacement.append(avg_disp)

		# fetch final locations after first simulation
		sensing_range,belt_width,belt_height,number_of_sensors,X,Y,barrier = simulator.fetch_final_locations()
		# Take a small random sample of sensors on the barrier
		barrier.sort()
		sample = random.sample(range(len(barrier)), no_of_sensors_to_delete)
		# Delete some of the sensors on the barrier
		for i in sorted(sample, reverse=True):
			idx = barrier[i]
			del X[idx]
			del Y[idx]

		simulator.init_param(sensingRange,beltWidth,beltHeight,numberOfSensors - no_of_sensors_to_delete,X,Y)
		nm, max_dist,min_dist,max_disp,min_disp,avg_dist,avg_disp = simulator.simulate()
		maintenance_number_of_sensors_moved.append(nm)
		maintenance_max_distance.append(max_dist)
		maintenance_max_displacement.append(max_disp)
		maintenance_min_distance.append(min_dist)
		maintenance_min_displacement.append(min_disp)
		maintenance_avg_distance.append(avg_dist)
		maintenance_avg_displacement.append(avg_disp)


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
	print maintenance_number_of_sensors_moved
	print maintenance_max_distance
	print maintenance_min_distance
	print maintenance_max_displacement
	print maintenance_min_displacement
	print maintenance_avg_distance
	print maintenance_avg_displacement
	
	nm = np.array(number_of_sensors_moved)
	max_dist = np.array(max_distance)
	min_dist = np.array(min_distance)
	max_disp = np.array(max_displacement)
	min_disp = np.array(min_displacement)
	avg_dist = np.array(avg_distance)
	avg_disp = np.array(avg_displacement)
	maintenance_nm = np.array(maintenance_number_of_sensors_moved)
	maintenance_max_dist = np.array(maintenance_max_distance)
	maintenance_min_dist = np.array(maintenance_min_distance)
	maintenance_max_disp = np.array(maintenance_max_displacement)
	maintenance_min_disp = np.array(maintenance_min_displacement)
	maintenance_avg_dist = np.array(maintenance_avg_distance)
	maintenance_avg_disp = np.array(maintenance_avg_displacement)

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

	print np.mean(maintenance_nm)
	print np.mean(maintenance_max_dist)
	print np.mean(maintenance_min_dist)
	print np.mean(maintenance_max_disp)
	print np.mean(maintenance_min_disp)
	print np.mean(maintenance_avg_dist)
	print np.mean(maintenance_avg_disp)	

	print np.std(maintenance_nm)
	print np.std(maintenance_max_dist)
	print np.std(maintenance_min_dist)
	print np.std(maintenance_max_disp)
	print np.std(maintenance_min_disp)
	print np.std(maintenance_avg_dist)
	print np.std(maintenance_avg_disp)

if __name__ == '__main__':
	main()