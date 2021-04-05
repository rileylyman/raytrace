#include <stdio.h>
#include <stdlib.h>

#include <limits>
#include <vector>

#include "Math.h"
#include "Object.h"
#include "Sdl.h"

using namespace math;
constexpr double kEpsD = 0.01;
constexpr double kPi = 3.1415;
constexpr double kContProb = 0.9;
constexpr int kMaxSteps = 5;

struct Camera {
  Camera()
      : fov_degrees(60), position{0, 50, 120}, forward{0, 0, -1}, up{0, 1, 0} {
    aspect_ratio = static_cast<double>(sdl::kWindowHeight) /
                   static_cast<double>(sdl::kWindowWidth);
  }

  double fov_degrees;
  double aspect_ratio;
  Vec3<double> position;
  Vec3<double> forward;
  Vec3<double> up;
};

Mat4x4<double> WorldToCamera(Camera camera) {
  return glm::lookAt(camera.position, camera.position + camera.forward,
                     camera.up);
}

Mat4x4<double> CameraToWorld(Camera camera) {
  return glm::inverse(WorldToCamera(camera));
}

Vec2<double> GetPlaneDimensions(Camera camera) {
  double xdim = abs(2 * tan(glm::radians(camera.fov_degrees / 2)));
  return {xdim, xdim * camera.aspect_ratio};
}

Ray GetWorldSpaceRayFromImageSpace(Camera camera, Vec2<double> pixel_pos) {
  Vec2<double> camera_space_plane_dimensions = GetPlaneDimensions(camera);
  Vec3<double> camera_space_plane_position = {0, 0, -1};

  Vec4<double> camera_space_endpoint = {
      (pixel_pos.x - 0.5) * camera_space_plane_dimensions.x,
      (pixel_pos.y - 0.5) * camera_space_plane_dimensions.y,
      camera_space_plane_position.z, 1};

  Vec4<double> world_space_endpoint =
      CameraToWorld(camera) * camera_space_endpoint;
  Vec3<double> world_space_direction =
      glm::normalize(Vec3<double>(world_space_endpoint) - camera.position);

  return Ray(camera.position, world_space_direction);
}

enum class LightType {
  kAreaLight,
};

struct Light {
  Light(LightType type, Vec3<double> min_corner, Vec3<double> max_corner,
        Object object)
      : type(type),
        min_corner(min_corner),
        max_corner(max_corner),
        object(object) {}

  LightType type;

  Vec3<double> min_corner, max_corner;

  Object object;
};

struct Scene {
  std::vector<Object> objects;
  std::vector<Light> lights;
  Camera camera;
};

struct SceneIntersection {
  Intersection isect;
  bool is_light;
  union {
    Object* object;
    Light* light;
  };
};

SceneIntersection RaySceneIntersect(const Ray& ray, const Scene& scene) {
  SceneIntersection scene_isect;
  scene_isect.isect.valid = false;
  double smallest_t = std::numeric_limits<double>::max();
  for (const Object& object : scene.objects) {
    Intersection curr_isect = RayObjectIntersect(ray, object);
    if (curr_isect.valid && smallest_t > curr_isect.t && curr_isect.t > 0) {
      smallest_t = curr_isect.t;
      scene_isect.isect = curr_isect;
      scene_isect.is_light = false;
      scene_isect.object = const_cast<Object*>(&object);
    }
  }
  for (const Light& light : scene.lights) {
    Intersection curr_isect = RayObjectIntersect(ray, light.object);
    if (curr_isect.valid && smallest_t > curr_isect.t && curr_isect.t > 0) {
      smallest_t = curr_isect.t;
      scene_isect.isect = curr_isect;
      scene_isect.is_light = true;
      scene_isect.light = const_cast<Light*>(&light);
    }
  }
  return scene_isect;
}

Scene gScene;

Vec4<double> ZeroBounceRadiance(SceneIntersection scene_isect) {
  if (scene_isect.isect.valid) {
    return scene_isect.is_light ? scene_isect.light->object.color
                                : Vec4<double>{0, 0, 0, 0};
  } else {
    return Vec4<double>{0, 0, 0, 0};
  }
}

