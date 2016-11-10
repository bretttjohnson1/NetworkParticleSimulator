#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/mman.h>

#include <iostream>
#include "Cluster.hpp"
#include "Particle.hpp"
#include <cmath>
#include <cmath>
//#include <muParser.h>
//#include <muParserBase.h>
#include <string>
#include <sys/time.h>
#include <vector>
#include <thread>
#include <pthread.h>
#include "server.hpp"
#include "globals.hpp"
#include "vfunctions.h"

#define ESCAPE 27
#define KEY_W 119
#define KEY_A 97
#define KEY_S 115
#define KEY_D 100
using namespace std;
//using namespace mu;


float offx=1.5f;
float offy=0.0f;
float offz=-6.0f;
bool keys[256];
float phi=0;
float thet=0;

void fail(char *err);
void *physics(void *args);

string gravityx,gravityy,gravityz;
Cluster *clusters;
Server::Server(Cluster *clu,string gravx, string gravy, string gravz){
	clusters=clu;
	gravityx=gravx;
	gravityy=gravy;
	gravityz=gravz;
	cout<<xsize;
}

void Server::start(int argc, char **argv){

	if(argc>1) {
		if(strcmp(argv[1],"-h")==0) {
			printf("\n");
			printf("usage ./run_server <port>\n");
			exit(0);
		}
	}
	if(argc !=2) {
		printf("missing args\nRun ./run_server -h for help\n");
		exit(1);
	}
	char *port = argv[1];

	long numparticles = 0;
	for(int a = 0; a<clustervalx; a++)
		for(int b = 0; b<clustervaly; b++)
			for(int c =0; c<clustervalz; c++)
				for(unsigned long d = 0; d<clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.size(); d++) {
					numparticles++;
				}
	printf("%ld\n",numparticles );

	int status;
	struct addrinfo hints;
	struct addrinfo *serverinfo;
	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	status = getaddrinfo(NULL, port, &hints,&serverinfo);
	if(status!=0) {
		printf("err %s \n",strerror(errno));
		exit(1);
	}
	int sockfd = socket(serverinfo->ai_family,serverinfo->ai_socktype,serverinfo->ai_protocol);

	int yes=1;
	if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
		perror("setsockopt");
		exit(1);
	}
	bind(sockfd, serverinfo->ai_addr,serverinfo->ai_addrlen);
	listen(sockfd, 5);
	struct sockaddr_storage their_addr;
	socklen_t addr_size;
	addr_size = sizeof(their_addr);
	int success;
	int newfd;
	while(1) {
		printf("waiting...\n");
		fflush(stdout);
		while(1) {
			newfd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
			if(newfd == -1)
				close(newfd);
			else{

				success = send(newfd, &numparticles,4,0);
				if(success == -1) {
					fail((char *)"send length");
				}
				success = send(newfd, &xsize,sizeof(double),0);
				if(success == -1) {
					fail((char *)"send x");
				}
				//printf("%f\n",ysize );
				success = send(newfd, &ysize,sizeof(double),0);
				if(success == -1) {
					fail((char *)"send y");
				}
				success = send(newfd, &zsize,sizeof(double),0);
				if(success == -1) {
					fail((char *)"send z");
				}
				break;
			}

		}
		long frame = 0;
		pthread_t tid1;
		pthread_attr_t attr1;
		pthread_attr_init(&attr1);
		double *data = (double *)malloc(numparticles*3*sizeof(double));
		while(1) {
			long index = 0;
			for(int a = 0; a<clustervalx; a++) {
				for(int b = 0; b<clustervaly; b++) {
					for(int c =0; c<clustervalz; c++) {
						for(unsigned long d = 0; d<clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.size(); d++) {
							data[index] = clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles[d].x;
							data[index+1] = clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles[d].y;
							data[index+2] = clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles[d].z;
							index+=3;
						}
					}
				}
			}
			pthread_create(&tid1,&attr1,physics,NULL);
			for(long a =0; a<numparticles*3; a++) {
				success = send(newfd, &a,sizeof(long),0);
				success = send(newfd, data+a,sizeof(double),0);
			}
			long end_transmission = numparticles*3;
			success = send(newfd, &end_transmission,sizeof(long),0);
			pthread_join(tid1,NULL);
			//gettimeofday(&end, NULL);


			if(success == -1) {
				printf("Transmission closed at %ld frames\n",frame );
				break;
			}
			//gettimeofday(&end, NULL);
			//printf("All done in %lu ms\n",end.tv_usec-begin.tv_usec);

			frame+=1;
		}
		free(data);
		printf("sent data\n");
		fflush(stdout);
	}
	close(newfd);
	close(sockfd);
	//exit(0);
}

