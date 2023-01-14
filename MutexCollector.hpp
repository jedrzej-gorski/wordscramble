#pragma once
#include <mutex>

class MutexCollector {
    protected:
        static std::mutex mutex_collectors;
};