#include <functional>
#include <iostream>
using namespace std;

bool IsPrime(int x) {
    // чтобы проверить x на простоту
    // перебираем все числа до корня из x
    for (int i = 2; i * i <= x; ++i) {
        if (x % i == 0) {
            return false;
        }
    }

    return true;
}

int main() {
    // function из <functional> позволяет сохранить объект, 
    // ведущий себя подобно функции, и вызывать его. Например, лямбда-функцию.
    // В качестве шаблонного параметра указывается сигнатура нужной функции
    function<int(int)> mult_prime;
    static const int N = 100'000;

    // Между числами N и 2N всегда есть простое число.
    // Найдём самое большое из них в цикле.
    for (int i = N; i < 2 * N; ++i) {
        if (IsPrime(i)) {
            mult_prime = [&](int x) {
                //stack-use-after-scope
                return x * i;
            };
        }
    }

    // mult_prime должна умножать аргумент на найденное простое число
    cout << mult_prime(100) << endl;
}
