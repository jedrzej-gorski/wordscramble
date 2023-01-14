#include "User.hpp"

User::User() : username(""), userStatus(networkingConstants::noUser) {
}

User::User(std::string username, networkingConstants::UserStatus userStatus) : username(username), userStatus(userStatus) {}

void User::changeStatus(networkingConstants::UserStatus newUserStatus) {
    userStatus = newUserStatus;
}

std::string User::getUsername() {
    return username;
}

networkingConstants::UserStatus User::getStatus() {
    return userStatus;
}