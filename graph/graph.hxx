#ifndef GPSOINN_GRAPH_HXX
#define GPSOINN_GRAPH_HXX

#include <functional>
#include <iterator>
#include <vector>

namespace GPSOINN {

template <typename Value> union multiset_value_type {
    size_t next;
    Value value;
};
template <typename Key, typename Compare = std::less<Key>,
          typename Allocator = std::allocator<multiset_value_type<Key>>>
class multiset : protected std::vector<multiset_value_type<Key>, Allocator> {
  public:
    class iterator;
    class const_iterator;
    class reverse_iterator;
    class const_reverse_iterator;

    // member types
    typedef Key key_type;
    typedef Key value_type;
    typedef Compare key_compare;
    typedef Compare value_compare;
    typedef std::vector<multiset_value_type<value_type>, Allocator> vector;
    typedef std::vector<bool> bit_vector;

    typedef typename vector::allocator_type allocator_type;
    typedef typename vector::size_type size_type;
    typedef std::ptrdiff_t difference_type;
    typedef value_type &reference;
    typedef const value_type &const_reference;

    /* constructors */
    multiset() : multiset(Compare()){};
    explicit multiset(const Compare &comp, const Allocator &alloc = Allocator())
        : less(comp), vector(alloc){};
    // copy constructors
    multiset(const multiset &other) = default;
    multiset(const multiset &other, const Allocator &alloc);
    // move constructors
    multiset(multiset &&other) noexcept = default;
    multiset(multiset &&other, const Allocator &alloc);

    /* destructors */
    virtual ~multiset() = default;

    /* modifiers */
    // insert
    iterator insert(const value_type &value);
    iterator insert(value_type &&value);
    // erase
    iterator erase(const_iterator pos);
    iterator erase(const_iterator beg, const_iterator end);
    size_type erase(const key_type &key);
    // clear
    void clear() noexcept;

    /* capacity */
    [[nodiscard]] bool empty() const noexcept;
    size_type size() const noexcept;

    /* operators */
    multiset &operator=(const multiset &other);
    multiset &operator=(multiset &&other) noexcept;

    allocator_type get_allocator() const;

    // iterators
    iterator begin() noexcept;
    const_iterator begin() const noexcept;
    const_iterator cbegin() const noexcept;
    iterator end() noexcept;
    const_iterator end() const noexcept;
    const_iterator cend() const noexcept;

  private:
    Compare less;
    size_type count = 0;
    size_type first_free = 0;
    bit_vector valid_bits;
};

} // namespace GPSOINN

namespace GPSOINN {
    
template <typename Key, typename Compare, typename Allocator>
class multiset<Key, Compare, Allocator>::iterator {
    friend multiset<Key, Compare, Allocator>;
    friend multiset<Key, Compare, Allocator>::const_iterator;

  public:
    friend bool operator==(const iterator &lhs, const iterator &rhs) {
        return lhs.iter == rhs.iter;
    }
    friend bool operator!=(const iterator &lhs, const iterator &rhs) {
        return !(lhs == rhs);
    }

    typedef std::ptrdiff_t difference_type;
    typedef Key value_type;
    typedef value_type *pointer;
    typedef value_type &reference;
    typedef std::bidirectional_iterator_tag iterator_category;

    iterator() : vector(nullptr) {}

    // dereference
    reference operator*() const { return iter->value; }
    pointer operator->() const { return &(iter->value); }
    // increment
    iterator &operator++() {
        ++bit_iter;
        ++iter;
        while (iter != vector->end() && !*bit_iter) {
            ++bit_iter;
            ++iter;
        }
        return *this;
    }
    iterator operator++(int) {
        iterator previous = *this;
        this->operator++();
        return previous;
    }
    // decrement
    iterator &operator--() {
        while (iter != vector->begin() && !*bit_iter) {
            --bit_iter;
            --iter;
        }
        return *this;
    }
    iterator operator--(int) {
        iterator previous = *this;
        this->operator--();
        return previous;
    }

  private:
    typename vector::iterator iter;
    const vector *vector;
    typename bit_vector::const_iterator bit_iter;
};

template <typename Key, typename Compare, typename Allocator>
class multiset<Key, Compare, Allocator>::const_iterator {
    friend multiset<Key, Compare, Allocator>;

  public:
    friend bool operator==(const const_iterator &lhs, const const_iterator &rhs) {
        return lhs.iter == rhs.iter;
    }
    friend bool operator!=(const const_iterator &lhs, const const_iterator &rhs) {
        return !(lhs == rhs);
    }
    typedef std::ptrdiff_t difference_type;
    typedef const Key value_type;
    typedef value_type *pointer;
    typedef value_type &reference;
    typedef std::bidirectional_iterator_tag iterator_category;

