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
        is_empty_ = true;
        cout << "GetCat box"s << endl;
        return cat_;
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


/*
(pytorch4) test$ ./a.out
Default Ctor cat
Default Ctor box
HasCat box
GetCat box
Copy Ctor cat
end if
Dtor cat
end main
Dtor box
Dtor cat
(pytorch4) test$ g++  -fno-elide-constructors test_cat.cpp
(pytorch4) villedepommes@devfair044:/scratch/villedepommes/github/pytorch4/test$ ./a.out
Default Ctor cat
Copy Ctor cat
Dtor cat
Default Ctor box
HasCat box
GetCat box
Copy Ctor cat
Copy Ctor cat
Dtor cat
end if
Dtor cat
end main
Dtor box
Dtor cat
*/