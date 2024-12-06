#pragma once

#include<iostream>
#include<SDL.h>
#include<imgui.h>
#include<imgui_impl_sdl.h>
#include<imgui_impl_sdlrenderer.h>
#include<string>
#include <vector>
#include "Pattern.h"

#include "Pattern.h"
#include"Timer.h"

class Window {
public:
    Window();

    ~Window();

    void gameLoop();

    static void createPopUp();

    void update();

    void render();

    void life();

    static constexpr int SCREEN_FPS = 60;
    static constexpr int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;
    static constexpr int SCREEN_WIDTH = 1000;
    static constexpr int SCREEN_HEIGHT = 768;
    static constexpr int CELL_SIZE = 6;

    int posOverideX = -1;
    int posOverideY = -1;

private:
    SDL_Window *_window;
    SDL_Renderer *_renderer;

    bool _quit;
    SDL_Event m_event;





    static constexpr int NUM_CELLS = 130;

    bool _cells[NUM_CELLS][NUM_CELLS];
    bool _backupCells[NUM_CELLS][NUM_CELLS];

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
