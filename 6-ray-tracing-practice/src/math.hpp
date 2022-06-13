#ifndef MATH_H
#define MATH_H

class vec3;

#include <random>

const double pi = 3.1415926535897932385;

inline double degrees_to_radians(double degrees);

inline double random_double();

inline double clamp(double x, double min, double max);

inline double random_double(double min, double max);

vec3 random_in_unit_disk();

#endif