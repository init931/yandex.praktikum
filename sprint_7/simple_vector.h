#pragma once

#include <cassert>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include "array_ptr.h"
#include <utility>

using std::string_literals::operator""s;

class ReserveProxyObj {
public:
    ReserveProxyObj(size_t size = 0) 
        : Size(size) {
    }

    size_t Size;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

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
        std::move(init.begin(), init.end(), arr_.begin());
    }

    //конструктор копирования
    SimpleVector(const SimpleVector& other)
            : arr_((size_t)0), size_(0), capacity_(0) {
        SimpleVector<Type> t(other.GetSize());
        std::copy(other.begin(), other.end(), t.begin());
        t.size_ = other.size_;
        t.capacity_ = other.capacity_;
        swap(t);
    }

    SimpleVector(ReserveProxyObj reserveObj)
            : arr_((size_t)0), size_((size_t)0), capacity_(reserveObj.Size) {
        
    }

    //конструктор перемещения
    SimpleVector(SimpleVector&& other)
            : arr_((size_t)0), size_(0), capacity_(0) {
        swap(other);
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
            std::move(begin(), end(), new_arr.begin());

            Iterator it_start = new_arr.begin() + size_;
            Iterator it_end = new_arr.begin() + new_size;
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

    SimpleVector& operator=(const SimpleVector& rhs) {
        if (*this != rhs) {
            SimpleVector<Type> t(rhs);
            swap(t);
        }
        return *this;
    }
    
    SimpleVector& operator=(SimpleVector&& other) {
        size_ = std::exchange(other.size_, 0);
		capacity_ = std::exchange(other.capacity_, 0);
        std::move(other.begin(), other.end(), arr_.begin());
        return *this;
    }

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item) {
        Resize(size_ + 1);
        *(arr_.begin() + size_ - 1) = item;
    }

    void PushBack(Type&& value) {
        Insert(end(), std::move(value));
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
	Iterator Insert(ConstIterator pos, Type &&value) {
		size_t dis_to_begin = pos - cbegin();
		if (capacity_ == 0) {
			Reserve(capacity_ + 1);
		}
		while (size_ >= capacity_) {
			capacity_ *= 2;
		}
        Iterator it_dis = begin() + dis_to_begin;
		ArrayPtr<Type> t(capacity_);
		std::move(begin(), it_dis, t.begin());
		std::move_backward(it_dis, begin() + size_, t.begin() + size_ + 1);
		t[dis_to_begin] = std::move(value);
		arr_.swap(t);
		size_++;
		return begin() + dis_to_begin;
	}

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        if (size_ > 0) {
            size_--;
        }
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        if (size_ == 0) {
            throw std::out_of_range("size_ is 0"s);
        }
        else {
            int dist = pos - begin();
            auto it = begin() + dist;
            std::move(it + 1, end(), it);
            --size_;
            return begin() + dist;
        }
    }

    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        arr_.swap(other.arr_);
        std::swap(capacity_, other.capacity_);
        std::swap(size_, other.size_);
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity == capacity_) {
            return;
        }
        else if (new_capacity < capacity_) {
            return;
        }
        else { //new_capacity > capacity_
            ArrayPtr<Type> new_arr(new_capacity);
            std::move(begin(), end(), new_arr.begin());
            arr_.swap(new_arr);
            capacity_ = new_capacity;
        }
    }

private:
    ArrayPtr<Type> arr_;
    size_t size_;
    size_t capacity_;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (&lhs == &rhs) 
        || (lhs.GetSize() == rhs.GetSize() 
            && std::equal(lhs.begin(), lhs.end(), rhs.begin())
        );
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs >= lhs;
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}
