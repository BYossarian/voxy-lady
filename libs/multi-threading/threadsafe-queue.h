
// a threadsafe_queue - mainly built with the use-case of a work queue in mind

// TODO: think about destructor - what will happen if it's called whilst threads are waiting?
// TODO: maintain a count of waiting threads?
// TODO: add flush member function that will block until the queue is empty, or until no 
// threads are waiting? (NB: in the case of a work queue - this would only mean all work 
// has been taken up by some thread, not that all work has completed)

#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class threadsafe_queue {

public:

    threadsafe_queue() {};
    ~threadsafe_queue() {};

    void push(T value) {

        {
            std::lock_guard lock(queue_mutex);
            if (finished) { throw; }
            data_queue.push(std::move(value));
        }
        is_empty.notify_one();

    }

    bool try_and_pop(T &return_value) {

        std::lock_guard lock(queue_mutex);

        if (data_queue.empty()) {
            return false;
        }

        return_value = std::move(data_queue.front());
        data_queue.pop();
        return true;

    }

    bool wait_and_pop(T &return_value) {

        std::unique_lock lock(queue_mutex);

        is_empty.wait(lock, [this]() { return !data_queue.empty() || finished; });

        if (data_queue.empty()) { return false; }

        return_value = std::move(data_queue.front());
        data_queue.pop();
        return true;

    }

    // mark that no more items will be added, so consumers know 
    // not to keep waiting/consuming
    void finish() {

        {
            std::lock_guard lock(queue_mutex);
            finished = true;
        }
        is_empty.notify_all();

    }

    threadsafe_queue(const threadsafe_queue&) = delete;
    threadsafe_queue& operator=(const threadsafe_queue&) = delete;

private:

    std::queue<T> data_queue;
    std::mutex queue_mutex;
    std::condition_variable is_empty;
    bool finished = false;

};
