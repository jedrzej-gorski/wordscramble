#pragma once
#include "EventHandler.hpp"
#include "Poller.hpp"
#include "UserManager.hpp"
#include "Queue.hpp"
#include "User.hpp"
#include "Game.hpp"
#include <memory>
#include <sstream>
#include <cstring>


class ClientConnection : public EventHandler {
    private:
        std::unique_ptr<User> connectedUser;
        std::shared_ptr<Game> currentGame;
    protected:
    public:
        ClientConnection(int socketfd, sockaddr_in addressData, std::shared_ptr<Poller>& descriptorPoller, std::shared_ptr<Queue>& matchQueue, std::shared_ptr<UserManager>& loginManager);
        void handleReadEvent();
        void startGame(std::shared_ptr<Game>& gameReference);
        void endGame();
        template<typename... Args>
        void sendMsg(networkingConstants::ServerInstructionType msgToSend, Args... args) {
            char buffer[networkingConstants::MAX_MSG_SIZE];
            uint16_t msgSize = 0;
            std::stringstream ss;
            std::string message;

            // Use a stream to compose the message
            ss << msgToSend << " ";
            ((ss << args << " "), ...);


            // Remove the last space
            message = std::move(ss.str());
            message.pop_back();
            msgSize = message.length();
            std::strcpy(buffer + 2, message.c_str());
            *(u_int16_t*)buffer = htons(msgSize); 
 
            if (msgSize + 2 != send(socketfd, static_cast<void*>(buffer), msgSize + 2, MSG_NOSIGNAL)) {
                closeConnection();
                return;
            }
        };
        void setStatus(networkingConstants::UserStatus newStatus);
        void finishLogOut();
        void finishLogin(std::string username);
        void logOut();
        void closeConnection();
        std::string getConnectedUsername();

};