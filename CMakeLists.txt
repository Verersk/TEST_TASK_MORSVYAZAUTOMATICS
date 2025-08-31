cmake_minimum_required(VERSION 4.0.0)
project(CircleQueue VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(circle_queue src/main.cpp)

target_include_directories(circle_queue PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Линкуем библиотеку для потоков
find_package(Threads REQUIRED)
target_link_libraries(circle_queue PRIVATE Threads::Threads)