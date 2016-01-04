import math
# The centralized version for solving barrier coverage from paper : http://163.13.128.75/member/seminar/files/edenboy_2008_9_25.pdf

number_of_sensors = None
x = []
y = []
x2 = []
y2 = []
L = None
R = None
INFINITY = 10000000000

def init_sensors(input_x, input_y, width, height, sensingRange):
	global x
	global y
	global x2
	global y2
	global L
	global R
	x = input_x
	y = input_y
	# print "Before"
	# for e in zip(x, y):
	# 	print e
	# sort y based on x
	y = [py for px, py in sorted(zip(x, y))]
	x.sort()
	# print "After"
	# for e in zip(x, y):
	# 	print e

	x2 = [None] * len(x)
	y2 = [None] * len(y)
	L = width
	R = sensingRange

def solve():
	# first set the new x coordinates
	x2[0] = R
	x2[-1] = L - R
	N = len(x)
	for i in range(1, (N - 1)):
		x2[i] = R + (i-1)*(L- (2.0*R))/(N-1)

	# now we try to align everyone on the line y = a*x + b
	# calculating a
	sigma_x2 = sum(x2)
	sigma_x2_square = 0
	for val in x2:
		sigma_x2_square += val*val

	sigma_y = sum(y)
	sigma_x2_y = 0
	for i in range(N):
		sigma_x2_y += x2[i]*y[i]

	a = (N*sigma_x2_y - sigma_x2 * sigma_y) / (N * sigma_x2_square - sigma_x2 * sigma_x2)

	b = (sigma_y - a * sigma_x2) / N

	# calucate y using a and b
	for i in range(N):
		y2[i] = a * x2[i] + b

def euclidean_distance(x1, y1, x2, y2):
	change_x = x2 - x1
	change_y = y2 - y1
	distance_square = change_x*change_x + change_y*change_y
	return math.sqrt(distance_square)

def print_results():
	max_displacement = 0.0
	min_displacement = INFINITY
	displacement = 0.0
	print "CBarrier Results:"
	N = len(x)
	for i in range(N):
		current_displacement = euclidean_distance(x[i], y[i], x2[i], y2[i])
		displacement += current_displacement
		max_displacement = max(current_displacement, max_displacement)
		min_displacement = min(current_displacement, min_displacement)

	avg_displacement = displacement / N
	print "Max Displacement : " + str(max_displacement)
	print "Min Displacement : " + str(min_displacement)
	print "Avg Displacement : " + str(avg_displacement)

	return max_displacement,min_displacement,avg_displacement

def simulate(input_x, input_y, width, height, sensingRange):
	# init the sensors
	init_sensors(input_x, input_y, width, height, sensingRange)
	# solve the test case
	solve()
	# print results
	return print_results()
