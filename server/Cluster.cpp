#include "Particle.hpp"
#include "Cluster.hpp"
#include <vector>
using namespace std;


Cluster::Cluster(){}
Cluster::Cluster(double x1,double x2, double y1,double y2, double z1,double z2){
    bounds[0] = x1;
    bounds[1] = x2;
    bounds[2] = y1;
    bounds[3] = y2;
    bounds[4] = z1;
    bounds[5] = z2;
  }
