#include "ClientConnection.hpp"
#include "networkingConstants.hpp"
#include <sstream>

ClientConnection::ClientConnection(int socketfd, sockaddr_in addressData, std::shared_ptr<Poller>& descriptorPoller, std::shared_ptr<Queue>& matchQueue, std::shared_ptr<UserManager>& loginManager) : EventHandler(socketfd, addressData, descriptorPoller, matchQueue, loginManager), connectedUser(nullptr), currentGame(nullptr)  {}

void ClientConnection::handleReadEvent() {
    char buffer[networkingConstants::MAX_MSG_SIZE];
    int command;
    // incoming message size
    uint16_t msgSize;

    if (recv(socketfd, &msgSize, 2, MSG_WAITALL) != 2) {
        closeConnection();
        return;
    }
    msgSize = ntohs(msgSize);

    if (recv(socketfd, buffer, msgSize, MSG_WAITALL) != msgSize) {
        closeConnection();
        return;
    }
    // Terminate the C string
    buffer[msgSize] = '\0';

    std::istringstream ss(buffer);
    std::string newArgument;
    std::vector<std::string> argumentList;
    ss >> command;
    while (ss >> newArgument) {
        argumentList.push_back(newArgument);
    }
    
    switch (command) {
        case networkingConstants::logIn:
                loginManager->validateLogin(argumentList[0], argumentList[1], this);
            break;
        case networkingConstants::logOut:
                if (connectedUser != nullptr && connectedUser->getStatus() == networkingConstants::connected) {
                    logOut();
                }
            break;
        case networkingConstants::queueForGame:
                matchQueue->addToQueue(this);
            break;
        case networkingConstants::leaveQueue:
                matchQueue->removeFromQueue(this);
            break;
        case networkingConstants::sendWord:
                if (currentGame.get() != nullptr) {
                    currentGame->processAnswer(argumentList[0], this);
                }
            break;
    }

    //uint16_t len = sprintf(buffer + 2, );
}

void ClientConnection::closeConnection() {
    if (connectedUser.get() != nullptr) {
        if (connectedUser->getStatus() != networkingConstants::inGame) {
            if (connectedUser->getStatus() == networkingConstants::inQueue) {
                matchQueue->removeFromQueue(this);
            }
        }
        else {
            currentGame->surrenderGame(this);
        }
        logOut();
    }
    closeable = true;
    connectedUser.reset();
}

void ClientConnection::finishLogin(std::string username) {
    connectedUser = std::make_unique<User>(username, networkingConstants::connected);
}

void ClientConnection::logOut() {
    loginManager->logOut(connectedUser->getUsername(), this);
}

void ClientConnection::setStatus(networkingConstants::UserStatus newStatus) {
    connectedUser->changeStatus(newStatus);
}

void ClientConnection::finishLogOut() {
    connectedUser.reset();
}

void ClientConnection::startGame(std::shared_ptr<Game>& gamePtr) {
    currentGame = gamePtr;
    connectedUser->changeStatus(networkingConstants::UserStatus::inGame);
}


void ClientConnection::endGame() {
    currentGame.reset();
    connectedUser->changeStatus(networkingConstants::UserStatus::connected);
}

std::string ClientConnection::getConnectedUsername() {
    return connectedUser->getUsername();
}

