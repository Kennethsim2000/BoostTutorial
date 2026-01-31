find_package(GTest REQUIRED) # tell CMAKE to find googleTest, add its include directories, link against it

# Basic test
add_executable(test_basic test_order_book_basic.cpp test_csv_logger.cpp test_helpers.cpp)
target_link_libraries(test_basic
    PRIVATE 
    trader_core
    GTest::gtest
    GTest::gtest_main
)

# Cancel test
# add_executable(test_cancel test_order_book_cancel.cpp)
# target_link_libraries(test_cancel
#     trader_core
#     GTest::gtest
#     GTest::gtest_main
# )

include(GoogleTest)
gtest_discover_tests(test_basic)
