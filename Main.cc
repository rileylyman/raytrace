#include <stdio.h>
#include <stdlib.h>

#include <limits>
#include <vector>

#include "Math.h"
#include "Object.h"
#include "Sdl.h"

using namespace math;

struct Camera {
  Camera()
      : fov_degrees(60), position{0, 50, 200}, forward{0, 0, -1}, up{0, 1, 0} {
    aspect_ratio = static_cast<float>(sdl::kWindowHeight) /
                   static_cast<float>(sdl::kWindowWidth);
  }

  float fov_degrees;
  float aspect_ratio;
  Vec3<float> position;
  Vec3<float> forward;
  Vec3<float> up;
};

Mat4x4<float> WorldToCamera(Camera camera) {
  return glm::lookAt(camera.position, camera.position + camera.forward,
                     camera.up);
}

Mat4x4<float> CameraToWorld(Camera camera) {
  return glm::inverse(WorldToCamera(camera));
}

Vec2<float> GetPlaneDimensions(Camera camera) {
  float xdim = abs(2 * tan(glm::radians(camera.fov_degrees / 2)));
  return {xdim, xdim * camera.aspect_ratio};
}

Ray GetWorldSpaceRayFromImageSpace(Camera camera, Vec2<float> pixel_pos) {
  Vec2<float> camera_space_plane_dimensions = GetPlaneDimensions(camera);
  Vec3<float> camera_space_plane_position = {0, 0, -1};

  Vec4<float> camera_space_endpoint = {
      (pixel_pos.x - 0.5) * camera_space_plane_dimensions.x,
      (pixel_pos.y - 0.5) * camera_space_plane_dimensions.y,
      camera_space_plane_position.z, 1};

  Vec4<float> world_space_endpoint =
      CameraToWorld(camera) * camera_space_endpoint;
  Vec3<float> world_space_direction =
      glm::normalize(Vec3<float>(world_space_endpoint) - camera.position);

  return Ray(camera.position, world_space_direction);
}

struct Scene {
  std::vector<Object> objects;
  Camera camera;
};

struct SceneIntersection {
  Intersection isect;
  Object* object;
};

SceneIntersection RaySceneIntersect(const Ray& ray, const Scene& scene) {
  SceneIntersection scene_isect;
  scene_isect.isect.valid = false;
  float smallest_t = std::numeric_limits<float>::max();
  for (const Object& object : scene.objects) {
    Intersection curr_isect = RayObjectIntersect(ray, object);
    if (curr_isect.valid && smallest_t > curr_isect.t) {
      smallest_t = curr_isect.t;
      scene_isect.isect = curr_isect;
      scene_isect.object = const_cast<Object*>(&object);
    }
  }
  return scene_isect;
}

int main(void) {
  math::Vec4<float> clear_color = {0.2, 0.3, 0.35, 1};
  math::Vec4<float> sphere_color = {.8, .4, .2, 1};

  sdl::Initialize();
  sdl::ClearScreen({0, 0, 0, 1});

  Camera camera;
  Scene scene;
  scene.camera = camera;
  scene.objects = {
      {ObjectType::kSphere, false, {1, 0, 0, 1}, {{30, 15, -20}, 20}},
      {ObjectType::kSphere, false, {1, 0, 0, 1}, {{-30, 15, -30}, 20}},
      {ObjectType::kSphere, false, {1, 0, 0, 1}, {{-75, 75, -75}, 20}},
      // floor
      {ObjectType::kTriangleMesh,
       false,
       {0, 1, 0, 1},
       {
           {{-75, 0, 0}, {-75, 0, -75}, {75, 0, 0}},
           {{75, 0, 0}, {-75, 0, -75}, {75, 0, -75}},
       }},
      // light
      {ObjectType::kTriangleMesh,
       true,
       {0, 1, 0, 1},
       {
           {{-25, 74, -25}, {-25, 74, -50}, {25, 74, -25}},
           {{25, 74, -25}, {-25, 74, -50}, {25, 74, -50}},
       }},
      // roof
      {ObjectType::kTriangleMesh,
       false,
       {0, 1, 0, 1},
       {
           {{-75, 75, 0}, {-75, 75, -75}, {75, 75, 0}},
           {{75, 75, 0}, {-75, 75, -75}, {75, 75, -75}},
       }},
      // left wall
      {ObjectType::kTriangleMesh,
       false,
       {0, 1, 0, 1},
       {
           {{-75, 0, 0}, {-75, 75, 0}, {-75, 75, -75}},
           {{-75, 0, 0}, {-75, 0, -75}, {-75, 75, -75}},
       }},
      // right wall
      {ObjectType::kTriangleMesh,
       false,
       {0, 1, 0, 1},
       {
           {{75, 0, 0}, {75, 75, 0}, {75, 75, -75}},
           {{75, 0, 0}, {75, 0, -75}, {75, 75, -75}},
       }},
      // back wall
      {ObjectType::kTriangleMesh,
       false,
       {0, 1, 0, 1},
       {
           {{-75, 0, -75}, {-75, 75, -75}, {75, 75, -75}},
           {{-75, 0, -75}, {75, 0, -75}, {75, 75, -75}},
       }},
  };

  for (uint16_t x = 0; x < sdl::kWindowWidth; x++) {
    for (uint16_t y = 0; y < sdl::kWindowHeight; y++) {
      Ray ray = GetWorldSpaceRayFromImageSpace(
          camera, {static_cast<float>(x) / sdl::kWindowWidth,
                   static_cast<float>(y) / sdl::kWindowHeight});

      SceneIntersection scene_isect = RaySceneIntersect(ray, scene);

      if (scene_isect.isect.valid) {
        if (scene_isect.object->is_emissive) {
          sdl::SetPixel({x, y}, {1, 1, 1, 1});
        } else {
          math::Vec4<float> normal_color =
              math::Vec4<float>(scene_isect.isect.isect_normal.x,
                                scene_isect.isect.isect_normal.y,
                                scene_isect.isect.isect_normal.z, 1.0);
          sdl::SetPixel({x, y}, normal_color);
        }
      } else {
        sdl::SetPixel({x, y}, clear_color);
      }
    }
    if (x % 4 == 0) {
      sdl::Show();
    }
  }

  sdl::Show();

  while (1) {
    if (sdl::PollEvent() == sdl::EventType::kSdlWantsQuit) {
      break;
    }
  }

  return 0;
}

