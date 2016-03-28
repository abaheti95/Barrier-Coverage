simulator: simulator.cpp simulator.hpp Sensor.o Coordinate.o Force.o
	g++ -Wall -g simulator.cpp Sensor.o Force.o Coordinate.o -o simulator -lgraph

Sensor.o: Sensor.cpp Sensor.hpp Force.o
	g++ -Wall -g -c Sensor.cpp Force.o -o Sensor.o

Coordinate.o: Coordinate.cpp Coordinate.hpp
	g++ -Wall -g -c Coordinate.cpp -o Coordinate.o

Force.o: Force.hpp Force.cpp Coordinate.o
	g++ -Wall -g -c Force.cpp -o Force.o

clean:
	rm Coordinate.o Sensor.o Force.o simulator 