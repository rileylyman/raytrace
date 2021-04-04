#pragma once

#include <stdint.h>
#include <stdlib.h>

#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace math {

template <typename T>
using Vec2 = glm::vec<2, T, glm::defaultp>;
template <typename T>
using Vec3 = glm::vec<3, T, glm::defaultp>;
template <typename T>
using Vec4 = glm::vec<4, T, glm::defaultp>;
template <typename T>
using Mat4x4 = glm::mat<4, 4, T, glm::defaultp>;

struct Ray {
  Ray(Vec3<float> o, Vec3<float> d) : origin(o), direction(d) {}

  Vec3<float> origin;
  Vec3<float> direction;
};

struct Sphere {
  Sphere(Vec3<float> o, float r) : origin(o), radius(r) {}

  Vec3<float> origin;
  float radius;
};

struct Plane {
  Plane(Vec3<float> pos, Vec3<float> n) : position(pos), normal(n) {}

  Vec3<float> position;
  Vec3<float> normal;
};

struct Triangle {
  Triangle(Vec3<float> p0, Vec3<float> p1, Vec3<float> p2)
      : p0(p0), p1(p1), p2(p2) {}

  Vec3<float> p0, p1, p2;
};

struct Intersection {
  bool valid;
  float t;
  Vec3<float> isect_point;
  Vec3<float> isect_normal;
};

Intersection RaySphereIntersect(const Ray& ray, const Sphere& sphere);

Intersection RayPlaneIntersect(const Ray& ray, const Plane& plane);

Intersection RayTriangleIntersect(const Ray& ray,
                                             const Triangle& tri);

}  // namespace math
