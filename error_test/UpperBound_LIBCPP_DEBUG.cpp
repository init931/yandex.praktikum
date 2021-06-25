#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

// /usr/local/Cellar/gcc/11.1.0_1/bin/g++-11 -std=c++17 -v -g /Users/maierkirill/Documents/yandex_practikum/git/error_test/UpperBound.cpp -o /Users/maierkirill/Documents/yandex_practikum/git/bin/UpperBound -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_LIBCPP_DEBUG=1

int main() {
    vector v = {1, 81, 100, 50, 70, 30, 90, 200};
    auto iter = upper_bound(v.begin(), v.end(), 80);
    if (iter != v.end()) {
        cout << "Found element "s << *iter << endl;
    }
    return 0;
}
