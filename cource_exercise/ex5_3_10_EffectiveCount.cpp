#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <set>
#include <stack>
#include <chrono>
#include <thread>
#include <cassert>
#include <deque>
#include <string>
#include <cmath>
#include <cstdint>

#include "../search_server/log_duration.h"

using namespace std;

int EffectiveCount(const vector<int>& v, int n, int i) {
    // место для вашего решения

    int64_t border = (int64_t)log2(v.size());
    int64_t wait = (int64_t)((v.size()) * i / n);
    //cout << << wait << " "s << border << endl;

    if (wait <= border) {
        cout << "Using find_if"s << endl;
        auto find_it = find_if(v.begin(), v.end(), [&v, i] (const int& item) {
            return item > i;
        });
        return find_it - v.begin();
    } else {
        cout << "Using upper_bound"s << endl;
        auto upper_it = upper_bound(v.begin(), v.end(), i);
        return upper_it - v.begin();
    }
}

int main() {
    static const int NUMBERS = 1'000'000;
    static const int MAX = 1'000'000'000;

    mt19937 r;
    uniform_int_distribution<int> uniform_dist(0, MAX);

    vector<int> nums;
    for (int i = 0; i < NUMBERS; ++i) {
        int random_number = uniform_dist(r);
        nums.push_back(random_number);
    }
    sort(nums.begin(), nums.end());

    int i;
    cin >> i;
    int result = EffectiveCount(nums, MAX, i);
    cout << "Total numbers before "s << i << ": "s << result << endl;
}