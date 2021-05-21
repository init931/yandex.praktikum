#include <algorithm>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>
#include <random>

using namespace std;

void PrintSpacesPositions(string& str) {
    auto it = std::find(str.begin(), str.end(), ' ');
    while (it != str.end()) {
        cout << distance(str.begin(), it) << endl;
        it = std::find(it + 1, str.end(), ' ');
    }
}

int main() {
    string str = " He said: one and one and one is three"s;
    PrintSpacesPositions(str);
    return 0;
}