Vec4<double> SampleLights(SceneIntersection scene_isect, Vec3<double> wo) {
  if (scene_isect.isect.valid && scene_isect.is_light) {
    return {0, 0, 0, 0};
  }
  Vec4<double> result = {0, 0, 0, 1};
  // return {1, 0, 0, 1};
  int N = 5;
  for (int i = 0; i < N; i++) {
    Light* light =
        &gScene
             .lights[static_cast<int>(SampleUniform() * gScene.lights.size()) %
                     gScene.lights.size()];
    double r1 = SampleUniform(), r2 = SampleUniform(), r3 = SampleUniform();
    Vec3<double> point_on_light = {
        (1.0 - r1) * light->min_corner.x + r1 * light->max_corner.x,
        (1.0 - r2) * light->min_corner.y + r2 * light->max_corner.y,
        (1.0 - r3) * light->min_corner.z + r3 * light->max_corner.z};

    Vec3<double> wi = point_on_light - scene_isect.isect.isect_point;
    double wi_magnitude = glm::dot(wi, wi) / 63;
    wi = glm::normalize(wi);

    SceneIntersection light_isect = RaySceneIntersect(
        Ray(scene_isect.isect.isect_point + kEpsD * wi, wi), gScene);
    if (!light_isect.is_light || light_isect.light != light) {
      continue;
    }

    double x_edge = abs(light->max_corner.x - light->min_corner.x);
    x_edge = x_edge == 0 ? 1 : x_edge;
    double y_edge = abs(light->max_corner.y - light->min_corner.y);
    y_edge = y_edge == 0 ? 1 : y_edge;
    double z_edge = abs(light->max_corner.z - light->min_corner.z);
    z_edge = z_edge == 0 ? 1 : z_edge;
    double surface_area = x_edge * y_edge * z_edge;

    Vec4<double> emission = scene_isect.object->color * light->object.color;
    double costheta =
        abs(glm::dot(glm::normalize(scene_isect.isect.isect_normal), wi));
    double costheta_prime = abs(glm::dot(
        light_isect.isect.isect_normal,
        glm::normalize(scene_isect.isect.isect_point - point_on_light)));

    if (costheta != 0.0) {
      // printf(
      //     "costheta=%f, costheta_prime=%f,
      //     surface_area=%f,wi_magnitude=%f\n", costheta, costheta_prime,
      //     surface_area, wi_magnitude);
    }
    result += emission * costheta * costheta_prime * surface_area / (2 * kPi) /
              wi_magnitude;
  }
  return result * (1.0 / N);

  // return light_color * brdf(wi, wo) * costheta / pdf;
}

Vec4<double> AtLeastOneBounceRadiance(SceneIntersection scene_isect,
                                      Vec3<double> wo, int* step) {
  if (!scene_isect.isect.valid) {
    return {0, 0, 0, 0};
  }
  if (scene_isect.is_light) {
    return scene_isect.light->object.color;
  }
  Vec4<double> L = SampleLights(scene_isect, wo);

  // Get sample direction wi and prob pdf
  double phi = SampleUniform() * 2 * kPi;
  double theta = SampleUniform() * kPi;
  Vec3<double> wi = {sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta)};

  double costheta = abs(glm::dot(glm::normalize(scene_isect.isect.isect_normal),
                                 glm::normalize(wi)));

  // intersect scene again
  Ray wi_ray(scene_isect.isect.isect_point + wi * kEpsD, wi);
  SceneIntersection wi_isect = RaySceneIntersect(wi_ray, gScene);

  // get continuation prob cpdf
  if (SampleUniform() < kContProb && *step < kMaxSteps) {
    *step += 1;
    L += AtLeastOneBounceRadiance(wi_isect, wi, step) * costheta * (2 * kPi) /
         (2 * kPi) / kContProb;
  }
  return L;
}

Vec4<double> EstRadianceIn(Camera camera, Vec2<double> pixel_pos) {
  Ray ray = GetWorldSpaceRayFromImageSpace(camera, pixel_pos);

  Vec4<double> result = {0, 0, 0, 1};
  int N = 8;
  for (int i = 0; i < N; i++) {
    SceneIntersection scene_isect = RaySceneIntersect(ray, gScene);

    if (!scene_isect.isect.valid) {
      continue;
    }

    if (scene_isect.is_light) {
      result += ZeroBounceRadiance(scene_isect);
    } else {
      int number_of_steps = 1;
      Vec4<double> recurse = AtLeastOneBounceRadiance(
          scene_isect, camera.position - scene_isect.isect.isect_point,
          &number_of_steps);
      result +=
          ZeroBounceRadiance(scene_isect) + recurse * (1.0 / number_of_steps);
    }
  }

  return result * (1.0 / N);
}

