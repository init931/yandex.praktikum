#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <cassert>
#include <sstream>
#include <algorithm>

using namespace std;

struct my_class {
    my_class(int num) {
        my_number = num;
    }
    int my_number;

    my_class& operator+=(const my_class& val) {
        my_number += val.my_number;
        return *this;
    }
};

my_class operator+(const my_class& left, const my_class& right) {
    my_class ret{left.my_number + right.my_number};
    return ret;
}

int main() {
    my_class val1(5);
    my_class val2(6);
    my_class sum = val1 + val2;
    cout << sum.my_number << endl;

    val1 += sum;
    cout << val1.my_number << endl;

    return 0;
}