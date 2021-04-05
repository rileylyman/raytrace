#pragma once

#include <stdint.h>
#include <stdlib.h>

#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <random>

namespace math {

template <typename T>
using Vec2 = glm::vec<2, T, glm::defaultp>;
template <typename T>
using Vec3 = glm::vec<3, T, glm::defaultp>;
template <typename T>
using Vec4 = glm::vec<4, T, glm::defaultp>;
template <typename T>
using Mat4x4 = glm::mat<4, 4, T, glm::defaultp>;

double SampleUniform();

struct Ray {
  Ray(Vec3<double> o, Vec3<double> d) : origin(o), direction(d) {}

  Vec3<double> origin;
  Vec3<double> direction;
};

struct Sphere {
  Sphere(Vec3<double> o, double r) : origin(o), radius(r) {}

  Vec3<double> origin;
  double radius;
};

struct Plane {
  Plane(Vec3<double> pos, Vec3<double> n) : position(pos), normal(n) {}

  Vec3<double> position;
  Vec3<double> normal;
};

struct Triangle {
  Triangle(Vec3<double> p0, Vec3<double> p1, Vec3<double> p2)
      : p0(p0), p1(p1), p2(p2) {}

  Vec3<double> p0, p1, p2;
};

struct Intersection {
  bool valid;
  double t;
  Vec3<double> isect_point;
  Vec3<double> isect_normal;
};

Intersection RaySphereIntersect(const Ray& ray, const Sphere& sphere);

Intersection RayPlaneIntersect(const Ray& ray, const Plane& plane);

Intersection RayTriangleIntersect(const Ray& ray, const Triangle& tri);

}  // namespace math
