#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "Math.h"
#include "Sdl.h"

struct Scene {
  std::vector<math::Sphere> spheres;
};

int main(void) {
  math::Vec4<float> clear_color = {0.2, 0.3, 0.35, 1};
  math::Vec4<float> sphere_color = {.8, .4, .2, 1};

  sdl::Initialize();
  sdl::ClearScreen(clear_color);

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
        math::Vec3<float> plane_point = view_plane_pos;
        plane_point.x += (static_cast<float>(x) / sdl::kWindowWidth - 0.5) *
                             view_plane_dims.x +
                         (sample == 1 || sample == 3 ? pixel_dims.x : 0);
        plane_point.y += (static_cast<float>(y) / sdl::kWindowHeight - 0.5) *
                             view_plane_dims.y +
                         (sample == 2 || sample == 3 ? pixel_dims.y : 0);
        math::Ray ray(camera_pos, plane_point - camera_pos);
        math::RaySphereIntersection isect =
            math::RaySphereIntersect(ray, sphere);
        if (isect.n == 1 && isect.t1 > 0) {
          final_color =
              final_color + sphere_color / static_cast<float>(kNumSamples);
        } else if (isect.n == 2 && (isect.t1 > 0 || isect.t2 > 0)) {
          final_color =
              final_color + sphere_color / static_cast<float>(kNumSamples);
        } else {
          final_color =
              final_color + clear_color / static_cast<float>(kNumSamples);
        }
        sdl::SetPixel({x, y}, final_color);
      }
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

