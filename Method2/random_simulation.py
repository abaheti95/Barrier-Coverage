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
	
	multiplier = 5

	beltWidth = initial_beltWidth * multiplier
	numberOfSensors = initial_numberOfSensors * multiplier

	generate_random_input()
	simulator.init_param(sensingRange,beltWidth,beltHeight,numberOfSensors,x,y)
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