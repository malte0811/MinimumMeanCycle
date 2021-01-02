#ifndef MINIMUMMEANCYCLE_HEAP_BASE_H
#define MINIMUMMEANCYCLE_HEAP_BASE_H

#include <functional>

namespace MMC::heap {
template<class T>
using IndexFunc = std::function<size_t&(T const&)>;

template<typename Value>
class HeapBase {
public:
    bool contains(Value const& val) const;

protected:
    HeapBase(IndexFunc<Value>&& index, size_t invalid_index);

    [[nodiscard]] size_t index(Value const& val) const;

    [[nodiscard]] size_t& index(Value const& val);

    [[nodiscard]] size_t invalid_index() const;

private:
    IndexFunc<Value> _index;
    size_t const _invalid_index;
};

template<typename Value>
bool HeapBase<Value>::contains(Value const& val) const {
    return _index(val) != _invalid_index;
}

template<typename Value>
HeapBase<Value>::HeapBase(IndexFunc<Value>&& index, size_t const invalid_index):
        _index(index),
        _invalid_index(invalid_index) {}

template<typename Value>
size_t HeapBase<Value>::index(Value const& val) const {
    return _index(val);
}

template<typename Value>
size_t& HeapBase<Value>::index(Value const& val) {
    return _index(val);
}

template<typename Value>
size_t HeapBase<Value>::invalid_index() const {
    return _invalid_index;
}
}

#endif //MINIMUMMEANCYCLE_HEAP_BASE_H
