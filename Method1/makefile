test: Tester.cpp Simulator.o GeometricGraph.o Coordinate.o
	g++ -Wall -g Tester.cpp Simulator.o GeometricGraph.o Coordinate.o `pkg-config opencv --cflags --libs` -o test

Coordinate.o: Coordinate.cpp Coordinate.hpp
	g++ -Wall -g -c Coordinate.cpp -o Coordinate.o

GeometricGraph.o: GeometricGraph.hpp GeometricGraph.cpp Coordinate.o
	g++ -Wall -g -c GeometricGraph.cpp -o GeometricGraph.o

Simulator.o: Simulator.cpp Simulator.hpp
	g++ -c Simulator.cpp `pkg-config opencv --cflags --libs` -o Simulator.o

clean:
	rm Coordinate.o GeometricGraph.o Simulator.o test