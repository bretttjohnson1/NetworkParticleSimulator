#include <iostream>
#include "Particle.hpp"
#include "globals.hpp"
using namespace std;


Particle::Particle(double x, double y, double z, double vx, double vy, double vz, double mass){
	this->x=x;
	this->y=y;
	this->z=z;
	this->vx=vx;
	this->vy=vy;
	this->vz=vz;
   this->mass=mass;
}
void Particle::updatelocation(){
	x+=vx*speed;
	y+=vy*speed;
	z+=vz*speed;
}
void Particle::setvelocity(double vx,double vy,double vz){
	this->vx=vx;
	this->vy=vy;
	this->vz=vz;
}
void Particle::addforce(double fx, double fy, double fz){
	vx+=fx;
	vy+=fy;
	vz+=fz;

}
