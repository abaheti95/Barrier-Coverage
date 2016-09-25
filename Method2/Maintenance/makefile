plot_data_generator: plot_data_generator.cpp Simulator.o
	g++ -Wall -g plot_data_generator.cpp Simulator.o Event.o Sensor.o Force.o Coordinate.o -o plot_data_generator -L/usr/local/lib -lglfw -lrt -lXrandr -lXi -lGL -lm -ldl -lXrender -ldrm -lXdamage -lX11-xcb -lxcb-glx -lxcb-dri2 -lxcb-dri3 -lxcb-present -lxcb-sync -lxshmfence -lXxf86vm -lXfixes -lXext -lX11 -lpthread -lxcb -lXau -lXdmcp

mass_tester: mass_tester.cpp Simulator.o
	g++ -Wall -g mass_tester.cpp Simulator.o Event.o Sensor.o Force.o Coordinate.o -o mass_tester -L/usr/local/lib -lglfw -lrt -lXrandr -lXi -lGL -lm -ldl -lXrender -ldrm -lXdamage -lX11-xcb -lxcb-glx -lxcb-dri2 -lxcb-dri3 -lxcb-present -lxcb-sync -lxshmfence -lXxf86vm -lXfixes -lXext -lX11 -lpthread -lxcb -lXau -lXdmcp

tester: tester.cpp Simulator.o
	g++ -Wall -g tester.cpp Simulator.o Event.o Sensor.o Force.o Coordinate.o -o tester -L/usr/local/lib -lglfw -lrt -lXrandr -lXi -lGL -lm -ldl -lXrender -ldrm -lXdamage -lX11-xcb -lxcb-glx -lxcb-dri2 -lxcb-dri3 -lxcb-present -lxcb-sync -lxshmfence -lXxf86vm -lXfixes -lXext -lX11 -lpthread -lxcb -lXau -lXdmcp

Simulator.o: Simulator.cpp Simulator.hpp Sensor.o Coordinate.o Force.o Event.o
	g++ -Wall -g -c Simulator.cpp Sensor.o Force.o Coordinate.o Event.o -o Simulator.o -L/usr/local/lib -lglfw -lrt -lXrandr -lXi -lGL -lm -ldl -lXrender -ldrm -lXdamage -lX11-xcb -lxcb-glx -lxcb-dri2 -lxcb-dri3 -lxcb-present -lxcb-sync -lxshmfence -lXxf86vm -lXfixes -lXext -lX11 -lpthread -lxcb -lXau -lXdmcp

Event.o: Event.cpp Event.hpp
	g++ -Wall -g -c Event.cpp -o Event.o

Sensor.o: Sensor.cpp Sensor.hpp Force.o
	g++ -Wall -g -c Sensor.cpp Force.o -o Sensor.o

Coordinate.o: Coordinate.cpp Coordinate.hpp
	g++ -Wall -g -c Coordinate.cpp -o Coordinate.o

Force.o: Force.hpp Force.cpp Coordinate.o
	g++ -Wall -g -c Force.cpp -o Force.o

clean:
	rm tester Coordinate.o Event.o Sensor.o Force.o Simulator.o