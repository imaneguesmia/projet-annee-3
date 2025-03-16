cmake_minimum_required(VERSION 3.14)
project(ChessEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Download and setup CPM.cmake
set(CPM_DOWNLOAD_VERSION 0.38.0)

if(CPM_SOURCE_CACHE)
    set(CPM_DOWNLOAD_LOCATION "${CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
elseif(DEFINED ENV{CPM_SOURCE_CACHE})
    set(CPM_DOWNLOAD_LOCATION "$ENV{CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
else()
    set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
endif()

if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
    message(STATUS "Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
    file(DOWNLOAD 
            https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake
            ${CPM_DOWNLOAD_LOCATION}
    )
endif()

include(${CPM_DOWNLOAD_LOCATION})

# Add MantaRay as a CPM package
CPMAddPackage(
    NAME MantaRay
    GITHUB_REPOSITORY TheBlackPlague/MantaRay
    GIT_TAG master
    OPTIONS 
        "MANTARAY_BUILD_EXAMPLES OFF"
)

# Ajouter nlohmann_json
CPMAddPackage(
    NAME nlohmann_json
    VERSION 3.11.2
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)

# Include chess library (header files only)
include_directories(external/chess-library)

# Include our headers
include_directories(include)

# Include MantaRay headers
include_directories(${CMAKE_BINARY_DIR}/_deps/mantaray-src/src)

# ==============================
# Chess Engine Executable
# ==============================

set(CHESS_ENGINE_SOURCES
        src/main.cpp
        src/Player.cpp
        src/MinimaxAI.cpp
        src/MinimaxAI2.cpp
        src/MinimaxAI3.cpp
        src/HumanPlayer.cpp
        src/GameManager.cpp
        src/Search.cpp
        src/TranspositionTable.cpp
        src/MoveOrdering.cpp
        src/Evaluator.cpp
        src/QuiescenceSearch.cpp
        src/NNUEEvaluator.cpp
        src/BelugaNNUE.cpp
        src/QuiescenceSearchNNUE.cpp
)

add_executable(chess-engine ${CHESS_ENGINE_SOURCES})

# Link all libraries for chess-engine
target_link_libraries(chess-engine PRIVATE 
    MantaRay 
    nlohmann_json::nlohmann_json
)

# ==============================
# NNUE Test Executable
# ==============================

set(NNUE_TEST_SOURCES
        src/NNUEInteractiveTest.cpp
        src/NNUEEvaluator.cpp         # Ajout du fichier source NNUEEvaluator
        src/BelugaNNUE.cpp
        src/QuiescenceSearchNNUE.cpp
        src/QuiescenceSearch.cpp 
        src/Evaluator.cpp
        src/GameManager.cpp
        src/Search.cpp
        src/MoveOrdering.cpp
        src/TranspositionTable.cpp
)

add_executable(nnue-test ${NNUE_TEST_SOURCES})

# Link all libraries for nnue-test (identique à chess-engine)
target_link_libraries(nnue-test PRIVATE 
    MantaRay 
    nlohmann_json::nlohmann_json
)
