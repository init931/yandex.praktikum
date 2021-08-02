#include <algorithm>
#include <execution>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <future>

using namespace std;

template <typename ForwardRange, typename Function>
void ForEach(ForwardRange& range, Function function) {
    //for_each(execution::par, range.begin(), range.end(), function);

    const int count = 10;
    auto part_begin = range.begin();
    auto part_end = next(part_begin, range.size() / count);

    vector<future<void>> tasks;
    for (int i = 0; i < count; ++i) {
        tasks.push_back(async([part_begin, part_end, function] { 
            for_each(part_begin, part_end, function); 
        }));
        part_begin = part_end;
        if (count - 1 == i) {
            part_end = range.end(); //last
        }
        else {
            part_end = next(part_begin, range.size() / count);
        }
    }
    tasks.push_back(async([part_begin, part_end, function] { 
        for_each(part_begin, part_end, function); 
    }));
    for (auto& task : tasks) {
        task.get();
    }
}

int main() {
    // для итераторов с произвольным доступом тоже должно работать
    vector<string> strings = {"cat", "dog", "code"};

    ForEach(strings, [](string& s) { reverse(s.begin(), s.end()); });

    for (string_view s : strings) {
        cout << s << " ";
    }
    cout << endl;
    
    // вывод: tac god edoc

    return 0;
}
