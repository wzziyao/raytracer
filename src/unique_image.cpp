// Raytracer framework from https://raytracing.github.io by Peter Shirley, 2018-2020
// alinen 2021, modified to use glm and ppm_image class
// wzziyao 2021, modified materials.cpp

#include "ppm_image.h"
#include "AGLM.h"
#include "ray.h"
#include "sphere.h"
#include "box.h"
#include "camera.h"
#include "material.h"
#include "hittable_list.h"

using namespace glm;
using namespace agl;
using namespace std;

color ray_color(const ray& r, const hittable_list& world, int depth)
{
   hit_record rec;
   if (depth <= 0)
   {
      return color(0);
   }

   if (world.hit(r, 0.001f, infinity, rec))
   {
      ray scattered;
      color attenuation;
      if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
      {
         color recurseColor = ray_color(scattered, world, depth - 1);
         return attenuation * recurseColor;
      }
      return attenuation;
   }
   vec3 unit_direction = normalize(r.direction());
   auto t = 0.5f * (unit_direction.y + 1.0f);
   return (1.0f - t) * color(1, 1, 1) + t * color(0.9f, 0.26f, 0.46f);
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
    const auto aspect = 3.0 / 2.0;
    const int samples_per_pixel = 10;
    const int max_depth = 10;

    // Camera
   //  vec3 camera_pos(0, 0, 15);
   //  float viewport_height = 2.0f;
   //  float focal_length = 4.0; 
   //  camera cam(camera_pos, viewport_height, aspect, focal_length);
    point3 lookfrom(12,0,-5);
    point3 lookat(0, 0.8, 2);
    vec3 vup(0,1,0);
    auto dist_to_focus = 10;
    auto aperture = 0.1;

    camera cam(lookfrom, lookat, vup, 20, aspect, aperture, dist_to_focus);

    hittable_list world;
    
    shared_ptr<material> gray = make_shared<lambertian>(color(0.5f));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, gray));

    shared_ptr<material> cardbox = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    shared_ptr<material> eye = make_shared<lambertian>(color(0, 0, 0));
    shared_ptr<material> glass = make_shared<dielectric>(1.5f);
    shared_ptr<material> material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);

   world.add(make_shared<box>(point3(0, 0.8, 2), vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1), 
   vec3(0.5f, 0, 0), vec3(0, 0.7f, 0), vec3(0, 0, 0.5f), cardbox)); // body
   world.add(make_shared<box>(point3(0.15, 2.15, 2), vec3(0.8, -0.6, 0), vec3(0.6, 0.8, 0), vec3(0, 0, 1), 
   vec3(0.6f, 0, 0), vec3(0, 0.45f, 0), vec3(0, 0, 0.45f), cardbox)); // head
   world.add(make_shared<box>(point3(0, 0.9, 2.7), vec3(0, 0, 1), vec3(0.9231, -0.3846, 0), vec3(0.3846, 0.9231, 0), 
   vec3(0.15f, 0, 0), vec3(0, 0.15f, 0), vec3(0, 0, 0.5f), cardbox)); // right arm
   world.add(make_shared<box>(point3(0.1, 1.1, 1.1), vec3(0.7071, 0, 0.7071), vec3(0.7071, 0, -0.7071), vec3(0, 1, 0), 
   vec3(0.15f, 0, 0), vec3(0, 0, 0.5f), vec3(0, 0.15f, 0), cardbox)); // left arm
   world.add(make_shared<box>(point3(0, -0.4, 2.3), vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1), 
   vec3(0.18f, 0, 0), vec3(0, 0, 0.5f), vec3(0, 0.18f, 0), cardbox)); // right leg
   world.add(make_shared<box>(point3(0, -0.4, 1.7), vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1), 
   vec3(0.18f, 0, 0), vec3(0, 0, 0.5f), vec3(0, 0.18f, 0), cardbox)); // left leg

   world.add(make_shared<sphere>(point3(0.7, 1.9, 2.15), 0.05f, eye)); // right eye
   world.add(make_shared<sphere>(point3(0.7, 1.9, 1.85), 0.05f, eye)); // left eye

   world.add(make_shared<sphere>(point3(0.5, 0, -0.8), 1.1f, material3));
   world.add(make_shared<sphere>(point3(-0.1, 0, 3.5), 0.6f, glass));

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

    image.save("unique_image1.png");
}
