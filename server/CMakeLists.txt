cmake_minimum_required(VERSION 3.8)
project(server)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread --std=c++11 -Wall")

set(SOURCE_FILES main.cpp src/Server.cpp src/Server.h src/Room.cpp src/Room.h src/Game.cpp src/Game.h src/Player.cpp src/Player.h)
add_executable(server ${SOURCE_FILES})