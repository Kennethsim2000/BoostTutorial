cmake_minimum_required(VERSION 3.10)
project(mini_trader LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BOOST_ROOT "/usr/local/opt/boost")
set(BOOST_INCLUDEDIR "/usr/local/opt/boost/include")
set(BOOST_LIBRARYDIR "/usr/local/opt/boost/lib")

find_package(Boost REQUIRED)
find_package(Threads REQUIRED)

add_executable(mini_trader src/main.cpp)

target_include_directories(mini_trader PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(mini_trader PRIVATE ${Boost_LIBRARIES} Threads::Threads)
