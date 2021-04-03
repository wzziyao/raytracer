#ifndef MATERIAL_H
#define MATERIAL_H

#include <cmath>
#include "AGLM.h"
#include "ray.h"
#include "hittable.h"

class material {
public:
  virtual bool scatter(const ray& r_in, const hit_record& rec, 
     glm::color& attenuation, ray& scattered) const = 0;
  virtual ~material() {}
};

class lambertian : public material {
public:
  lambertian(const glm::color& a) : albedo(a) {}

  virtual bool scatter(const ray& r_in, const hit_record& rec, 
     glm::color& attenuation, ray& scattered) const override 
  {
      glm::vec3 scatter_direction = rec.normal + random_unit_vector(); 
      if (near_zero(scatter_direction)) {
         scatter_direction = rec.normal;
      }
      scattered = ray(rec.p, scatter_direction); 
      attenuation = albedo;
      return true;
  }

public:
  glm::color albedo;
};

class phong : public material {
public:
  phong(const glm::vec3& view) :
     diffuseColor(0,0,1), 
     specColor(1,1,1),
     ambientColor(.01f, .01f, .01f),
     lightPos(5,5,0),
     viewPos(view), 
     kd(0.45), ks(0.45), ka(0.1), shininess(10.0) 
  {}

  phong(const glm::color& idiffuseColor, 
        const glm::color& ispecColor,
        const glm::color& iambientColor,
        const glm::point3& ilightPos, 
        const glm::point3& iviewPos, 
        float ikd, float iks, float ika, float ishininess) : 
     diffuseColor(idiffuseColor), 
     specColor(ispecColor),
     ambientColor(iambientColor),
     lightPos(ilightPos),
     viewPos(iviewPos), kd(ikd), ks(iks), ka(ika), shininess(ishininess) 
  {}

  virtual bool scatter(const ray& r_in, const hit_record& hit, 
     glm::color& attenuation, ray& scattered) const override 
  {
      glm::color Ia = ka * ambientColor;

      glm::vec3 L = normalize(lightPos - hit.p);
      glm::vec3 n = normalize(hit.normal);
      glm::color Id = kd * fmax(0.0f, dot(L, n)) * diffuseColor;

      glm::vec3 v = normalize(viewPos - hit.p);
      glm::vec3 r = 2 * dot(L, n) * n - L;
      glm::color Is = ks * pow(fmax(0.0f, dot(v, r)), shininess) * specColor;

      glm::color final_color = Ia + Id + Is;

      attenuation = final_color;
      return false;
  }

public:
  glm::color diffuseColor;
  glm::color specColor;
  glm::color ambientColor;
  glm::point3 lightPos;
  glm::point3 viewPos; 
  float kd; 
  float ks;
  float ka; 
  float shininess;
};

class metal : public material {
public:
   metal(const glm::color& a, float f) : albedo(a), fuzz(glm::clamp(f,0.0f,1.0f)) {}

   virtual bool scatter(const ray& r_in, const hit_record& rec, 
      glm::color& attenuation, ray& scattered) const override 
   {
      glm::vec3 R_in = r_in.direction();
      glm::vec3 R_out = -2 * dot(R_in, rec.normal) * rec.normal + R_in;
      glm::vec3 scatter_direction = R_out + fuzz * random_unit_vector();
      if (near_zero(scatter_direction)) {
         scatter_direction = rec.normal;
      }
      scattered = ray(rec.p, scatter_direction); 
      attenuation = albedo;
      return true;
   }

public:
   glm::color albedo;
   float fuzz;
};

class dielectric : public material {
public:
  dielectric(float index_of_refraction) : ir(index_of_refraction) {}

  virtual bool scatter(const ray& r_in, const hit_record& rec, 
     glm::color& attenuation, ray& scattered) const override 
   {
      attenuation = glm::color(1.0, 1.0, 1.0);
      float n_ratio = rec.front_face ? (1.0f / ir) : ir;
      
      glm::vec3 R = normalize(r_in.direction());
      glm::vec3 n = rec.normal;
      float cos_in = fmin(dot(-R, n), 1.0f);
      float sin = sqrt(1.0f - cos_in * cos_in);

      glm::vec3 scatter_direction;
      if (n_ratio * sin > 1.0f) {
         scatter_direction = -2 * dot(R, n) * n + R;
      } else {
         float cos_out = fmin(glm::dot(-R, n), 1.0f);
         glm::vec3 R_perp = n_ratio * (R + cos_out * n);
         glm::vec3 R_para = (float)-sqrt(fabs(1.0f - pow(glm::length(R_perp), 2))) * n;
         scatter_direction = R_perp + R_para;
      }
     scattered = ray(rec.p, scatter_direction); 
     return true;
   }

public:
  float ir; // Index of Refraction
};


#endif

