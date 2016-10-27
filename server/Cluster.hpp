#ifndef CLUSTER_H
#define CLUSTER_H
#include <vector>
#include "Particle.hpp"

using namespace std;

class Cluster{
public:
  double bounds[6];
  vector<Particle> particles;
  Cluster();
  Cluster(double x1,double x2, double y1,double y2, double z1,double z2);
};
#endif
