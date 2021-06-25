#include <iostream>
#include <vector>

using namespace std;

template <class T>
double mean(const std::vector<T>& v) {
    T acc;
    const T* v_ptr = v.data();

    for (size_t i = 0; i <= v.size(); ++i) {
        //heap-buffer-overflow
        //Ошибка говорит, что мы вышли за пределы допустимого диапазона в куче
        acc += v_ptr[i];
    }

    return static_cast<double>(acc) / v.size();
}

int main() {
    vector<int> v = {1, 2, 3, 4, 5};
    //v.reserve(10); //с этой штукой ошибки от санитайзера не будет, но и работа будет некорректная
    cout << "Mean: "s << mean(v) << endl;
}
