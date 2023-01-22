#include "Queue.hpp"
#include "ClientConnection.hpp"
#include <algorithm>
#include <chrono>
#include <thread>


void Queue::addToQueue(ClientConnection* newUser) {
    if (std::find(queuedUsers.begin(), queuedUsers.end(), newUser) != queuedUsers.end()) {
        newUser->sendMsg(networkingConstants::addedToQueue, networkingConstants::genericFailure);
    }
    queuedUsers.push_back(newUser);
    newUser->setStatus(networkingConstants::inQueue);
    newUser->sendMsg(networkingConstants::addedToQueue, networkingConstants::genericSuccess);
}

void Queue::removeFromQueue(ClientConnection* removedUser) {
    //If the user has already been matchmade, the function won't do anything
    bool removed = false;
    for (auto it = queuedUsers.begin(); it != queuedUsers.end(); ++it) {
        if (*it == removedUser) {
            queuedUsers.erase(it);
            removed = true;
            break;
        }
    }
    if (removed) {
        removedUser->setStatus(networkingConstants::connected);
        removedUser->sendMsg(networkingConstants::removedFromQueue, networkingConstants::genericSuccess);
    }
    else {
        removedUser->sendMsg(networkingConstants::removedFromQueue, networkingConstants::genericFailure);
    }
}

void Queue::matchmake() {
    
    // Matchmaking needs to happen constantly, hence the loop
    while (true) {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(10ms);
        std::lock_guard<std::mutex> collectorLock(MutexCollector::mutex_collectors);
        ClientConnection *playerOne, *playerTwo;
        if (queuedUsers.size() > 1) {
            playerOne = queuedUsers[0];
            playerTwo = queuedUsers[1];
            queuedUsers.erase(queuedUsers.begin());
            queuedUsers.erase(queuedUsers.begin());
            auto gamePtr = std::make_shared<Game>(playerOne, playerTwo);
            playerOne->startGame(gamePtr);
            playerOne->sendMsg(networkingConstants::gameReady, gamePtr->getOtherPlayerName(playerOne), gamePtr->getCharset(playerOne));
            playerTwo->startGame(gamePtr);
            playerTwo->sendMsg(networkingConstants::gameReady, gamePtr->getOtherPlayerName(playerTwo), gamePtr->getCharset(playerTwo));
        }
    }
}
