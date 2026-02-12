cmake_minimum_required(VERSION 3.10)
project(mini_trader LANGUAGES CXX)

option(ENABLE_TSAN  "Enable ThreadSanitizer" OFF)
option(ENABLE_ASAN  "Enable AddressSanitizer" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BOOST_ROOT "/usr/local/opt/boost")
set(BOOST_INCLUDEDIR "/usr/local/opt/boost/include")
set(BOOST_LIBRARYDIR "/usr/local/opt/boost/lib")

find_package(Boost REQUIRED)
find_package(Threads REQUIRED)

add_library(trader_core
    src/order_book.cpp
    src/tcp_server.cpp
)

target_include_directories(trader_core
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(trader_core
    PUBLIC
    ${Boost_LIBRARIES}
    Threads::Threads
)

add_executable(mini_trader src/main.cpp) #important to add the other cpp files for linking

# target_include_directories(mini_trader PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(mini_trader PRIVATE trader_core)

set(SANITIZER_FLAGS)

if (ENABLE_TSAN)
    message(STATUS "ThreadSanitizer enabled")
    list(APPEND SANITIZER_FLAGS
        -fsanitize=thread
        -g
        -O1
    )
endif()

if (ENABLE_ASAN)
    message(STATUS "AddressSanitizer enabled")
    list(APPEND SANITIZER_FLAGS
        -fsanitize=address
        -g
        -O1
    )
endif()

if (ENABLE_UBSAN)
    message(STATUS "UndefinedBehaviorSanitizer enabled")
    list(APPEND SANITIZER_FLAGS
        -fsanitize=undefined
    )
endif()

if (SANITIZER_FLAGS)
    target_compile_options(trader_core PUBLIC ${SANITIZER_FLAGS})
    target_link_options(trader_core PUBLIC ${SANITIZER_FLAGS})

    target_compile_options(mini_trader PUBLIC ${SANITIZER_FLAGS})
    target_link_options(mini_trader PUBLIC ${SANITIZER_FLAGS})
endif()

# testing
enable_testing() 
add_subdirectory(tests) #Go into this folder, read its CMakeLists.txt, and treat whatever is defined there as part of the same build