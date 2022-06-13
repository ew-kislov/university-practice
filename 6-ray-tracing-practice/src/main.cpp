#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>

#include "vec3.cpp"
#include "ray.cpp"
#include "math.cpp"
#include "camera.cpp"
#include "Object.cpp"
#include "sphere.cpp"
#include "Plane.cpp"
#include "Triangle.cpp"

using namespace std;

Object* hit(const ray& r, double t_min, double t_max, vector<Object*> objects, hit_record& rec) {
    hit_record temp_rec;
    bool hit_anything = false;
    auto closest_so_far = t_max;

    Object* obj = nullptr;

    for (Object* object : objects) {
        if (object->hit(r, t_min, closest_so_far, temp_rec)) {
            obj = object;
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    return obj;
}

double reflectance(double cosine, double ref_idx) {
    // Use Schlick's approximation for reflectance.
    auto r0 = (1-ref_idx) / (1+ref_idx);
    r0 = r0*r0;
    return r0 + (1-r0)*pow((1 - cosine),5);
}

bool scatter(const ray& r_in, hit_record& rec, vec3& attenuation, ray& scattered) {
    // attenuation = color(1.0, 1.0, 1.0);
    double ir = 1.5;
    double refraction_ratio = rec.front_face ? (1.0/ir) : ir;

    vec3 unit_direction = unit_vector(r_in.direction());
    double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
    double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

    bool cannot_refract = refraction_ratio * sin_theta > 1.0;
    vec3 direction;
    if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
        direction = reflect(unit_direction, rec.normal);
    else
        direction = refract(unit_direction, rec.normal, refraction_ratio);

    scattered = ray(rec.p, direction);
    return true;
}

vec3 write_color(vec3 pixel_color, int samples_per_pixel) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    auto scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r / 255) * 255;
    g = sqrt(scale * g / 255) * 255;
    b = sqrt(scale * b / 255) * 255;

    return vec3(r, g, b);
}

vec3 ray_color(const ray& r, vector<Object*> objects, int depth) {
    Object* obj;

    hit_record rec;

    if ((obj = hit(r, 1e-15, numeric_limits<double>::infinity(), objects, rec)) != nullptr) {
        if (--depth <= 0) {
            return obj->getEmission();
        }

        vec3 target = rec.p + random_in_hemisphere(rec.normal);

        ray new_ray = ray(rec.p, target - rec.p);

        const float p = 1 / (2 * pi);
        float cos_theta = dot(new_ray.dir, rec.normal);

        function<bool(const ray&, hit_record&, vec3&, ray&)> scatterFunction = obj->getScatterFunction();
        if (scatterFunction != nullptr) {
            vec3 color = obj->getColor();
            ray new_ray;

            scatterFunction(r, rec, color, new_ray);

            vec3 incoming = ray_color(new_ray, objects, depth);

            return obj->getEmission() + vec3(255, 255, 255) * incoming * p * cos_theta / pi;
        }


        vec3 incoming = ray_color(new_ray, objects, depth);
        return obj->getEmission() + obj->getColor() * incoming * p * cos_theta / pi;
    }

    return vec3(0, 0, 0);
}

