#include "Game.hpp"
#include "ClientConnection.hpp"
#include "wordProcessing.hpp"
#include <math.h>

Game::Game(ClientConnection* playerOne, ClientConnection* playerTwo) : players({playerOne, playerTwo}), roundNumbers({1, 1}) {
    // TODO: Resarch - how do you work with templates?
    for (int i = 0; i < charsets.size(); i++) {
        charsets[i] = wordProcessing::generateCharset();
    }
    for (int i = 0; i < playerAnswers.size(); i++) {
        playerAnswers[i] = {{"0", "0"}};
    }
}

int Game::getPlayerIndex(ClientConnection* searchedPlayer) {
    if (searchedPlayer == players[0]) {
        return 0;
    }
    if (searchedPlayer == players[1]) {
        return 1;
    }
    return -1;
}

void Game::advancePlayer(int playerIndex) {
    roundNumbers[playerIndex] += 1;
    

    if (roundNumbers[playerIndex] > networkingConstants::MAX_ROUND) {
        finishGame(playerIndex);
    }
    else {
        int otherPlayerIndex = abs(playerIndex - 1);

        players[playerIndex]->sendMsg(networkingConstants::playerAdvancedRound, castCharset(charsets[roundNumbers[playerIndex]]));
        players[otherPlayerIndex]->sendMsg(networkingConstants::enemyAdvancedRound);
    }
}

void Game::finishGame(int playerIndex) {
    int otherPlayerIndex = abs(playerIndex - 1);
    players[playerIndex]->sendMsg(networkingConstants::gameOver, networkingConstants::gameWon);
    players[otherPlayerIndex]->sendMsg(networkingConstants::gameOver, networkingConstants::gameLost);
    players[playerIndex]->endGame();
    players[otherPlayerIndex]->endGame();
}

void Game::surrenderGame(ClientConnection* player) {
    finishGame(abs(getPlayerIndex(player) - 1));
}

void Game::rejectAnswer(int playerIndex) {
    players[playerIndex]->sendMsg(networkingConstants::answerRejected);
}

int Game::processAnswer(std::string playerAnswer, ClientConnection* player) {
    int playerIndex = getPlayerIndex(player);
    if (playerIndex == -1) {
        // If player not in game, return 0 (signalize failure)
        return 0;
    }
    // Pass in the charset corresponding to the player's current round
    if (wordProcessing::validateAnswer(charsets[static_cast<int>(roundNumbers[playerIndex])], playerAnswer)) {
        playerAnswers[roundNumbers[playerIndex]][playerIndex] = playerAnswer;
        advancePlayer(playerIndex);
    }
    else {
        rejectAnswer(playerIndex);
    }
    return 1;

}

std::string Game::getOtherPlayerName(ClientConnection* player) {
    if (players[0] == player) {
        return players[1]->getConnectedUsername();
    }
    else if (players[1] == player) {
        return players[0]->getConnectedUsername();
    }
}

std::string Game::castCharset(std::array<char, 16> charset) {
    std::string result = "";
    for (auto it = charset.begin(); it != charset.end(); std::advance(it, 1)) {
        result += *it;
    }
    return result;
}

std::string Game::getCharset(ClientConnection* player) {
    if (players[0] == player) {
        return static_cast<std::string>(castCharset(charsets[0]));
    }
    else if (players[1] == player) {
        return static_cast<std::string>(castCharset(charsets[1]));
    }
}