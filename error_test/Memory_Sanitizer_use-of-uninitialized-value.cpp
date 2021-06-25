#include <iostream>
#include <vector>

using namespace std;

//не тестил
//запускать с аргументом -fsanitize=memory

template <class T>
double mean(const std::vector<T>& v) {
    T acc;
    const T* v_ptr = v.data();

    for (size_t i = 0; i < v.size(); ++i) {
        //Uninitialized bytes in __interceptor_memcmp at offset 168 inside [0x7fff5bd922a0, 256)
        //==24619==WARNING: MemorySanitizer: use-of-uninitialized-value
        acc += v_ptr[i];
    }

    return static_cast<double>(acc) / v.size();
}

int main() {
    vector<int> v = {1, 2, 3, 4, 5};
    cout << "Mean: "s << mean(v) << endl;
    return 0;
}
