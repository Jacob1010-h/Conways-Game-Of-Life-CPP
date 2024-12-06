#pragma once

#include <SDL.h>

class Timer {
public:
    Timer();

    void Start();

    void Stop();

    void Pause();

    void UnPause();

    [[nodiscard]] Uint32 GetTicks() const;

    [[nodiscard]] bool IsStarted() const;

    [[nodiscard]] bool IsPaused() const;

private:
    Uint32 _startTicks;
    Uint32 _pausedTicks;

    bool _paused;
    bool _started;
};
