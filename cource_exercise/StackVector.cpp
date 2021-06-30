#include "stack_vector.h"
#include "my_assert.h"
#include "../search_server/log_duration.h"

#include <iostream>
#include <random>
#include <stdexcept>
#include <iterator>

using namespace std;

template <typename T, size_t N>
StackVector<T, N>::StackVector(size_t a_size) : size_(a_size) {
    if (a_size > arr_.size()) {
        throw invalid_argument("a_size"s);
    }
}

template <typename T, size_t N>
T& StackVector<T, N>::operator[](size_t index) {
    return arr_[index];
}

template <typename T, size_t N>
const T& StackVector<T, N>::operator[](size_t index) const {
    return arr_.at(index);
}

template <typename T, size_t N>
typename std::array<T, N>::iterator StackVector<T, N>::begin() {
    return arr_.begin();
}

template <typename T, size_t N>
typename StackVector<T, N>::iterator StackVector<T, N>::end() {
    return arr_.begin() + size_;
}

template <typename T, size_t N>
auto StackVector<T, N>::begin() const {
    return arr_.begin();
}

template <typename T, size_t N>
auto StackVector<T, N>::end() const {
    return arr_.begin() + size_;
}

template <typename T, size_t N>
size_t StackVector<T, N>::Size() const {
    return size_;
}

template <typename T, size_t N>
size_t StackVector<T, N>::Capacity() const {
    return N;
}

template <typename T, size_t N>
void StackVector<T, N>::PushBack(const T& value) {
    if (size_ == N) {
        throw overflow_error("overflow_error"s);
    }
    arr_[size_] = value;
    size_++;
}

template <typename T, size_t N>
T StackVector<T, N>::PopBack() {
    if (size_ == 0) {
        throw underflow_error("underflow_error"s);
    }
    T* buf = &arr_[size_ - 1];
    size_--;
    return *buf;
}

void TestConstruction() {
    StackVector<int, 10> v;
    assert(v.Size() == 0u);
    assert(v.Capacity() == 10u);

    StackVector<int, 8> u(5);
    assert(u.Size() == 5u);
    assert(u.Capacity() == 8u);

    try {
        StackVector<int, 10> u(50);
        cout << "Expect invalid_argument for too large size"s << endl;
        assert(false);
    } catch (invalid_argument&) {
    } catch (...) {
        cout << "Expect invalid_argument for too large size"s << endl;
        assert(false);
    }
}

void TestPushBack() {
    StackVector<int, 5> v;
    for (size_t i = 0; i < v.Capacity(); ++i) {
        v.PushBack(i);
    }

    try {
        v.PushBack(0);
        cout << "Expect overflow_error for PushBack in full vector"s << endl;
        assert(false);
    } catch (overflow_error&) {
    } catch (...) {
        cout << "Unexpected exception for PushBack in full vector"s << endl;
        assert(false);
    }
}

void TestPopBack() {
    StackVector<int, 5> v;
    for (size_t i = 1; i <= v.Capacity(); ++i) {
        v.PushBack(i);
    }
    for (int i = v.Size(); i > 0; --i) {
        assert(v.PopBack() == i);
    }

    try {
        v.PopBack();
        cout << "Expect underflow_error for PopBack from empty vector"s << endl;
        assert(false);
    } catch (underflow_error&) {
    } catch (...) {
        cout << "Unexpected exception for PopBack from empty vector"s << endl;
        assert(false);
    }
}

int main() {
    TestConstruction();
    TestPushBack();
    TestPopBack();

    cerr << "Running benchmark..."s << endl;
    const size_t max_size = 2500;

    default_random_engine re;
    uniform_int_distribution<int> value_gen(1, max_size);

    vector<vector<int>> test_data(50000);
    for (auto& cur_vec : test_data) {
        cur_vec.resize(value_gen(re));
        for (int& x : cur_vec) {
            x = value_gen(re);
        }
    }

    {
        LOG_DURATION("vector w/o reserve");
        for (auto& cur_vec : test_data) {
            vector<int> v;
            for (int x : cur_vec) {
                v.push_back(x);
            }
        }
    }
    {
        LOG_DURATION("vector with reserve");
        for (auto& cur_vec : test_data) {
            vector<int> v;
            v.reserve(cur_vec.size());
            for (int x : cur_vec) {
                v.push_back(x);
            }
        }
    }
    {
        LOG_DURATION("StackVector");
        for (auto& cur_vec : test_data) {
            StackVector<int, max_size> v;
            for (int x : cur_vec) {
                v.PushBack(x);
            }
        }
    }
    cerr << "Done"s << endl;
    return 0;
}
