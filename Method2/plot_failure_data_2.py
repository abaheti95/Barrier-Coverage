# Updated the plotting code to plot the new test cases i.e. t is fixed and k varies from 1-5
import numpy as np
import matplotlib.pyplot as plt
import matplotlib
matplotlib.rcParams['lines.linewidth'] = 5
matplotlib.rcParams.update({'font.size': 30})
marker_style = dict(markersize=15)
marker_style2 = dict(markersize=12)
marker_cent = 'd'
marker = 'o'

color_cent = np.array([[0.0],[0.0],[0.0]]) # black
color_k1 = np.array([[1.0],[0.0],[0.0]]) # red
color_k2 = np.array([[0.0],[1.0],[0.0]]) # green
color_k3 = np.array([[0.0],[0.0],[1.0]]) # blue
color_k4 = np.array([[1.0],[0.0],[1.0]]) # voilet
color_k5 = np.array([[1.0],[1.0],[0.0]]) # yellow
color_k5 = np.array([[0.0],[1.0],[1.0]]) # cyan



# PLOT_DATA_FILE = "Maintenance/plot_data_saved_2.txt"
# PLOT_DATA_FILE = "Maintenance/plot_data_saved_2_low_gamma.txt"
PLOT_DATA_FILE = "Maintenance/plot_data_saved_2_high_gamma.txt"

# Centralized lists
plot_c_avg_dist = []
plot_c_avg_disp = []
plot_c_max_dist = []
plot_c_max_disp = []
plot_c_avg_dist_err = []
plot_c_avg_disp_err = []
plot_c_max_dist_err = []
plot_c_max_disp_err = []

# Distributed Lists
avg_distance1K = []
avg_distance2K = []
avg_distance3K = []
avg_distance4K = []
avg_distance5K = []
max_distance1K = []
max_distance2K = []
max_distance3K = []
max_distance4K = []
max_distance5K = []
timestamp1K = []
timestamp2K = []
timestamp3K = []
timestamp4K = []
timestamp5K = []
n_moved1K = []
n_moved2K = []
n_moved3K = []
n_moved4K = []
n_moved5K = []
accuracy1K = []
accuracy2K = []
accuracy3K = []
accuracy4K = []
accuracy5K = []
avg_distance1K_err = []
avg_distance2K_err = []
avg_distance3K_err = []
avg_distance4K_err = []
avg_distance5K_err = []
max_distance1K_err = []
max_distance2K_err = []
max_distance3K_err = []
max_distance4K_err = []
max_distance5K_err = []
timestamp1K_err = []
timestamp2K_err = []
timestamp3K_err = []
timestamp4K_err = []
timestamp5K_err = []
n_moved1K_err = []
n_moved2K_err = []
n_moved3K_err = []
n_moved4K_err = []
n_moved5K_err = []

def read_5_floats_from_file(f):
	l = []
	for i in range(5):
		l.append(float(f.readline()))
	return l

def read_centralized_data():
	global plot_c_avg_dist, plot_c_avg_disp, plot_c_max_dist, plot_c_max_disp, plot_c_avg_dist_err, plot_c_avg_disp_err, plot_c_max_dist_err, plot_c_max_disp_err
	with open("centralized_failure_plot_data.txt", 'r') as f:
		plot_c_avg_dist = read_5_floats_from_file(f)
		plot_c_avg_disp = read_5_floats_from_file(f)
		plot_c_max_dist = read_5_floats_from_file(f)
		plot_c_max_disp = read_5_floats_from_file(f)
		plot_c_avg_dist_err = read_5_floats_from_file(f)
		plot_c_avg_disp_err = read_5_floats_from_file(f)
		plot_c_max_dist_err = read_5_floats_from_file(f)
		plot_c_max_disp_err = read_5_floats_from_file(f)

