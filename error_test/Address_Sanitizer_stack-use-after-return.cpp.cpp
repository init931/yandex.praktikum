#include <iostream>
#include <tuple>

using namespace std;

auto GetTuple(int x, int y) {
    return tie(x, y);
}

//для выявления ошибки нужно определить
//export ASAN_OPTIONS=detect_stack_use_after_return=1

int main() {
    auto [x, y] = GetTuple(10, 15);
    //stack-use-after-return 
    //or stack-use-after-scope
    cout << x << ' ' << y << endl;
    return 0;
}
