// Raytracer framework from https://raytracing.github.io by Peter Shirley, 2018-2020
// alinen 2021, modified to use glm and ppm_image class

#include "ppm_image.h"
#include "AGLM.h"
#include "ray.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "hittable_list.h"
#include <cmath>

using namespace glm;
using namespace agl;
using namespace std;

vec3 random_in_unit_sphere() {
   while (true) {
      vec3 p = vec3(random_float(-1,1), random_float(-1,1), random_float(-1,1));
      if (pow(p[0], 2) + pow(p[1], 2) + pow(p[2], 2) >= 1) continue;
      return p;
   }
}

vec3 random_in_hemisphere(const vec3& normal) {
    vec3 in_unit_sphere = random_in_unit_sphere();
    if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return in_unit_sphere;
    else
        return -in_unit_sphere;
}

color ray_color(const ray& r, const hittable_list& world, int depth)
{
   hit_record rec;
   if (depth <= 0)
   {
      return color(0);
   }

   if (world.hit(r, 0.001f, infinity, rec)) {
      point3 target = rec.p + random_in_hemisphere(rec.normal);
      return 0.5f * ray_color(ray(rec.p, target - rec.p), world, depth-1);
   }

   vec3 unit_direction = normalize(r.direction());
   auto t = 0.5f * (unit_direction.y + 1.0f);
   return (1.0f - t) * color(1, 1, 1) + t * color(0.5f, 0.7f, 1.0f);
}

color normalize_color(const color& c, int samples_per_pixel)
{
   // todo: implement me!
   float scale = 1.0f / samples_per_pixel;
   float r = std::min(0.999f, std::max(0.0f, c.r * scale));
   float g = std::min(0.999f, std::max(0.0f, c.g * scale));
   float b = std::min(0.999f, std::max(0.0f, c.b * scale));

   // apply gamma correction 
   r = sqrt(r);
   g = sqrt(g);
   b = sqrt(b);

   return color(r, g, b);
}

void ray_trace(ppm_image& image)
{
   // Image
   int height = image.height();
   int width = image.width();
   float aspect = width / float(height);
   int samples_per_pixel = 50; // higher => more anti-aliasing
   int max_depth = 50; // higher => less shadow acne

   // World
   shared_ptr<material> gray = make_shared<lambertian>(color(0.5f));

   hittable_list world;
   world.add(make_shared<sphere>(point3(0, 0, -1), 0.5f, gray));
   world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, gray));

   // Camera
   vec3 camera_pos(0);
   float viewport_height = 2.0f;
   float focal_length = 1.0; 
   camera cam(camera_pos, viewport_height, aspect, focal_length);

   // Ray trace
   for (int j = 0; j < height; j++)
   {
      for (int i = 0; i < width; i++)
      {
         color c(0, 0, 0);
         for (int s = 0; s < samples_per_pixel; s++) // antialias
         {
            float u = float(i + random_float()) / (width - 1);
            float v = float(height - j - 1 - random_float()) / (height - 1);

            ray r = cam.get_ray(u, v);
            c += ray_color(r, world, max_depth);
         }
         c = normalize_color(c, samples_per_pixel);
         image.set_vec3(j, i, c);
      }
   }

   image.save("basic.png");
}
