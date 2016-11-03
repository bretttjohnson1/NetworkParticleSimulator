#include <stdio.h>

double particle_interaction_function(int dist){
   return (2/(dist*dist+4)-2/((dist-2)*(dist-2)+8));
}

double vfield_x(double x, double y, double z){
   return 0;
}
double vfield_y(double x, double y, double z){
   return 0;
}
double vfield_z(double x, double y, double z){
   return 0;
}