int main() {

    // Image

    const auto aspect_ratio = 1;
    const int image_width = 256;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 1000;

    const int max_depth = 5;

    // Camera

    vec3 lookfrom(-10, 5, 10);
    vec3 lookat(1, 3.5, 0);

    vec3 vup(0,1,0);

    camera cam(lookfrom, lookat, vup, 20, aspect_ratio);

    // World

    vector<Object*> objects;
    objects.push_back(new Plane(vec3(0, 1, 0), vec3(0, 0, 0), 10, 10, vec3(99, 55, 24), vec3(0, 0, 0))); // bottom
    objects.push_back(new Plane(vec3(0, 0, 1), vec3(0, 10, -10), 10, 10, vec3(178, 60, 63), vec3(0, 0, 0))); // back
    objects.push_back(new Plane(vec3(0, 0, 1), vec3(0, 10, 14), 10, 10, vec3(50, 50, 50), vec3(0, 0, 0))); // front
    objects.push_back(new Plane(vec3(0, -1, 0), vec3(0, 20, 0), 10, 10, vec3(255, 0, 0), vec3(0, 0, 0))); // top
    objects.push_back(new Plane(vec3(1, 0, 0), vec3(-13, 10, 0), 10, 10, vec3(120, 120, 120), vec3(0, 0, 0))); // left
    objects.push_back(new Plane(vec3(-1, 0, 0), vec3(10, 10, 0), 10, 10, vec3(120, 120, 120), vec3(0, 0, 0))); // right

    objects.push_back(new sphere(vec3(2,6,2.8), 0.5, vec3(255, 255, 255), vec3(255, 255, 255))); // light
    objects.push_back(new sphere(vec3(-4, 7, 2), 0.5, vec3(255, 255, 255), vec3(255, 255, 255))); // light


    objects.push_back(new sphere(vec3(0, 1.4, 4), 1.4, vec3(95, 37, 153), vec3(0, 0, 0))); 
    objects.push_back(new sphere(vec3(0, 3.6, 4), 0.8, vec3(95, 37, 153), vec3(0, 0, 0), scatter)); 

    // Plain below

    objects.push_back(new Triangle(vec3(-2, 1, -2), vec3(2, 1 , 2), vec3(2, 1, -2), vec3(163, 163, 163), vec3(0, 0, 0)));
    objects.push_back(new Triangle(vec3(-2, 1, -2), vec3(2, 1, 2), vec3(-2, 1 , 2), vec3(163, 163, 163), vec3(0, 0, 0)));

    objects.push_back(new Triangle(vec3(-2, 1, -2), vec3(-2, 1, 2), vec3(-2, 0, -2), vec3(163, 163, 163), vec3(0, 0, 0)));
    objects.push_back(new Triangle(vec3(-2, 0, -2), vec3(-2, 1, 2), vec3(-2, 0, 2), vec3(163, 163, 163), vec3(0, 0, 0)));

    objects.push_back(new Triangle(vec3(-2, 1, 2), vec3(2, 1, 2), vec3(2, 0, 2), vec3(163, 163, 163), vec3(0, 0, 0)));
    objects.push_back(new Triangle(vec3(-2, 0, 2), vec3(-2, 1, 2), vec3(2, 0, 2), vec3(163, 163, 163), vec3(0, 0, 0)));

    objects.push_back(new Triangle(vec3(-2, 1, -2), vec3(2, 1, -2), vec3(2, 0, -2), vec3(163, 163, 163), vec3(0, 0, 0)));
    objects.push_back(new Triangle(vec3(-2, 0, -2), vec3(-2, 1, -2), vec3(2, 0, -2), vec3(163, 163, 163), vec3(0, 0, 0)));

    // Ikosaeder

    const float tx = .525731112119133606 * 3;
    const float tz = .850650808352039932 * 3;
    const float ty = 4;

    objects.push_back(new Triangle(vec3(-tx, 0.0 + ty, tz), vec3(0.0, tz + ty, tx), vec3(tx, 0.0 + ty, tz), vec3(255, 0, 0), vec3(0, 0, 0), scatter));
    objects.push_back(new Triangle(vec3(-tx, 0.0 + ty, tz), vec3(-tz, tx + ty, 0), vec3(0.0, tz + ty, tx), vec3(255, 0, 0), vec3(0, 0, 0), scatter));
    objects.push_back(new Triangle(vec3(-tz, tx + ty, 0), vec3(0.0, tz + ty, -tx), vec3(0.0, tz + ty, tx), vec3(255, 0, 0), vec3(0, 0, 0), scatter));
    objects.push_back(new Triangle(vec3(0.0, tz + ty, tx), vec3(0.0, tz + ty, -tx), vec3(tz, tx + ty, 0.0), vec3(255, 0, 0), vec3(0, 0, 0), scatter));
    objects.push_back(new Triangle(vec3(0.0, tz + ty, tx), vec3(tz, tx + ty, 0.0), vec3(tx, 0.0 + ty, tz), vec3(255, 0, 0), vec3(0, 0, 0), scatter));

    objects.push_back(new Triangle(vec3(tz, tx + ty, 0.0), vec3(tz, -tx + ty, 0.0), vec3(tx, 0.0 + ty, tz), vec3(255, 0, 0), vec3(0, 0, 0), scatter));
    objects.push_back(new Triangle(vec3(tz, tx + ty, 0.0), vec3(tx, 0.0 + ty, -tz), vec3(tz, -tx + ty, 0.0), vec3(255, 0, 0), vec3(0, 0, 0), scatter));
    objects.push_back(new Triangle(vec3(0.0, tz + ty, -tx), vec3(tx, 0.0 + ty, -tz), vec3(tz, tx + ty, 0.0), vec3(255, 0, 0), vec3(0, 0, 0), scatter));
    objects.push_back(new Triangle(vec3(0.0, tz + ty, -tx), vec3(-tx, 0.0 + ty, -tz), vec3(tx, 0.0 + ty, -tz), vec3(255, 0, 0), vec3(0, 0, 0), scatter));
    objects.push_back(new Triangle(vec3(-tx, 0.0 + ty, -tz), vec3(0.0, -tz + ty, -tx), vec3(tx, 0.0 + ty, -tz), vec3(255, 0, 0), vec3(0, 0, 0), scatter));

    objects.push_back(new Triangle(vec3(0.0, -tz + ty, -tx), vec3(tz, -tx + ty, 0.0), vec3(tx, 0.0 + ty, -tz), vec3(255, 0, 0), vec3(0, 0, 0), scatter));
    objects.push_back(new Triangle(vec3(0.0, -tz + ty, -tx), vec3(0, -tz + ty, tx), vec3(tz, -tx + ty, 0.0), vec3(255, 0, 0), vec3(0, 0, 0), scatter));
    objects.push_back(new Triangle(vec3(0.0, -tz + ty, -tx), vec3(-tz, -tx + ty, 0.0), vec3(0, -tz + ty, tx), vec3(255, 0, 0), vec3(0, 0, 0), scatter));
    objects.push_back(new Triangle(vec3(-tz, -tx + ty, 0.0), vec3(-tx, 0.0 + ty, tz), vec3(0, -tz + ty, tx), vec3(255, 0, 0), vec3(0, 0, 0), scatter));
    objects.push_back(new Triangle(vec3(-tx, 0.0 + ty, tz), vec3(tx, 0.0 + ty, tz), vec3(0, -tz + ty, tx), vec3(255, 0, 0), vec3(0, 0, 0), scatter));

    objects.push_back(new Triangle(vec3(0, -tz + ty, tx), vec3(tx, 0.0 + ty, tz), vec3(tz, -tx + ty, 0.0), vec3(255, 0, 0), vec3(0, 0, 0), scatter));
    objects.push_back(new Triangle(vec3(-tz, tx + ty, 0), vec3(-tx, 0.0 + ty, tz), vec3(-tz, -tx + ty, 0.0), vec3(255, 0, 0), vec3(0, 0, 0), scatter));
    objects.push_back(new Triangle(vec3(-tz, tx + ty, 0), vec3(-tz, -tx + ty, 0.0), vec3(-tx, 0.0 + ty, -tz), vec3(255, 0, 0), vec3(0, 0, 0), scatter));
    objects.push_back(new Triangle(vec3(-tz, tx + ty, 0), vec3(-tx, 0.0 + ty, -tz), vec3(0.0, tz + ty, -tx), vec3(255, 0, 0), vec3(0, 0, 0), scatter));
    objects.push_back(new Triangle(vec3(0.0, -tz + ty, -tx), vec3(-tx, 0.0 + ty, -tz), vec3(-tz, -tx + ty, 0.0), vec3(255, 0, 0), vec3(0, 0, 0), scatter));

    // FIRE

    objects.push_back(new sphere(vec3(0, 3.7, 0), 0.4, vec3(250, 192, 0), vec3(5, 4, 0))); // inner sphere

    objects.push_back(new sphere(vec3(0, 3.7, 0.4), 0.2, vec3(255, 117, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0, 3.7, -0.4), 0.2, vec3(255, 117, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.4, 3.7, 0), 0.2, vec3(255, 117, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(-0.4, 3.7, 0), 0.2, vec3(255, 117, 0), vec3(0, 0, 0))); // inner sphere

    objects.push_back(new sphere(vec3(0, 3.4, 0.0), 0.15, vec3(215, 53, 2), vec3(0, 0, 0))); // inner sphere

    objects.push_back(new sphere(vec3(-0.4, 3.7, 0.4), 0.15, vec3(215, 53, 2), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(-0.4, 3.7, 0.4), 0.15, vec3(215, 53, 2), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.4, 3.7, -0.4), 0.15, vec3(215, 53, 2), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.4, 3.7, 0.4), 0.15, vec3(215, 53, 2), vec3(0, 0, 0))); // inner sphere

    objects.push_back(new sphere(vec3(0, 3.8, 0.5), 0.15, vec3(215, 53, 2), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0, 3.8, -0.5), 0.15, vec3(215, 53, 2), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.5, 3.8, 0), 0.15, vec3(215, 53, 2), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(-0.5, 3.8, 0), 0.15, vec3(215, 53, 2), vec3(0, 0, 0))); // inner sphere

    objects.push_back(new sphere(vec3(0, 3.9, 0.6), 0.15, vec3(215, 53, 2), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0, 3.9, -0.6), 0.15, vec3(215, 53, 2), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.6, 3.9, 0), 0.15, vec3(215, 53, 2), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(-0.6, 3.9, 0), 0.15, vec3(215, 53, 2), vec3(0, 0, 0))); // inner sphere

    objects.push_back(new sphere(vec3(0, 3.5, 0.3), 0.15, vec3(215, 53, 2), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0, 3.5, -0.3), 0.15, vec3(215, 53, 2), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.3, 3.5, 0), 0.15, vec3(215, 53, 2), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(-0.3, 3.5, 0), 0.15, vec3(215, 53, 2), vec3(0, 0, 0))); // inner sphere

    objects.push_back(new sphere(vec3(-0.5, 3.9, -0.5), 0.1, vec3(182, 34, 3), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(-0.5, 3.9, 0.5), 0.1, vec3(182, 34, 3), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.5, 3.9, -0.5), 0.1, vec3(182, 34, 3), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.5, 3.9, 0.5), 0.1, vec3(182, 34, 3), vec3(0, 0, 0))); // inner sphere

    objects.push_back(new sphere(vec3(-0.3, 3.9, -0.6), 0.1, vec3(182, 34, 3), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(-0.3, 3.9, 0.6), 0.1, vec3(182, 34, 3), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.3, 3.9, -0.6), 0.1, vec3(182, 34, 3), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.3, 3.9, 0.6), 0.1, vec3(182, 34, 3), vec3(0, 0, 0))); // inner sphere

    objects.push_back(new sphere(vec3(-0.6, 3.9, -0.3), 0.1, vec3(182, 34, 3), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(-0.6, 3.9, 0.3), 0.1, vec3(182, 34, 3), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.6, 3.9, -0.3), 0.1, vec3(182, 34, 3), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.6, 3.9, 0.3), 0.1, vec3(182, 34, 3), vec3(0, 0, 0))); // inner sphere

    objects.push_back(new sphere(vec3(0, 4.1, -0.5), 0.1, vec3(182, 34, 3), vec3(3, 1, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0, 4.1, 0.5), 0.1, vec3(182, 34, 3), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(-0.5, 4.1, 0), 0.1, vec3(182, 34, 3), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.5, 4.1, 0), 0.1, vec3(182, 34, 3), vec3(0, 0, 0))); // inner sphere

    objects.push_back(new sphere(vec3(-0.5, 4.1, -0.5), 0.1, vec3(128, 17, 0), vec3(3, 1, 0))); // inner sphere
    objects.push_back(new sphere(vec3(-0.5, 4.1, 0.5), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.5, 4.1, -0.5), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.5, 4.1, 0.5), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere

    objects.push_back(new sphere(vec3(-0.55, 4.3, -0.55), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(-0.55, 4.3, 0.55), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.55, 4.3, -0.55), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.55, 4.3, 0.55), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere

    objects.push_back(new sphere(vec3(-0.65, 4.1, -0.65), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(-0.65, 4.1, 0.65), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.65, 4.1, -0.65), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.65, 4.1, 0.65), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere

    objects.push_back(new sphere(vec3(-0.7, 4.3, -0.7), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(-0.7, 4.3, 0.7), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.7, 4.3, -0.7), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.7, 4.3, 0.7), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere

    objects.push_back(new sphere(vec3(-0.45, 3.9, -0.45), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(-0.45, 3.9, 0.45), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.45, 3.9, -0.45), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.45, 3.9, 0.45), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere

    objects.push_back(new sphere(vec3(-0.6, 3.9, -0.6), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(-0.6, 3.9, 0.6), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.6, 3.9, -0.6), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.6, 3.9, 0.6), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere

    objects.push_back(new sphere(vec3(-0.35, 3.7, -0.35), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(-0.35, 3.7, 0.35), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.35, 3.7, -0.35), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.35, 3.7, 0.35), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere

    objects.push_back(new sphere(vec3(-0.5, 3.7, -0.5), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(-0.5, 3.7, 0.5), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.5, 3.7, -0.5), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.5, 3.7, 0.5), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere

    objects.push_back(new sphere(vec3(-0.25, 3.5, -0.25), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(-0.25, 3.5, 0.25), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.25, 3.5, -0.25), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.25, 3.5, 0.25), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere

    objects.push_back(new sphere(vec3(-0.4, 3.5, -0.4), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(-0.4, 3.5, 0.4), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.4, 3.5, -0.4), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.4, 3.5, 0.4), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere

    // 

    objects.push_back(new sphere(vec3(0, 2.9, -0.45), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0, 2.9, 0.45), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.45, 2.9, 0), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(-0.45, 2.9, 0), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere

    objects.push_back(new sphere(vec3(0, 2.9, -0.6), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0, 2.9, 0.6), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(-0.6, 2.9, 0), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.6, 2.9, 0), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere

    objects.push_back(new sphere(vec3(0, 2.7, -0.35), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0, 2.7, 0.35), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(-0.35, 2.7, 0), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.35, 2.7, 0), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere

    objects.push_back(new sphere(vec3(0, 2.7, -0.5), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0, 2.7, 0.5), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(-0.5, 2.7, 0), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.5, 2.7, 0), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere

    objects.push_back(new sphere(vec3(0, 2.5, -0.25), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0, 2.5, 0.25), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(-0.25, 2.5, 0), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.25, 2.5, 0), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere

    objects.push_back(new sphere(vec3(0, 2.5, -0.4), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0, 2.5, 0.4), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(-0.4, 2.5, 0), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.4, 2.5, 0), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere

    //

    objects.push_back(new sphere(vec3(0.2, 3.1, -0.2), 0.03, vec3(215, 53, 2), vec3(5, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(-0.2, 3.1, -0.2), 0.15, vec3(215, 53, 2), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(-0.2, 3.1, 0.2), 0.15, vec3(215, 53, 2), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.2, 3.1, 0.2), 0.15, vec3(215, 53, 2), vec3(0, 0, 0))); // inner sphere

    objects.push_back(new sphere(vec3(0.3, 3.3, -0.3), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(-0.3, 3.3, -0.3), 0.03, vec3(128, 17, 0), vec3(5, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(-0.3, 3.3, 0.3), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0.3, 3.3, 0.3), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere

    //

    objects.push_back(new sphere(vec3(0.2, 3.1, 0), 0.03, vec3(215, 53, 2), vec3(5, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(-0.2, 3.1, 0), 0.15, vec3(215, 53, 2), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0, 3.1, -0.2), 0.15, vec3(215, 53, 2), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0, 3.1, 0.2), 0.15, vec3(215, 53, 2), vec3(0, 0, 0))); // inner sphere

    objects.push_back(new sphere(vec3(0.3, 3.3, 0), 0.03, vec3(128, 17, 0), vec3(5, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(-0.3, 3.3, 0), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0, 3.3, -0.3), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere
    objects.push_back(new sphere(vec3(0, 3.3, 0.3), 0.1, vec3(128, 17, 0), vec3(0, 0, 0))); // inner sphere 
    
    // Render

    vector<vec3> colors = vector<vec3>(image_height * image_width);

    vec3 pixel_color;
    int j, i;
    #pragma omp parallel for schedule(dynamic, 1) private(pixel_color, i, j)
    for (j = image_height-1; j >= 0; --j) {
        cout << "\rScanlines remaining: " << j << ' ' << flush;
        for (i = 0; i < image_width; ++i) {
            pixel_color = vec3(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (image_width-1);
                auto v = (j + random_double()) / (image_height-1);
                ray r = cam.get_ray(u, v);
                vec3 sample_color = ray_color(r, objects, max_depth);
                pixel_color += sample_color;
            }

            colors[(image_height - 1 - j) * image_width + i] = write_color(pixel_color, samples_per_pixel);
        }
    }

    FILE *f = fopen("image.ppm", "w");
    fprintf(f, "P3\n%d %d\n%d\n", image_width, image_height, 255);
    for (int i = 0; i < colors.size(); i++)
        fprintf(f, "%d %d %d ", static_cast<int>(clamp(colors[i].x(), 0, 255)), static_cast<int>(clamp(colors[i].y(), 0, 255)), static_cast<int>(clamp(colors[i].z(), 0, 255)));

    cout << "\nDone.\n";
}
