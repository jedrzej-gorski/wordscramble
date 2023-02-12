#pragma once

namespace networkingConstants {
    const int MAX_MSG_SIZE = 1024;
    const int MAX_ROUND = 5;
    const int MAX_EVENTS = 10;
    const int LOCALPORT = 5050;

    enum ClientInstructionType {
        logIn = 1,
        logOut,
        queueForGame,
        leaveQueue,
        sendWord
    };

    enum ServerInstructionType {
        connectionCheck = 1,
        logInResult,
        logOutResult,
        addedToQueue,
        removedFromQueue,
        gameReady,
        gameOver,
        playerAdvancedRound,
        answerRejected,
        enemyAdvancedRound,

    };

    enum UserStatus {
        noUser = 1,
        connected,
        inGame,
        disconnected,
        inQueue
    };

    enum GameOverParams {
        gameWon = 1,
        gameLost
    };

    enum LogInResultType {
        success = 1,
        failurePassword,
        failureUser,
        failureLoggedIn
    };

    enum LogOutResultType {
        logOutSuccess = 1,
        logOutFailureLoggedOut,
        logOutFailureUser
    };

    enum GenericResponseType {
        genericSuccess = 1,
        genericFailure
    };
}