#pragma once

#include <cassert>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include "array_ptr.h"

using std::string_literals::operator""s;

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept
        : arr_((size_t)0), size_(0), capacity_(0) {

    };

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size)
            : arr_(size), size_(size), capacity_(size) {
        std::fill(arr_.begin(), arr_.begin()+size_, Type());
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value)
            : arr_(size), size_(size), capacity_(size) {
        std::fill(arr_.begin(), arr_.begin()+size_, value);
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init)
            : arr_(init.size()), size_(init.size()), capacity_(init.size()) {
        int count = 0;
        for (Type item : init) {
            arr_[count] = item;
            ++count;
        }
    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        return arr_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        return arr_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("out of range"s);
        }
        return arr_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("out of range"s);
        }
        return arr_[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if (new_size <= size_) {
            //При уменьшении размера вектора просто уменьшите его размер
        }
        else if (new_size < capacity_) {
            //Если при увеличении размера массива новый размер вектора 
            //не превышает его вместимость, заполните добавленные элементы значением 
            //по умолчанию для типа Type
            Iterator it_start = arr_.begin()+size_;
            Iterator it_end = arr_.begin()+new_size;
            std::fill(it_start, it_end, Type());
        }
        else { //new_size >= capacity_
            //создайте новый массив с нужной вместимостью, скопируйте в него прежнее 
            //содержимое и заполните остальные элементы значением по умолчанию. 
            //Затем старый массив можно удалить и использовать копию. После этого 
            //не забудьте обновить размер и вместимость вектора.
            size_t new_capacity = new_size;
            ArrayPtr<Type> new_arr(new_capacity);
            for (size_t i = 0; i < size_; ++i) {
                new_arr[i] = arr_[i];
            }

            Iterator it_start = new_arr.begin()+size_;
            Iterator it_end = new_arr.begin()+new_size;
            std::fill(it_start, it_end, Type());

            arr_.swap(new_arr);
            capacity_ = new_capacity;
        }
        size_ = new_size;
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return arr_.begin();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return arr_.begin()+size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return arr_.begin();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return arr_.begin()+size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return arr_.begin();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return arr_.begin()+size_;
    }

private:
    ArrayPtr<Type> arr_;
    size_t size_;
    size_t capacity_;
};
