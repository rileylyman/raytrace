#include "Sdl.h"

namespace sdl {

SDL_Renderer* renderer;
SDL_Window* window;

void Initialize() {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(kWindowHeight, kWindowWidth, 0, &window,
                              &renderer);
}

void ClearScreen(math::Vec4<uint8_t> color) {
  SDL_SetRenderDrawColor(renderer, color.x, color.y, color.z, color.w);
  SDL_RenderClear(renderer);
}

void SetPixel(math::Vec2<uint16_t> pixel, math::Vec4<float> color) {
  SDL_SetRenderDrawColor(renderer, color.x * 255, color.y * 255, color.z * 255, color.w * 255);
  SDL_RenderDrawPoint(renderer, pixel.x, pixel.y);
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
