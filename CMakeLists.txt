cmake_minimum_required(VERSION 3.0)

project(ArrivalTest VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a -Wall -pthread")

add_executable(Server   src/Server.cpp
                        src/Core/Net.cpp
                        src/Core/TasksQueue.cpp)

add_executable(Client   src/Client.cpp
                        src/Core/Net.cpp)

add_executable(Tests    src/Core/Net.cpp
                        src/Core/TasksQueue.cpp
                        src/Tests/TasksQueue_test.cpp)
