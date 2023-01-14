#pragma once
#include <string>
#include "networkingConstants.hpp"

class User {
    private:
        std::string username;
        networkingConstants::UserStatus userStatus; 
    public:
        User();
        User(std::string username, networkingConstants::UserStatus userStatus);
        void changeStatus(networkingConstants::UserStatus userStatus);
        std::string getUsername();
        networkingConstants::UserStatus getStatus();
};