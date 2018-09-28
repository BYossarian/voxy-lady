
// a poor man's https://en.cppreference.com/w/cpp/experimental/latch

#pragma once

#include <atomic>
#include <future>

class latch {

public:

    latch(int count): counter(count), latch_future(latch_promise.get_future()) {

        if (count <= 0) {
            throw;
        }

    }

    void count_down() {

        counter--;
        if (counter == 0) {
            latch_promise.set_value();
        }

    }

    void count_down_and_wait() {

        count_down();
        latch_future.wait();

    }

    bool is_ready() {

        return (counter == 0);

    }

    void wait() {

        latch_future.wait();

    }

private:

    std::atomic<int> counter;
    std::promise<void> latch_promise;
    std::future<void> latch_future;

};
