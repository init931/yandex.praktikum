#include <cstdlib>
#include <future>
#include <map>
#include <numeric>
#include <random>
#include <string>
#include <vector>
#include <math.h>

#include "../search_server/log_duration.h"
#include "../search_server/runner_test_p.h"

using namespace std::string_literals;

template <typename Key, typename Value>
class ConcurrentMap {
public:
    //static_assert в начале класса не даст программе скомпилироваться при попытке использовать в качестве типа ключа что-либо, кроме целых чисел
    static_assert(std::is_integral_v<Key>, "ConcurrentMap supports only integer keys"s);

    //Структура Access должна вести себя так же, как в шаблоне Synchronized: предоставлять ссылку на значение словаря и обеспечивать синхронизацию доступа к нему.
    struct Access {
        std::lock_guard<std::mutex> lock_obj;
        Value& ref_to_value;
    };

    //Конструктор класса ConcurrentMap<Key, Value> принимает количество подсловарей, на которые надо разбить всё пространство ключей.
    explicit ConcurrentMap(size_t bucket_count) 
        : buckets_(bucket_count) {

    };

    Access operator[](const Key& key) {
        //operator[] должен вести себя так же, как аналогичный оператор у map: 
        //если ключ key есть в словаре, должен возвращаться объект класса Access, 
        //содержащий ссылку на соответствующее ему значение. Если key в словаре нет, 
        //в него надо добавить пару (key, Value()) и вернуть объект класса Access, 
        //содержащий ссылку на только что добавленное значение.

        std::lock_guard<std::mutex> lock(cmap_lock_obj_);
        uint64_t abs = std::abs(static_cast<int>(key)) % buckets_.size();
        auto& bucket = buckets_[static_cast<int>(abs)];
        
        return { 
            std::lock_guard<std::mutex>(bucket.bucket_lock_obj), 
            bucket.part[key] 
        };
    }

    std::map<Key, Value> BuildOrdinaryMap() {
        //Метод BuildOrdinaryMap должен сливать вместе части словаря и возвращать 
        //весь словарь целиком. При этом он должен быть потокобезопасным, 
        //то есть корректно работать, когда другие потоки выполняют операции с ConcurrentMap.

        std::lock_guard<std::mutex> lock(cmap_lock_obj_);
        std::map<Key, Value> ret;
        for (Bucket& item : buckets_) {
            std::lock_guard<std::mutex> lock_item(item.bucket_lock_obj);

            for (auto& [key, value] : item.part) {
                ret[key] = value;
            }
        }
        return ret;
    }

private:
    struct Bucket {
        std::map<Key, Value> part;
        std::mutex bucket_lock_obj;
    };
    std::vector<Bucket> buckets_;
    std::mutex cmap_lock_obj_;
};

using namespace std;

void RunConcurrentUpdates(
    ConcurrentMap<int, int>& cm, size_t thread_count, int key_count
) {
    auto kernel = [&cm, key_count](int seed) {
        vector<int> updates(key_count);
        iota(begin(updates), end(updates), -key_count / 2);
        shuffle(begin(updates), end(updates), mt19937(seed));

        for (int i = 0; i < 2; ++i) {
            for (auto key : updates) {
                ++cm[key].ref_to_value;
            }
        }
    };

    vector<future<void>> futures;
    for (size_t i = 0; i < thread_count; ++i) {
        futures.push_back(async(kernel, i));
    }
}

void TestConcurrentUpdate() {
    constexpr size_t THREAD_COUNT = 3;
    constexpr size_t KEY_COUNT = 50000;

    ConcurrentMap<int, int> cm(THREAD_COUNT);
    RunConcurrentUpdates(cm, THREAD_COUNT, KEY_COUNT);

    const auto result = cm.BuildOrdinaryMap();
    ASSERT_EQUAL(result.size(), KEY_COUNT);
    for (auto& [k, v] : result) {
        AssertEqual(v, 6, "Key = " + to_string(k));
    }
}

void TestReadAndWrite() {
    ConcurrentMap<size_t, string> cm(5);

    auto updater = [&cm] {
        for (size_t i = 0; i < 50000; ++i) {
            cm[i].ref_to_value.push_back('a');
        }
    };
    auto reader = [&cm] {
        vector<string> result(50000);
        for (size_t i = 0; i < result.size(); ++i) {
            result[i] = cm[i].ref_to_value;
        }
        return result;
    };

    auto u1 = async(updater);
    auto r1 = async(reader);
    auto u2 = async(updater);
    auto r2 = async(reader);

    u1.get();
    u2.get();

    for (auto f : {&r1, &r2}) {
        auto result = f->get();
        ASSERT(all_of(result.begin(), result.end(), [](const string& s) {
            return s.empty() || s == "a" || s == "aa";
        }));
    }
}

void TestSpeedup() {
    {
        ConcurrentMap<int, int> single_lock(1);

        LOG_DURATION("Single lock");
        RunConcurrentUpdates(single_lock, 4, 50000);
    }
    {
        ConcurrentMap<int, int> many_locks(100);

        LOG_DURATION("100 locks");
        RunConcurrentUpdates(many_locks, 4, 50000);
    }
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestConcurrentUpdate);
    RUN_TEST(tr, TestReadAndWrite);
    RUN_TEST(tr, TestSpeedup);
}
