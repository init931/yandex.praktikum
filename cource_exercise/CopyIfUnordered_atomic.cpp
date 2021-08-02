#include <iostream>
#include <numeric>
#include <vector>
#include <algorithm>
#include <execution>
#include <atomic>

using namespace std;

template <typename Container, typename Predicate>
vector<typename Container::value_type> CopyIfUnordered(const Container& container, Predicate predicate) {
    vector<typename Container::value_type> result;
    result.resize(container.size());
    atomic_int insert_position = 0;
    for_each(
        execution::par,
        container.begin(), container.end(),
        [&insert_position, &result, predicate](const auto& item) {
            if (predicate(item)) {
                result[insert_position++] = item;
            }
        }
    );
    result.resize(insert_position);
    return result;
}

int main() {
    vector<int> numbers(1'000);
    iota(numbers.begin(), numbers.end(), 0);
    
    const vector<int> even_numbers =
        CopyIfUnordered(numbers, [](int number) { return number % 2 == 0; });
    for (const int number : even_numbers) {
        cout << number << " "s;
    }
    cout << endl;
		// выведет все чётные числа от 0 до 999
}
