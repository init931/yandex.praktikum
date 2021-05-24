#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <random>
#include <set>

using namespace std;

template <typename It>
void PrintRange(It range_begin, It range_end) {
    for (auto it = range_begin; it != range_end; ++it) {
        cout << *it << " "s;
    }
    cout << endl;
}

template <typename Type>
class Stack {
public:
    void Push(const Type& element) {
        elements_.push_back(element);
    }
    void Pop() {
        if (elements_.size() == 0) {
            return;
        }

        elements_.pop_back();
    }
    const Type& Peek() const {
        if (elements_.size() == 0) {
            throw logic_error("Stack is empty"s);
        }

        //return elements_.back();
        return elements_.at(elements_.size() - 1);
    }
    Type& Peek() {
        if (elements_.size() == 0) {
            throw logic_error("Stack is empty"s);
        }

        //return elements_.back();
        return elements_[elements_.size() - 1];
    }
    void Print() const {
        PrintRange(elements_.begin(), elements_.end());
    }
    uint64_t Size() const {
        return elements_.size();
    }
    bool IsEmpty() const {
        return elements_.empty();
    }

private:
    vector<Type> elements_;
};

template <typename Type>
class StackMin {
public:
    void Push(const Type& element) {
        elements_.Push(element);
        elems_copy_.push_back(element);
    }
    void Pop() {
        elements_.Pop();
        elems_copy_.pop_back();
    }
    const Type& Peek() const {
        return elements_.Peek();
    }
    Type& Peek() {
        return elements_.Peek();
    }
    void Print() const {
        elements_.Print();
    }
    uint64_t Size() const {
        return elements_.size();
    }
    bool IsEmpty() const {
        return elements_.IsEmpty();
    }
    const Type& PeekMin() const {
        return *min_element(elems_copy_.begin(), elems_copy_.end());
    }
    Type& PeekMin() {
        //return min(elems_copy_, [](const Type& el1, const Type& el2) {
        //    return el1 < el2;
        //});
        return *min_element(elems_copy_.begin(), elems_copy_.end());
    }
private:
    Stack<Type> elements_;
    vector<Type> elems_copy_; // =(
};

template <typename Type>
class SortedStack {
public:
    void Push(const Type& element) {
        if (IsEmpty()) {
            elements_.push_back(element);
        }
        else {
            //сортировка от меньшего к большему
            //auto lower = lower_bound(elements_.begin(), elements_.end(), element);

            auto lower = lower_bound(elements_.rbegin(), elements_.rend(), element).base();
            elements_.insert(lower, element);
        }
    }
    void Pop() {
        elements_.pop_back();
    }
    const Type& Peek() const {
        if (elements_.size() == 0) {
            throw logic_error("Stack is empty"s);
        }

        //return elements_.back();
        return elements_.at(elements_.size() - 1);
    }
    Type& Peek() {
        if (elements_.size() == 0) {
            throw logic_error("Stack is empty"s);
        }

        //return elements_.back();
        return elements_[elements_.size() - 1];
    }
    void Print() const {
        PrintRange(elements_.begin(), elements_.end());
    }
    uint64_t Size() const {
        return elements_.size();
    }
    bool IsEmpty() const {
        return elements_.empty();
    }
private:
    vector<Type> elements_;
};

int main() {
    SortedStack<int> stack;
    vector<int> values(5);

    // заполняем вектор для тестирования нашего стека
    iota(values.begin(), values.end(), 1);
    // перемешиваем значения
    std::random_device rd;
    std::mt19937 g(rd());
    shuffle(values.begin(), values.end(), g);

    // заполняем стек и проверяем, что сортировка сохраняется после каждой вставки
    for (int i = 0; i < 5; ++i) {
        cout << "Вставляемый элемент = "s << values[i] << endl;
        stack.Push(values[i]);
        stack.Print();
    }
}
