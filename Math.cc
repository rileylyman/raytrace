#include "Math.h"

#include <math.h>

namespace math {

RaySphereIntersection RaySphereIntersect(const Ray& ray, const Sphere& sphere) {
  RaySphereIntersection intersection;

  float a = glm::dot(ray.direction, ray.direction);
  float b = 2 * glm::dot((ray.origin - sphere.origin), ray.direction);
  float c =
      glm::dot((ray.origin - sphere.origin), (ray.origin - sphere.origin)) -
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

float RayPlaneIntersect(const Ray& ray, const Plane& plane) {
  return glm::dot(plane.position - ray.origin, plane.normal) /
         glm::dot(ray.direction, plane.normal);
}

RayTriangleIntersection RayTriangleIntersect(const Ray& ray,
                                             const Triangle& tri) {
  Vec3<float> e1 = tri.p1 - tri.p0;
  Vec3<float> e2 = tri.p2 - tri.p0;
  Vec3<float> s = ray.origin - tri.p0;
  Vec3<float> s1 = glm::cross(ray.direction, e2);
  Vec3<float> s2 = glm::cross(s, e1);

  Vec3<float> result =
      (1 / glm::dot(s1, e1)) * Vec3<float>{glm::dot(s2, e2), glm::dot(s1, s),
                                           glm::dot(s2, ray.direction)};

  RayTriangleIntersection isect;
  isect.t = result.x;
  isect.p = (1 - result.y - result.z) * tri.p0 + result.y * tri.p1 +
            result.z * tri.p2;
  isect.normal = glm::normalize(glm::cross(e1, e2));
  isect.success = 0 <= result.y && result.y <= 1 && 0 <= result.z &&
                  result.z <= 1 && result.y + result.z <= 1;

  return isect;
}

}  // namespace math
