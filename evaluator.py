import Implementations.Shen.cbarrier as cbarrier
import Method2.simulator as method2
import Method2.mass_simulations as simulator

number_of_sensors_moved = []
Max_displacement1 = []
Min_displacement1 = []
Avg_displacement1 = []
Max_displacement2 = []
Min_displacement2 = []
Avg_displacement2 = []

def cbarrier_vs_method2():
	# run the input on cbarrier and get the results
	max_displacement1, min_displacement1, avg_displacement1 = cbarrier.simulate(simulator.x, simulator.y, simulator.beltWidth, simulator.beltHeight, simulator.sensingRange)
	# Initialize the input on method2 and get the results
	method2.init_param(simulator.sensingRange, simulator.beltWidth, simulator.beltHeight, simulator.numberOfSensors, simulator.x, simulator.y)
	number_of_sensors_moved, max_distance, min_distance, max_displacement2, min_displacement2, avg_distance, avg_displacement2 = method2.simulate()

	# append the data to the array
	Max_displacement1.append(max_displacement1)
	Min_displacement1.append(min_displacement1)
	Avg_displacement1.append(avg_displacement1)
	Max_displacement2.append(max_displacement2)
	Min_displacement2.append(min_displacement2)
	Avg_displacement2.append(avg_displacement2)

def main():
	# run 10 phases each time increasing the multiplier by 1
	initial_beltWidth = simulator.beltWidth
	initial_numberOfSensors = simulator.numberOfSensors

	
	multiplier = 5
	number_of_simulations = 5

	simulator.beltWidth = initial_beltWidth * multiplier
	simulator.numberOfSensors = initial_numberOfSensors * multiplier
	for i in range(number_of_simulations):
		simulator.generate_random_input()
		cbarrier_vs_method2()

	print "Final result after comparison"
	print "cbarrier"
	print "Max Displacement"
	print Max_displacement1
	print sum(Max_displacement1) / float(len(Max_displacement1))
	print "Min Displacement"
	print Min_displacement1
	print sum(Min_displacement1) / float(len(Min_displacement1))
	print "Avg Displacement"
	print Avg_displacement1
	print sum(Avg_displacement1) / float(len(Avg_displacement1))
	print "method2"
	print "Max Displacement"
	print Max_displacement2
	print sum(Max_displacement2) / float(len(Max_displacement2))
	print "Min Displacement"
	print Min_displacement2
	print sum(Min_displacement2) / float(len(Min_displacement2))
	print "Avg Displacement"
	print Avg_displacement2
	print sum(Avg_displacement2) / float(len(Avg_displacement2))

if __name__ == '__main__':
	main()
