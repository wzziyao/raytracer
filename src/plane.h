#ifndef PLANE_H_
#define PLANE_H_

#include "hittable.h"
#include "AGLM.h"

class plane : public hittable {
public:
   plane() : a(0), n(0), mat_ptr(0) {}
   plane(const glm::point3& p, const glm::vec3& normal, 
      std::shared_ptr<material> m) : a(p), n(normal), mat_ptr(m) {};

   virtual bool hit(const ray& r, hit_record& rec) const override
   {
      float len = length(r.direction());
      glm::vec3 v = r.direction() / len;
      if (glm::dot(v, n) == 0) return false;

      float t = glm::dot((a - r.origin()), n) / glm::dot(v, n);

      rec.t = t;
      rec.p = r.at(t);
      rec.mat_ptr = mat_ptr;

      rec.set_face_normal(r, n);

      return true;
   }

public:
   glm::vec3 a;
   glm::vec3 n;
   std::shared_ptr<material> mat_ptr;
};

#endif
