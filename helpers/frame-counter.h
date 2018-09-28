
#pragma once

#include <chrono>
#include <vector>

template <size_t bufferSize = 10>
class FrameCounter {

public:
    FrameCounter(): start(std::chrono::steady_clock::now()), frameTimes(bufferSize), frameCount(0), cursor(0) {}

    void frame() {

        frameTimes[cursor] = std::chrono::steady_clock::now();
        frameCount++;
        cursor = (cursor + 1) % bufferSize;

    }

    int getCurrentFPS() {
        int previousCursor = (cursor == 0 ? bufferSize - 1 : cursor - 1);
        auto timeTaken = std::chrono::duration_cast<std::chrono::milliseconds>(frameTimes[previousCursor] - frameTimes[cursor]).count();
        return (1000 * (bufferSize - 1)) / timeTaken;
    }

    int getTotalFPS() {

        auto timeTaken = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
        return (1000 * frameCount) / timeTaken;

    }

private:
    std::chrono::time_point<std::chrono::steady_clock> start;
    std::vector<std::chrono::time_point<std::chrono::steady_clock>> frameTimes;
    int frameCount;
    int cursor;

};
