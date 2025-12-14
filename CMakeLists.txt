cmake_minimum_required(VERSION 3.11)

project(AsciiPong)

set(CMAKE_CXX_STANDARD 17)

# 1. Fetch Raylib
include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG master
)
FetchContent_MakeAvailable(raylib)

# 2. Define the Executable
# NOTE: We only list main.cpp here because the engine is now a Header (.h)
add_executable(${PROJECT_NAME} 
    src/main.cpp
)

# 3. Include Directories
# This tells C++ to look inside "engine" folder when you type #include "AsciiFramework.h"
target_include_directories(${PROJECT_NAME} PRIVATE engine)

# 4. Link Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

if (LINUX)
    target_link_libraries(${PROJECT_NAME} PRIVATE m pthread dl rt X11)
endif()