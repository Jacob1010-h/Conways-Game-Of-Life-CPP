#include"Window.h"

#include <fstream>
#include <nfd.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

struct Pattern {
    std::vector<std::vector<bool> > pattern;
    int numRows = 0;
    int numCols = 0;
    std::string title;
    std::vector<std::string> description;
    std::string rule;
    int positionX = 0;
    int positionY = 0;


    Pattern() {
    }


    ~Pattern() {
        pattern.clear();
    }

    [[nodiscard]] bool getCell(const int x, const int y) const {
        return pattern[x][y];
    }

    void setCell(const int x, const int y, const bool value) {
        pattern[x][y] = value;
    }


    void toPattern(const std::string &patternData) {
        pattern.clear();
        description.clear();
        rule.clear();
        title.clear();
        positionX = 0;
        positionY = 0;

        std::istringstream stream(patternData);
        std::string line;
        int maxCols = 0;


        std::vector<std::string> lines;
        bool readingPattern = false;

        while (std::getline(stream, line)) {
            line = line.substr(0, line.find_last_not_of(" \n\r\t") + 1);


            if (line.empty()) {
                continue;
            }


            if (line[0] == '#' && line.find("Life 1.05") != std::string::npos) {
                title = line;
                continue;
            }


            if (line[0] == '#' && line.find("#D") != std::string::npos) {
                description.push_back(line.substr(2));
                continue;
            }


            if (line[0] == '#' && line.find("#R") != std::string::npos) {
                rule = line.substr(2);
                continue;
            } else if (line[0] == '#' && line.find("#N") != std::string::npos) {
                rule = "#R 23/3";
                continue;
            }


            if (line[0] == '#' && line.find("#P") != std::string::npos) {
                std::istringstream positionStream(line.substr(2));
                positionStream >> positionX >> positionY;
                continue;
            }


            readingPattern = true;
            if (readingPattern) {
                lines.push_back(line);
                maxCols = std::max(maxCols, static_cast<int>(line.size()));
            }
        }


        numRows = lines.size();
        numCols = maxCols;


        pattern.resize(numRows, std::vector<bool>(numCols, false));


        for (int y = 0; y < numRows; ++y) {
            const std::string &line = lines[y];
            for (int x = 0; x < line.size(); ++x) {
                if (line[x] == '*') {
                    setCell(x, y, true);
                } else if (line[x] == '.') {
                    setCell(x, y, false);
                }
            }
        }

    }


    void printPattern() const {
        for (const auto &row: pattern) {
            for (bool cell: row) {
                std::cout << (cell ? '*' : '.') << " ";
            }
            std::cout << '\n';
        }
    }


    void printSize() const {
        std::cout << "Rows: " << numRows << ", Columns: " << numCols << std::endl;
    }


    void printMetadata() const {
        std::cout << "Title: " << title << std::endl;
        for (const auto &line: description) {
            std::cout << "Description: " << line << std::endl;
        }
        std::cout << "Rule: " << rule << std::endl;
        std::cout << "Position: (" << positionX << ", " << positionY << ")\n";
    }
};

Window::Window()
    : _quit(false), m_event(), _cells{}, _backupCells{}, _countedFrames(0), _mouseX(0), _mouseY(0),
      _leftBottonHold(false),
      _rightBottonHold(false),
      _canDraw(false),
      _pause(false),
      _speed(15) {

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

Window::~Window() {
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);

    SDL_Quit();
}

void Window::gameLoop() {
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

        float avgFps = static_cast<float>(_countedFrames) / (static_cast<float>(_fpsTimer.GetTicks()) / 1000.f);
        if (avgFps > 2000000)
            avgFps = 0.f;

        update();
        render();

        ++_countedFrames;

        int framesTicks = static_cast<int>(_capTimer.GetTicks());
        if (framesTicks < SCREEN_TICKS_PER_FRAME) {
            SDL_Delay(SCREEN_TICKS_PER_FRAME - framesTicks);
        }
    }
}

