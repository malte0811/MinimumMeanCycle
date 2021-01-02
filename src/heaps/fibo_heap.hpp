#ifndef FIBO_HEAP_H
#define FIBO_HEAP_H

#include <vector>
#include <stdexcept>
#include <memory>

namespace MMC::heap {

//TODO rework with unique_ptr
template<typename T, typename C>
class FibonacciHeap : public HeapBase<T> {
public:
    FibonacciHeap(IndexFunc <T> index, size_t max_size);

    bool empty() const;

    void insert(T const& value, C key);

    T extract_min();

    void decrease_key(T const& obj, C newKey);

private:
    struct HeapEntry {
        HeapEntry* parent;
        std::vector<HeapEntry*> children;
        C key;
        T value;
        char phi;
        int parentIndex;
    };

    void plant(HeapEntry* e);

    std::vector<HeapEntry*> roots;
    std::vector<HeapEntry*> all;
};

template<typename T, typename C>
FibonacciHeap<T, C>::FibonacciHeap(IndexFunc <T> index, size_t max_size) :
        HeapBase<T>(std::move(index), max_size) {}

template<typename T, typename C>
void FibonacciHeap<T, C>::decrease_key(T const& obj, C newKey) {
    HeapEntry* ref = all[this->index(obj)];
    ref->key = newKey;
    if (ref->parent == nullptr) {
        return;
    }
    std::vector<HeapEntry*> path;
    HeapEntry* lastOnPath = ref;
    do {
        path.push_back(lastOnPath);
        HeapEntry* parent = lastOnPath->parent;
        if (parent != nullptr) {
            lastOnPath->parent = nullptr;
            parent->children.back()->parentIndex = lastOnPath->parentIndex;
            parent->children[lastOnPath->parentIndex] = parent->children.back();
            parent->children.pop_back();
            parent->phi = static_cast<char>(1 - parent->phi);
        }
        lastOnPath = parent;
    } while (lastOnPath != nullptr && lastOnPath->phi == 1);
    if (lastOnPath != nullptr && lastOnPath->parent == nullptr) {
        path.push_back(lastOnPath);
        lastOnPath = nullptr;
    }
    bool isLastRoot = lastOnPath == nullptr;
    auto it = path.begin();
    auto maxIt = path.end();
    if (isLastRoot) {
        if (roots[path.back()->children.size() + 1] != path.back()) {
            throw;
        }
        roots[path.back()->children.size() + 1] = nullptr;
    }
    while (it != maxIt) {
        plant(*it);
        ++it;
    }
}

template<typename T, typename C>
T FibonacciHeap<T, C>::extract_min() {
    HeapEntry** min = nullptr;
    for (auto& root:roots) {
        if (root != nullptr && (min == nullptr || root->key < (*min)->key)) {
            min = &root;
        }
    }
    if (min == nullptr) {
        throw std::runtime_error("Tried to remove Entry from empty heap!");
    }
    auto& minCp = *min;
    *min = nullptr;
    for (size_t i = 0; i < minCp->children.size(); ++i) {
        auto&& child = minCp->children.at(i);
        child->parent = nullptr;
        plant(std::move(child));
    }
    while (!roots.empty() && roots.back() == nullptr) {
        roots.pop_back();
    }
    T ret = minCp->value;
    size_t& index = this->index(ret);
    all[index] = std::move(all.back());
    this->index(all[index]->value) = index;
    index = this->invalid_index();
    all.pop_back();
    return ret;
}

template<typename T, typename C>
void FibonacciHeap<T, C>::insert(T const& value, C key) {
    auto* newNode = new HeapEntry{nullptr, std::vector<HeapEntry*>(), key, value, 0, 0};
    this->index(value) = all.size();
    all.push_back(newNode);
    plant(newNode);
}

template<typename T, typename C>
bool FibonacciHeap<T, C>::empty() const {
    return roots.empty();
}

template<typename T, typename C>
void FibonacciHeap<T, C>::plant(HeapEntry* e) {
    size_t childCount = e->children.size();
    while (e != nullptr) {
        while (childCount >= roots.size()) {
            roots.push_back(nullptr);
        }
        if (roots[childCount] == nullptr) {
            roots[childCount] = std::move(e);
            e = nullptr;
        } else {
            HeapEntry* other = roots[childCount];
            roots[childCount] = nullptr;
            if (other->key > e->key) {
                other->parentIndex = static_cast<int>(e->children.size());
                other->parent = e;
                e->children.push_back(other);
            } else {
                e->parentIndex = static_cast<int>(other->children.size());
                e->parent = other;
                other->children.push_back(e);
                e = other;
            }
            ++childCount;
        }
    }
}

}

#endif
