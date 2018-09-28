
// TODO: exception handling for tasks!!
// currently, if task throws, then exception isn't caught by worker and 
// therefore the thread will call std::terminate
// TODO: add waiting for tasks to finish - right now this is more or less 
// just fire and forget

#include <thread>
#include <vector>

#include "./threadsafe-queue.h"

class thread_pool {

public:

    thread_pool(int num_threads = std::thread::hardware_concurrency()) {

        // make at least one thread:
        num_threads = std::max(num_threads, 1);

        for (int i = 0; i < num_threads; i++) {

            try {
                threads.emplace_back(&thread_pool::worker, this);
            } catch (...) {
                clean_up();
                throw;
            }

        }

    }
    ~thread_pool() {

        clean_up();

    }

    void submit(std::function<void()> task) {

        work_queue.push(task);

    }

    int get_thread_count() {
        return threads.size();
    }

    thread_pool(const thread_pool&) = delete;
    thread_pool& operator=(const thread_pool&) = delete;

private:

    std::vector<std::thread> threads;
    threadsafe_queue<std::function<void()>> work_queue;

    void clean_up() {

        work_queue.finish();

        for (int i = 0, l = threads.size(); i < l; i++) {
            if (threads[i].joinable()) {
                threads[i].join();
            }
        }

    }

    void worker() {

        while (true) {
            std::function<void()> task;
            bool more_work = work_queue.wait_and_pop(task);
            if (!more_work) { break; }
            task();
        }

    }

};
