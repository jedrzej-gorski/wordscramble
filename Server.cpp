#include "Server.hpp"
#include <thread>
#include <signal.h>

Server::Server() {
    matchQueue = std::make_shared<Queue>();
    loginManager = std::make_shared<UserManager>();
    descriptorPoller = std::make_shared<Poller>(epoll_create1(0));
}

void Server::run() {
    descriptorPoller->initializeServer(loginManager, matchQueue);
    std::thread closeThread(&Poller::checkForCloseable, descriptorPoller);
    std::thread pollThread(&Poller::pollEvents, descriptorPoller);
    std::thread matchmakeThread(&Queue::matchmake, matchQueue);
    closeThread.join();
    pollThread.join();
    matchmakeThread.join();
}