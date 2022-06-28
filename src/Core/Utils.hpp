#pragma once

#include <chrono>

namespace Utils
{
    using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

    inline TimePoint now()
    {
        return std::chrono::system_clock::now();
    }

    inline size_t msSinceEpoch()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(now().time_since_epoch()).count();
    }
}
