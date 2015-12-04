import math
import time

import pygame
from pygame.locals import *
from pygame.color import *

INFINITY = 10000000000
FORCE_THRESHOLD = 1
# Enable or disable DEBUG
DEBUG = True

#Taking sensing range as input
sensingRange = float(input("Enter the sensing range of the sensors : "))
beltWidth = float(input("Enter the beltWidth : "))
beltHeight = float(input("Enter the beltHeight: "))

# Initializing pymunk and pygame
# Small error quantity and small pointmass radius
delta = 0.01

pygame.init()
padding = 10
max_screen_width = 1180
max_screen_height = 600
screen_width = None
screen_height = None
# calculate screen width and heigh based on the belt width and height
# if (max_screen_width/max_screen_height) <= (beltWidth/beltHeight):
# 	# width preference
# 	screen_height = (max_screen_width * beltHeight / beltWidth);
# 	screen_width = max_screen_width;
# else:
# 	# height preference
screen_height = max_screen_height;
screen_width = (max_screen_height * beltWidth / beltHeight);

pygame_screen = pygame.display.set_mode((max_screen_width + 2*padding + 100,max_screen_height + 2*padding + 100))
pygame.display.set_caption("Barrier Coverage Simulation")
pygame_clock = pygame.time.Clock()

def within_screen_limits(x,y):
	return (padding <= x <= padding + screen_width and padding <= y <= padding + screen_height)

def get_screen_coordinates(x, y):
	scale = screen_width / beltWidth
	screen_x = int(x * scale) + padding
	screen_y = int(screen_height - y * scale) + padding
	return screen_x, screen_y

def get_belt_coordinates(x, y):
	scale = beltWidth / screen_width
	belt_x = (x - padding) * scale
	belt_y = (screen_height - y + padding) * scale
	return belt_x, belt_y

class SensorCoordinate():
	def __init__(self,x,y):
		self.x = x
		self.y = y
	
	def get_screen_coordinates(self):
		return get_screen_coordinates(self.x, self.y)

	def draw_sensor(self):
		scale = screen_width / beltWidth
		p = self.get_screen_coordinates()
		pygame.draw.circle(pygame_screen, THECOLORS["red"], p, int(sensingRange * scale), 1)
		coordinate_text = "(%.1f,%.1f)" % (self.x, self.y)
		coordinate_font = pygame.font.SysFont("comicsansms", 15)
		coordinate_label = coordinate_font.render(coordinate_text, 1, THECOLORS["black"])
		p = list(p)
		p[0] -= 30
		p[1] -= 25
		p = tuple(p)
		pygame_screen.blit(coordinate_label, p)

def draw_belt():
	# pygame considers inverse of y coordinates therefore base is actually the top in game screen
	# left
	p1 = (padding, padding)
	p2 = (padding, padding + int(screen_height))
	pygame.draw.lines(pygame_screen, THECOLORS["black"], False, [p1,p2])
	# base
	p1 = (padding, padding)
	p2 = (padding + int(screen_width), padding)
	pygame.draw.lines(pygame_screen, THECOLORS["gray"], False, [p1,p2])
	# right
	p1 = (padding + int(screen_width), padding)
	p2 = (padding + int(screen_width), padding + int(screen_height))
	pygame.draw.lines(pygame_screen, THECOLORS["black"], False, [p1,p2])
	# top
	p1 = (padding, padding + int(screen_height))
	p2 = (padding + int(screen_width), padding + int(screen_height))
	pygame.draw.lines(pygame_screen, THECOLORS["gray"], False, [p1,p2])

def init_screen():
	pygame_screen.fill(THECOLORS["white"])
	draw_belt()

coordinates = []

def draw_all_sensors():
	for sensor in coordinates:
		sensor.draw_sensor()

def save_input(save_file):
	f = open(save_file, 'w')
	f.write("%f\n" % sensingRange)
	f.write("%f\n" % beltWidth)
	f.write("%f\n" % beltHeight)
	f.write("%d\n" % len(coordinates))
	# save all the coordinates
	for sensor in coordinates:
		f.write("%f\n" % sensor.x)
		f.write("%f\n" % sensor.y)

LEFT = 1
RIGHT = 3
def main():
	global coordinates
	running = True
	init_screen()
	while running:
		for event in pygame.event.get():
			if event.type == QUIT:
				running = False
			elif event.type == pygame.MOUSEBUTTONDOWN and event.button == LEFT:
				print "left mouse button at (%d, %d)" % event.pos
				# Add a sensor
				if not within_screen_limits(*event.pos):
					continue
				belt_coordinates = get_belt_coordinates(*event.pos)
				dummy_sensor = SensorCoordinate(*belt_coordinates)
				coordinates.append(dummy_sensor)
				init_screen()
				draw_all_sensors()
	 		elif event.type == pygame.MOUSEBUTTONDOWN and event.button == RIGHT:
		 		print "right mouse button at (%d, %d)" % event.pos
		 		# Pop a sensor
		 		coordinates.pop()
		 		init_screen()
		 		draw_all_sensors()

		pygame.display.flip()
		pygame_clock.tick(50)

	# ask user to save input
	print "Enter filename to save input or leave blank otherwise"
	save_file = raw_input("Do you want to save the input? : ")
	print save_file
	if save_file:
		save_input(save_file)


if __name__ == '__main__':
	main()