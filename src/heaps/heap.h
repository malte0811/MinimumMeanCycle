#ifndef MINIMUMMEANCYCLE_HEAP_H
#define MINIMUMMEANCYCLE_HEAP_H

#include "bag_heap.hpp"
#include "fibo_heap.hpp"
#include "binary_heap.h"

namespace MMC::heap {

template<class Value, class Cost>
using Heap = BinaryHeap<Value, Cost>;

}

#endif //MINIMUMMEANCYCLE_HEAP_H
