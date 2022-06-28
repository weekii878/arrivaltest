#pragma once

#include <map>
#include <deque>
#include <mutex>
#include <functional>
#include <condition_variable>

#include "Utils.hpp"

using Task = std::function<void()>;

class TasksQueue
{
private:
    TasksQueue(const TasksQueue &) = delete;
    TasksQueue(TasksQueue &&) = delete;
    TasksQueue &operator=(const TasksQueue &) = delete;
    TasksQueue &operator=(TasksQueue &&) = delete;

public:
    TasksQueue() = default;

    size_t size();
    void push(Task &&task);
    void push(Task &&task, size_t delay);

    void stop();
    void run();

private:
    void getReadyTasks(std::deque<Task> &tasks);

    bool m_active = true;

    std::deque<Task> m_tasks;
    std::map<Utils::TimePoint, Task> m_delayedTasks;

    std::condition_variable m_cv;
    std::mutex m_mut;
};
