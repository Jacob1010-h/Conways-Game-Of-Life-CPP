#pragma once

#include<iostream>
#include<SDL.h>
#include<imgui.h>
#include<imgui_impl_sdl.h>
#include<imgui_impl_sdlrenderer.h>
#include<string>

#include"Timer.h"

class Game {
public:
    Game();

    ~Game();

    void GameLoop();

    void Update();

    void Render();

    void Life();

private:
    SDL_Window *_window;
    SDL_Renderer *_renderer;

    bool _quit;
    SDL_Event m_event;

    const int CELL_SIZE = 6;
    const int SCREEN_WIDTH = 1000;
    const int SCREEN_HEIGHT = 768;

    static constexpr int NUM_CELLS = 130;

    bool _cells[NUM_CELLS][NUM_CELLS];
    bool _backupCells[NUM_CELLS][NUM_CELLS];

    const int SCREEN_FPS = 60;
    const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

    Timer _fpsTimer;
    Timer _capTimer;

    int _countedFrames;
    int _mouseX, _mouseY;

    bool _leftBottonHold;
    bool _rightBottonHold;

    bool _canDraw;

    bool _pause;
    bool _play;
    int _speed;
    int _generation;

    float _currentTime;
};
