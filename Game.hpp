#pragma once
#include <array>
#include "ClientConnection.hpp"

class Game {
    private:
        std::array<ClientConnection*, 2> players;
        std::array<std::array<char, 16>, 5> charsets;
        std::array<std::array<std::string, 2>, 5> playerAnswers;
        std::array<unsigned int, 2> roundNumbers;
    protected:
        void advancePlayer(int playerIndex);
        void finishGame(int winnerIndex);
        void rejectAnswer(int playerIndex);
        int getPlayerIndex(ClientConnection* player);
    public:
        void surrenderGame(ClientConnection* player);
        Game(ClientConnection& playerOne, ClientConnection& playerTwo);
        int processAnswer(std::string playerAnswer, ClientConnection* player);
        std::array<char, 16> getCharset(ClientConnection* player);
        std::string getOtherPlayerName(ClientConnection* player);

};