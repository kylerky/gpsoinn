#ifndef GPSOINN_GRAPH_HXX
#define GPSOINN_GRAPH_HXX

#include "multiset.hxx"
#include <forward_list>
#include <functional>
#include <unordered_set>
#include <utility>

namespace GPSOINN {

template <typename ValueT, typename WeightT,
          typename Compare = std::less<ValueT>>
class Digraph {
  public:
    typedef typename multiset<ValueT, Compare>::size_type index_t;

    friend void swap(Digraph &left, Digraph &right) noexcept {
        using std::swap;
        swap(left.vertices, right.vertices);
        swap(left.edge_cnt, right.edge_cnt);
    }

  protected:
    struct EdgeNode {
        WeightT weight;
        index_t head;
    };
    struct VertexNode {
        ValueT value;
        std::forward_list<EdgeNode> edges;
    };
    typedef multiset<VertexNode, Compare> set_type;

  public:
    typedef typename std::forward_list<EdgeNode>::iterator edge_iterator;
    typedef typename std::forward_list<EdgeNode>::const_iterator
        const_edge_iterator;
    typedef typename multiset<ValueT, Compare>::size_type size_type;
    typedef ValueT value_type;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef WeightT weight_type;
    typedef Compare value_compare;

    class vertex_iterator : public set_type::iterator {
      public:
        vertex_iterator(typename set_type::iterator const &other)
            : set_type::iterator(other) {}
        vertex_iterator(typename set_type::iterator &&other)
            : set_type::iterator(other) {}
        typedef ValueT &reference;
        typedef ValueT *pointer;
        reference operator*() const {
            return set_type::iterator::operator*().value;
        }
        pointer operator->() const {
            return &set_type::iterator::operator*().value;
        }
    };
    class const_vertex_iterator : public set_type::const_iterator {
      public:
        const_vertex_iterator(typename set_type::const_iterator const &other)
            : set_type::const_iterator(other) {}
        const_vertex_iterator(typename set_type::const_iterator &&other)
            : set_type::const_iterator(other) {}
        typedef const ValueT &reference;
        typedef const ValueT *pointer;
        reference operator*() const {
            return set_type::const_iterator::operator*().value;
        }
        pointer operator->() const {
            return &set_type::const_iterator::operator*().value;
        }
    };

    // constructors
    Digraph() : Digraph(Compare()) {}
    explicit Digraph(const Compare &comp) : vertices(comp), edge_cnt(0) {}
    // copy constructors
    Digraph(const Digraph &other) = default;
    Digraph(Digraph &&other) = default;

    /* operators */
    Digraph &operator=(Digraph other) {
        std::swap(*this, other);
        return *this;
    }

    // destructors
    virtual ~Digraph() = default;

    // vertex related modifiers
    vertex_iterator insert_vertex(const ValueT &value) {
        VertexNode vertex;
        vertex.value = value;
        return vertices.insert(vertex);
    }
    vertex_iterator insert_vertex(ValueT &&value) {
        VertexNode vertex;
        vertex.value = value;
        return vertices.insert(std::move(vertex));
    }
    vertex_iterator erase_vertex(const_vertex_iterator pos);
    vertex_iterator erase_vertex(const_vertex_iterator beg,
                                 const_vertex_iterator end);
    size_type erase_vertex(const ValueT &key);

    // edge related modifiers
    void insert_edge(const_vertex_iterator tail, const_vertex_iterator head,
                     const WeightT &weight); // tail -> head
    void insert_edge(const_vertex_iterator tail, const_vertex_iterator head,
                     WeightT &&weight);
    edge_iterator erase_after_edge(const_vertex_iterator tail,
                                   const_edge_iterator edge);
    edge_iterator
    erase_after_edge(const_vertex_iterator tail, const_edge_iterator beg,
                     const_edge_iterator end); // range: (beg, end)

    /* iterators */
    // vertex related
    vertex_iterator begin() noexcept { return vertices.begin(); }
    const_vertex_iterator begin() const noexcept { return vertices.cbegin(); }
    const_vertex_iterator cbegin() const noexcept { return vertices.cbegin(); }
    vertex_iterator end() noexcept { return vertices.end(); }
    const_vertex_iterator end() const noexcept { return vertices.cend(); }
    const_vertex_iterator cend() const noexcept { return vertices.cend(); }
    // edge related
    edge_iterator edge_begin(const_vertex_iterator vertex) noexcept;
    const_edge_iterator edge_begin(const_vertex_iterator vertex) const noexcept;
    const_edge_iterator cedge_begin(const_vertex_iterator vertex) const
        noexcept;
    edge_iterator edge_end(const_vertex_iterator vertex) noexcept;
    const_edge_iterator edge_end(const_vertex_iterator vertex) const noexcept;
    const_edge_iterator cedge_end(const_vertex_iterator vertex) const noexcept;
    edge_iterator edge_before_begin(const_vertex_iterator vertex) noexcept;
    const_edge_iterator edge_before_begin(const_vertex_iterator vertex) const
        noexcept;
    const_edge_iterator cedge_before_begin(const_vertex_iterator vertex) const
        noexcept;

