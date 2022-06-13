#ifndef PLANE_H
#define PLANE_H

#include <iostream>

#include "Object.cpp"
#include "vec3.cpp"

using namespace std;

class Plane : public Object {
    public:
        Plane(vec3 mNormal, vec3 mPoint, double mWidth, double mHeight, vec3 color, vec3 emission, function<bool(const ray&, hit_record&, vec3&, ray&)> mScatterFunction = nullptr) : Object(color, emission, mScatterFunction), normal(mNormal), point(mPoint), width(mWidth), height(mHeight) {};

        virtual bool hit(
            const ray& r, double t_min, double t_max, hit_record& rec) const override;

        string name() { return "Plane"; }

    public:
        vec3 normal;
        vec3 point;
        double width;
        double height;
};

bool Plane::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 diff = r.orig - this->point;
    double prod1 = dot(diff, this->normal);
    double prod2 = dot(r.dir, this->normal);

    if (fabs(prod2) < 0.000001) {
        return false;
    }

    double prod3 = prod1 / prod2;

    if (fabs(prod3) < 0.000001) {
        return false;
    }

    vec3 p = r.orig - r.dir * prod3;
    double t = (p - r.orig).length();

    if (t < t_min || t > t_max) {
        return false;
    }

    vec3 temp_vec;
    if (this->normal.x() == 0 && this->normal.y() == 0 && this->normal.z() == 1) {
        temp_vec = vec3(0, 1, 0);
    } else {
        temp_vec = vec3(0, 0, 1);
    }

    vec3 pos_diff = this->point - p;

    vec3 width_basis = normalize(cross(this->normal, temp_vec));
    vec3 height_basis = normalize(cross(width_basis, this->normal));

    vec3 width_proj = dot(width_basis, pos_diff) * width_basis;
    vec3 height_proj = dot(height_basis, pos_diff) * height_basis;

    double width_proj_metric = width_proj.x() != 0 ? width_proj.x() : width_proj.y() != 0 ? width_proj.y() : width_proj.z();
    double height_proj_metric = height_proj.x() != 0 ? height_proj.x() : height_proj.y() != 0 ? height_proj.y() : height_proj.z();
    double width_basis_metric = width_basis.x() != 0 ? width_basis.x() : width_basis.y() != 0 ? width_basis.y() : width_basis.z();
    double height_basis_metric = height_basis.x() != 0 ? height_basis.x() : height_basis.y() != 0 ? height_basis.y() : height_basis.z();

    if (abs(width_proj_metric / width_basis_metric) > width) {
        return false;
    } 

    if (abs(height_proj_metric / height_basis_metric) > height) {
        return false;
    }

    
    rec.p = r.orig - r.dir * prod3;
    rec.set_face_normal(r, this->normal);
    rec.t = (rec.p - r.orig).length();

    // cerr << rec.p << endl;

    return true;
}

#endif