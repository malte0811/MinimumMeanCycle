#ifndef MINIMUMMEANCYCLE_HEAP_H
#define MINIMUMMEANCYCLE_HEAP_H

#include "bag_heap.hpp"
#include "fibo_heap.hpp"
#include "binary_heap.hpp"

namespace MMC::heap {

template<class Value, class Cost>
using Heap = BinaryHeap<Value, Cost>;

template<class Value, class Cost>
void decrease_or_insert(Heap<Value, Cost>& heap, Value const& obj, Cost cost) {
    if (not heap.contains(obj)) {
        heap.insert(obj, cost);
    } else {
        heap.decrease_key(obj, cost);
    }
}

}

#endif //MINIMUMMEANCYCLE_HEAP_H
