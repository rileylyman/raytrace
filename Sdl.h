#pragma once

#include <SDL2/SDL.h>
#include <stdlib.h>

#include "Math.h"

namespace sdl {

constexpr uint16_t kWindowWidth = 800;
constexpr uint16_t kWindowHeight = 600;

enum class EventType {
  kNone,
  kSdlWantsQuit,
};

extern SDL_Renderer* renderer;
extern SDL_Window* window;

void Initialize();

void ClearScreen(math::Vec4<uint8_t> color);

void SetPixel(math::Vec2<uint16_t> pixel, math::Vec4<float> color);

void Show();

EventType PollEvent();

void Destroy();

}  // namespace sdl