void phys(string gravityx, string gravityy,string gravityz, int quad, Cluster *c){
	Cluster *clusters = c;
	//determines which quadrant to do math on
	int xbound=0,ybound=0,zbound=0;
	switch(quad) {
	case 0:
		xbound=0; ybound=0; zbound=0;
		break;
	case 1:
		xbound=clustervalx/2; ybound=0; zbound=0;
		break;
	case 2:
		xbound=0; ybound=clustervaly/2; zbound=0;
		break;
	case 3:
		xbound=0; ybound=0; zbound=clustervalz/2;
		break;
	case 4:
		xbound=clustervalx/2; ybound=clustervaly/2; zbound=0;
		break;
	case 5:
		xbound=clustervalx/2; ybound=0; zbound=clustervalz/2;
		break;
	case 6:
		xbound=0; ybound=clustervaly/2; zbound=clustervalz/2;
		break;
	case 7:
		xbound=clustervalx/2; ybound=clustervaly/2; zbound=clustervalz/2;
		break;
	}

	//these values offset the rounding error from divide by two of clusterval
	int roundoffx=clustervalx%2,roundoffy=clustervaly%2,roundoffz=clustervalz%2;
	if(xbound==0) roundoffx=0;
	if(ybound==0) roundoffy=0;
	if(zbound==0) roundoffz=0;
	for(int a = xbound; a<clustervalx/2+xbound+roundoffx; a++) {
		for(int b = ybound; b<clustervaly/2+ybound+roundoffy; b++) {
			for(int c = zbound; c<clustervalz/2+zbound+roundoffz; c++) {
				for(unsigned long d = 0; d<clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.size(); d++) {
					double mass = clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).mass; //mass of particle whose force is being calculated
					double forcex=0,forcey=0,forcez=0;
					for(int i=a-1; i<=a+1; i++) {
						for(int j=b-1; j<=b+1; j++) {
							for(int k=c-1; k<=c+1; k++) {
								if(i>=0 && j>=0 && k>=0 && i<clustervalx && j<clustervaly && k<clustervalz) {
									for(unsigned long l=0; l<clusters[i*clustervaly*clustervalz+j*clustervalz+k].particles.size(); l++) {
										double disx=clusters[i*clustervaly*clustervalz+j*clustervalz+k].particles.at(l).x-clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).x;
										double disy=clusters[i*clustervaly*clustervalz+j*clustervalz+k].particles.at(l).y-clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).y;
										double disz=clusters[i*clustervaly*clustervalz+j*clustervalz+k].particles.at(l).z-clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).z;
										if(!(disx==0 && disy==0 && disz==0)) {
											double mass2 = clusters[i*clustervaly*clustervalz+j*clustervalz+k].particles.at(l).mass; //mass of other particle
											double dist = sqrt(disx*disx+disy*disy+disz*disz);
											/*if(dist<2) {
												clusters[i*clustervaly*clustervalz+j*clustervalz+k].particles.at(l).mass+=mass;
												//Particle tmp = clusters[i*clustervaly*clustervalz+j*clustervalz+k].particles.at(l);
												clusters[i*clustervaly*clustervalz+j*clustervalz+k].particles[l] = clusters[i*clustervaly*clustervalz+j*clustervalz+k].particles[clusters[i*clustervaly*clustervalz+j*clustervalz+k].particles.size()-1];
												//clusters[i*clustervaly*clustervalz+j*clustervalz+k].particles[clusters[i*clustervaly*clustervalz+j*clustervalz+k].particles.size()-1] = tmp;
												clusters[i*clustervaly*clustervalz+j*clustervalz+k].particles.pop_back();
												break;

											}*/
											double accel = particle_interaction_function(dist,mass2);
											forcex-=accel*mass*disx/dist;
											forcey-=accel*mass*disy/dist;
											forcez-=accel*mass*disz/dist;

										}
									}
								}
							}
						}
					}
					clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).vx+=forcex;
					clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).vy+=forcey;
					clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).vz+=forcez;

					clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).vx+=vfield_x(clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).x,
					                                                                                 clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).y,
					                                                                                 clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).z);

					clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).vy+=vfield_y(clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).x,
					                                                                                 clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).y,
					                                                                                 clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).z);

					clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).vz+=vfield_z(clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).x,
					                                                                                 clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).y,
					                                                                                 clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).z);

					//edge checking if wrapxyz=true, particles loop from one side to another
					//if wrapxyz=false particles bounce off the boundaries
					if(!wrapx) {
						if(clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).x<0)
							clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).vx=abs(clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).vx);
						if(clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).x>=xsize)
							clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).vx=-1*abs(clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).vx);
					}else{
						if(clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).x<0)
							clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).x=xsize;
						if(clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).x>xsize)
							clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).x=0;
					}
					if(!wrapy) {
						if(clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).y<0)
							clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).vy=abs(clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).vy);
						if(clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).y>=ysize)
							clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).vy=
							        -1*abs(clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).vy);
					}else{
						if(clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).y<0)
							clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).y=ysize;
						if(clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).y>ysize)
							clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).y=0;
					}
					if(!wrapz) {
						if(clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).z<0)
							clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).vz=
							        abs(clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).vz);
						if(clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).z>=zsize)
							clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).vz=
							        -1*abs(clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).vz);
					}else{
						if(clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).z<0)
							clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).z=zsize;
						if(clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).z>zsize)
							clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).z=0;
					}
				}
			}
		}
	}
	for(int a = xbound; a<clustervalx/2+xbound+roundoffx; a++) {
		for(int b = ybound; b<clustervaly/2+ybound+roundoffy; b++) {
			for(int c = zbound; c<clustervalz/2+zbound+roundoffz; c++) {
				for(unsigned long d = 0; d<clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.size(); d++) {
					clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).updatelocation();
				}
			}
		}
	}
}
void *physics(void *args){
	for(int a = 0; a<clustervalx; a++) {
		for(int b = 0; b<clustervaly; b++) {
			for(int c =0; c<clustervalz; c++) {
				for(unsigned long d = 0; d<clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.size(); d++) {
					int clusterx = clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).x/xsize*clustervalx;
					int clustery = clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).y/ysize*clustervaly;
					int clusterz = clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.at(d).z/zsize*clustervalz;
					if(clusterx<clustervalx && clustery<clustervaly && clusterz<clustervalz && clusterx>=0 && clustery>=0 && clusterz>=0) {
						if(clusterx!=a ||clustery!=b || clusterz!=c) {
							clusters[clusterx*clustervaly*clustervalz+clustery*clustervalz+clusterz].particles.push_back(clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles[d]);
							Particle tmp = clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles[d];
							clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles[d]=clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles[clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.size()-1];
							clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles[clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.size()-1]=tmp;
							clusters[a*clustervaly*clustervalz+b*clustervalz+c].particles.pop_back();
							d--;
						}
					}
				}
			}
		}
	}



	thread threads[8];
	for(int a = 0; a<8; a++) {
		threads[a]=thread (phys,gravityx,gravityy,gravityz,a,clusters);
	}
	for(int a = 0; a<8; a++) {
		threads[a].join();
	}
	pthread_exit(0);
}



void fail(char *err){
	printf("%s Error %s \n",err,strerror(errno));
	exit(1);
}
