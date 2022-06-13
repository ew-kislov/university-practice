#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

#include "math.hpp"

using std::sqrt;

class vec3 {
    public:
        vec3();
        vec3(double e0, double e1, double e2);

        double x() const;
        double y() const;
        double z() const;

        void setX(double x);
        void setY(double y);
        void setZ(double z);

        vec3 operator-() const;
        double operator[](int i) const;
        double& operator[](int i);

        vec3& operator+=(const vec3 &v);

        vec3& operator*=(const double t);

        vec3& operator/=(const double t);

        double length() const;

        double length_squared() const;

        inline static vec3 random();

        inline static vec3 random(double min, double max);

    public:
        double e[3];
};

#endif