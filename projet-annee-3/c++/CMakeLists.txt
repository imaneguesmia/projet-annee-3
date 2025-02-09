cmake_minimum_required(VERSION 3.14)
project(ChessEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(external/chess-library)

include_directories(include)


set(SOURCES
        src/main.cpp
        src/Player.cpp
        src/MinimaxAI.cpp
        src/MinimaxAI2.cpp
        src/HumanPlayer.cpp
        src/GameManager.cpp
)

add_executable(chess-engine ${SOURCES})




#cmake_minimum_required(VERSION 3.30)
#project(ChessEngine)
#
#set(CMAKE_CXX_STANDARD 20)
#
#add_executable(ChessEngine
#        include/chess/Player.h
#        include/chess/MinimaxAI.h
#        include/chess/HumanPlayer.h
#        include/chess/GameManager.h
#        src/main.cpp
#        src/HumanPlayer.cpp
#        src/GameManager.cpp
#        src/Player.cpp
#        src/MinimaxAI.cpp)
