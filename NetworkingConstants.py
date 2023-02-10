from enum import Enum


class ClientInstructionType(Enum):
    logIn = 1
    logOut = 2
    queueForGame = 3
    leaveQueue = 4
    sendWord = 5


class ServerInstructionType(Enum):
    connectionCheck = 1  # unused
    logInResult = 2
    logOutResult = 3
    addedToQueue = 4
    removedFromQueue = 5
    gameReady = 6
    gameOver = 7
    playerAdvancedRound = 8
    answerRejected = 9
    enemyAdvancedRound = 10


class UserStatus(Enum):
    noUser = 1
    connected = 2
    inGame = 3
    disconnected = 4
    inQueue = 5


class GameOverParams(Enum):
    gameWon = 1
    gameLost = 2


class LogInResultType(Enum):
    success = 1
    failurePassword = 2
    failureUser = 3
    failureLoggedIn = 4


class LogOutResultType(Enum):
    logOutSuccess = 1
    logOutFailureLoggedOut = 2
    logOutFailureUser = 3


class GenericResponseType(Enum):
    genericSuccess = 1
    genericFailure = 2
