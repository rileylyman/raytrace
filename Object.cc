#include "Object.h"

#include <limits>

Intersection RayObjectIntersect(const Ray& ray, const Object& object) {
  if (object.type == ObjectType::kSphere) {
    return RaySphereIntersect(ray, object.sphere);
  } else {
    Intersection isect;
    isect.valid = false;
    float smallest_t = std::numeric_limits<float>::max();
    for (const Triangle& triangle : object.triangles) {
      Intersection curr_isect = RayTriangleIntersect(ray, triangle);
      if (curr_isect.valid && smallest_t > curr_isect.t) {
        isect = curr_isect;
        smallest_t = curr_isect.t;
      }
    }
    return isect;
  }
}
