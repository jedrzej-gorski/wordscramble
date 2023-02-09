#include "Poller.hpp"
#include "EventHandler.hpp"
#include "ServerListener.hpp"
#include "networkingConstants.hpp"
#include <thread>
#include <chrono>
#include <iterator>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <error.h>
#include <errno.h>
#include <sys/types.h>
#include <arpa/inet.h>


Poller::Poller(int pollfd) : pollfd(pollfd) {}


void Poller::initializeServer(std::shared_ptr<UserManager>& loginManager, std::shared_ptr<Queue>& matchQueue) {
    int servFd = socket(AF_INET, SOCK_STREAM, 0);
    if (servFd == -1) {
        error(1, errno, "socket failed");
    }
    int opt;
    setsockopt(servFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in serverAddr{.sin_family=AF_INET, .sin_port=htons(static_cast<short>(networkingConstants::LOCALPORT)), .sin_addr={INADDR_ANY}};
    int res = bind(servFd, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));

    if (res) {
        error(1, errno, "bind failed");
    }

    res = listen(servFd, 1);
    if (res) {
        error(1, errno, "listen failed");
    }

    std::shared_ptr<Poller> thisPointer(this);

    auto listener = new ServerListener(servFd, serverAddr, thisPointer, matchQueue, loginManager);
    epoll_event server_event;
    server_event.events = EPOLLIN;
    server_event.data.ptr = static_cast<void*>(listener);
    addHandler(server_event);
}

void Poller::pollEvents() {
    while (true) {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(10ms);
        epoll_event events[networkingConstants::MAX_EVENTS];
        std::lock_guard<std::mutex> collectorLock(MutexCollector::mutex_collectors);
        int nfds = epoll_wait(pollfd, static_cast<epoll_event*>(events), networkingConstants::MAX_EVENTS, 0);
        if (nfds < 0) {
            continue;
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
    int clientSocket = (*it)->getSocketfd();
    epoll_ctl(pollfd, EPOLL_CTL_DEL, clientSocket, nullptr);
    shutdown(clientSocket, SHUT_RDWR);
    close(clientSocket);
    
}

void Poller::checkForCloseable() {
    while (true) {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(10ms);
        std::lock_guard<std::mutex> collectorLock(MutexCollector::mutex_collectors);
        std::lock_guard<std::mutex> lock(mutex_handlers);
        auto it = descriptorHandlers.begin();
        // Iterate over all active descriptors and erase the pointers to those, which are closeable
        while (it != descriptorHandlers.end()) {
            if ((*it)->isCloseable()) {
                removeHandler(it);
                descriptorHandlers.erase(it);
            }
            else {
                std::advance(it, 1);
            }
        }
    }
}

void Poller::shutdownAll() {
    auto it = descriptorHandlers.begin();
    while (it != descriptorHandlers.end()) {
        removeHandler(it);
        descriptorHandlers.erase(it);
    }
    exit(0);
}