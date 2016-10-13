import numpy as np
import matplotlib.pyplot as plt

import matplotlib
matplotlib.rcParams['lines.linewidth'] = 5
matplotlib.rcParams.update({'font.size': 30})
marker_style = dict(markersize=15)
marker_style2 = dict(markersize=10)
marker_dist = 'd'
marker_disp = 'o'

# Save the relevant data
plot_shen_max_disp = np.load("cent_data/np_plot_shen_max_disp.npy")
plot_shen_max_disp_err = np.load("cent_data/np_plot_shen_max_disp_err.npy")
plot_max_dist = np.load("cent_data/np_plot_max_dist.npy")
plot_max_dist_error = np.load("cent_data/np_plot_max_dist_error.npy")
plot_max_disp = np.load("cent_data/np_plot_max_disp.npy")
plot_max_disp_error = np.load("cent_data/plot_max_disp_error.npy")


plot_shen_avg_disp = np.load("cent_data/np_plot_shen_avg_disp.npy")
plot_shen_avg_disp_err = np.load("cent_data/np_plot_shen_avg_disp_err.npy")
plot_avg_dist = np.load("cent_data/np_plot_avg_dist.npy")
plot_avg_dist_error = np.load("cent_data/np_plot_avg_dist_error.npy")
plot_avg_disp = np.load("cent_data/np_plot_avg_disp.npy")
plot_avg_disp_error = np.load("cent_data/plot_avg_disp_error.npy")

plot_n_sensors = np.arange(50,76,5)
print plot_n_sensors
# Draw Plots

# Max Distance and Displacement
plt.xlabel("Number of Sensors")
plt.ylabel("Distance (m)")
plt.errorbar(plot_n_sensors, plot_shen_max_disp, yerr=plot_shen_max_disp_err, label="shen max displacement", color="red", marker=marker_disp, **marker_style2)
plt.errorbar(plot_n_sensors, plot_max_dist, yerr=plot_max_dist_error, label="centralized max distance", color="green", marker=marker_disp, **marker_style2)
plt.errorbar(plot_n_sensors, plot_max_disp, yerr=plot_max_disp_error, label="centralized max displacement", color="blue", marker=marker_disp, **marker_style2)
plt.legend(loc="upper right", bbox_to_anchor=(1.0, 1.02))
plt.gcf().set_size_inches(12.5, 10.5)
x1,x2,y1,y2 = plt.axis()
plt.axis((x1-1, x2+1,y1,y2+2))
plt.tight_layout()
plt.savefig("max_cent.png", dpi=300)
plt.clf()
# Avg Distance and Displacement
plt.xlabel("Number of Sensors")
plt.ylabel("Distance (m)")
plt.errorbar(plot_n_sensors, plot_shen_avg_disp, yerr=plot_shen_avg_disp_err, label="shen avg displacement", color="red", marker=marker_disp, **marker_style2)
plt.errorbar(plot_n_sensors, plot_avg_dist, yerr=plot_avg_dist_error, label="centralized avg distance", color="green", marker=marker_disp, **marker_style2)
plt.errorbar(plot_n_sensors, plot_avg_disp, yerr=plot_avg_disp_error, label="centralized avg displacement", color="blue", marker=marker_disp, **marker_style2)
plt.legend(loc="upper right", bbox_to_anchor=(1.0, 1.02))
plt.gcf().set_size_inches(12.5, 10.5)
x1,x2,y1,y2 = plt.axis()
plt.axis((x1-1, x2+1,y1,y2+2))
plt.tight_layout()
plt.savefig("avg_cent.png", dpi=300)

