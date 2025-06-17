/*
 *  Filename:   CountDownLatchTest.cpp
 *  Author:     xiebaoma
 *  Date:       2025-06-16
 *              2025-06-17
 *  Description:test countdown and thread
 *  test:       g++ CountDownLatchTest.cpp ../base/CountDownLatch.cpp -o test.exe
 */

#include <iostream>
#include <thread>
#include <vector>
#include "../base/CountDownLatch.h"

void worker(int id, CountDownLatch &latch)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100 * id));
    std::cout << "Worker " << id << " finished work.\n";
    latch.countDown();
}

int main()
{
    const int numThreads = 5;
    CountDownLatch latch(numThreads);
    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(worker, i + 1, std::ref(latch));
    }

    std::cout << "Main thread is waiting for workers...\n";
    latch.wait();
    std::cout << "All workers are done. Main thread continues.\n";

    for (auto &t : threads)
    {
        t.join();
    }

    return 0;
}
