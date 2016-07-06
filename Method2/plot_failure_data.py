import numpy as np
import matplotlib.pyplot as plt

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
avg_distance2K5t = []
avg_distance2K10t = []
avg_distance2K50t = []
avg_distance2K100t = []
avg_distance5K5t = []
avg_distance5K10t = []
avg_distance5K50t = []
avg_distance5K100t = []
max_distance2K5t = []
max_distance2K10t = []
max_distance2K50t = []
max_distance2K100t = []
max_distance5K5t = []
max_distance5K10t = []
max_distance5K50t = []
max_distance5K100t = []
timestamp2K5t = []
timestamp2K10t = []
timestamp2K50t = []
timestamp2K100t = []
timestamp5K5t = []
timestamp5K10t = []
timestamp5K50t = []
timestamp5K100t = []
n_moved2K5t = []
n_moved2K10t = []
n_moved2K50t = []
n_moved2K100t = []
n_moved5K5t = []
n_moved5K10t = []
n_moved5K50t = []
n_moved5K100t = []
accuracy2K5t = []
accuracy2K10t = []
accuracy2K50t = []
accuracy2K100t = []
accuracy5K5t = []
accuracy5K10t = []
accuracy5K50t = []
accuracy5K100t = []
avg_distance2K5t_err = []
avg_distance2K10t_err = []
avg_distance2K50t_err = []
avg_distance2K100t_err = []
avg_distance5K5t_err = []
avg_distance5K10t_err = []
avg_distance5K50t_err = []
avg_distance5K100t_err = []
max_distance2K5t_err = []
max_distance2K10t_err = []
max_distance2K50t_err = []
max_distance2K100t_err = []
max_distance5K5t_err = []
max_distance5K10t_err = []
max_distance5K50t_err = []
max_distance5K100t_err = []
timestamp2K5t_err = []
timestamp2K10t_err = []
timestamp2K50t_err = []
timestamp2K100t_err = []
timestamp5K5t_err = []
timestamp5K10t_err = []
timestamp5K50t_err = []
timestamp5K100t_err = []
n_moved2K5t_err = []
n_moved2K10t_err = []
n_moved2K50t_err = []
n_moved2K100t_err = []
n_moved5K5t_err = []
n_moved5K10t_err = []
n_moved5K50t_err = []
n_moved5K100t_err = []

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
	global avg_distance2K5t, avg_distance2K10t, avg_distance2K50t, avg_distance2K100t, avg_distance5K5t, avg_distance5K10t, avg_distance5K50t, avg_distance5K100t, max_distance2K5t, max_distance2K10t, max_distance2K50t, max_distance2K100t, max_distance5K5t, max_distance5K10t, max_distance5K50t, max_distance5K100t, timestamp2K5t, timestamp2K10t, timestamp2K50t, timestamp2K100t, timestamp5K5t, timestamp5K10t, timestamp5K50t, timestamp5K100t, n_moved2K5t, n_moved2K10t, n_moved2K50t, n_moved2K100t, n_moved5K5t, n_moved5K10t, n_moved5K50t, n_moved5K100t, accuracy2K5t, accuracy2K10t, accuracy2K50t, accuracy2K100t, accuracy5K5t, accuracy5K10t, accuracy5K50t, accuracy5K100t, avg_distance2K5t_err, avg_distance2K10t_err, avg_distance2K50t_err, avg_distance2K100t_err, avg_distance5K5t_err, avg_distance5K10t_err, avg_distance5K50t_err, avg_distance5K100t_err, max_distance2K5t_err, max_distance2K10t_err, max_distance2K50t_err, max_distance2K100t_err, max_distance5K5t_err, max_distance5K10t_err, max_distance5K50t_err, max_distance5K100t_err, timestamp2K5t_err, timestamp2K10t_err, timestamp2K50t_err, timestamp2K100t_err, timestamp5K5t_err, timestamp5K10t_err, timestamp5K50t_err, timestamp5K100t_err, n_moved2K5t_err, n_moved2K10t_err, n_moved2K50t_err, n_moved2K100t_err, n_moved5K5t_err, n_moved5K10t_err, n_moved5K50t_err, n_moved5K100t_err
	with open("Maintenance/plot_data_saved.txt", 'r') as f:
		# K = 2, t = 5
		avg_distance2K5t = read_5_floats_from_file(f)
		avg_distance2K5t_err = read_5_floats_from_file(f)
		max_distance2K5t = read_5_floats_from_file(f)
		max_distance2K5t_err = read_5_floats_from_file(f)
		timestamp2K5t = read_5_floats_from_file(f)
		timestamp2K5t_err = read_5_floats_from_file(f)
		n_moved2K5t = read_5_floats_from_file(f)
		n_moved2K5t_err = read_5_floats_from_file(f)
		accuracy2K5t = read_5_floats_from_file(f)
		# K = 2, t = 10
		avg_distance2K10t = read_5_floats_from_file(f)
		avg_distance2K10t_err = read_5_floats_from_file(f)
		max_distance2K10t = read_5_floats_from_file(f)
		max_distance2K10t_err = read_5_floats_from_file(f)
		timestamp2K10t = read_5_floats_from_file(f)
		timestamp2K10t_err = read_5_floats_from_file(f)
		n_moved2K10t = read_5_floats_from_file(f)
		n_moved2K10t_err = read_5_floats_from_file(f)
		accuracy2K10t = read_5_floats_from_file(f)
		# K = 2, t = 50
		avg_distance2K50t = read_5_floats_from_file(f)
		avg_distance2K50t_err = read_5_floats_from_file(f)
		max_distance2K50t = read_5_floats_from_file(f)
		max_distance2K50t_err = read_5_floats_from_file(f)
		timestamp2K50t = read_5_floats_from_file(f)
		timestamp2K50t_err = read_5_floats_from_file(f)
		n_moved2K50t = read_5_floats_from_file(f)
		n_moved2K50t_err = read_5_floats_from_file(f)
		accuracy2K50t = read_5_floats_from_file(f)
		# K = 2, t = 100
		avg_distance2K100t = read_5_floats_from_file(f)
		avg_distance2K100t_err = read_5_floats_from_file(f)
		max_distance2K100t = read_5_floats_from_file(f)
		max_distance2K100t_err = read_5_floats_from_file(f)
		timestamp2K100t = read_5_floats_from_file(f)
		timestamp2K100t_err = read_5_floats_from_file(f)
		n_moved2K100t = read_5_floats_from_file(f)
		n_moved2K100t_err = read_5_floats_from_file(f)
		accuracy2K100t = read_5_floats_from_file(f)
		# K = 5, t = 5
		avg_distance5K5t = read_5_floats_from_file(f)
		avg_distance5K5t_err = read_5_floats_from_file(f)
		max_distance5K5t = read_5_floats_from_file(f)
		max_distance5K5t_err = read_5_floats_from_file(f)
		timestamp5K5t = read_5_floats_from_file(f)
		timestamp5K5t_err = read_5_floats_from_file(f)
		n_moved5K5t = read_5_floats_from_file(f)
		n_moved5K5t_err = read_5_floats_from_file(f)
		accuracy5K5t = read_5_floats_from_file(f)
		# K = 5, t = 10
		avg_distance5K10t = read_5_floats_from_file(f)
		avg_distance5K10t_err = read_5_floats_from_file(f)
		max_distance5K10t = read_5_floats_from_file(f)
		max_distance5K10t_err = read_5_floats_from_file(f)
		timestamp5K10t = read_5_floats_from_file(f)
		timestamp5K10t_err = read_5_floats_from_file(f)
		n_moved5K10t = read_5_floats_from_file(f)
		n_moved5K10t_err = read_5_floats_from_file(f)
		accuracy5K10t = read_5_floats_from_file(f)
		# K = 5, t = 50
		avg_distance5K50t = read_5_floats_from_file(f)
		avg_distance5K50t_err = read_5_floats_from_file(f)
		max_distance5K50t = read_5_floats_from_file(f)
		max_distance5K50t_err = read_5_floats_from_file(f)
		timestamp5K50t = read_5_floats_from_file(f)
		timestamp5K50t_err = read_5_floats_from_file(f)
		n_moved5K50t = read_5_floats_from_file(f)
		n_moved5K50t_err = read_5_floats_from_file(f)
		accuracy5K50t = read_5_floats_from_file(f)
		# K = 5, t = 100
		avg_distance5K100t = read_5_floats_from_file(f)
		avg_distance5K100t_err = read_5_floats_from_file(f)
		max_distance5K100t = read_5_floats_from_file(f)
		max_distance5K100t_err = read_5_floats_from_file(f)
		timestamp5K100t = read_5_floats_from_file(f)
		timestamp5K100t_err = read_5_floats_from_file(f)
		n_moved5K100t = read_5_floats_from_file(f)
		n_moved5K100t_err = read_5_floats_from_file(f)
		accuracy5K100t = read_5_floats_from_file(f)

