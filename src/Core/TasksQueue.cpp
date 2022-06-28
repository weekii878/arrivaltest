#include "TasksQueue.h"

size_t TasksQueue::size()
{
    std::lock_guard lk(m_mut);
    return m_tasks.size() + m_delayedTasks.size();
}

void TasksQueue::push(Task &&task)
{
    {
        std::lock_guard lk(m_mut);
        m_tasks.push_back(std::move(task));
    }
    m_cv.notify_all();
}

void TasksQueue::push(Task &&task, size_t delay)
{
    Utils::TimePoint tp = Utils::now() + std::chrono::milliseconds(delay);
    {
        std::lock_guard lk(m_mut);
        m_delayedTasks.insert({tp, std::move(task)});
    }
    m_cv.notify_all();
}

void TasksQueue::stop()
{
    {
        std::unique_lock lk(m_mut);
        m_active = false;
        m_tasks.clear();
        m_delayedTasks.clear();
    }
    m_cv.notify_all();
}

void TasksQueue::run()
{
    std::deque<Task> tasks;
    while (m_active)
    {
        // Blocking call.
        getReadyTasks(tasks);
        for (Task &task : tasks)
            task();
        tasks.clear();
    }
}

void TasksQueue::getReadyTasks(std::deque<Task> &tasks)
{
    Utils::TimePoint now;
    const auto condition = [this, &now]()
    {
        now = Utils::now();
        // If m_tasks is not empty
        // or there is at least one delayed task ready to be done.
        return !this->m_tasks.empty() || (!this->m_delayedTasks.empty() && this->m_delayedTasks.begin()->first <= now);
    };

    std::unique_lock lk(m_mut);

    // Get now() after lock to prevent race with pushing.
    now = Utils::now();

    while (!condition())
    {
        if (!m_active)
            return;

        // Sleep until next notify or timeout happening.
        if (!m_delayedTasks.empty())
            m_cv.wait_until(lk, m_delayedTasks.begin()->first);
        else
            m_cv.wait(lk);

        now = Utils::now();
    }

    // Get all instant tasks.
    std::swap(m_tasks, tasks);

    // Get all delayed tasks ready to be done.
    auto boundaryIt = m_delayedTasks.upper_bound(now);
    for (auto it = m_delayedTasks.begin(); it != boundaryIt; ++it)
        tasks.push_back(std::move(it->second));
    m_delayedTasks.erase(m_delayedTasks.begin(), boundaryIt);
}
