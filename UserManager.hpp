#pragma once
#include <map>
#include "ClientConnection.hpp"
#include <mutex>

class UserManager {
    private:
        std::map<std::string, std::string> userCredentials;
        std::map<std::string, bool> isUserLoggedIn;
        std::map<std::string, std::mutex> userMutexes;
    public:
        UserManager();
        void validateLogin(std::string username, std::string password, ClientConnection* connection);
        void logOut(std::string username, ClientConnection* connection);
};