#define BOOST_TEST_MODULE testNet
#include <boost/test/included/unit_test.hpp>

#include <thread>
#include <chrono>

#include "../Core/TasksQueue.h"

using namespace std::chrono_literals;

BOOST_AUTO_TEST_CASE(testTasksQueueSimple)
{
    TasksQueue tq;
    std::thread th([&tq]()
                   { tq.run(); });
    int n = 0;

    tq.push([&n]()
            { ++n; });
    std::this_thread::sleep_for(5ms);
    BOOST_CHECK_EQUAL(n, 1);
    BOOST_CHECK_EQUAL(tq.size(), 0);

    for (int i = 0; i != 1000; ++i)
        tq.push([&n]()
                { ++n; });
    std::this_thread::sleep_for(10ms);
    BOOST_CHECK_EQUAL(n, 1001);
    BOOST_CHECK_EQUAL(tq.size(), 0);

    for (int i = 0; i != 10000; ++i)
        tq.push([&n]()
                { ++n; });
    for (int i = 0; i != 10000; ++i)
        tq.push([&n]()
                { ++n; },
                100);
    std::this_thread::sleep_for(120ms);
    BOOST_CHECK_EQUAL(n, 21001);
    BOOST_CHECK_EQUAL(tq.size(), 0);

    tq.stop();
    th.join();
}

BOOST_AUTO_TEST_CASE(testTasksQueueDelayed)
{
    TasksQueue tq;
    std::thread th([&tq]()
                   { tq.run(); });
    int n = 0;

    tq.push([&n]()
            { ++n; },
            100);
    tq.push([&n]()
            { ++n; },
            150);
    tq.push([&n]()
            { ++n; },
            200);
    tq.push([&n]()
            { ++n; },
            250);
    tq.push([&n]()
            { ++n; },
            300);
    tq.push([&n]()
            { ++n; },
            100);
    BOOST_CHECK_EQUAL(tq.size(), 6);

    // 100ms x2 ended
    std::this_thread::sleep_for(110ms);
    BOOST_CHECK_EQUAL(n, 2);
    BOOST_CHECK_EQUAL(tq.size(), 4);

    // 150ms ended
    std::this_thread::sleep_for(50ms);
    BOOST_CHECK_EQUAL(n, 3);
    BOOST_CHECK_EQUAL(tq.size(), 3);

    // 200ms ended
    std::this_thread::sleep_for(50ms);
    BOOST_CHECK_EQUAL(n, 4);
    BOOST_CHECK_EQUAL(tq.size(), 2);

    // 250ms ended
    std::this_thread::sleep_for(50ms);
    BOOST_CHECK_EQUAL(n, 5);
    BOOST_CHECK_EQUAL(tq.size(), 1);

    // 300ms ended
    std::this_thread::sleep_for(50ms);
    BOOST_CHECK_EQUAL(n, 6);
    BOOST_CHECK_EQUAL(tq.size(), 0);

    tq.stop();
    th.join();
}

BOOST_AUTO_TEST_CASE(testTasksQueueStop)
{
    TasksQueue tq;
    std::thread th([&tq]()
                   { tq.run(); });

    for (int i = 0; i != 10000; ++i)
        tq.push([]() {}, 100);
    for (int i = 0; i != 10000; ++i)
        tq.push([]() {});
    tq.stop();
    BOOST_CHECK_EQUAL(tq.size(), 0);

    th.join();
}
