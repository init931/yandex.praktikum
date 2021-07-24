#include <algorithm>
#include <execution>
#include <iostream>
#include <future>

using namespace std;

template <typename RandomAccessIterator, typename Value>
RandomAccessIterator LowerBound(const execution::parallel_policy&, RandomAccessIterator range_begin, RandomAccessIterator range_end, const Value& value) {
    RandomAccessIterator left_bound = range_begin;
    RandomAccessIterator right_bound = range_end;
    while (left_bound + 1 < right_bound) {
        int dis = (right_bound - left_bound) / 3;
        if (dis < 1) {
            dis = 1;
        }
        const auto middle_left = left_bound + dis;
        const auto middle_right = right_bound - dis;

        future<bool> mid_left_feature = async([middle_left, &value] { 
            return *middle_left < value;
        });
        // future<bool> mid_right_feature = async([value, middle_right] { 
        //     return *middle_right < value;
        // });

        //const auto mid_left = mid_left_feature.get();
        //const auto mid_right = mid_right_feature.get();

        if (*middle_right < value) {
            left_bound = middle_right;
        }
        else if (mid_left_feature.get()) {
            left_bound = middle_left;
            right_bound = middle_right;
        }
        else {
            right_bound = middle_left;
        }

        // if (*middle < value) {
        //     if (mid_right) {
        //         left_bound = middle_right;
        //         right_bound = range_end;
        //     }
        //     else {
        //         left_bound = middle;
        //         right_bound = middle_right;
        //     }
        // } else {
        //     if (mid_left) {
        //         left_bound = middle_left;
        //         right_bound = middle;
        //     }
        //     else {
        //         left_bound = range_begin;
        //         right_bound = middle_left;
        //     }
        // }
    }
    if (left_bound == range_begin && !(*left_bound < value)) {
        return left_bound;
    } else {
        return right_bound;
    }
}

template <typename RandomAccessIterator, typename Value>
RandomAccessIterator LowerBound(const execution::sequenced_policy&, RandomAccessIterator range_begin, RandomAccessIterator range_end, const Value& value) {
    return lower_bound(range_begin, range_end, value);
}

template <typename RandomAccessIterator, typename Value>
RandomAccessIterator LowerBound(RandomAccessIterator range_begin, RandomAccessIterator range_end, const Value& value) {
    return LowerBound(execution::seq, range_begin, range_end, value);
}

int main() {
    const vector<string> strings = {
        "cat",
        "dog",
        "dog",
        "horse"
    };

    const vector<string> requests = {
        "bear",
        "cat",
        "deer",
        "dog",
        "dogs",
        "horses"
    };

    // последовательные версии
    cout << "Request [" << requests[0] << "] → position "
         << LowerBound(strings.begin(), strings.end(), requests[0]) - strings.begin() << endl;
    cout << "Request [" << requests[1] << "] → position "
         << LowerBound(execution::seq, strings.begin(), strings.end(), requests[1]) - strings.begin() << endl;
    cout << "Request [" << requests[2] << "] → position "
         << LowerBound(execution::seq, strings.begin(), strings.end(), requests[2]) - strings.begin() << endl;
    
    // параллельные
    cout << "Request [" << requests[3] << "] → position "
         << LowerBound(execution::par, strings.begin(), strings.end(), requests[3]) - strings.begin() << endl;
    cout << "Request [" << requests[4] << "] → position "
         << LowerBound(execution::par, strings.begin(), strings.end(), requests[4]) - strings.begin() << endl;
    cout << "Request [" << requests[5] << "] → position "
         << LowerBound(execution::par, strings.begin(), strings.end(), requests[5]) - strings.begin() << endl;
}
