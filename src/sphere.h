// ray.h, from https://raytracing.github.io by Peter Shirley, 2018-2020
#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "AGLM.h"

class sphere : public hittable {
public:
   sphere() : radius(0), center(0), mat_ptr(0) {}
   sphere(const glm::point3& cen, float r, std::shared_ptr<material> m) : 
      center(cen), radius(r), mat_ptr(m) {};

   virtual bool hit(const ray& r, hit_record& rec) const override;

public:
   glm::point3 center;
   float radius;
   std::shared_ptr<material> mat_ptr;
};

// geometric intersection test
bool sphere::hit(const ray& r, hit_record& rec) const {
   glm::vec3 el = center - r.origin();
   float len = length(r.direction());
   glm::vec3 d = r.direction() / len;

   float s = glm::dot(el, d);
   float elSqr = glm::dot(el, el);
   float rSqr = radius*radius;
   if (s < 0 and elSqr > rSqr) return false;

   float mSqr = elSqr - s*s;
   if (mSqr > rSqr) return false;

   float q = sqrt(rSqr - mSqr);
   float t;
   if (elSqr > rSqr) t = s - q;
   else t = s + q;

   rec.t = t / len;
   rec.p = r.at(t / len);
   rec.mat_ptr = mat_ptr;

   glm::vec3 outward_normal = normalize(rec.p - center);
   rec.set_face_normal(r, outward_normal);

   return true;
}

#endif

