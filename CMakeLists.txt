cmake_minimum_required(VERSION 3.10)
project(mini_trader LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED)
find_package(Threads REQUIRED)

add_executable(mini_trader src/main.cpp)

target_include_directories(mini_trader PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(mini_trader PRIVATE ${Boost_LIBRARIES} Threads::Threads)
