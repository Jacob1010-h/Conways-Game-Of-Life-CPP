#pragma once

#include <iostream>
#include <SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_sdlrenderer.h>
#include <string>

#include "Game.h"
#include "Timer.h"

class Window
{
public:
  Window();
  ~Window();

  void checkQuit();

  void refresh();
  void update();
  void render();

private:
  SDL_Window *window;
  SDL_Renderer *renderer;

  bool _quit;
  SDL_Event m_event{};

  const int CELL_SIZE = 6;
  const int SCREEN_WIDTH = 1000;
  const int SCREEN_HEIGHT = 768;

  const int SCREEN_FPS = 60;
  const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

  Timer _fpsTimer;
  Timer _capTimer;

  int _countedFrames;
  int _mouseX{}, _mouseY{};

  bool _leftBottonHold;
  bool _rightBottonHold;

  bool _canDraw;


  float _currentTime;
};