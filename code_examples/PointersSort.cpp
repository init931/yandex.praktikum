#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

using namespace std;

int main() {
    vector<int> items = {5, 2, 3, 4};
    vector<const int*> item_pointers;
    // Перебирать элементы вектора здесь следует по константной ссылке,
    // а не по значению.
    // В противном случае item_pointers будут хранить адреса объектов,
    // область видимости которых ограничена телом цикла
    for (const int& item : items) {
        item_pointers.push_back(&item);
    }

    // сортируем указатели в порядке убывания чисел, на которые они ссылаются.
    sort(item_pointers.begin(), item_pointers.end(), [](const int* lhs, const int* rhs) {
        // Чтобы сравнить значения, надо разыменовать указатели
        return *lhs > *rhs;
    });

    for (const int* item_ptr : item_pointers) {
        cout << *item_ptr << ", "s;
    }
}
