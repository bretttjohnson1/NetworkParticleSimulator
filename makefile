all:run_server

run_server: server.o test.o Particle.o Cluster.o vfunctions.o run_client
	gcc server.o test.o Particle.o Cluster.o vfunctions.o -O2 -g -lpthread -lm -o run_server -std=c++14 -lstdc++
	rm server.o test.o Particle.o Cluster.o
test.o: server/test.cpp server/server.hpp server/globals.hpp
	gcc -c server/test.cpp -std=c++14 -lstdc++
server.o: server/server.cpp server/server.hpp
	gcc -c server/server.cpp -O2 -g -lpthread -lm -std=c++14 -lstdc++
Particle.o: server/Particle.cpp server/Particle.hpp
	gcc -c server/Particle.cpp -std=c++14 -lstdc++
Cluster.o: server/Cluster.cpp	server/Particle.hpp
	gcc -c server/Cluster.cpp -std=c++14 -lstdc++
vfunctions.o: server/vfunctions.hpp server/vfunctions.cpp
	gcc -c server/vfunctions.cpp

run_client: client/client.c
	gcc client/client.c -lm -lGL -lglut -lGLU  -o run_client
clean:
	rm *o run
