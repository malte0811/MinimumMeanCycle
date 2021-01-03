#ifndef BAG_HEAP_HPP
#define BAG_HEAP_HPP

#include "heap_base.h"
#include <vector>

namespace MMC::heap {

template<typename T, typename C>
class BagHeap : public HeapBase<T, C, BagHeap> {
public:

    BagHeap(IndexFunc<T> index, size_t max_size);

    //O(n)
    T extract_min();

    //O(1)
    void insert(T const& obj, C cost);

    //O(1)
    void decrease_key(T const& obj, C cost);

    [[nodiscard]] bool empty() const;

private:
    struct Entry {
        T val;
        C cost;
    };

    std::vector<Entry> _contents;
};

template<typename T, typename C>
BagHeap<T, C>::BagHeap(IndexFunc<T> index, size_t max_size)
        : HeapBase<T, C, BagHeap>(std::move(index), max_size) {}

template<typename T, typename C>
T BagHeap<T, C>::extract_min() {
    auto const& result_it = std::min_element(_contents.begin(), _contents.end(), [](auto const& a, auto const& b) {
        return a.cost < b.cost;
    });
    auto const result = result_it->val;
    *result_it = _contents.back();
    this->index(result) = this->invalid_index();
    if (result_it + 1 != _contents.end()) {
        auto const index = std::distance(_contents.begin(), result_it);
        this->index(_contents[index].val) = index;
    }
    _contents.pop_back();
    return result;
}

template<typename T, typename C>
void BagHeap<T, C>::insert(T const& obj, C cost) {
    this->index(obj) = _contents.size();
    _contents.push_back({obj, cost});
}

template<typename T, typename C>
void BagHeap<T, C>::decrease_key(T const& obj, C cost) {
    size_t const index = this->index(obj);
    _contents[index].cost = cost;
}

template<typename T, typename C>
bool BagHeap<T, C>::empty() const {
    return _contents.empty();
}

}

#endif
