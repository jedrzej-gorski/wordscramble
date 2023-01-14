#include "EventHandler.hpp"

EventHandler::EventHandler(int socketfd, sockaddr_in socketData, std::shared_ptr<Poller>& descriptorPoller, std::shared_ptr<Queue>& matchQueue, std::shared_ptr<UserManager>& loginManager) : socketfd(socketfd), socketData(socketData), closeable(false), descriptorPoller(descriptorPoller), matchQueue(matchQueue), loginManager(loginManager) {}

int EventHandler::getSocketfd() {
    return socketfd;
}

bool EventHandler::isCloseable() {
    return closeable;
}