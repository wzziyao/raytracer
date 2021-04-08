#ifndef BOX_H_
#define BOX_H_

#include "hittable.h"
#include "AGLM.h"

class box : public hittable {
public:
   box() : c(0), ax(0), ay(0), az(0), hx(0), hy(0), hz(0), mat_ptr(0) {}
   box(const glm::point3& center, 
       const glm::vec3& xdir, const glm::vec3& ydir, const glm::vec3& zdir,
       const glm::vec3& halfx, const glm::vec3& halfy, const glm::vec3& halfz,
       std::shared_ptr<material> m) : c(center), ax(xdir), ay(ydir), az(zdir), 
          hx(halfx), hy(halfy), hz(halfz), mat_ptr(m) {};

   virtual bool hit(const ray& r, hit_record& rec) const override
   {
      float tmin = -INFINITY;
      float tmax = INFINITY;
      glm::vec3 p = c - r.origin();
      glm::vec3 a[3] = {ax, ay, az};
      glm::vec3 h[3] = {hx, hy, hz};
      glm::vec3 current_normal;
      for(int i = 0; i < sizeof(a)/sizeof(*a); i++) {
         current_normal = a[i];
         float e = glm::dot(a[i], p);
         float f = glm::dot(a[i], r.direction());
         if (abs(f) > 1e-8) {
            float t1 = (e + glm::length(h[i])) / f;
            float t2 = (e - glm::length(h[i])) / f;
            if (t1 > t2) std::swap(t1, t2);
            if (t1 > tmin) tmin = t1;
            if (t2 < tmax) tmax = t2;
            if (tmin > tmax) return false; 
            if (tmax < 0) return false;
         }
         else if (-e-glm::length(h[i]) > 0 || -e+glm::length(h[i]) < 0) return false; 
      }
      float t;
      if (tmin > 0) t = tmin;
      else t = tmax;

      rec.t = t;
      rec.p = r.at(t);
      rec.mat_ptr = mat_ptr;

      glm::vec3 outward_normal = current_normal;
      rec.set_face_normal(r, outward_normal);

      return true;
   }

public:
   glm::vec3 c;
   glm::vec3 ax;
   glm::vec3 ay;
   glm::vec3 az;
   glm::vec3 hx;
   glm::vec3 hy;
   glm::vec3 hz;
   std::shared_ptr<material> mat_ptr;
};

#endif
