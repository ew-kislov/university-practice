#ifndef SPHERE_H
#define SPHERE_H

#include "Object.cpp"
#include "vec3.cpp"

class sphere : public Object {
    public:
        sphere(vec3 cen, double r, vec3 color, vec3 emission, function<bool(const ray&, hit_record&, vec3&, ray&)> mScatterFunction = nullptr) : Object(color, emission, mScatterFunction), center(cen), radius(r) {};

        virtual bool hit(
            const ray& r, double t_min, double t_max, hit_record& rec) const override;

        string name() { return "Sphere"; }

    public:
        vec3 center;
        double radius;
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius*radius;

    auto discriminant = half_b*half_b - a*c;
    if (discriminant < 0) return false;
    auto sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);

    return true;
}

#endif