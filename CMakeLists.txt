cmake_minimum_required(VERSION 3.9)
project(tp_http_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core Network)
IF (CMAKE_BUILD_TYPE MATCHES "Release")
    add_definitions(-DQT_NO_DEBUG_OUTPUT)
ENDIF (CMAKE_BUILD_TYPE MATCHES "Release")

add_executable(tp_http_server main.cpp
        Server/HttpServer/HttpServer.cpp
        Server/HttpServer/HttpServer.h
        Config/Config.cpp
        Config/Config.h
        Server/ConcurrentTcpServer/ConcurrentTcpServer.cpp
        Server/ConcurrentTcpServer/ConcurrentTcpServer.h
        Server/HttpHandlerHolder/HttpHandlerHolder.cpp
        Server/HttpHandlerHolder/HttpHandlerHolder.h
        Server/HolderController/HolderController.cpp
        Server/HolderController/HolderController.h
        Server/HttpHandler/HttpHandler.cpp
        Server/HttpHandler/HttpHandler.h
        Server/HttpParser/HttpParser.cpp
        Server/HttpParser/HttpParser.h
        Server/ConcurrentTcpServer/HandlersHoldersFactory.h
        Server/ConcurrentTcpServer/Handler.cpp
        Server/ConcurrentTcpServer/Handler.h
        Server/ConcurrentTcpServer/HandlerHolder.cpp
        Server/ConcurrentTcpServer/HandlerHolder.h
        Server/HttpServer/HttpHandlersHoldersFactory.cpp
        Server/HttpServer/HttpHandlersHoldersFactory.h
        Server/HttpParser/HttpRequestInfo.h
        Server/HttpParser/defs.h
        Server/HttpParser/defs.cpp
        IOBuffer/IOBuffer.cpp
        IOBuffer/IOBuffer.h)

qt5_use_modules(tp_http_server Core Network)