#include "Math.h"
#include <math.h>

namespace math {

RaySphereIntersection RaySphereIntersect(const Ray& ray, const Sphere& sphere) {
  RaySphereIntersection intersection;

  float a = glm::dot(ray.direction , ray.direction);
  float b = 2 * glm::dot((ray.origin - sphere.origin) , ray.direction);
  float c =
      glm::dot((ray.origin - sphere.origin) , (ray.origin - sphere.origin)) -
      sphere.radius * sphere.radius;

  float radical = b * b - 4 * a * c;
  if (radical < 0.0) {
    intersection.n = 0;
  } else if (radical == 0) {
    intersection.n = 1;
    intersection.t1 = -b / (2 * a);
  } else {
    intersection.n = 2;
    intersection.t1 = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
    intersection.t2 = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
  }

  return intersection;
}

}  // namespace math
