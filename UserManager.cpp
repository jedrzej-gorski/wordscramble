#include "UserManager.hpp"
#include <fstream>
#include <iterator>

UserManager::UserManager() {
    // Manager initialization
    std::ifstream userListStream("res/userList.txt");
    std::string newUser, newPass;
    while (userListStream >> newUser >> newPass) {
        userCredentials.insert({newUser, newPass});
        isUserLoggedIn.insert({newUser, false});
    }
}

void UserManager::validateLogin(std::string username, std::string password, ClientConnection* connection) {
    if (userCredentials.find(username) != userCredentials.end()) {
        // Lock the ability to process this user's login simultaneously different connections
        std::lock_guard<std::mutex> lock(userMutexes[username]);
        if (isUserLoggedIn[username] == false) {
            if (userCredentials[username] == password) {
                isUserLoggedIn[username] = true;
                connection->finishLogin(username);
                connection->sendMsg(networkingConstants::logInResult, networkingConstants::success, username);
            }
            else {
                connection->sendMsg(networkingConstants::logInResult, networkingConstants::failurePassword);
            }
        }
        else {
            connection->sendMsg(networkingConstants::logInResult, networkingConstants::failureLoggedIn);
        }
    }
    else {
        connection->sendMsg(networkingConstants::logInResult, networkingConstants::failureUser);
    }

}

void UserManager::logOut(std::string username, ClientConnection* connection) {
    if (userCredentials.find(username) != userCredentials.end()) {
        if (isUserLoggedIn[username] == true) {
            isUserLoggedIn[username] = false;
            connection->finishLogOut();
            connection->sendMsg(networkingConstants::logOutResult, networkingConstants::logOutSuccess);
        }
        else {
            connection->sendMsg(networkingConstants::logOutResult, networkingConstants::logOutFailureLoggedOut);
        }
    }
    connection->sendMsg(networkingConstants::logOutResult, networkingConstants::logOutFailureUser);
}