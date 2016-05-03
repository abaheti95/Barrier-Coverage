simulator: simulator.cpp simulator.hpp Sensor.o Coordinate.o Force.o Event.o
	g++ -Wall -g simulator.cpp Sensor.o Force.o Coordinate.o Event.o -o simulator -L/usr/local/lib -lglfw3 -lrt -lXrandr -lXi -lGL -lm -ldl -lXrender -ldrm -lXdamage -lX11-xcb -lxcb-glx -lxcb-dri2 -lxcb-dri3 -lxcb-present -lxcb-sync -lxshmfence -lXxf86vm -lXfixes -lXext -lX11 -lpthread -lxcb -lXau -lXdmcp -I/usr/include/freetype2

Event.o: Event.cpp Event.hpp
	g++ -Wall -g -c Event.cpp -o Event.o

Sensor.o: Sensor.cpp Sensor.hpp Force.o
	g++ -Wall -g -c Sensor.cpp Force.o -o Sensor.o

Coordinate.o: Coordinate.cpp Coordinate.hpp
	g++ -Wall -g -c Coordinate.cpp -o Coordinate.o

Force.o: Force.hpp Force.cpp Coordinate.o
	g++ -Wall -g -c Force.cpp -o Force.o

clean:
	rm Coordinate.o Sensor.o Force.o simulator 