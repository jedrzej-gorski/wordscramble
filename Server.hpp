#pragma once
#include <Queue.hpp>
#include <UserManager.hpp>
#include <memory>

class Server {
    private:
        std::shared_ptr<Queue> matchQueue;
        std::shared_ptr<UserManager> loginManager;
        std::shared_ptr<Poller> descriptorPoller;
    public:
        Server();
        void run();
};