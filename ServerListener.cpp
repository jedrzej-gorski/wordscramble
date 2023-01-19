#include "ServerListener.hpp"
#include "ClientConnection.hpp"
#include <arpa/inet.h>

ServerListener::ServerListener(int socketfd, sockaddr_in socketData, std::shared_ptr<Poller>& descriptorPoller, std::shared_ptr<Queue>& matchQueue, std::shared_ptr<UserManager>& loginManager) : EventHandler(socketfd, socketData, descriptorPoller, matchQueue, loginManager) {}

void ServerListener::handleReadEvent() {
    sockaddr_in clientAddress{};
    socklen_t socketDataSize = sizeof(clientAddress);
    int newSocket = accept(socketfd, (sockaddr*)&clientAddress, &socketDataSize);

    printf("new connection from: %s:%hu (fd: %d)\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port), newSocket);
    epoll_event newEvent;
    newEvent.events = EPOLLIN;
    // The pointer to the ClientConnection instance will be turned into a shared_ptr by the descriptorPoller
    newEvent.data.ptr = new ClientConnection(newSocket, clientAddress, descriptorPoller, matchQueue, loginManager);
    descriptorPoller->addHandler(newEvent);
}
