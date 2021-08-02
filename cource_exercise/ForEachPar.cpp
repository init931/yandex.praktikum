#include <algorithm>
#include <execution>
#include <future>
#include <iostream>
#include <iterator>
#include <list>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

using namespace std;

template <typename ExecutionPolicy, typename ForwardRange, typename Function>
void ForEach(const ExecutionPolicy& policy, ForwardRange& range, Function function) {
    if constexpr (
            is_same_v<ExecutionPolicy, execution::sequenced_policy>
            || is_same_v<
                    typename iterator_traits<typename ForwardRange::iterator>::iterator_category,
                    random_access_iterator_tag
            >
            ) {

        for_each(policy, range.begin(), range.end(), function);

    } else {

        static constexpr int PART_COUNT = 4;
        const auto part_length = size(range) / PART_COUNT;
        auto part_begin = range.begin();
        auto part_end = next(part_begin, part_length);

        vector<future<void>> futures;
        for (
                int i = 0;
                i < PART_COUNT;
                ++i,
                        part_begin = part_end,
                        part_end = (i == PART_COUNT - 1
                                    ? range.end()
                                    : next(part_begin, part_length))
                ) {
            futures.push_back(async([function, part_begin, part_end] { for_each(part_begin, part_end, function); }));
        }

    }
}

template <typename ForwardRange, typename Function>
void ForEach(ForwardRange& range, Function function) {
    ForEach(execution::seq, range, function);
}


template <typename Strings>
void PrintStrings(const Strings& strings) {
    for (string_view s : strings) {
        cout << s << " ";
    }
    cout << endl;
}

int main() {
    auto reverser = [](string& s) { reverse(s.begin(), s.end()); };

    list<string> strings_list = {"cat", "dog", "code"};

    ForEach(strings_list, reverser);
    PrintStrings(strings_list);
    // tac god edoc

    ForEach(execution::seq, strings_list, reverser);
    PrintStrings(strings_list);
    // cat dog code

    // единственный из вызовов, где должна работать ваша версия
    // из предыдущего задания
    ForEach(execution::par, strings_list, reverser);
    PrintStrings(strings_list);
    // tac god edoc

    vector<string> strings_vector = {"cat", "dog", "code"};

    ForEach(strings_vector, reverser);
    PrintStrings(strings_vector);
    // tac god edoc

    ForEach(execution::seq, strings_vector, reverser);
    PrintStrings(strings_vector);
    // cat dog code

    ForEach(execution::par, strings_vector, reverser);
    PrintStrings(strings_vector);
    // tac god edoc

    return 0;
}
