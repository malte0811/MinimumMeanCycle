#ifndef MINIMUMMEANCYCLE_TYPESAFEWRAPPER_H
#define MINIMUMMEANCYCLE_TYPESAFEWRAPPER_H

#include <utility>

namespace MMC {

enum class Type {
    vertex,
    edge,
    half_edge,
};

template<typename BaseType, Type T>
class TypesafeWrapper {
public:
    using Self = TypesafeWrapper<BaseType, T>;

    explicit TypesafeWrapper(BaseType const& arg) : _inner(arg) {}

    TypesafeWrapper() : _inner() {}

    BaseType const& get() const {
        return _inner;
    }

    bool operator<(Self const& other) const {
        return get() < other.get();
    }

    bool operator==(Self const& other) const {
        return get() == other.get();
    }

    bool operator!=(Self const& other) const {
        return get() != other.get();
    }

    TypesafeWrapper& operator++() {
        ++_inner;
        return *this;
    }

private:
    BaseType _inner;
};

template<typename BaseType, Type T>
std::ostream& operator<<(std::ostream& out, TypesafeWrapper<BaseType, T> const& to_print) {
    return out << to_print.get();
}

}

#endif //MINIMUMMEANCYCLE_TYPESAFEWRAPPER_H
