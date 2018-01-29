#include "graph.hxx"
#include "gtest/gtest.h"
#include <iostream>

using namespace GPSOINN;

TEST(basic, init) { Digraph<unsigned, int> graph; }

TEST(insert, vertex) {
    Digraph<int, int> graph;
    for (int i = 0; i != 4; ++i) {
        graph.insert_vertex(i);
    }
    ASSERT_EQ(4, graph.vertex_count());
    {
        auto iter = graph.cbegin();
        for (unsigned i = 0; i != 4; ++i, ++iter) {
            EXPECT_EQ(i, iter->value());
        }
    }
}

TEST(insert, edge) {
    Digraph<int, int> graph;
    for (int i = 0; i != 5; ++i) {
        graph.insert_vertex(i);
    }
    graph.insert_edge(0, 1, 1);
    graph.insert_edge(1, 2, 2);
    graph.insert_edge(1, 3, 4);
    graph.insert_edge(1, 3, 4);
    graph.insert_edge(3, 4, 1);
    graph.insert_edge(4, 2, 1);
    // graph.erase_after_edge(ver, edge);
    for (auto ver : graph)
        for (auto edge : ver)
            std::cout << ver.value() << " - " << edge.head << std::endl;
}