void placePattern(Pattern pattern, bool cells[130][130], const ImVec2 posOverride) {
    // Loop through the pattern and copy it into the `cells` grid
    if (posOverride.x != -1 && posOverride.y != -1) {
        pattern.positionX = posOverride.x;
        pattern.positionY = posOverride.y;
    }

    for (int y = 0; y < pattern.numRows; ++y) {
        for (int x = 0; x < pattern.numCols; ++x) {
            // Calculate the target position in the cells grid
            int targetX = pattern.positionX + x;
            int targetY = pattern.positionY + y;

            // Ensure the target position is within bounds
            if (targetX >= 0 && targetX < 130 && targetY >= 0 && targetY < 130) {
                cells[targetY][targetX] = pattern.getCell(x, y);
            }
        }
    }
}

auto read_file(std::string_view path) -> std::string {
    constexpr auto read_size = std::size_t(4096);
    auto stream = std::ifstream(path.data());
    stream.exceptions(std::ios_base::badbit);

    if (not stream) {
        throw std::ios_base::failure("file does not exist");
    }

    auto out = std::string();
    auto buf = std::string(read_size, '\0');
    while (stream.read(& buf[0], read_size)) {
        out.append(buf, 0, stream.gcount());
    }
    out.append(buf, 0, stream.gcount());
    return out;
}

void Window::update() {
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Cellular Automaton", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

    ImGui::SetWindowPos(ImVec2(128 * 6, 0));
    ImGui::SetWindowSize(ImVec2(238, static_cast<float>(Window::SCREEN_HEIGHT)));

    //buttons
    ImGui::NewLine();
    ImGui::NewLine();
    ImGui::NewLine();
    ImGui::SameLine(238.f / 2.f - 35);

    ImGui::Text("Generation");
    const std::string str = std::to_string(_generation);
    ImGui::NewLine();
    ImGui::SameLine(238.f / 2.f - 10);
    ImGui::Text(str.c_str());

    if (!_pause) {
        ImGui::NewLine();
        ImGui::NewLine();
        ImGui::NewLine();
        ImGui::SameLine(238.f / 2.f - 80);
        ImGui::InputInt("X", &posOverideX);

        ImGui::NewLine();
        ImGui::SameLine(238.f / 2.f - 80);
        ImGui::InputInt("Y", &posOverideY);

        ImGui::NewLine();
        ImGui::SameLine(238.f / 2.f - 60);
        if (ImGui::Button("Import Pattern", ImVec2(120, 30))) {
            nfdchar_t *outPath = NULL;
            nfdresult_t result = NFD_OpenDialog( NULL, NULL, &outPath );

            if ( result == NFD_OKAY ) {
                puts("Success!");
                puts(outPath);
                Pattern p;
                p.toPattern(read_file(outPath));
                placePattern(p, _cells, ImVec2(static_cast<float>(posOverideX), static_cast<float>(posOverideY)));
                free(outPath);
                posOverideX = -1;
                posOverideY = -1;
            }
            else if ( result == NFD_CANCEL ) {
                puts("User pressed cancel.");
            }
            else {
                printf("Error: %s\n", NFD_GetError() );
            }
        }
    }
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
        _currentTime += static_cast<float>(Window::SCREEN_TICKS_PER_FRAME) / 1000;
        if (_currentTime >= 1.f / static_cast<float>(_speed)) {
            life();
            _currentTime = 0.f;
        }
    }

    ImGui::NewLine();
    ImGui::NewLine();
    ImGui::NewLine();
    ImGui::SameLine(238.f / 2.f - 30);

    if (ImGui::Button("Step", ImVec2(60, 30))) {
        life();
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
        const bool mouseInside = _mouseX >= 0 && _mouseY >= 0 && _mouseX < SCREEN_HEIGHT && _mouseY < SCREEN_WIDTH;
        if (_leftBottonHold) {
            if (mouseInside) {
                const int x = _mouseY / CELL_SIZE;
                const int y = _mouseX / CELL_SIZE;
                _cells[x][y] = true;
            }
        } else if (_rightBottonHold) {
            if (mouseInside) {
                const int x = _mouseY / CELL_SIZE;
                const int y = _mouseX / CELL_SIZE;
                _cells[x][y] = false;
            }
        }
    }
}

void Window::render() {
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

void Window::life() {
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
                _cells[y][x] = false;
            else if (neighbours > 3)
                _cells[y][x] = false;
            else if (neighbours == 3)
                _cells[y][x] = true;
        }
    }
}
