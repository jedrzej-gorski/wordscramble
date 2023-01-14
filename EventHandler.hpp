#pragma once
#include <netinet/in.h>
#include <Poller.hpp>
#include <Queue.hpp>
#include <UserManager.hpp>

class EventHandler {
    protected:
        int socketfd;
        sockaddr_in socketData;
        bool closeable; 
        std::shared_ptr<Poller> descriptorPoller;
        std::shared_ptr<Queue> matchQueue;
        std::shared_ptr<UserManager> loginManager;
    public:
        EventHandler(int socketfd, sockaddr_in socketData, std::shared_ptr<Poller>& descriptorPoller, std::shared_ptr<Queue>& matchQueue, std::shared_ptr<UserManager>& loginManager);
        virtual ~EventHandler() {};
        virtual void handleReadEvent() = 0;
        int getSocketfd();
        bool isCloseable();
};