int main(void) {
  math::Vec4<double> clear_color = {0.2, 0.3, 0.35, 1};
  math::Vec4<double> sphere_color = {.8, .4, .2, 1};

  sdl::Initialize();
  sdl::ClearScreen({0, 0, 0, 1});

  Camera camera;
  gScene.camera = camera;
  gScene.lights = {
      // light
      {LightType::kAreaLight,
       {-25, 74.99, -50},
       {25, 74.99, -25},
       {ObjectType::kTriangleMesh,
        true,
        {1, 1, 1, 1},
        {
            {{-25, 74, -25}, {-25, 74, -50}, {25, 74, -25}},
            {{25, 74, -25}, {-25, 74, -50}, {25, 74, -50}},
        }}},
  };
  gScene.objects = {
      {ObjectType::kSphere, false, {0.7, 0.7, 0.7, 1}, {{30, 15, -20}, 20}},
      {ObjectType::kSphere, false, {0.7, 0.7, 0.7, 1}, {{-30, 15, -30}, 20}},
      {ObjectType::kSphere, false, {0.7, 0.7, 0.7, 1}, {{-75, 75, -75}, 20}},
      // floor
      {ObjectType::kTriangleMesh,
       false,
       {.5, .5, .5, 1},
       {
           {{-75, 0, 0}, {-75, 0, -75}, {75, 0, 0}},
           {{75, 0, 0}, {-75, 0, -75}, {75, 0, -75}},
       }},
      // roof
      {ObjectType::kTriangleMesh,
       false,
       {.5, .5, .5, 1},
       {
           {{-75, 75, 0}, {-75, 75, -75}, {75, 75, 0}},
           {{75, 75, 0}, {-75, 75, -75}, {75, 75, -75}},
       }},
      // left wall
      {ObjectType::kTriangleMesh,
       false,
       {0, 0, .5, 1},
       {
           {{-75, 0, 0}, {-75, 75, 0}, {-75, 75, -75}},
           {{-75, 0, 0}, {-75, 0, -75}, {-75, 75, -75}},
       }},
      // right wall
      {ObjectType::kTriangleMesh,
       false,
       {.5, 0, 0, 1},
       {
           {{75, 0, 0}, {75, 75, 0}, {75, 75, -75}},
           {{75, 0, 0}, {75, 0, -75}, {75, 75, -75}},
       }},
      // back wall
      {ObjectType::kTriangleMesh,
       false,
       {0.5, 0.5, 0.5, 1},
       {
           {{-75, 0, -75}, {-75, 75, -75}, {75, 75, -75}},
           {{-75, 0, -75}, {75, 0, -75}, {75, 75, -75}},
       }},
  };

  for (uint16_t x = 0; x < sdl::kWindowWidth; x++) {
    for (uint16_t y = 0; y < sdl::kWindowHeight; y++) {
      // sdl::SetPixel({x, y}, {1, 0, 0, 1});
      // sdl::Show();
      Vec4<double> color =
          EstRadianceIn(camera, {static_cast<double>(x) / sdl::kWindowWidth,
                                 static_cast<double>(y) / sdl::kWindowHeight});

      // SceneIntersection scene_isect = RaySceneIntersect(ray, gScene);

      // if (scene_isect.isect.valid) {
      //   if (scene_isect.object->is_emissive) {
      //     sdl::SetPixel({x, y}, {1, 1, 1, 1});
      //   } else {
      //     math::Vec4<double> normal_color =
      //         math::Vec4<double>(scene_isect.isect.isect_normal.x,
      //                           scene_isect.isect.isect_normal.y,
      //                           scene_isect.isect.isect_normal.z, 1.0);
      //     sdl::SetPixel({x, y}, normal_color);
      //   }
      // } else {
      //   sdl::SetPixel({x, y}, clear_color);
      // }

      sdl::SetPixel({x, y}, color);
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

