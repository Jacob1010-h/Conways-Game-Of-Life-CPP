#include "Window.h"

Game gameOfLife;

Window::Window()
    : _quit(false), _countedFrames(0), _leftBottonHold(false), _rightBottonHold(false), _canDraw(false) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;

    window = SDL_CreateWindow("Conways Game Of Life", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH,
                              SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (window == nullptr)
        std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == nullptr)
        std::cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer_Init(renderer);

    _fpsTimer.Start();

    _currentTime = 0;
}

Window::~Window() {
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
}

void Window::checkQuit() {
    if (m_event.type == SDL_QUIT)
        _quit = true;
}

void Window::refresh() {
    while (!_quit) {
        _capTimer.Start();

        while (SDL_PollEvent(&m_event) != 0) {
            ImGui_ImplSDL2_ProcessEvent(&m_event);

            checkQuit();

            if (m_event.type == SDL_MOUSEBUTTONDOWN) {
                if (m_event.button.button == SDL_BUTTON_LEFT)
                    _leftBottonHold = true;

                else if (m_event.button.button == SDL_BUTTON_RIGHT)

                    _rightBottonHold = true;
            }

            if (m_event.type == SDL_MOUSEBUTTONUP) {
                if (m_event.button.button == SDL_BUTTON_LEFT)
                    _leftBottonHold = false;
                else if (m_event.button.button == SDL_BUTTON_RIGHT)
                    _rightBottonHold = false;
            }
        }

        if (float avgFps = _countedFrames / (_fpsTimer.GetTicks() / 1000.f); avgFps > 2000000)
            avgFps = 0.f;

        update();
        render();

        ++_countedFrames;

        if (const int framesTicks = _capTimer.GetTicks(); framesTicks < SCREEN_TICKS_PER_FRAME) {
            SDL_Delay(SCREEN_TICKS_PER_FRAME - framesTicks);
        }
    }
}

void Window::update() {
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Conways Game Of Life Control", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

    ImGui::SetWindowPos(ImVec2(128 * 6, 0));
    ImGui::SetWindowSize(ImVec2(238, 768));

    // buttons
    ImGui::NewLine();
    ImGui::NewLine();
    ImGui::NewLine();
    ImGui::SameLine(238.f / 2.f - 35);

    ImGui::NewLine();
    ImGui::NewLine();
    ImGui::NewLine();
    ImGui::SameLine(238.f / 2.f - 30);

    ImGui::NewLine();
    ImGui::NewLine();
    ImGui::NewLine();
    ImGui::SameLine(238.f / 2.f - 30);

    if (ImGui::Button("Reset", ImVec2(60, 30))) {
        gameOfLife.resetCellsMemAlloc();
    }

    if (ImGui::IsWindowHovered() || ImGui::IsAnyItemActive())
        _canDraw = false;
    else
        _canDraw = true;

    ImGui::End();

    if (_canDraw) {
        if (_leftBottonHold) {
            const int x = _mouseY / CELL_SIZE;
            const int y = _mouseX / CELL_SIZE;
            gameOfLife.setCell(x, y, true);
        }
        else if (_rightBottonHold) {
            const int x = _mouseY / CELL_SIZE;
            const int y = _mouseX / CELL_SIZE;
            gameOfLife.setCell(x, y, false);
        }
    }
}

void Window::render() {
    ImGui::Render();

    SDL_Rect vertical_rect = {0, 0, 1, SCREEN_HEIGHT};
    SDL_Rect horizontal_rect = {0, 0, SCREEN_WIDTH, 1};

    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);

    for (int i = 0; i < gameOfLife.getBoardSize(); i++) {
        vertical_rect.x = (CELL_SIZE * i) - 1;
        SDL_RenderDrawRect(renderer, &vertical_rect);

        horizontal_rect.y = (CELL_SIZE * i) - 1;
        SDL_RenderDrawRect(renderer, &horizontal_rect);
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // mouse pos rect
    SDL_GetMouseState(&_mouseX, &_mouseY);
    _mouseX -= _mouseX % CELL_SIZE;
    _mouseY -= _mouseY % CELL_SIZE;
    SDL_Rect cell_rect = {_mouseX, _mouseY, CELL_SIZE - 1, CELL_SIZE - 1};
    if (_canDraw)
        SDL_RenderFillRect(renderer, &cell_rect);

    // draw rect
    for (int row = 0; row < gameOfLife.getBoardSize(); row++) {
        for (int col = 0; col < gameOfLife.getBoardSize(); col++) {
            if (gameOfLife.getCell(row,col) == 1) {
                cell_rect = {col * CELL_SIZE, row * CELL_SIZE, CELL_SIZE - 1, CELL_SIZE - 1};
                SDL_RenderFillRect(renderer, &cell_rect);
            }
        }
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

    SDL_RenderPresent(renderer);
}