def main():
	read_centralized_data()
	read_distributed_data()

	plot_fail_sensors = [1,2,3,4,5]
	#Number of Sensors moved Vs number of sensors failed
	plt.xlabel("Number of failed sensors")
	plt.ylabel("Number of sensors moved")
	plt.errorbar(plot_fail_sensors, n_moved2K5t, yerr=n_moved2K5t_err, label="K=2,t=5", color=np.random.rand(3,1))
	plt.errorbar(plot_fail_sensors, n_moved2K10t, yerr=n_moved2K10t_err, label="K=2,t=10", color=np.random.rand(3,1))
	plt.errorbar(plot_fail_sensors, n_moved2K50t, yerr=n_moved2K50t_err, label="K=2,t=50", color=np.random.rand(3,1))
	plt.errorbar(plot_fail_sensors, n_moved2K100t, yerr=n_moved2K100t_err, label="K=2,t=100", color=np.random.rand(3,1))
	plt.errorbar(plot_fail_sensors, n_moved5K5t, yerr=n_moved5K5t_err, label="K=5,t=5", color=np.random.rand(3,1))
	plt.errorbar(plot_fail_sensors, n_moved5K10t, yerr=n_moved5K10t_err, label="K=5,t=10", color=np.random.rand(3,1))
	plt.errorbar(plot_fail_sensors, n_moved5K50t, yerr=n_moved5K50t_err, label="K=5,t=50", color=np.random.rand(3,1))
	plt.errorbar(plot_fail_sensors, n_moved5K100t, yerr=n_moved5K100t_err, label="K=5,t=100", color=np.random.rand(3,1))
	# plt.legend(bbox_to_anchor=(0., 1.02, 1., .102), loc=3, ncol=4, mode="expand", borderaxespad=0.)
	plt.legend(loc=9,ncol=4,borderaxespad=-2)
	plt.savefig("n moved.png")
	plt.clf()
	#Avg distance Vs Number of Sensors failed
	plt.xlabel("Number of failed sensors")
	plt.ylabel("Distance/Displacement(m.)")
	plt.errorbar(plot_fail_sensors, plot_c_avg_dist, yerr=plot_c_avg_dist_err, label="dist.", color=np.random.rand(3,1))
	plt.errorbar(plot_fail_sensors, plot_c_avg_disp, yerr=plot_c_avg_disp_err, label="disp.", color=np.random.rand(3,1))
	plt.errorbar(plot_fail_sensors, avg_distance2K5t, yerr=avg_distance2K5t_err, label="K=2,t=5", color=np.random.rand(3,1))
	plt.errorbar(plot_fail_sensors, avg_distance2K10t, yerr=avg_distance2K10t_err, label="K=2,t=10", color=np.random.rand(3,1))
	plt.errorbar(plot_fail_sensors, avg_distance2K50t, yerr=avg_distance2K50t_err, label="K=2,t=50", color=np.random.rand(3,1))
	plt.errorbar(plot_fail_sensors, avg_distance2K100t, yerr=avg_distance2K100t_err, label="K=2,t=100", color=np.random.rand(3,1))
	plt.errorbar(plot_fail_sensors, avg_distance5K5t, yerr=avg_distance5K5t_err, label="K=5,t=5", color=np.random.rand(3,1))
	plt.errorbar(plot_fail_sensors, avg_distance5K10t, yerr=avg_distance5K10t_err, label="K=5,t=10", color=np.random.rand(3,1))
	plt.errorbar(plot_fail_sensors, avg_distance5K50t, yerr=avg_distance5K50t_err, label="K=5,t=50", color=np.random.rand(3,1))
	plt.errorbar(plot_fail_sensors, avg_distance5K100t, yerr=avg_distance5K100t_err, label="K=5,t=100", color=np.random.rand(3,1))
	# plt.legend(bbox_to_anchor=(0., 1.02, 1., .102), loc=3, ncol=4, mode="expand", borderaxespad=0.)
	plt.legend(loc=9,ncol=4,borderaxespad=-2)
	plt.savefig("avg distance.png")
	plt.clf()
	#Accuracy Vs Number of Sensors failed
	plt.xlabel("Number of failed sensors")
	plt.ylabel("Accuracy in 1000 runs")
	plt.plot(plot_fail_sensors, [1.0,1.0,1.0,1.0,1.0], label="cent.", color=np.random.rand(3,1))
	plt.plot(plot_fail_sensors, accuracy2K5t, label="K=2,t=5", color=np.random.rand(3,1))
	plt.plot(plot_fail_sensors, accuracy2K10t, label="K=2,t=10", color=np.random.rand(3,1))
	plt.plot(plot_fail_sensors, accuracy2K50t, label="K=2,t=50", color=np.random.rand(3,1))
	plt.plot(plot_fail_sensors, accuracy2K100t, label="K=2,t=100", color=np.random.rand(3,1))
	plt.plot(plot_fail_sensors, accuracy5K5t, label="K=5,t=5", color=np.random.rand(3,1))
	plt.plot(plot_fail_sensors, accuracy5K10t, label="K=5,t=10", color=np.random.rand(3,1))
	plt.plot(plot_fail_sensors, accuracy5K50t, label="K=5,t=50", color=np.random.rand(3,1))
	plt.plot(plot_fail_sensors, accuracy5K100t, label="K=5,t=100", color=np.random.rand(3,1))
	# plt.legend(bbox_to_anchor=(0., 1.02, 1., .102), loc=3, ncol=4, mode="expand", borderaxespad=0.)
	plt.legend(loc=3,ncol=2,borderaxespad=4)
	plt.savefig("accuracy.png")
	plt.clf()
	
	#Timestamps Vs Number of Sensors failed
	plt.xlabel("Number of failed sensors")
	plt.ylabel("Timestamps")
	plt.errorbar(plot_fail_sensors, timestamp2K5t, yerr=timestamp2K5t_err, label="K=2,t=5", color=np.random.rand(3,1))
	plt.errorbar(plot_fail_sensors, timestamp2K10t, yerr=timestamp2K10t_err, label="K=2,t=10", color=np.random.rand(3,1))
	plt.errorbar(plot_fail_sensors, timestamp2K50t, yerr=timestamp2K50t_err, label="K=2,t=50", color=np.random.rand(3,1))
	plt.errorbar(plot_fail_sensors, timestamp2K100t, yerr=timestamp2K100t_err, label="K=2,t=100", color=np.random.rand(3,1))
	plt.errorbar(plot_fail_sensors, timestamp5K5t, yerr=timestamp5K5t_err, label="K=5,t=5", color=np.random.rand(3,1))
	plt.errorbar(plot_fail_sensors, timestamp5K10t, yerr=timestamp5K10t_err, label="K=5,t=10", color=np.random.rand(3,1))
	plt.errorbar(plot_fail_sensors, timestamp5K50t, yerr=timestamp5K50t_err, label="K=5,t=50", color=np.random.rand(3,1))
	plt.errorbar(plot_fail_sensors, timestamp5K100t, yerr=timestamp5K100t_err, label="K=5,t=100", color=np.random.rand(3,1))
	# plt.legend(bbox_to_anchor=(0., 1.02, 1., .102), loc=3, ncol=4, mode="expand", borderaxespad=0.)
	plt.legend(loc=9,ncol=4,borderaxespad=-2)
	plt.savefig("timestamp.png")
	plt.clf()



if __name__ == '__main__':
	main()


