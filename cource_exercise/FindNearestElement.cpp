#include <algorithm>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>
#include <random>
#include <set>

using namespace std;

set<int>::const_iterator FindNearestElement(const set<int>& numbers, int border) {
    // set<int>::const_iterator — тип итераторов для константного множества целых чисел
    if (numbers.size() == 0) {
        return numbers.end();
    }

    //auto lb = lower_bound(numbers.begin(), numbers.end(), border);
    auto lb = numbers.lower_bound(border);
    if (lb == numbers.end()) {
        return prev(numbers.end(), 1);
    }
    if (lb != numbers.begin()) {
        auto prev_it = prev(lb);
        if (border - *prev_it <= *lb - border) {
            return prev_it;
        }
    }
    return lb;
}

int main() {
    set<int> numbers = {1, 4, 6};
    cout << *FindNearestElement(numbers, 0) << " " << *FindNearestElement(numbers, 3) << " "
         << *FindNearestElement(numbers, 5) << " " << *FindNearestElement(numbers, 6) << " "
         << *FindNearestElement(numbers, 100) << endl;

    set<int> empty_set;

    cout << (FindNearestElement(empty_set, 8) == end(empty_set)) << endl;
    return 0;
}
