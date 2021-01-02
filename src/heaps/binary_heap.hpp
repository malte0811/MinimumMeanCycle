#ifndef BINARY_HEAP_HPP
#define BINARY_HEAP_HPP

#include "heap_base.h"
#include <vector>
#include <functional>
#include <iostream>

namespace MMC::heap {

template<typename T, typename C>
class BinaryHeap : public HeapBase<T> {
public:
    BinaryHeap(IndexFunc <T> index, size_t max_size);

    //O(log n)
    T extract_min();

    //O(log n)
    void insert(T const& obj, C cost);

    //O(log n)
    void decrease_key(T const& obj, C cost);

    bool empty() const;

private:
    struct Entry {
        T val;
        C cost;
    };

    void sift_down(size_t index);

    void sift_up(size_t index);

    size_t up(size_t in);

    size_t left(size_t in);

    size_t right(size_t in);

    std::vector<Entry> _contents;
};

template<typename T, typename C>
BinaryHeap<T, C>::BinaryHeap(IndexFunc <T> index, size_t max_size)
        :HeapBase<T>(std::move(index), max_size), _contents(0) {}

template<typename T, typename C>
T BinaryHeap<T, C>::extract_min() {
    this->index(_contents.front().val) = this->invalid_index();
    T ret = _contents.front().val;
    _contents.front() = _contents.back();
    this->index(_contents.front().val) = 0;
    _contents.pop_back();
    if (_contents.size() > 1) {
        sift_down(0);
    }
    return ret;
}

template<typename T, typename C>
void BinaryHeap<T, C>::insert(T const& obj, C cost) {
    this->index(obj) = _contents.size();
    _contents.push_back({obj, cost});
    sift_up(_contents.size() - 1);
}

template<typename T, typename C>
void BinaryHeap<T, C>::decrease_key(T const& obj, C cost) {
    size_t const index = this->index(obj);
    _contents[index].cost = cost;
    sift_up(index);
}

template<typename T, typename C>
bool BinaryHeap<T, C>::empty() const {
    return _contents.empty();
}

template<typename T, typename C>
void BinaryHeap<T, C>::sift_down(size_t index) {
    bool moved = true;
    Entry* at = &_contents.at(index);
    const size_t max_with_left = _contents.size() / 2 - 1;
    while (moved and index <= max_with_left) {
        size_t const left_pos = left(index);
        size_t right_pos = right(index);
        if (right_pos >= _contents.size()) {
            right_pos = index;
        }
        Entry& leftObj = _contents.at(left_pos);
        Entry& rightObj = _contents.at(right_pos);
        if (leftObj.cost < at->cost or rightObj.cost < at->cost) {
            if (leftObj.cost < rightObj.cost) {
                std::swap(this->index(at->val), this->index(leftObj.val));
                std::swap(*at, leftObj);
                at = &leftObj;
                index = left_pos;
            } else {
                std::swap(this->index(at->val), this->index(rightObj.val));
                std::swap(*at, rightObj);
                index = right_pos;
                at = &rightObj;
            }
        } else {
            moved = false;
        }
    }
}

template<typename T, typename C>
void BinaryHeap<T, C>::sift_up(size_t index) {
    bool moved = true;
    Entry* at = &_contents[index];
    while (moved and index > 0) {
        size_t up_pos = up(index);
        Entry& up_obj = _contents[up_pos];
        if (at->cost < up_obj.cost) {
            std::swap(this->index(at->val), this->index(up_obj.val));
            std::swap(*at, up_obj);
            at = &up_obj;
            index = up_pos;
        } else {
            moved = false;
        }
    }
}

template<typename T, typename C>
size_t BinaryHeap<T, C>::up(size_t in) {
    return (in - 1) / 2;
}

template<typename T, typename C>
size_t BinaryHeap<T, C>::left(size_t in) {
    return in * 2 + 1;
}

template<typename T, typename C>
size_t BinaryHeap<T, C>::right(size_t in) {
    return in * 2 + 2;
}

}

#endif
