#include "Poller.hpp"
#include "EventHandler.hpp"
#include <iterator>

Poller::Poller(int pollfd) : pollfd(pollfd) {
    // Implement creation of the ServerListener
}

// TODO: Implement - finish whatever's going on here
void Poller::pollEvents() {
    std::lock_guard<std::mutex> collectorLock(mutex_collectors);
    for (const auto& handlerPtr : descriptorHandlers) {
        handlerPtr->handleReadEvent();
    }
}

void Poller::addHandler(epoll_event& handledEvent) {


    epoll_ctl(pollfd, EPOLL_CTL_ADD, static_cast<EventHandler*>(handledEvent.data.ptr)->getSocketfd(), &handledEvent);

    // Add new shared_ptr from the pointer in the handledEvent structure
    std::shared_ptr<EventHandler> newPointer(static_cast<EventHandler*>(handledEvent.data.ptr));
    std::lock_guard<std::mutex> lock(mutex_handlers);
    descriptorHandlers.push_back(std::move(newPointer));
}

void Poller::checkForCloseable() {
    std::lock_guard<std::mutex> collectorLock(mutex_collectors);
    std::lock_guard<std::mutex> lock(mutex_handlers);
    auto it = descriptorHandlers.begin();
    // Iterate over all active descriptors and erase the pointers to those, which are closeable
    while (it != descriptorHandlers.end()) {
        if ((*it)->isCloseable()) {
            descriptorHandlers.erase(it);
        }
        else {
            ++it;
        }
    }
}