def read_distributed_data():
	global avg_distance1K, avg_distance2K, avg_distance3K, avg_distance4K, avg_distance5K, max_distance1K, max_distance2K, max_distance3K, max_distance4K, max_distance5K, timestamp1K, timestamp2K, timestamp3K, timestamp4K, timestamp5K, n_moved1K, n_moved2K, n_moved3K, n_moved4K, n_moved5K, accuracy1K, accuracy2K, accuracy3K, accuracy4K, accuracy5K, avg_distance1K_err, avg_distance2K_err, avg_distance3K_err, avg_distance4K_err, avg_distance5K_err, max_distance1K_err, max_distance2K_err, max_distance3K_err, max_distance4K_err, max_distance5K_err, timestamp1K_err, timestamp2K_err, timestamp3K_err, timestamp4K_err, timestamp5K_err, n_moved1K_err, n_moved2K_err, n_moved3K_err, n_moved4K_err, n_moved5K_err
	with open(PLOT_DATA_FILE, 'r') as f:
		# K = 1
		avg_distance1K = read_5_floats_from_file(f)
		avg_distance1K_err = read_5_floats_from_file(f)
		max_distance1K = read_5_floats_from_file(f)
		max_distance1K_err = read_5_floats_from_file(f)
		timestamp1K = read_5_floats_from_file(f)
		timestamp1K_err = read_5_floats_from_file(f)
		n_moved1K = read_5_floats_from_file(f)
		n_moved1K_err = read_5_floats_from_file(f)
		accuracy1K = read_5_floats_from_file(f)
		# K = 2
		avg_distance2K = read_5_floats_from_file(f)
		avg_distance2K_err = read_5_floats_from_file(f)
		max_distance2K = read_5_floats_from_file(f)
		max_distance2K_err = read_5_floats_from_file(f)
		timestamp2K = read_5_floats_from_file(f)
		timestamp2K_err = read_5_floats_from_file(f)
		n_moved2K = read_5_floats_from_file(f)
		n_moved2K_err = read_5_floats_from_file(f)
		accuracy2K = read_5_floats_from_file(f)
		# K = 3
		avg_distance3K = read_5_floats_from_file(f)
		avg_distance3K_err = read_5_floats_from_file(f)
		max_distance3K = read_5_floats_from_file(f)
		max_distance3K_err = read_5_floats_from_file(f)
		timestamp3K = read_5_floats_from_file(f)
		timestamp3K_err = read_5_floats_from_file(f)
		n_moved3K = read_5_floats_from_file(f)
		n_moved3K_err = read_5_floats_from_file(f)
		accuracy3K = read_5_floats_from_file(f)
		# K = 4
		avg_distance4K = read_5_floats_from_file(f)
		avg_distance4K_err = read_5_floats_from_file(f)
		max_distance4K = read_5_floats_from_file(f)
		max_distance4K_err = read_5_floats_from_file(f)
		timestamp4K = read_5_floats_from_file(f)
		timestamp4K_err = read_5_floats_from_file(f)
		n_moved4K = read_5_floats_from_file(f)
		n_moved4K_err = read_5_floats_from_file(f)
		accuracy4K = read_5_floats_from_file(f)
		# K = 5
		avg_distance5K = read_5_floats_from_file(f)
		avg_distance5K_err = read_5_floats_from_file(f)
		max_distance5K = read_5_floats_from_file(f)
		max_distance5K_err = read_5_floats_from_file(f)
		timestamp5K = read_5_floats_from_file(f)
		timestamp5K_err = read_5_floats_from_file(f)
		n_moved5K = read_5_floats_from_file(f)
		n_moved5K_err = read_5_floats_from_file(f)
		accuracy5K = read_5_floats_from_file(f)

