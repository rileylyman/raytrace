#include <stdio.h>
#include <vector>


struct Vec3 {
  float x, y, z;
};

struct Sphere {
  Vec3 origin;
  float radius;
};

struct Scene {
  std::vector<Sphere> spheres;
};

int main() {
  printf("Hello, world.");

  return 0;
}
