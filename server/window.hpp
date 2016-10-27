#ifndef WINDOW_H
#define WINDOW_H
#include <stdio.h>
#include <stdlib.h>
#include "Cluster.hpp"
#include <vector>
#include "Particle.hpp"
#include "globals.hpp"
#include <string>
//#define CLUSTERVAL 40

using namespace std;
class Window
{
 public:
  void start(int argc, char **argv);
  void update(vector<Particle> &p);
  Window(Cluster *clu, string gravx, string gravy, string gravz);

};
#endif
