#ifndef MINIMUMMEANCYCLE_HEAP_BASE_H
#define MINIMUMMEANCYCLE_HEAP_BASE_H

#include <functional>

namespace MMC::heap {
template<class T>
using IndexFunc = std::function<size_t&(T const&)>;

//Self: "Curiously recurring template pattern", used in decrease_or_insert
template<typename Value, typename Cost, template<typename V, typename C> typename Self>
class HeapBase {
public:
    bool contains(Value const& val) const;

    void decrease_or_insert(Value const& obj, Cost cost);

protected:
    HeapBase(IndexFunc<Value>&& index, size_t invalid_index);

    [[nodiscard]] size_t index(Value const& val) const;

    [[nodiscard]] size_t& index(Value const& val);

    [[nodiscard]] size_t invalid_index() const;

private:
    IndexFunc<Value> _index;
    size_t const _invalid_index;
};

template<typename Value, typename Cost, template<typename, typename> typename Self>
bool HeapBase<Value, Cost, Self>::contains(Value const& val) const {
    return _index(val) != _invalid_index;
}

template<typename Value, typename Cost, template<typename, typename> typename Self>
HeapBase<Value, Cost, Self>::HeapBase(IndexFunc<Value>&& index, size_t const invalid_index):
        _index(index),
        _invalid_index(invalid_index) {}

template<typename Value, typename Cost, template<typename, typename> typename Self>
size_t HeapBase<Value, Cost, Self>::index(Value const& val) const {
    return _index(val);
}

template<typename Value, typename Cost, template<typename, typename> typename Self>
size_t& HeapBase<Value, Cost, Self>::index(Value const& val) {
    return _index(val);
}

template<typename Value, typename Cost, template<typename, typename> typename Self>
size_t HeapBase<Value, Cost, Self>::invalid_index() const {
    return _invalid_index;
}

template<typename Value, typename Cost, template<typename, typename> typename Self>
void HeapBase<Value, Cost, Self>::decrease_or_insert(Value const& obj, Cost cost) {
    auto* heap = static_cast<Self<Value, Cost>*>(this);
    if (not heap->contains(obj)) {
        heap->insert(obj, cost);
    } else {
        heap->decrease_key(obj, cost);
    }
}

}

#endif //MINIMUMMEANCYCLE_HEAP_BASE_H
