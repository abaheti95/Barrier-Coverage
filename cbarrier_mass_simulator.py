import Implementations.Shen.cbarrier as cbarrier
import numpy as np
import Method2.mass_simulations as simulator

Max_disp = []
Min_disp = []
Avg_disp = []
Max_disp_err = []
Min_disp_err = []
Avg_disp_err = []

def cbarrier_vs_method2():
	# run the input on cbarrier and get the results
	max_displacement1, min_displacement1, avg_displacement1 = cbarrier.simulate(simulator.x, simulator.y, simulator.beltWidth, simulator.beltHeight, simulator.sensingRange)

	return max_displacement1, min_displacement1, avg_displacement1

def main():
	# run 10 phases each time increasing the multiplier by 1
	initial_beltWidth = simulator.beltWidth
	initial_numberOfSensors = simulator.numberOfSensors

	multiplier = 5
	number_of_simulations = 100

	simulator.beltWidth = initial_beltWidth * multiplier	
	simulator.numberOfSensors = 45

	for multiplier in range(5,11):
		max_disp = []
		min_disp = []
		avg_disp = []
		for i in range(number_of_simulations):
			simulator.generate_random_input()
			max_displacement, min_displacement, avg_displacement = cbarrier_vs_method2()
			max_disp.append(max_displacement)
			min_disp.append(min_displacement)
			avg_disp.append(avg_displacement)
		np_max_disp = np.array(max_disp)
		np_min_disp = np.array(min_disp)
		np_avg_disp = np.array(avg_disp)
		Max_disp.append(np.mean(np_max_disp))
		Min_disp.append(np.mean(np_min_disp))
		Avg_disp.append(np.mean(np_avg_disp))
		Max_disp_err.append(np.std(np_max_disp))
		Min_disp_err.append(np.std(np_min_disp))
		Avg_disp_err.append(np.std(np_avg_disp))

	print Max_disp
	print Min_disp
	print Avg_disp
	print Max_disp_err
	print Min_disp_err
	print Avg_disp_err

if __name__ == '__main__':
	main()
