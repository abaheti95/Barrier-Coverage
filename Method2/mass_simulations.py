import simulator
import random
import time
import numpy as np
import matplotlib.pyplot as plt

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
	numberOfSensors = 45

	
	multiplier = 5
	number_of_simulations = 100

	beltWidth = initial_beltWidth * multiplier

	#Plotting data lists
	plot_n_sensors = []
	plot_nm = []
	plot_nm_error = []
	plot_max_dist = []
	plot_max_dist_error = []
	plot_max_disp = []
	plot_max_disp_error = []
	plot_min_dist = []
	plot_min_dist_error = []
	plot_min_disp = []
	plot_min_disp_error = []
	plot_avg_dist = []
	plot_avg_dist_error = []
	plot_avg_disp = []
	plot_avg_disp_error = []

	plot_shen_max_disp = [6.5847253545339273, 6.6117608216199404, 6.4192075153444463, 6.6164191064697562, 6.6639842554210205, 6.4927645960537177]
	plot_shen_min_disp = [0.55206944490859367, 0.5881646691259973, 0.62145414991628112, 0.54591553627513401, 0.61210839858004917, 0.60764768453846119]
	plot_shen_avg_disp = [3.3400166194771548, 3.3517884086521512, 3.3261676687555295, 3.4597997399282456, 3.5281488325915098, 3.3426063805085882]
	plot_shen_max_disp_err = [1.7656543286905806, 1.7227929946753668, 1.4365956609650914, 1.7175653402593485, 1.6028350971500152, 1.7209914677193205]
	plot_shen_min_disp_err = [0.35900896439895458, 0.3668827484592142, 0.37343265193577502, 0.38037340676020426, 0.46806762994047352, 0.38997712922131117]
	plot_shen_avg_disp_err = [0.83888188994870172, 0.82912032104583155, 0.80500147935182631, 0.94462236214880113, 0.91215022673699286, 0.90153058022993404]


	for multiplier in range(5,11):
		number_of_sensors_moved = []
		max_distance = []
		min_distance = []
		max_displacement = []
		min_displacement = []
		avg_distance = []
		avg_displacement = []
		
		numberOfSensors += 5
		print numberOfSensors
		
		plot_n_sensors.append(numberOfSensors)

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


		# print "number of sensors = " + str(numberOfSensors)
		# print "sensing range = " + str(sensingRange)
		# print "belt width = " + str(beltWidth)
		# print "belt height = " + str(beltHeight)
		# print number_of_sensors_moved
		# print max_distance
		# print min_distance
		# print max_displacement
		# print min_displacement
		# print avg_distance
		# print avg_displacement
		
		nm = np.array(number_of_sensors_moved)
		max_dist = np.array(max_distance)
		min_dist = np.array(min_distance)
		max_disp = np.array(max_displacement)
		min_disp = np.array(min_displacement)
		avg_dist = np.array(avg_distance)
		avg_disp = np.array(avg_displacement)
		print "Means"
		print np.mean(nm)
		plot_nm.append(np.mean(nm))
		print np.mean(max_dist)
		plot_max_dist.append(np.mean(max_dist))
		print np.mean(min_dist)
		plot_min_dist.append(np.mean(min_dist))
		print np.mean(max_disp)
		plot_max_disp.append(np.mean(max_disp))
		print np.mean(min_disp)
		plot_min_disp.append(np.mean(min_disp))
		print np.mean(avg_dist)
		plot_avg_dist.append(np.mean(avg_dist))
		print np.mean(avg_disp)
		plot_avg_disp.append(np.mean(avg_disp))	
		print "SDs"
		print np.std(nm)
		plot_nm_error.append(np.std(nm))
		print np.std(max_dist)
		plot_max_dist_error.append(np.std(max_dist))
		print np.std(min_dist)
		plot_min_dist_error.append(np.std(min_dist))
		print np.std(max_disp)
		plot_max_disp_error.append(np.std(max_disp))
		print np.std(min_disp)
		plot_min_disp_error.append(np.std(min_disp))
		print np.std(avg_dist)
		plot_avg_dist_error.append(np.std(avg_dist))
		print np.std(avg_disp)
		plot_avg_disp_error.append(np.std(avg_disp))

	# Save the relevant data
	np_plot_shen_max_disp = np.array(plot_shen_max_disp)
	np.save("cent_data/np_plot_shen_max_disp", np_plot_shen_max_disp)
	np_plot_shen_max_disp_err = np.array(plot_shen_max_disp_err)
	np.save("cent_data/np_plot_shen_max_disp_err", np_plot_shen_max_disp_err)
	np_plot_max_dist = np.array(plot_max_dist)
	np.save("cent_data/np_plot_max_dist", np_plot_max_dist)
	np_plot_max_dist_error = np.array(plot_max_dist_error)
	np.save("cent_data/np_plot_max_dist_error", np_plot_max_dist_error)
	np_plot_max_disp = np.array(plot_max_disp)
	np.save("cent_data/np_plot_max_disp", np_plot_max_disp)
	np_plot_max_disp_error = np.array(plot_max_disp_error)
	np.save("cent_data/plot_max_disp_error", plot_max_disp_error)

	np_plot_shen_avg_disp = np.array(plot_shen_avg_disp)
	np.save("cent_data/np_plot_shen_avg_disp", np_plot_shen_avg_disp)
	np_plot_shen_avg_disp_err = np.array(plot_shen_avg_disp_err)
	np.save("cent_data/np_plot_shen_avg_disp_err", np_plot_shen_avg_disp_err)
	np_plot_avg_dist = np.array(plot_avg_dist)
	np.save("cent_data/np_plot_avg_dist", np_plot_avg_dist)
	np_plot_avg_dist_error = np.array(plot_avg_dist_error)
	np.save("cent_data/np_plot_avg_dist_error", np_plot_avg_dist_error)
	np_plot_avg_disp = np.array(plot_avg_disp)
	np.save("cent_data/np_plot_avg_disp", np_plot_avg_disp)
	np_plot_avg_disp_error = np.array(plot_avg_disp_error)
	np.save("cent_data/plot_avg_disp_error", plot_avg_disp_error)


	# Draw Plots
	# Number of sensors moved
	plt.xlabel("Number of Sensors")
	plt.ylabel("Number of Sensors moved")
	plt.errorbar(plot_n_sensors, plot_nm, yerr=plot_nm_error, color="red")
	plt.savefig("Number of sensors moved.png")

	plt.clf()
	# Max Distance and Displacement
	plt.xlabel("Number of Sensors")
	plt.ylabel("Distance (m)")
	plt.errorbar(plot_n_sensors, plot_shen_max_disp, yerr=plot_shen_max_disp_err, label="shen max displacement", color="red")
	plt.errorbar(plot_n_sensors, plot_max_dist, yerr=plot_max_dist_error, label="centralized max distance", color="green")
	plt.errorbar(plot_n_sensors, plot_max_disp, yerr=plot_max_disp_error, label="centralized max displacement", color="blue")
	plt.legend(loc=0)
	plt.savefig("Max distance.png")

	plt.clf()
	# Min Distance and Displacement
	plt.xlabel("Number of Sensors")
	plt.ylabel("Distance (m)")
	plt.errorbar(plot_n_sensors, plot_shen_min_disp, yerr=plot_shen_min_disp_err, label="shen min displacement", color="red")
	plt.errorbar(plot_n_sensors, plot_min_dist, yerr=plot_min_dist_error, label="centralized min distance", color="green")
	plt.errorbar(plot_n_sensors, plot_min_disp, yerr=plot_min_disp_error, label="centralized min displacement", color="blue")
	plt.legend(loc=0)
	plt.savefig("Min distance.png")

	plt.clf()
	# Avg Distance and Displacement
	plt.xlabel("Number of Sensors")
	plt.ylabel("Distance (m)")
	plt.errorbar(plot_n_sensors, plot_shen_avg_disp, yerr=plot_shen_avg_disp_err, label="shen avg displacement", color="red")
	plt.errorbar(plot_n_sensors, plot_avg_dist, yerr=plot_avg_dist_error, label="centralized avg distance", color="green")
	plt.errorbar(plot_n_sensors, plot_avg_disp, yerr=plot_avg_disp_error, label="centralized avg displacement", color="blue")
	plt.legend(loc=0)
	plt.savefig("Avg distance.png")

if __name__ == '__main__':
	main()