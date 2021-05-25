#include <chrono>
#include <iostream>
#include <thread>

using namespace std;
using namespace chrono;

int main() {
    cout << "Ожидание 5s..."s << endl;
    const chrono::steady_clock::time_point start_time = chrono::steady_clock::now();

    // операция - ожидание 5 секунд
    this_thread::sleep_for(chrono::seconds(5));
    const chrono::steady_clock::time_point end_time = chrono::steady_clock::now();

    const chrono::steady_clock::duration dur = end_time - start_time;
    cerr << "Продолжительность сна: "s << duration_cast<milliseconds>(dur).count() << " ms"s << endl;

    cout << "Ожидание завершено"s << endl;

    return 0;
}
