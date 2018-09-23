
#pragma once

#include <chrono>

template <typename T = std::chrono::milliseconds>
class Timer {

public:
    Timer(): start(std::chrono::steady_clock::now()) {}

    void reset() {
        start = std::chrono::steady_clock::now();
    }

    int getTicks() {
        return std::chrono::duration_cast<T>(std::chrono::steady_clock::now() - start).count();
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> start;

};
