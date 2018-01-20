cmake_minimum_required (VERSION 2.8.2)
project (graph_test)

add_executable(graph graph_test.cxx)
target_link_libraries(graph gtest_main)
