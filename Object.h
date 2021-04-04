#pragma once

#include <vector>

#include "Math.h"

using namespace math;

enum class ObjectType {
  kTriangleMesh,
  kSphere,
};

struct Object {
  Object(ObjectType type, bool is_emissive, Vec4<float> color,
         std::vector<Triangle> triangles)
      : type(type),
        is_emissive(is_emissive),
        color(color),
        triangles(triangles) {}

  Object(ObjectType type, bool is_emissive, Vec4<float> color, Sphere sphere)
      : type(type), is_emissive(is_emissive), color(color), sphere(sphere) {}

  ObjectType type;

  bool is_emissive;
  Vec4<float> color;

    std::vector<Triangle> triangles;
    Sphere sphere = {{0,0,0}, 0};
};

Intersection RayObjectIntersect(const Ray& ray, const Object& object);
