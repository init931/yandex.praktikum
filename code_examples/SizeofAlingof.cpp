#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <cstdint>

using namespace std;

struct Sportsman {
    int id;
    int height;
};

int main() {
    cout << "char: size="s << sizeof(char) << ", alignment="s << alignof(char) << endl;
    cout << "int: size="s << sizeof(int) << ", alignment="s << alignof(int) << endl;
    cout << "double: size="s << sizeof(double) << ", alignment="s << alignof(double) << endl;
    cout << "Sportsman: size="s << sizeof(Sportsman) << ", alignment="s << alignof(Sportsman) << endl;

    return 0;
}
