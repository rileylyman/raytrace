#include "Sdl.h"

namespace sdl {

SDL_Renderer* renderer;
SDL_Window* window;

void Initialize() {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(kWindowWidth, kWindowHeight, 0, &window,
                              &renderer);
}

void ClearScreen(math::Vec4<uint8_t> color) {
  SDL_SetRenderDrawColor(renderer, color.x, color.y, color.z, color.w);
  SDL_RenderClear(renderer);
}

void SetPixel(math::Vec2<uint16_t> pixel, math::Vec4<double> color) {
  using namespace math;
  Vec3<double> rgb = Vec3<double>{color.x, color.y, color.z};
  if (rgb.x > 1 || rgb.y > 1 || rgb.z > 1) {
    double max_comp = glm::max(rgb.x, glm::max(rgb.y, rgb.z));
    rgb.x = rgb.x / max_comp;
    rgb.y = rgb.y / max_comp;
    rgb.z = rgb.z / max_comp;

  }
  SDL_SetRenderDrawColor(renderer, abs(rgb.x) * 255, abs(rgb.y) * 255,
                         abs(rgb.z) * 255, abs(color.w) * 255);
  SDL_RenderDrawPoint(renderer, pixel.x, kWindowHeight - pixel.y - 1);
}

void Show() {
  SDL_RenderPresent(renderer);
}

EventType PollEvent() {
  SDL_Event event;
  if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
    return EventType::kSdlWantsQuit;
  }
  return EventType::kNone;
}

void Destroy() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

}  // namespace sdl
