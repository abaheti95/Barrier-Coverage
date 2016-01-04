# The centralized version for solving barrier coverage from paper : http://163.13.128.75/member/seminar/files/edenboy_2008_9_25.pdf

number_of_sensors = None
x = []
y = []
x2 = []
y2 = []
Fr = []
Fa = []
L = None
W = None
R = None
Rc = None
INFINITY = 10000000000
MAX_STEP = 100

padding = 10
max_screen_width = 1180
max_screen_height = 600

screen_width = None
screen_height = None
screen_width = None
pygame_screen = None
pygame_clock = None

alpha = None
beta = None
delta_x = None
delta_y = None

def init():
	global screen_width
	global screen_height
	global pygame_screen
	global pygame_clock
	global alpha
	global beta
	global delta_x
	global delta_y
	#Taking sensing range as input

	# Initializing pymunk and pygame
	# Small error quantity and small pointmass radius

	pygame.init()
	max_screen_width = 1180
	max_screen_height = 600
	screen_width = None
	screen_height = None
	screen_height = max_screen_height;
	screen_width = (max_screen_height * beltWidth / beltHeight);

	pygame_screen = pygame.display.set_mode((max_screen_width + 2*padding + 100,max_screen_height + 2*padding + 100))
	pygame.display.set_caption("Barrier Coverage Simulation")
	pygame_clock = pygame.time.Clock()

	alpha = 1
	beta = 1
	delta_x = 0.1
	delta_y = 0.1

def init_sensors(input_x, input_y, width, height, sensingRange, networkRange):
	global x
	global y
	global x2
	global y2
	global Fr
	global Fa
	global L
	global W
	global R
	global Rc
	x = input_x
	y = input_y
	x2 = list(x)
	y2 = list(y)
	Fr = [0] * len(x)
	Fa = [0] * len(y)
	L = width
	W = height
	R = sensingRange
	Rc = networkRange

def euclidean_distance(x1, y1, x2, y2):
	change_x = x2 - x1
	change_y = y2 - y1
	distance_square = change_x*change_x + change_y*change_y
	return math.sqrt(distance_square)

def solve():
	global x2
	global y2
	global Fr
	global Fa
	t = 0
	N = len(x)
	# TODO: show the steps graphically on pygame window
	while(t < MAX_STEP):
		t = t+1
		flag = True
		for i in range(N):
			# compute the 2 hop neighbors of current sensor
			neighbors = []
			for j in range(N):
				if i == j:
					continue
				if euclidean_distance(x2[i], y2[i], x2[j], y2[j]) < (2*Rc):
					neighbors.append(j)

			# Now caclulate repulsive and attractive forces based on the neighborhood sensors
			Fr[i] = 0.0
			Fa[i] = 0.0
			for j in neighbors:
				Fr[i] += alpha / (x2[i] - x2[j])
				Fa[i] += beta * (y2[j] - y2[i])
			# Calculate the force by boundary
			if x2[i] < Rc:
				Fr[i] += -1/x2[i]
			else if x2[i] > (L - Rc):
				Fr[i] += 1/(x2[i] - L)

			flag = flag & (max(Fr[i], Fa[i]) < epsilon)

		if flag:
			break

		for i in range(N):
			# Move the sensor based on the force
			if Fr[i] != 0.0:
				x2[i] = x2[i] + delta_x * Fr[i] / abs(Fr[i])
			if Fa[i] != 0.0:
				y2[i] = y2[i] + delta_y * Fa[i] / abs(Fa[i])

	return t

def print_results():
	# verify edges
	number_of_sensors_moved = 0
	max_displacement = 0.0
	min_displacement = INFINITY
	displacement = 0.0
	print "CBarrier Results:"
	N = len(x)
	for i in range(N):
		current_displacement = euclidean_distance(x[i], y[i], x2[i], y2[i])
		displacement += current_displacement
		if current_displacement > delta:
			number_of_sensors_moved += 1
		max_displacement = max(current_displacement, max_displacement)
		min_displacement = min(current_displacement, min_displacement)

	avg_displacement = displacement / N
	print "\n\nNumber of sensors Moved : " + str(number_of_sensors_moved)
	print "Max Displacement : " + str(max_displacement)
	print "Min Displacement : " + str(min_displacement)
	print "Avg Displacement : " + str(avg_displacement)

	return number_of_sensors_moved,max_displacement,min_displacement,avg_displacement



