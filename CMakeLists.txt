cmake_minimum_required(VERSION 3.0)

project(Client VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(BUILD_SHARED_LIBS TRUE)

#https://think-async.com/Asio/AsioStandalone.html
set(ASIO_STANDALONE TRUE)

add_subdirectory(lib/protobuf_messages)

add_executable(Client main.cpp)

target_include_directories(Client PRIVATE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third_party/asio-1.28.0/include>)

target_link_libraries(Client PRIVATE protobuf_messages)