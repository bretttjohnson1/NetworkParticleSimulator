#include <stdio.h>
#include <math.h>
#include <stdlib.h>

double particle_interaction_function(double dist,double mass){

   dist = dist/2;
   //return -.001;
   //return -mass/(dist*dist+5);
   return mass*(2/(dist*dist+4)-2/((dist-2)*(dist-2)+8));
}

double vfield_x(double x, double y, double z){
   return 0;
}
double vfield_y(double x, double y, double z){
   //return -.001;  //this makes downward gravity
   return 0;
}
double vfield_z(double x, double y, double z){
   return 0;
}

double mass_function(double x, double y, double z){
   return pow(2,-((double)(rand()%5)))+1;
   //return 1;
}
