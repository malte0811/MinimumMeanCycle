#ifndef MINIMUMMEANCYCLE_BINARY_HEAP_H
#define MINIMUMMEANCYCLE_BINARY_HEAP_H

#include <functional>

namespace MMC {
template<class Value>
using IndexFunc = std::function<size_t&(Value const&)>;

//This probably isn't the most "natural" interface for a binary heap, but the same interface can also be used for
// Fibonacci- and "trivial" heaps (i.e. a vector with linear search)
template<typename Value, typename Cost>
class BinaryHeap {
public:
    BinaryHeap(IndexFunc<Value> const& index, size_t max_size);

    void decrease_or_insert(Value const& obj, Cost cost);

    //O(log n)
    Value extract_min();

    //O(log n)
    void insert(Value const& obj, Cost cost);

    //O(log n)
    void decrease_key(Value const& obj, Cost cost);

    [[nodiscard]] bool empty() const;

private:
    struct Entry {
        Value val;
        Cost cost;
    };

    void sift_down(size_t index);

    void sift_up(size_t index);

    size_t up(size_t in);

    size_t left(size_t in);

    size_t right(size_t in);

    IndexFunc<Value> _index;
    size_t const _invalid_index;
    std::vector<Entry> _contents;
};

template<typename Value, typename Cost>
BinaryHeap<Value, Cost>::BinaryHeap(IndexFunc<Value> const& index, size_t max_size):
        _index(index),
        _invalid_index(max_size),
        _contents(0) {}

template<typename Value, typename Cost>
void BinaryHeap<Value, Cost>::decrease_or_insert(Value const& obj, Cost cost) {
    if (_index(obj) == _invalid_index) {
        insert(obj, cost);
    } else {
        decrease_key(obj, cost);
    }
}

template<typename Value, typename Cost>
Value BinaryHeap<Value, Cost>::extract_min() {
    _index(_contents.front().val) = _invalid_index;
    auto const ret = _contents.front().val;
    _contents.front() = _contents.back();
    _index(_contents.front().val) = 0;
    _contents.pop_back();
    if (_contents.size() > 1) {
        sift_down(0);
    }
    return ret;
}

template<typename Value, typename Cost>
void BinaryHeap<Value, Cost>::insert(Value const& obj, Cost cost) {
    _index(obj) = _contents.size();
    _contents.push_back({obj, cost});
    sift_up(_contents.size() - 1);
}

template<typename Value, typename Cost>
void BinaryHeap<Value, Cost>::decrease_key(Value const& obj, Cost cost) {
    size_t const index = _index(obj);
    _contents[index].cost = cost;
    sift_up(index);
}

template<typename Value, typename Cost>
bool BinaryHeap<Value, Cost>::empty() const {
    return _contents.empty();
}

template<typename Value, typename Cost>
void BinaryHeap<Value, Cost>::sift_down(size_t index) {
    bool moved = true;
    Entry* at_index = &_contents.at(index);
    size_t const max_with_child = _contents.size() / 2 - 1;
    while (moved and index <= max_with_child) {
        size_t const left_pos = left(index);
        size_t right_pos = right(index);
        if (right_pos >= _contents.size()) {
            right_pos = index;
        }
        Entry& left_obj = _contents.at(left_pos);
        Entry& right_obj = _contents.at(right_pos);
        if (left_obj.cost < at_index->cost or right_obj.cost < at_index->cost) {
            if (left_obj.cost < right_obj.cost) {
                std::swap(_index(at_index->val), _index(left_obj.val));
                std::swap(*at_index, left_obj);
                at_index = &left_obj;
                index = left_pos;
            } else {
                std::swap(_index(at_index->val), _index(right_obj.val));
                std::swap(*at_index, right_obj);
                index = right_pos;
                at_index = &right_obj;
            }
        } else {
            moved = false;
        }
    }
}

template<typename Value, typename Cost>
void BinaryHeap<Value, Cost>::sift_up(size_t index) {
    bool moved = true;
    Entry* at = &_contents[index];
    while (moved and index > 0) {
        size_t up_pos = up(index);
        Entry& up_obj = _contents[up_pos];
        if (at->cost < up_obj.cost) {
            std::swap(_index(at->val), _index(up_obj.val));
            std::swap(*at, up_obj);
            at = &up_obj;
            index = up_pos;
        } else {
            moved = false;
        }
    }
}

template<typename Value, typename Cost>
size_t BinaryHeap<Value, Cost>::up(size_t in) {
    return (in - 1) / 2;
}

template<typename Value, typename Cost>
size_t BinaryHeap<Value, Cost>::left(size_t in) {
    return in * 2 + 1;
}

template<typename Value, typename Cost>
size_t BinaryHeap<Value, Cost>::right(size_t in) {
    return in * 2 + 2;
}

}

#endif //MINIMUMMEANCYCLE_BINARY_HEAP_H
