#pragma once
#include "EventHandler.hpp"
#include "Poller.hpp"
#include "User.hpp"
#include "Game.hpp"
#include <memory>

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
            char buffer[networkingConstants:MAX_MSG_SIZE];
            uint16_t msgSize = 0;
            // Prepare format string for fold expression, the last set of braces mustn't be followed by a space, hence the pop_back after the termination
            for (int i = 0; i < static_cast<int>(sizeof...(Args)); i++) {
                formatString += "{} ";
            }
            formatString.pop_back();

            msgSize = sprintf(buffer + 2, std::format(formatString, msgToSend, args...).c_str());
            *(u_int16_t*)buffer = htons(msgSize);
 
            if (msgSize + 2 != send(socketfd, static_cast<void*>(buffer), msgSize + 2)) {
                // TODO: Error handling for disconnection
            }
        };
        void setStatus(networkingConstants::UserStatus newStatus);
        void finishLogOut();
        void finishLogin(std::string username);
        void logOut();
        void closeConnection();
        std::string getConnectedUsername();

};