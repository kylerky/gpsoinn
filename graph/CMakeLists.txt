cmake_minimum_required (VERSION 2.8.2)
project (graph_test)

add_executable(multiset multiset_test.cxx)
target_link_libraries(multiset gtest_main)
