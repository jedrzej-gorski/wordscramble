#pragma once
#include "EventHandler.hpp"
#include "Poller.hpp"
#include <string>
#include "Server.hpp"

class ServerListener: public EventHandler {
    private:
        int localport;

    public:
        ServerListener(int socketfd, sockaddr_in socketData, int localport, std::shared_ptr<Poller>& descriptorPoller, std::shared_ptr<Queue>& matchQueue, std::shared_ptr<UserManager>& loginManager);
        void handleReadEvent();
};