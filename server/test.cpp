#include <iostream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include <muParser.h>
//#include <muParserBase.h>
#include "Particle.hpp"
#include "Cluster.hpp"
#include "window.hpp"
#include <vector>
#include <math.h>
#include <fstream>
#include "globals.hpp"
using namespace std;
//using namespace mu;
double xsize,ysize,zsize;
int particles;
int clustervalx=10;
int clustervaly=10;
int clustervalz=10;
bool wrapx=false,
  wrapy=false,
  wrapz=false;
double speed;
double mrand(){
  return ((double)rand())/(double)RAND_MAX-.5;

}
int main(int args, char **args1){
  string ifieldx="rand()/5";
  string ifieldy="rand()/5";
  string ifieldz="rand()/5";
  string vfieldx="0";
  string vfieldy="0";
  string vfieldz="0";
  xsize=1000;
  ysize=1000;
  zsize=1000;
  particles=64000;
  ifstream config("config.txt");
  string line;
  if(config.good()){
    if(config.is_open()){
      while(getline(config,line)){
	if(line.substr(0,8).compare("IFIELDX:")==0)
	  ifieldx=line.substr(8,line.length());
	if(line.substr(0,8).compare("IFIELDY:")==0)
	  ifieldy=line.substr(8,line.length());
	if(line.substr(0,8).compare("IFIELDZ:")==0)
	  ifieldz=line.substr(8,line.length());
	if(line.substr(0,8).compare("VFIELDX:")==0)
	  vfieldx=line.substr(8,line.length());
	if(line.substr(0,8).compare("VFIELDY:")==0)
	  vfieldy=line.substr(8,line.length());
	if(line.substr(0,8).compare("VFIELDZ:")==0)
	  vfieldz=line.substr(8,line.length());
	if(line.substr(0,6).compare("XSIZE:")==0)
	  xsize=atoi(line.substr(6,line.length()).data());
	if(line.substr(0,6).compare("YSIZE:")==0)
	  ysize=atoi(line.substr(6,line.length()).data());
	if(line.substr(0,6).compare("ZSIZE:")==0)
	  zsize=atoi(line.substr(6,line.length()).data());
	if(line.substr(0,10).compare("PARTICLES:")==0)
	  particles=atoi(line.substr(10,line.length()).data());
	if(line.substr(0,6).compare("SPEED:")==0)
	  speed=atof(line.substr(6,line.length()).data());
	if(line.substr(0,6).compare("WRAPX:")==0)
	  wrapx=(line.substr(6,line.length()).compare("true")==0);
	if(line.substr(0,6).compare("WRAPY:")==0)
	  wrapy=line.substr(6,line.length()).compare("true")==0;
	if(line.substr(0,6).compare("WRAPZ:")==0)
	  wrapy=line.substr(6,line.length()).compare("true")==0;
      }
    }
  }
  else{
    config.close();
    ofstream config("config.txt");
    config<<"#IFIELD is the initial vector field, leave as rand for random values or write functions of x and y\n";
    config<<"#VFIELD is the working vector field, it is the same as IFIELD, but it increments the x,y,z velocities by its function value every tick. basically acceleration field\n";
    config<<"#SIZE is the dimensions of the rectangular prism in x,y,z that houses the points\n";
    config<<"#PARTICLES is the number of particles\n";
    config<<"#SPEED is the field that determines how fast the simulation runs, higher speed reduces accuracy\n";
    config<<"IFIELDX:rand()/5\n";
    config<<"IFIELDY:rand()/5\n";
    config<<"IFIELDY:rand()/5\n";
    config<<"VFIELDX:0\n";
    config<<"VFIELDY:0\n";
    config<<"VFIELDZ:0\n";
    config<<"XSIZE:100\n";
    config<<"YSIZE:100\n";
    config<<"ZSIZE:100\n";
    config<<"PARTICLES:64000\n";
    config<<"SPEED:1\n";
    config<<"WRAPX:false\n";
    config<<"WRAPY:false\n";
    config<<"WRAPZ:false\n";
    config<<"END";
  }
  //  cout<<particles<<" "<<xsize<<"\n";
  clustervalx =(int)(pow(particles,1/3.0)+1);
  clustervaly =(int)(pow(particles,1/3.0)+1);
  clustervalz =(int)(pow(particles,1/3.0)+1);
  //  cout<<vfieldx<<" "<<vfieldy;
  Cluster *clusters;
  clusters = new Cluster[clustervalx*clustervaly*clustervalz];
  srand(time(0));
  double x,y,z;
  /*Parser initx;
  Parser inity;
  Parser initz;
  //cout<<ifieldx<<" "<<ifieldy<<" "<<ifieldz<<"\n";
  initx.SetExpr(ifieldx);
  inity.SetExpr(ifieldy);
  initz.SetExpr(ifieldz);

  initx.ParserBase::DefineVar("x",&x);
  initx.ParserBase::DefineVar("y",&y);
  initx.ParserBase::DefineVar("z",&z);

  inity.ParserBase::DefineVar("x",&x);
  inity.ParserBase::DefineVar("y",&y);
  inity.ParserBase::DefineVar("z",&z);

  initz.ParserBase::DefineVar("x",&x);
  initz.ParserBase::DefineVar("y",&y);
  initz.ParserBase::DefineVar("z",&z);

  initx.DefineFun("rand",mrand);
  inity.DefineFun("rand",mrand);
  initz.DefineFun("rand",mrand);
  */
   srand(time(NULL));
  for(int a = 0; a<clustervalx;a++){
    for(int b = 0;b<clustervaly;b++){
      for(int c =0;c<clustervalz;c++){
	clusters[a*clustervaly*clustervalz+b*clustervalz+c]= Cluster(a*xsize/clustervalx,b*ysize/clustervaly,c*zsize/clustervalz,a*xsize/clustervalx+xsize/clustervalx,b*ysize/clustervaly+ysize/clustervalz,c*zsize/clustervalz+zsize/clustervalz);
	for(int d = 0;d<1;d++){
	  x=((double)a)*xsize/((double)clustervalx)+((double)rand())/((double)RAND_MAX)*xsize/((double)clustervalx)-xsize/2;
	  y=((double)b)*ysize/((double)clustervaly)+((double)rand())/((double)RAND_MAX)*ysize/((double)clustervaly)-ysize/2;
	  z=((double)c)*zsize/((double)clustervalz)+((double)rand())/((double)RAND_MAX)*zsize/((double)clustervalx)-zsize/2;
	  clusters[a*clustervaly*clustervalz+b*clustervalz+c].

	    particles.push_back(Particle(x+xsize/2,y+ysize/2,z+zsize/2
					 ,0
					 ,0
					 ,0));
	}
      }
    }
  }

  Window w (clusters,vfieldx,vfieldy,vfieldz);
  w.start(args, args1);
}
