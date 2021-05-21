#include <algorithm>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>
#include <random>
#include <set>

using namespace std;

template <typename RandomIt>
pair<RandomIt, RandomIt> FindStartsWith(RandomIt range_begin, RandomIt range_end, string prefix) {
    // напишите реализацию
    //auto [lower, upper] = equal_range(range_begin, range_end, [](const string& str) {return str[0]==prefix;});
    
    RandomIt lower = lower_bound(range_begin, range_end, prefix, [](const string &str, string ch) {
        string sub = str.substr(0, ch.size());
        return sub < ch;
    });
    RandomIt upper = upper_bound(range_begin, range_end, prefix, [](string ch, const string &str) {
        string sub = str.substr(0, ch.size());
        return sub > ch;
    });

    //cout << *lower << " "s << (lower == range_begin) << endl;

    return {lower, upper};
}

int main() {
    const vector<string> sorted_strings = {"moscow", "motovilikha", "murmansk"};

    const auto mo_result = FindStartsWith(begin(sorted_strings), end(sorted_strings), "mo");
    for (auto it = mo_result.first; it != mo_result.second; ++it) {
        cout << *it << " ";
    }
    cout << endl;

    const auto mt_result = FindStartsWith(begin(sorted_strings), end(sorted_strings), "mt");
    cout << (mt_result.first - begin(sorted_strings)) << " " << (mt_result.second - begin(sorted_strings)) << endl;

    const auto na_result = FindStartsWith(begin(sorted_strings), end(sorted_strings), "na");
    cout << (na_result.first - begin(sorted_strings)) << " " << (na_result.second - begin(sorted_strings)) << endl;

    return 0;
}
