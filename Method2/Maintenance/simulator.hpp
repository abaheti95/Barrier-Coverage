#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include "Coordinate.hpp"
#include "Force.hpp"
#include "Sensor.hpp"
#include "Event.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cmath>
#include <unistd.h>
#include <utility>
#include <vector>
#include <algorithm>
#include <queue>
#include <GLFW/glfw3.h>
#include <FTGL/ftgl.h>

void draw_sensor(Sensor&, float, float, float);
#endif