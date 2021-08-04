#pragma once

#include <cstdlib>
#include <future>
#include <map>
#include <numeric>
#include <random>
#include <string>
#include <vector>
#include <math.h>

#include "log_duration.h"
#include "runner_test_p.h"

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

    void EraseKey(const Key& key)
	{
		uint64_t abs = std::abs(static_cast<int>(key)) % buckets_.size();
        auto& bucket = buckets_[static_cast<int>(abs)];
		std::lock_guard guard(bucket.bucket_lock_obj);
		bucket.part.erase(key);
	}

private:
    struct Bucket {
        std::map<Key, Value> part;
        std::mutex bucket_lock_obj;
    };
    std::vector<Bucket> buckets_;
    std::mutex cmap_lock_obj_;
};
