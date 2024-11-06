#pragma once
#include <cstring>


class Game {
public:
    Game() { _speed = 30; };

    static int getBoardSize() {
        return NUM_CELLS;
    }

    void setCell(const int row, const int col, const bool value) {
        _board[row][col] = value;
    }

    void setBackupCell(const int row, const int col, const bool value) {
        _backupBoard[row][col] = value;
    }

    [[nodiscard]] bool getCell(const int row, const int col) const { return _board[row][col]; }
    [[nodiscard]] bool getBackupCell(const int row, const int col) const { return _backupBoard[row][col]; }

    [[nodiscard]] bool getBoard() const { return _board; };
    [[nodiscard]] bool getBackupBoard() const { return _backupBoard; };

    void resetCellsMemAlloc() { memset(_board, 0, sizeof(_board)); };

private:
    static constexpr int NUM_CELLS = 130;

    bool _board[NUM_CELLS][NUM_CELLS]{};
    bool _backupBoard[NUM_CELLS][NUM_CELLS]{};

    int _speed;
};
