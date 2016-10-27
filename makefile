all:run_server

run_server: server.o test.o Particle.o Cluster.o run_client
	g++ server.o test.o Particle.o Cluster.o -O2 -g -std=c++0x -lpthread -lm -o run_server
	rm server.o test.o Particle.o Cluster.o
test.o: server/test.cpp server/server.hpp server/globals.hpp
	g++ -c server/test.cpp
server.o: server/server.cpp server/server.hpp
	g++ -c server/server.cpp -std=c++0x -O2 -g -lpthread -lm
Particle.o: server/Particle.cpp server/Particle.hpp
	g++ -c server/Particle.cpp
Cluster.o: server/Cluster.cpp	server/Particle.hpp
	g++ -c server/Cluster.cpp

run_client: client/client.c
	gcc client/client.c -lm -lGL -lglut -lGLU  -o run_client
clean:
	rm *o run
