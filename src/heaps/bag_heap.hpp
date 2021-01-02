#ifndef BAG_HEAP_HPP
#define BAG_HEAP_HPP

#include "heap_base.h"
#include <vector>

namespace MMC::heap {

template<typename T, typename C>
class BagHeap : public HeapBase<T> {
public:

    BagHeap(IndexFunc<T> index, size_t max_size);

    //O(n)
    T extract_min();

    //O(1)
    void insert(T const& obj, C cost);

    //O(1)
    void decrease_key(T const& obj, C cost);

    bool empty() const;

private:
    struct Entry {
        T val;
        C cost;
    };

    std::vector<Entry> _contents;
};

template<typename T, typename C>
BagHeap<T, C>::BagHeap(IndexFunc<T> index, size_t max_size)
        : HeapBase<T>(std::move(index), max_size) {}

template<typename T, typename C>
T BagHeap<T, C>::extract_min() {
    Entry min = _contents[0];
    size_t min_index = 0;
    for (size_t i = 1; i < _contents.size(); ++i) {
        Entry& curr = _contents[i];
        if (curr.cost < min.cost) {
            min = curr;
            min_index = i;
        }
    }
    _contents[min_index] = _contents.back();
    _contents.pop_back();
    this->index(min.val) = this->invalid_index();
    this->index(_contents[min_index].val) = min_index;
    return min.val;
}

template<typename T, typename C>
void BagHeap<T, C>::insert(T const& obj, C cost) {
    this->index(obj) = _contents.size();
    _contents.push_back({obj, cost});
}

template<typename T, typename C>
void BagHeap<T, C>::decrease_key(T const& obj, C cost) {
    size_t index = this->index(obj);
    _contents[index].cost = cost;
}

template<typename T, typename C>
bool BagHeap<T, C>::empty() const {
    return _contents.empty();
}

}

#endif
