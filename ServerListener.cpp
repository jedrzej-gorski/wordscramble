#include "ServerListener.hpp"
#include "ClientConnection.hpp"

ServerListener::ServerListener(int socketfd, sockaddr_in socketData, int localport, std::shared_ptr<Poller>& descriptorPoller, std::shared_ptr<Queue>& matchQueue, std::shared_ptr<UserManager>& loginManager) : EventHandler(socketfd, socketData, descriptorPoller, matchQueue, loginManager), localport(localport) {}

void ServerListener::handleReadEvent() {
    socklen_t socketDataSize = sizeof(socketData);
    int newSocket = accept(socketfd, (sockaddr*)&this->socketData, &socketDataSize);

    epoll_event newEvent;
    newEvent.events = EPOLLIN;
    // The pointer to the ClientConnection instance will be turned into a shared_ptr by the descriptorPoller
    newEvent.data.ptr = new ClientConnection(newSocket, socketData, descriptorPoller, matchQueue, loginManager);
    descriptorPoller->addHandler(newEvent);
}