    const_iterator() : vector(nullptr) {}
    const_iterator(const iterator &other) : iter(other.iter), vector(other.vector), bit_iter(other.bit_iter) {}

    // dereference
    reference operator*() const { return iter->value; }
    pointer operator->() const { return &(iter->value); }
    // increment
    const_iterator &operator++() {
        ++bit_iter;
        ++iter;
        while (iter != vector->cend() && !*bit_iter) {
            ++bit_iter;
            ++iter;
        }
        return *this;
    }
    const_iterator operator++(int) {
        iterator previous = *this;
        this->operator++();
        return previous;
    }
    // decrement
    const_iterator &operator--() {
        while (iter != vector->cbegin() && !*bit_iter) {
            --bit_iter;
            --iter;
        }
        return *this;
    }
    const_iterator operator--(int) {
        iterator previous = *this;
        this->operator--();
        return previous;
    }

  private:
    typename vector::const_iterator iter;
    const vector *vector;
    typename bit_vector::const_iterator bit_iter;
};

template <typename T, typename Compare, typename Allocator>
typename multiset<T, Compare, Allocator>::iterator
multiset<T, Compare, Allocator>::insert(const T &value) {
    if (count == vector::size()) {
        // construct the union to be inserted
        multiset_value_type<T> elem;
        elem.value = value;
        // maintain the valid bits
        valid_bits.push_back(1);
        // call the push back
        typename vector::iterator iter;
        try {
            vector::push_back(elem);
        } catch (...) {
            valid_bits.pop_back();
            throw;
        }
        // increment count
        ++count;

        iterator result;
        result.iter = vector::begin() + (vector::size() - 1);
        result.bit_iter = valid_bits.begin() + (vector::size() - 1);
        result.vector = this;
        return result;
    }

    // get the elem first
    auto &elem = vector::operator[](first_free);
    // store the position
    size_type pos = first_free;

    valid_bits[pos] = 1;
    // pop front operation on the free list
    first_free = elem.next;
    // assign
    elem.value = value;

    // increment count
    ++count;

    iterator result;
    result.iter = vector::begin() + pos;
    result.bit_iter = valid_bits.begin() + pos;
    result.vector = this;
    return result;
}

template <typename T, typename Compare, typename Allocator>
typename multiset<T, Compare, Allocator>::iterator
multiset<T, Compare, Allocator>::insert(T &&value) {
    if (count == vector::size()) {
        // construct the union to be inserted
        multiset_value_type<T> elem;
        elem.value = value;
        // maintain the valid bits
        valid_bits.push_back(1);
        // call the push back
        typename vector::iterator iter;
        try {
            vector::push_back(std::move(elem));
        } catch (...) {
            valid_bits.pop_back();
            throw;
        }
        // increment count
        ++count;


        iterator result;
        result.iter = vector::begin() + (vector::size() - 1);
        result.bit_iter = valid_bits.begin() + (vector::size() - 1);
        result.vector = this;
        return result;
    }

    // get the elem first
    auto &elem = vector::operator[](first_free);
    // store the position
    size_type pos = first_free;

    valid_bits[pos] = 1;
    // pop front operation on the free list
    first_free = elem.next;
    // assign
    elem.value = value;

    // increment count
    ++count;

    iterator result;
    result.iter = vector::begin() + pos;
    result.bit_iter = valid_bits.begin() + pos;
    result.vector = this;
    return result;
}

template <typename Key, typename Compare, typename Allocator>
typename multiset<Key, Compare, Allocator>::iterator
multiset<Key, Compare, Allocator>::erase(const_iterator pos) {
    typename vector::difference_type index = pos.iter - vector::cbegin();
    auto iter = vector::begin() + index;

    iterator result;
    result.iter = iter;
    result.bit_iter = pos.bit_iter;
    result.vector = pos.vector;
    ++result;

    valid_bits[index] = 0;

    iter->next = first_free;
    first_free = index;

    // decrement count
    --count;

    return result;
}

template <typename Key, typename Compare, typename Allocator>
typename multiset<Key, Compare, Allocator>::iterator
multiset<Key, Compare, Allocator>::erase(const_iterator beg,
                                         const_iterator end) {
    while (beg != end)
        beg = erase(beg);

    iterator result;
    result.iter = vector::begin + (end.iter - vector::cbegin());
    result.bit_iter = end.bit_iter;
    result.vector = end.bit_iter;
    return result;
}

template <typename Key, typename Compare, typename Allocator>
typename multiset<Key, Compare, Allocator>::size_type
multiset<Key, Compare, Allocator>::erase(const key_type &key) {
    auto iter = cbegin();
    size_type count = 0;
    while (iter != cend()) {
        if (!less(*iter, key) && !less(key, *iter)) {
            iter = erase(iter);
            ++count;
        }
        else
            ++iter;
    }
    return count;
}

template <typename Key, typename Compare, typename Allocator>
void multiset<Key, Compare, Allocator>::clear() noexcept {
    vector::clear();
    valid_bits.clear();
    first_free = 0;
    count = 0;
}

template <typename Key, typename Compare, typename Allocator>
bool multiset<Key, Compare, Allocator>::empty() const noexcept {
    return count == 0;
}

template <typename Key, typename Compare, typename Allocator>
typename multiset<Key, Compare, Allocator>::size_type
multiset<Key, Compare, Allocator>::size() const noexcept {
    return count;
}

template <typename Key, typename Compare, typename Allocator>
typename multiset<Key, Compare, Allocator>::allocator_type
multiset<Key, Compare, Allocator>::get_allocator() const {
    return vector::get_allocator();
}

template <typename Key, typename Compare, typename Allocator>
typename multiset<Key, Compare, Allocator>::iterator
multiset<Key, Compare, Allocator>::begin() noexcept {
    iterator iter;
    iter.iter = vector::begin();
    iter.bit_iter = valid_bits.cbegin();
    iter.vector = this;
    return iter;
}

template <typename Key, typename Compare, typename Allocator>
typename multiset<Key, Compare, Allocator>::const_iterator
multiset<Key, Compare, Allocator>::begin() const noexcept {
    iterator iter;
    iter.iter = vector::cbegin();
    iter.bit_iter = valid_bits.cbegin();
    iter.vector = this;
    return iter;
}

template <typename Key, typename Compare, typename Allocator>
typename multiset<Key, Compare, Allocator>::const_iterator
multiset<Key, Compare, Allocator>::cbegin() const noexcept {
    const_iterator iter;
    iter.iter = vector::cbegin();
    iter.bit_iter = valid_bits.cbegin();
    iter.vector = this;
    return iter;
}

template <typename Key, typename Compare, typename Allocator>
typename multiset<Key, Compare, Allocator>::iterator
multiset<Key, Compare, Allocator>::end() noexcept {
    iterator iter;
    iter.iter = vector::end();
    iter.bit_iter = valid_bits.cend();
    iter.vector = this;
    return iter;
}

template <typename Key, typename Compare, typename Allocator>
typename multiset<Key, Compare, Allocator>::const_iterator
multiset<Key, Compare, Allocator>::end() const noexcept {
    iterator iter;
    iter.iter = vector::cend();
    iter.bit_iter = valid_bits.cend();
    iter.vector = this;
    return iter;
}

template <typename Key, typename Compare, typename Allocator>
typename multiset<Key, Compare, Allocator>::const_iterator
multiset<Key, Compare, Allocator>::cend() const noexcept {
    const_iterator iter;
    iter.iter = vector::cend();
    iter.bit_iter = valid_bits.cend();
    iter.vector = this;
    return iter;
}

template <typename Key, typename Compare, typename Allocator>
multiset<Key, Compare, Allocator>::multiset(const multiset &other,
                                            const Allocator &alloc)
    : vector(other, alloc), valid_bits(other.valid_bits), less(other.less),
      count(other.count), first_free(other.first_free) {}

template <typename Key, typename Compare, typename Allocator>
multiset<Key, Compare, Allocator>::multiset(multiset &&other,
                                            const Allocator &alloc)
    : vector(other, alloc), valid_bits(other.valid_bits), less(other.less),
      count(other.count), first_free(other.first_free) {}

template <typename Key, typename Compare, typename Allocator>
multiset<Key, Compare, Allocator> &multiset<Key, Compare, Allocator>::
operator=(const multiset &other) {
    vector::operator=(other);
    valid_bits = other.valid_bits;
    less = other.less;
    count = other.count;
    first_free = other.first_free;
    return *this;
}
template <typename Key, typename Compare, typename Allocator>
multiset<Key, Compare, Allocator> &multiset<Key, Compare, Allocator>::
operator=(multiset &&other) noexcept {
    vector::operator=(other);
    valid_bits = other.valid_bits;
    less = other.less;
    count = other.count;
    first_free = other.first_free;
    return *this;
}
} // namespace GPSOINN

#endif // GPSOINN_GRAPH_HXX
