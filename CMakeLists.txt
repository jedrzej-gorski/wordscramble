cmake_minimum_required(VERSION 3.0.0)
project(word VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(word main.cpp
                    ClientConnection.hpp
                    ClientConnection.cpp
                    EventHandler.hpp
                    EventHandler.cpp
                    Game.hpp
                    Game.cpp
                    MutexCollector.hpp
                    MutexCollector.cpp
                    networkingConstants.hpp
                    Poller.hpp
                    Poller.cpp
                    Queue.hpp
                    Queue.cpp
                    Server.hpp
                    Server.cpp
                    ServerListener.hpp
                    ServerListener.cpp
                    User.hpp
                    User.cpp
                    UserManager.hpp
                    UserManager.cpp
                    wordProcessing.hpp
                    wordProcessing.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
