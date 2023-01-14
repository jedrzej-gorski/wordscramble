#include <Queue.hpp>
#include <algorithm>


void Queue::addToQueue(ClientConnection* newUser) {
    std::lock_guard<std::mutex> lock(queuedUsersMutex);
    if (std::find(queuedUsers.begin(), queuedUsers.end(), newUser) != queuedUsers.end()) {
        newUser->sendMsg(networkingConstants::addedToQueue, networkingConstants::genericFailure);
    }
    queuedUsers.push_back(newUser);
    newUser->setStatus(networkingConstants::inQueue);
    newUser->sendMsg(networkingConstants::addedToQueue, networkingConstants::genericSuccess);
}

void Queue::removeFromQueue(ClientConnection* removedUser) {
    std::lock_guard<std::mutex> lock(queuedUsersMutex);
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
    std::lock_guard<std::mutex> collectorLock(mutex_collectors);
    // Matchmaking needs to happen constantly, hence the loop
    while (true) {
        ClientConnection *playerOne, *playerTwo;
        std::lock_guard<std::mutex> lock(queuedUsersMutex);
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