    //
    void clear() noexcept;

    /* capacity */
    size_type vertex_count() const noexcept { return vertices.size(); }
    [[nodiscard]] bool empty() const noexcept { return vertex_count() == 0; }
    size_type edge_count() const noexcept { return edge_cnt; }

  protected:
    set_type vertices;
    size_type edge_cnt;
};

} // namespace GPSOINN

namespace GPSOINN {

template <typename ValueT, typename WeightT, typename Compare>
typename Digraph<ValueT, WeightT, Compare>::vertex_iterator
Digraph<ValueT, WeightT, Compare>::erase_vertex(const_vertex_iterator pos) {
    typename set_type::const_iterator &iter = pos;
    auto index = vertices.get_index(iter);
    auto result = vertices.erase(iter);
    for (auto &vertex : vertices) {
        auto pre = vertex.edges.cbefore_begin();
        for (auto edge = vertex.edges.cbegin(); edge != vertex.edges.cend();
             ++edge) {
            if (edge->head == index) {
                edge = vertex.edges.erase_after(pre);
                --edge_cnt;
            } else {
                ++edge;
                ++pre;
            }
        }
    }
    return result;
}

template <typename ValueT, typename WeightT, typename Compare>
typename Digraph<ValueT, WeightT, Compare>::vertex_iterator
Digraph<ValueT, WeightT, Compare>::erase_vertex(const_vertex_iterator beg,
                                                const_vertex_iterator end) {
    typename set_type::const_iterator &beg_iter = beg;
    typename set_type::const_iterator &end_iter = end;
    std::unordered_set<index_t> indices;
    for (auto iter = beg_iter; iter != end_iter; ++iter) {
        indices.insert(vertices.get_index(iter));
    }
    auto result = vertices.erase(beg_iter, end_iter);
    for (auto &vertex : vertices) {
        auto pre = vertex.edges.cbefore_begin();
        for (auto edge = vertex.edges.cbegin(); edge != vertex.edges.cend();
             ++edge) {
            if (indices.count(edge->head) == 1) {
                edge = vertex.edges.erase_after(pre);
                --edge_cnt;
            } else {
                ++edge;
                ++pre;
            }
        }
    }
    return result;
}
template <typename ValueT, typename WeightT, typename Compare>
typename Digraph<ValueT, WeightT, Compare>::size_type
Digraph<ValueT, WeightT, Compare>::erase_vertex(const ValueT &key) {
    std::unordered_set<index_t> indices;
    for (auto iter = vertices.cbegin(); iter != vertices.cend(); ++iter) {
        if (iter->value == key)
            indices.insert(vertices.get_index(iter));
    }

    for (auto index : indices)
        vertices.erase(vertices.get_iterator(index));

    for (auto &vertex : vertices) {
        auto pre = vertex.edges.cbefore_begin();
        for (auto edge = vertex.edges.cbegin(); edge != vertex.edges.cend();
             ++edge) {
            if (indices.count(edge->head) == 1)
                edge = vertex.edges.erase_after(pre);
            else {
                ++edge;
                ++pre;
            }
        }
    }
    edge_cnt -= indices.size();
    return indices.size();
}
template <typename ValueT, typename WeightT, typename Compare>
void Digraph<ValueT, WeightT, Compare>::insert_edge(const_vertex_iterator tail,
                                                    const_vertex_iterator head,
                                                    const WeightT &weight) {
    typename set_type::const_iterator &tail_iter = tail;
    typename set_type::const_iterator &head_iter = head;
    tail_iter->edges.push_front(
        {.weight = weight, .head = vertices.get_index(head_iter)});
    ++edge_cnt;
}
template <typename ValueT, typename WeightT, typename Compare>
void Digraph<ValueT, WeightT, Compare>::insert_edge(const_vertex_iterator tail,
                                                    const_vertex_iterator head,
                                                    WeightT &&weight) {
    typename set_type::const_iterator &tail_iter = tail;
    typename set_type::const_iterator &head_iter = head;
    tail_iter->edges.push_front(
        std::move({.weight = weight, .head = vertices.get_index(head_iter)}));
    ++edge_cnt;
}

template <typename ValueT, typename WeightT, typename Compare>
typename Digraph<ValueT, WeightT, Compare>::edge_iterator
Digraph<ValueT, WeightT, Compare>::erase_after_edge(const_vertex_iterator tail,
                                                    const_edge_iterator edge) {
    typename set_type::const_iterator &tail_iter = tail;
    auto result = tail_iter->edges.erase_after(edge);
    --edge_cnt;
    return result;
}
template <typename ValueT, typename WeightT, typename Compare>
typename Digraph<ValueT, WeightT, Compare>::edge_iterator
Digraph<ValueT, WeightT, Compare>::erase_after_edge(const_vertex_iterator tail,
                                                    const_edge_iterator beg,
                                                    const_edge_iterator end) {
    typename set_type::const_iterator &tail_iter = tail;
    auto result = tail_iter->edges.erase_after(beg, end);
    --edge_cnt;
    return result;
}

template <typename ValueT, typename WeightT, typename Compare>
typename Digraph<ValueT, WeightT, Compare>::edge_iterator
Digraph<ValueT, WeightT, Compare>::edge_begin(
    const_vertex_iterator vertex) noexcept {
    typename set_type::const_iterator &ver_iter = vertex;
    return ver_iter.edges.begin();
}

template <typename ValueT, typename WeightT, typename Compare>
typename Digraph<ValueT, WeightT, Compare>::const_edge_iterator
Digraph<ValueT, WeightT, Compare>::edge_begin(
    const_vertex_iterator vertex) const noexcept {
    typename set_type::const_iterator &ver_iter = vertex;
    return ver_iter.edges.cbegin();
}

template <typename ValueT, typename WeightT, typename Compare>
typename Digraph<ValueT, WeightT, Compare>::const_edge_iterator
Digraph<ValueT, WeightT, Compare>::cedge_begin(
    const_vertex_iterator vertex) const noexcept {
    typename set_type::const_iterator &ver_iter = vertex;
    return ver_iter.edges.cbegin();
}
template <typename ValueT, typename WeightT, typename Compare>
typename Digraph<ValueT, WeightT, Compare>::edge_iterator
Digraph<ValueT, WeightT, Compare>::edge_end(
    const_vertex_iterator vertex) noexcept {
    typename set_type::const_iterator &ver_iter = vertex;
    return ver_iter.edges.end();
}

template <typename ValueT, typename WeightT, typename Compare>
typename Digraph<ValueT, WeightT, Compare>::const_edge_iterator
Digraph<ValueT, WeightT, Compare>::edge_end(const_vertex_iterator vertex) const
    noexcept {
    typename set_type::const_iterator &ver_iter = vertex;
    return ver_iter.edges.cend();
}

template <typename ValueT, typename WeightT, typename Compare>
typename Digraph<ValueT, WeightT, Compare>::const_edge_iterator
Digraph<ValueT, WeightT, Compare>::cedge_end(const_vertex_iterator vertex) const
    noexcept {
    typename set_type::const_iterator &ver_iter = vertex;
    return ver_iter.edges.cend();
}
template <typename ValueT, typename WeightT, typename Compare>
typename Digraph<ValueT, WeightT, Compare>::edge_iterator
Digraph<ValueT, WeightT, Compare>::edge_before_begin(
    const_vertex_iterator vertex) noexcept {
    typename set_type::const_iterator &ver_iter = vertex;
    return ver_iter.edges.before_begin();
}

template <typename ValueT, typename WeightT, typename Compare>
typename Digraph<ValueT, WeightT, Compare>::const_edge_iterator
Digraph<ValueT, WeightT, Compare>::edge_before_begin(
    const_vertex_iterator vertex) const noexcept {
    typename set_type::const_iterator &ver_iter = vertex;
    return ver_iter.edges.cbefore_begin();
}

template <typename ValueT, typename WeightT, typename Compare>
typename Digraph<ValueT, WeightT, Compare>::const_edge_iterator
Digraph<ValueT, WeightT, Compare>::cedge_before_begin(
    const_vertex_iterator vertex) const noexcept {
    typename set_type::const_iterator &ver_iter = vertex;
    return ver_iter.edges.cbefore_begin();
}
template <typename ValueT, typename WeightT, typename Compare>
void Digraph<ValueT, WeightT, Compare>::clear() noexcept {
    vertices.clear();
    edge_cnt = 0;
}

} // namespace GPSOINN

#endif // GPSOINN_GRAPH_HXX
