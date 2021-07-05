#include <iostream>
#include <string>

using namespace std;

// класс кота Шрёдингера
class SchrodingerCat {
private:
    string color_;

public:
    SchrodingerCat() : color_("black"s) {
        cout << "Default Ctor cat"s << endl;
    }

    SchrodingerCat(const SchrodingerCat& other)
        : color_(other.color_)
    {
        cout << "Copy Ctor cat"s << endl;
    }

    ~SchrodingerCat() {
        cout << "Dtor cat"s << endl;
    }
};

// класс Коробки
class Box {
private:
    bool is_empty_;
    SchrodingerCat cat_;

public:
    Box() : cat_({}), is_empty_(false) {
        cout << "Default Ctor box"s << endl;
    }

    ~Box() {
        cout << "Dtor box"s << endl;
    }

    // кота из коробки можно вытащить
    SchrodingerCat GetCat() {
        cout << "GetCat box"s << endl;
        SchrodingerCat cat; //уже не временный объект т к компилятор оптимизует копирование при возврате из функции
        is_empty_ = true;
        return cat; //NRVO, или Named Return Value Optimization — второй вид оптимизации компилятора, помогающий избегать лишних копирований. Как видно из названия, речь идёт о возвращении из функции некой именованной переменной
    }
    
    bool HasCat() {
        cout << "HasCat box"s << endl;
        return !is_empty_;
    }
};

int main() {
    Box black_box = {};
    if (black_box.HasCat()) {
        SchrodingerCat fluffy = black_box.GetCat();
        cout << "end if"s << endl;
    }
    cout << "end main"s << endl;
    return 0;
}
