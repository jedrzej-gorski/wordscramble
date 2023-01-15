#include "Poller.hpp"
#include "EventHandler.hpp"
#include "networkingConstants.hpp"
#include <iterator>

Poller::Poller(int pollfd) : pollfd(pollfd) {
    
}

void Poller::pollEvents() {
    while (true) {
        epoll_event events[networkingConstants::MAX_EVENTS];
        std::lock_guard<std::mutex> collectorLock(mutex_collectors);
        int nfds = epoll_wait(pollfd, static_cast<epoll_event*>(events), networkingConstants::MAX_EVENTS, 0);
        if (nfds < 0) {
            return;
        }
        for (int i = 0; i < nfds; i++) {
            static_cast<EventHandler*>(events[i].data.ptr)->handleReadEvent();
        }
    }
}

void Poller::addHandler(epoll_event& handledEvent) {


    epoll_ctl(pollfd, EPOLL_CTL_ADD, static_cast<EventHandler*>(handledEvent.data.ptr)->getSocketfd(), &handledEvent);

    // Add new shared_ptr from the pointer in the handledEvent structure
    std::shared_ptr<EventHandler> newPointer(static_cast<EventHandler*>(handledEvent.data.ptr));
    std::lock_guard<std::mutex> lock(mutex_handlers);
    descriptorHandlers.push_back(std::move(newPointer));
}

void Poller::removeHandler(std::vector<std::shared_ptr<EventHandler>>::iterator& it) {
    epoll_ctl(pollfd, EPOLL_CTL_DEL, (*it)->getSocketfd(), nullptr);
}

void Poller::checkForCloseable() {
    std::lock_guard<std::mutex> collectorLock(mutex_collectors);
    std::lock_guard<std::mutex> lock(mutex_handlers);
    auto it = descriptorHandlers.begin();
    // Iterate over all active descriptors and erase the pointers to those, which are closeable
    while (it != descriptorHandlers.end()) {
        if ((*it)->isCloseable()) {
            removeHandler(it);
            descriptorHandlers.erase(it);
        }
        else {
            ++it;
        }
    }
}