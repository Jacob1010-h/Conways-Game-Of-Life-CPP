#include"Game.h"

Game::Game()
    : _quit(false), _countedFrames(0), _leftBottonHold(false), _rightBottonHold(false), _canDraw(false), _pause(false),
      _speed(30) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;

    _window = SDL_CreateWindow("Cellular Automaton", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH,
                               SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (_window == nullptr)
        std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;

    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);

    if (_renderer == nullptr)
        std::cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(_window, _renderer);
    ImGui_ImplSDLRenderer_Init(_renderer);

    _fpsTimer.Start();

    _generation = 0;
    _play = false;
    _currentTime = 0;
}

Game::~Game() {
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);

    SDL_Quit();
}

void Game::GameLoop() {
    while (!_quit) {
        _capTimer.Start();

        while (SDL_PollEvent(&m_event) != 0) {
            ImGui_ImplSDL2_ProcessEvent(&m_event);
            if (m_event.type == SDL_QUIT)
                _quit = true;
            if (m_event.type == SDL_MOUSEBUTTONDOWN) {
                if (m_event.button.button == SDL_BUTTON_LEFT) {
                    _leftBottonHold = true;
                } else if (m_event.button.button == SDL_BUTTON_RIGHT) {
                    _rightBottonHold = true;
                }
            }

            if (m_event.type == SDL_MOUSEBUTTONUP) {
                if (m_event.button.button == SDL_BUTTON_LEFT)
                    _leftBottonHold = false;
                else if (m_event.button.button == SDL_BUTTON_RIGHT)
                    _rightBottonHold = false;
            }
        }

        float avgFps = _countedFrames / (_fpsTimer.GetTicks() / 1000.f);
        if (avgFps > 2000000)
            avgFps = 0.f;

        Update();
        Render();

        ++_countedFrames;

        int framesTicks = _capTimer.GetTicks();
        if (framesTicks < SCREEN_TICKS_PER_FRAME) {
            SDL_Delay(SCREEN_TICKS_PER_FRAME - framesTicks);
        }
    }
}

void Game::Update() {
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Cellular Automaton", 0,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

    ImGui::SetWindowPos(ImVec2(128 * 6, 0));
    ImGui::SetWindowSize(ImVec2(238, 768));

    //buttons
    ImGui::NewLine();
    ImGui::NewLine();
    ImGui::NewLine();
    ImGui::SameLine(238.f / 2.f - 35);

    ImGui::Text("Generation");
    std::string str = std::to_string(_generation);
    ImGui::NewLine();
    ImGui::SameLine(238.f / 2.f - 10);
    ImGui::Text(str.c_str());

    ImGui::NewLine();
    ImGui::NewLine();
    ImGui::NewLine();
    ImGui::SameLine(238.f / 2.f - 30);

    if (!_pause) {
        if (ImGui::Button("Run", ImVec2(60, 30))) {
            _pause = true;
            _play = true;
        }
    } else {
        if (ImGui::Button("Pause", ImVec2(60, 30))) {
            _play = false;
            _pause = false;
        }
    }

    if (_play) {
        _currentTime += (float) SCREEN_TICKS_PER_FRAME / 1000;
        if (_currentTime >= 1.f / _speed) {
            Life();
            _currentTime = 0.f;
        }
    }

    ImGui::NewLine();
    ImGui::NewLine();
    ImGui::NewLine();
    ImGui::SameLine(238.f / 2.f - 30);

    if (ImGui::Button("Step", ImVec2(60, 30))) {
        Life();
    }

    ImGui::NewLine();
    ImGui::NewLine();
    ImGui::NewLine();
    ImGui::SameLine(238.f / 2.f - 30);

    if (ImGui::Button("Reset", ImVec2(60, 30))) {
        memset(_cells, 0, sizeof(_cells));
        _generation = 0;
    }

    ImGui::NewLine();
    ImGui::NewLine();
    ImGui::NewLine();
    ImGui::SameLine(238.f / 2.f - 20);
    ImGui::Text("Speed");
    ImGui::NewLine();
    ImGui::SameLine(238.f / 2.f - 75);
    ImGui::SliderInt(" ", &_speed, 0, 100);


    if (ImGui::IsWindowHovered() || ImGui::IsAnyItemActive())
        _canDraw = false;
    else
        _canDraw = true;

    ImGui::End();

    if (_canDraw) {
        if (_leftBottonHold)
            _cells[_mouseY / CELL_SIZE][_mouseX / CELL_SIZE] = 1;
        else if (_rightBottonHold)
            _cells[_mouseY / CELL_SIZE][_mouseX / CELL_SIZE] = 0;
    }
}

void Game::Render() {
    ImGui::Render();

    SDL_Rect vertical_rect = {0, 0, 1, SCREEN_HEIGHT};
    SDL_Rect horizontal_rect = {0, 0, SCREEN_WIDTH, 1};


    SDL_RenderClear(_renderer);
    SDL_SetRenderDrawColor(_renderer, 40, 40, 40, 255);


    for (int i = 0; i < NUM_CELLS; i++) {
        vertical_rect.x = (CELL_SIZE * i) - 1;
        SDL_RenderDrawRect(_renderer, &vertical_rect);

        horizontal_rect.y = (CELL_SIZE * i) - 1;
        SDL_RenderDrawRect(_renderer, &horizontal_rect);
    }

    SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);

    //mouse pos rect
    SDL_GetMouseState(&_mouseX, &_mouseY);
    _mouseX -= _mouseX % CELL_SIZE;
    _mouseY -= _mouseY % CELL_SIZE;
    SDL_Rect cell_rect = {_mouseX, _mouseY, CELL_SIZE - 1, CELL_SIZE - 1};
    if (_canDraw)
        SDL_RenderFillRect(_renderer, &cell_rect);

    //draw rect
    for (int y = 0; y < NUM_CELLS; y++) {
        for (int x = 0; x < NUM_CELLS; x++) {
            if (_cells[y][x] == 1) {
                cell_rect = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE - 1, CELL_SIZE - 1};
                SDL_RenderFillRect(_renderer, &cell_rect);
            }
        }
    }

    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);

    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

    SDL_RenderPresent(_renderer);
}

void Game::Life() {
    _generation++;
    for (int y = 0; y < NUM_CELLS; y++) {
        for (int x = 0; x < NUM_CELLS; x++) {
            _backupCells[y][x] = _cells[y][x];
        }
    }

    int neighbours = 0;
    for (int y = 0; y < NUM_CELLS; y++) {
        for (int x = 0; x < NUM_CELLS; x++) {
            neighbours = _backupCells[y - 1][x - 1] + _backupCells[y - 1][x] + _backupCells[y - 1][x + 1] +
                         _backupCells[y][x - 1] + _backupCells[y][x + 1] +
                         _backupCells[y + 1][x - 1] + _backupCells[y + 1][x] + _backupCells[y + 1][x + 1];

            if (neighbours < 2)
                _cells[y][x] = 0;
            else if (neighbours > 3)
                _cells[y][x] = 0;
            else if (neighbours == 3)
                _cells[y][x] = 1;
        }
    }
}
