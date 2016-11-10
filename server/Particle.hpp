//DFADSF
#ifndef PARTICLE_H
#define PARTICLE_H


class Particle{
public:
  double x,y,z;
  double vx,vy,vz;
  double mass;
  Particle(double x, double y, double z, double vx, double vy, double vz,double mass);
  void updatelocation();
  void setvelocity(double vx,double vy,double vz);
  void addforce(double fx, double fy, double fz);
};

#endif
