all: server client

server: server.o test.o Particle.o Cluster.o vfunctions.o
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
	gcc -c server/vfunctions.cpp -std=c++14

client: client/client.c
	gcc client/client.c -lm -lGL -lglut -lGLU  -o run_client
clean:
	rm *o run


linab_server: linab_server.o linab_test.o linab_Particle.o linab_Cluster.o linab_vfunctions.o
	gcc-4.6 server.o test.o Particle.o Cluster.o vfunctions.o -O2 -g -lpthread -lm -o run_server -std=gnu++0x -lstdc++
	rm server.o test.o Particle.o Cluster.o
linab_test.o: server/test.cpp server/server.hpp server/globals.hpp
	gcc-4.6 -c server/test.cpp -std=gnu++0x -lstdc++
linab_server.o: server/server.cpp server/server.hpp
	gcc-4.6 -c server/server.cpp -O2 -g -lpthread -lm -std=gnu++0x -lstdc++
linab_Particle.o: server/Particle.cpp server/Particle.hpp
	gcc-4.6 -c server/Particle.cpp -std=gnu++0x -lstdc++
linab_Cluster.o: server/Cluster.cpp	server/Particle.hpp
	gcc-4.6 -c server/Cluster.cpp -std=gnu++0x -lstdc++
linab_vfunctions.o: server/vfunctions.hpp server/vfunctions.cpp
	gcc-4.6 -c server/vfunctions.cpp -std=gnu++0x
