#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Object.cpp"
#include "vec3.cpp"

class Triangle : public Object {
    public:
        Triangle(vec3 mVert1, vec3 mVert2, vec3 mVert3, vec3 color, vec3 emission, function<bool(const ray&, hit_record&, vec3&, ray&)> mScatterFunction = nullptr) : Object(color, emission, mScatterFunction), vert1(mVert1), vert2(mVert2), vert3(mVert3) {};

        virtual bool hit(
            const ray& r, double t_min, double t_max, hit_record& rec) const override;

        string name() { return "Triangle"; }

    public:
        vec3 vert1;
        vec3 vert2;
        vec3 vert3;
};

bool Triangle::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 e1 = vert2 - vert1;
    vec3 e2 = vert3 - vert1;

    vec3 pvec = cross(r.dir, e2);
    float det = dot(e1, pvec);

    if (det < 1e-8 && det > -1e-8) {
        return false;
    }

    float inv_det = 1 / det;
    vec3 tvec = r.orig - vert1;
    float u = dot(tvec, pvec) * inv_det;
    if (u < 0 || u > 1) {
        return false;
    }

    vec3 qvec = cross(tvec, e1);
    float v = dot(r.dir, qvec) * inv_det;
    if (v < 0 || u + v > 1) {
        return false;
    }

    float dist = dot(e2, qvec) * inv_det;

    if (dist < t_min || t_max < dist) {
        return false;
    }

    vec3 b1 = vert3 - vert1;
    vec3 b2 = vert2 - vert1;

    rec.t = dist;
    rec.p = r.at(rec.t);
    rec.set_face_normal(r, normalize(cross(b1, b2)));
        
    return true;
}

#endif