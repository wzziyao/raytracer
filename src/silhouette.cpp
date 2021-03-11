#include "ppm_image.h"
#include "AGLM.h"
#include "ray.h"

using namespace glm;
using namespace agl;

color ray_color(const ray& ray)
{
   return color(0); 
}

void ray_trace(ppm_image& image)
{
   image.save("silhouette.ppm");
}
