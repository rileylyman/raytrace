#include "Math.h"

#include <math.h>

namespace math {

Intersection RaySphereIntersect(const Ray& ray, const Sphere& sphere) {
  Intersection isect;

  float a = glm::dot(ray.direction, ray.direction);
  float b = 2 * glm::dot((ray.origin - sphere.origin), ray.direction);
  float c =
      glm::dot((ray.origin - sphere.origin), (ray.origin - sphere.origin)) -
      sphere.radius * sphere.radius;

  float radical = b * b - 4 * a * c;
  if (radical < 0.0) {
    isect.valid = false;
  } else if (radical == 0) {
    isect.valid = true;
    isect.t = -b / (2 * a);
  } else {
    isect.valid = true;

    float t1 = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
    float t2 = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);

    if (t1 > 0 && t1 < t2) {
      isect.t = t1;
    } else {
      isect.t = t2;
    }
  }

  isect.isect_point = ray.origin + ray.direction * isect.t;
  isect.isect_normal = glm::normalize(isect.isect_point - sphere.origin);

  return isect;
}

Intersection RayPlaneIntersect(const Ray& ray, const Plane& plane) {
  Intersection isect;
  isect.valid = glm::dot(ray.direction, plane.normal) != 0;
  if (isect.valid) {
    isect.t = glm::dot(plane.position - ray.origin, plane.normal) /
              glm::dot(ray.direction, plane.normal);
    isect.isect_point = ray.origin + ray.direction * isect.t;
    isect.isect_normal = plane.normal;
  }
}

Intersection RayTriangleIntersect(const Ray& ray, const Triangle& tri) {
  Vec3<float> e1 = tri.p1 - tri.p0;
  Vec3<float> e2 = tri.p2 - tri.p0;
  Vec3<float> s = ray.origin - tri.p0;
  Vec3<float> s1 = glm::cross(ray.direction, e2);
  Vec3<float> s2 = glm::cross(s, e1);

  Vec3<float> result =
      (1 / glm::dot(s1, e1)) * Vec3<float>{glm::dot(s2, e2), glm::dot(s1, s),
                                           glm::dot(s2, ray.direction)};

  Intersection isect;
  isect.t = result.x;
  isect.isect_point = (1 - result.y - result.z) * tri.p0 + result.y * tri.p1 +
                      result.z * tri.p2;
  isect.isect_normal = glm::normalize(glm::cross(e1, e2));
  isect.valid = 0 <= result.y && result.y <= 1 && 0 <= result.z &&
                result.z <= 1 && result.y + result.z <= 1;

  return isect;
}

}  // namespace math
