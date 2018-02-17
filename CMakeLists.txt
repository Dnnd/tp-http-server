cmake_minimum_required(VERSION 3.9)
project(tp_http_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

set (CMAKE_CXX_FLAGS "--std=c++2a -fcoroutines-ts")
find_package(Qt5 COMPONENTS Core Network)
add_executable(tp_http_server main.cpp Server/HttpServer.cpp Server/HttpServer.h Config/Config.cpp Config/Config.h Server/ConcurrentTcpServer/ConcurrentTcpServer.cpp Server/ConcurrentTcpServer/ConcurrentTcpServer.h Server/ConcurrentTcpServer/SocketHandlerHolder.cpp Server/ConcurrentTcpServer/SocketHandlerHolder.h Server/ConcurrentTcpServer/HolderController.cpp Server/ConcurrentTcpServer/HolderController.h)
qt5_use_modules(tp_http_server Core Network)