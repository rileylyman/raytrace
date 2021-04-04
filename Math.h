#pragma once

#include <stdint.h>
#include <stdlib.h>

#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

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

struct RaySphereIntersection {
  uint8_t n;
  float t1, t2;
};

RaySphereIntersection RaySphereIntersect(const Ray& ray, const Sphere& sphere);

}  // namespace math
