#pragma once

#include <array>
#include <stdexcept>

template <typename T, size_t N>
class StackVector {
public:
    using iterator = typename std::array<T, N>::iterator;

    explicit StackVector(size_t a_size = 0);

    T& operator[](size_t index);
    const T& operator[](size_t index) const;

    typename std::array<T, N>::iterator begin();
    iterator end();
    auto begin() const;
    auto end() const;

    size_t Size() const;
    size_t Capacity() const;

    void PushBack(const T& value);
    T PopBack();

private:
    std::array<T, N> arr_;
    size_t size_;
};
