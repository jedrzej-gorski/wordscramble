#pragma once
#include "EventHandler.hpp"
#include <vector>
#include <memory>
#include <MutexCollector.hpp>
#include <sys/epoll.h>
#include <mutex>

class Poller : public MutexCollector {
    private:
        int pollfd;
        std::mutex mutex_handlers;
        std::vector<std::shared_ptr<EventHandler>> descriptorHandlers;
    protected:
        void removeHandler(std::vector<std::shared_ptr<EventHandler>>::iterator& handlerIndex);
    public:
        Poller(int pollfd);
        void initializeServer(std::shared_ptr<UserManager>& loginManager, std::shared_ptr<Queue>& matchQueue);
        void pollEvents();
        void addHandler(epoll_event& handledEvent);
        void checkForCloseable();
        void shutdownAll();
};