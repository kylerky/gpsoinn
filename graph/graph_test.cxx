#include "graph.hxx"
#include "gtest/gtest.h"

using namespace GPSOINN;

TEST(basic, init) { Digraph<unsigned, int> graph; }

TEST(insert, vertex) {
    Digraph<int, int> graph;
    graph.insert_vertex(0);
    graph.insert_vertex(1);
    graph.insert_vertex(2);
    graph.insert_vertex(3);
    graph.insert_vertex(4);
    {
        auto iter = graph.cbegin();
        for (unsigned i = 0; i != 4; ++i, ++iter) {
            EXPECT_EQ(i, *iter);
        }
    }
}
