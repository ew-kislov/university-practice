#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>
#include <functional>

#include "vec3.cpp"
#include "ray.cpp"

using namespace std;

struct hit_record {
    vec3 p;
    vec3 normal;
    double t;
    bool front_face;

    inline void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal :-outward_normal;
    }
};

class Object {
    public:
        Object(vec3 mColor, vec3 mEmission, function<bool(const ray&, hit_record&, vec3&, ray&)> mScatterFunction = nullptr): color(mColor), emission(mEmission), scatterFunction(mScatterFunction) {};

        vec3 getColor() { return this->color; }
        vec3 getEmission() { return this->emission; }

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
        virtual string name() = 0;
        function<bool(const ray&, hit_record&, vec3&, ray&)> getScatterFunction() { return this->scatterFunction; }

    private:
        vec3 color;
        vec3 emission;
        function<bool(const ray&, hit_record&, vec3&, ray&)> scatterFunction;
};

#endif