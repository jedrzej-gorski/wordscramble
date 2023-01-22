#pragma once
#include <vector>
#include <mutex>
#include "MutexCollector.hpp"

class ClientConnection;

// TODO: Research - Should this namespace be a class instead? Should its functions be methods in EventHandler or ClientConnection?
class Queue: public MutexCollector{
    private:
        std::vector<ClientConnection*> queuedUsers;
    public:
        void addToQueue(ClientConnection* newUser);
        void removeFromQueue(ClientConnection* removedUser);
        void matchmake();
};