def main():
	read_centralized_data()
	read_distributed_data()

	plot_fail_sensors = [1,2,3,4,5]
	#Number of Sensors moved Vs number of sensors failed
	plt.xlabel("Number of failed sensors")
	plt.ylabel("Number of sensors moved")
	plt.errorbar(plot_fail_sensors, n_moved1K, yerr=n_moved1K_err, label="K = 1", color=color_k1)
	plt.errorbar(plot_fail_sensors, n_moved2K, yerr=n_moved2K_err, label="K = 2", color=color_k2)
	plt.errorbar(plot_fail_sensors, n_moved3K, yerr=n_moved3K_err, label="K = 3", color=color_k3)
	plt.errorbar(plot_fail_sensors, n_moved4K, yerr=n_moved4K_err, label="K = 4", color=color_k4)
	plt.errorbar(plot_fail_sensors, n_moved5K, yerr=n_moved5K_err, label="K = 5", color=color_k5)
	# plt.legend(bbox_to_anchor=(0., 1.02, 1., .102), loc=3, ncol=4, mode="expand", borderaxespad=0.)
	plt.gcf().set_size_inches(12.5, 10.5)
	plt.legend(loc='upper center',ncol=3, bbox_to_anchor=(0.5, 1.1))
	# plt.legend(loc=9,ncol=3,borderaxespad=-1.5)
	x1,x2,y1,y2 = plt.axis()
	plt.axis((0.9,5.1,y1,y2))
	plt.xticks(np.arange(x1, 6, 1.0))
	plt.tight_layout()
	plt.savefig("n_moved.png",dpi=300)
	plt.clf()
	#Avg distance Vs Number of Sensors failed
	plt.xlabel("Number of failed sensors")
	plt.ylabel("Distance/Displacement(m)")
	plt.errorbar(plot_fail_sensors, plot_c_avg_disp, yerr=plot_c_avg_disp_err, label="cent.", color=color_cent)
	plt.errorbar(plot_fail_sensors, avg_distance1K, yerr=avg_distance1K_err, label="K = 1", color=color_k1)
	plt.errorbar(plot_fail_sensors, avg_distance2K, yerr=avg_distance2K_err, label="K = 2", color=color_k2)
	plt.errorbar(plot_fail_sensors, avg_distance3K, yerr=avg_distance3K_err, label="K = 3", color=color_k3)
	plt.errorbar(plot_fail_sensors, avg_distance4K, yerr=avg_distance4K_err, label="K = 4", color=color_k4)
	plt.errorbar(plot_fail_sensors, avg_distance5K, yerr=avg_distance5K_err, label="K = 5", color=color_k5)
	# plt.legend(bbox_to_anchor=(0., 1.02, 1., .102), loc=3, ncol=4, mode="expand", borderaxespad=0.)
	plt.gcf().set_size_inches(12.5, 10.5)
	# Put a legend to the right of the current axis
	plt.legend(loc='upper center',ncol=3, bbox_to_anchor=(0.5, 1.07))
	# plt.legend(loc=9,ncol=3,borderaxespad=-4.5)
	x1,x2,y1,y2 = plt.axis()
	plt.axis((0.9,5.1,-0.25,4.5))
	plt.xticks(np.arange(x1, 6, 1.0))
	plt.yticks(np.arange(0, 4.5, 0.5))
	plt.tight_layout()
	plt.savefig("avg_dist.png", dpi=300)
	plt.clf()
	#Accuracy Vs Number of Sensors failed
	plt.xlabel("Number of failed sensors")
	plt.ylabel("Accuracy in 1000 runs")
	plt.plot(plot_fail_sensors, [1.0,1.0,1.0,1.0,1.0], label="cent.", color=color_cent, marker=marker_cent, **marker_style)
	plt.plot(plot_fail_sensors, accuracy1K, label="K = 1", color=color_k1, marker=marker, **marker_style2)
	plt.plot(plot_fail_sensors, accuracy2K, label="K = 2", color=color_k2, marker=marker, **marker_style2)
	plt.plot(plot_fail_sensors, accuracy3K, label="K = 3", color=color_k3, marker=marker, **marker_style2)
	plt.plot(plot_fail_sensors, accuracy4K, label="K = 4", color=color_k4, marker=marker, **marker_style2)
	plt.plot(plot_fail_sensors, accuracy5K, label="K = 5", color=color_k5, marker=marker, **marker_style2)
	# plt.legend(bbox_to_anchor=(0., 1.02, 1., .102), loc=3, ncol=4, mode="expand", borderaxespad=0.)
	plt.legend(loc=3,ncol=1,borderaxespad=0)
	x1,x2,y1,y2 = plt.axis()
	plt.axis((x1,x2,-0.05,1.05))
	plt.xticks(np.arange(x1, 6, 1.0))
	plt.tight_layout()
	plt.savefig("accuracy.png",dpi=300)
	plt.clf()
	
	#Timestamps Vs Number of Sensors failed
	plt.xlabel("Number of failed sensors")
	plt.ylabel("Timestamps")
	plt.errorbar(plot_fail_sensors, timestamp1K, yerr=timestamp1K_err, label="K = 1", color=color_k1)
	plt.errorbar(plot_fail_sensors, timestamp2K, yerr=timestamp2K_err, label="K = 2", color=color_k2)
	plt.errorbar(plot_fail_sensors, timestamp3K, yerr=timestamp3K_err, label="K = 3", color=color_k3)
	plt.errorbar(plot_fail_sensors, timestamp4K, yerr=timestamp4K_err, label="K = 4", color=color_k4)
	plt.errorbar(plot_fail_sensors, timestamp5K, yerr=timestamp5K_err, label="K = 5", color=color_k5)
	# plt.legend(bbox_to_anchor=(0., 1.02, 1., .102), loc=3, ncol=4, mode="expand", borderaxespad=0.)
	plt.legend(loc=9,ncol=3,borderaxespad=-2)
	x1,x2,y1,y2 = plt.axis()
	plt.axis((0.9,5.1,y1,y2))
	plt.xticks(np.arange(x1, 6, 1.0))
	plt.tight_layout()
	plt.savefig("timestamp.png")
	plt.clf()



if __name__ == '__main__':
	main()


