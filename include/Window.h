#pragma once

#include<iostream>
#include<SDL.h>
#include<imgui.h>
#include<imgui_impl_sdl.h>
#include<imgui_impl_sdlrenderer.h>
#include <sstream>
#include<string>
#include <vector>
#include"Timer.h"

struct Pattern {
    std::vector<std::vector<bool> > pattern;
    int numRows = 0;
    int numCols = 0;
    std::string title;
    std::vector<std::string> description;
    std::string rule;
    int positionX = 0;
    int positionY = 0;


    Pattern() = default;


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

            lines.push_back(line);
            maxCols = std::max(maxCols, static_cast<int>(line.size()));
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

class Window {
public:
    Window();

    ~Window();

    void gameLoop();

    void checkIfDrawing();

    void update();

    void render();

    void life();

    static constexpr int SCREEN_FPS = 60;
    static constexpr int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;
    static constexpr int SCREEN_WIDTH = 1000;
    static constexpr int SCREEN_HEIGHT = 768;
    static constexpr int CELL_SIZE = 6;
    static constexpr int NUM_CELLS = 130;

    static auto placePattern(Pattern pattern, bool cells[NUM_CELLS][NUM_CELLS], ImVec2 posOverride) -> void;

    static auto read_file(std::string_view path) -> std::string;

    void createPausedButtons();

    void createPausedButtons() const;

    void createStartingButtons() const;

    void createButtons();

    void createPlayPauseControls();


    int posOverideX = -1;
    int posOverideY = -1;

private:
    SDL_Window *_window;
    SDL_Renderer *_renderer;

    bool _quit;
    SDL_Event m_event;

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
