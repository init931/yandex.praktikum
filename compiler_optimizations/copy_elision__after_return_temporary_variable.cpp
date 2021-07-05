#include <iostream>
#include <string>

using namespace std;

class Number {
private:
    int a_;

public:
    Number() {
        cout << "Default Ctor"s << endl;
    }
    Number(int num)
        : a_(num) 
    {
        cout << "Ctor(int)"s << endl;
    }
    Number(const Number& other)
        : a_(other.a_) 
    {
        cout << "Copy Ctor"s << endl;
    }
    ~Number() {
        cout << "Dtor"s << endl;
    }
    Number& operator=(const Number& other) {
        a_ = other.a_;
        cout << "Op="s << endl;
        return *this;
    }
    Number operator+(const Number& second) {
        cout << "Op+" << endl;
        return a_ + second.a_; //временный объект Ctor(int) не копируется еще раз через Copy Ctor. Тут компилятор оптимизует. Это и называется copy elision
    }

    int GetA() {
        return a_;
    }
};

int main() {
    Number a(1), b(1), c;
    cout << &c << endl;
    c = a + b; //временный объект возвращается из функции и присваевается уже существующей переменной. Конструктор копирования пропускается, но вызывается оператор присваивания
    cout << &c << endl;
    cout << "End of calculations"s << endl;
}
