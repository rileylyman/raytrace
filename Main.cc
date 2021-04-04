#include <stdio.h>
#include <stdlib.h>

#include <vector>

#include "Math.h"
#include "Sdl.h"

using namespace math;

constexpr float kPi = 3.1415;

struct Camera {
  Camera()
      : fov_degrees(60), position{0, 0, -100}, forward{0, 0, 1}, up{0, 1, 0} {
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
  return glm::lookAt(camera.position,
                     glm::normalize(camera.position + camera.forward),
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
  std::vector<math::Sphere> spheres;
  Camera camera;
};

int main(void) {
  math::Vec4<float> clear_color = {0.2, 0.3, 0.35, 1};
  math::Vec4<float> sphere_color = {.8, .4, .2, 1};

  sdl::Initialize();
  sdl::ClearScreen(clear_color);

  Camera camera;
  math::Sphere sphere({0, 0, 0}, 20);
  math::Vec3<float> camera_pos = {0, 0, -200};
  math::Vec3<float> view_plane_pos = {0, 0, -90};
  math::Vec2<float> view_plane_dims = {100, 100};

  math::Vec2<float> pixel_dims = {view_plane_dims.x / sdl::kWindowWidth,
                                  view_plane_dims.y / sdl::kWindowHeight};

  for (uint16_t x = 0; x < sdl::kWindowWidth; x++) {
    for (uint16_t y = 0; y < sdl::kWindowHeight; y++) {
      constexpr uint32_t kNumSamples = 4;
      math::Vec4<float> final_color = {0, 0, 0, 0};
      for (uint16_t sample = 0; sample < kNumSamples; sample++) {
        Ray ray = GetWorldSpaceRayFromImageSpace(
            camera, {static_cast<float>(x) / sdl::kWindowWidth,
                     static_cast<float>(y) / sdl::kWindowHeight});
        math::RaySphereIntersection isect =
            math::RaySphereIntersect(ray, sphere);
        if (isect.n == 1 && isect.t1 > 0) {
          math::Vec3<float> isect_point = ray.origin + ray.direction * isect.t1;
          math::Vec3<float> normal =
              glm::normalize(glm::max(isect_point, sphere.origin) -
                             glm::min(isect_point, sphere.origin));
          math::Vec4<float> normal_color =
              math::Vec4<float>(normal.x, normal.y, normal.z, 1.0);
          final_color =
              final_color + normal_color / static_cast<float>(kNumSamples);
        } else if (isect.n == 2 && (isect.t1 > 0 || isect.t2 > 0)) {
          math::Vec3<float> isect_point =
              ray.origin + ray.direction * glm::min(isect.t1, isect.t2);
          math::Vec3<float> normal =
              glm::normalize(glm::max(isect_point, sphere.origin) -
                             glm::min(isect_point, sphere.origin));
          math::Vec4<float> normal_color =
              math::Vec4<float>(normal.x, normal.y, normal.z, 1.0);
          final_color =
              final_color + normal_color / static_cast<float>(kNumSamples);
        } else {
          final_color =
              final_color + clear_color / static_cast<float>(kNumSamples);
        }
      }
      sdl::SetPixel({x, y}, final_color);
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

