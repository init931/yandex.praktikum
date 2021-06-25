#include <iostream>

using namespace std;

struct Point {
    int x;
    int y;
};

class PointHolder {
public:
    PointHolder(Point* pt)
        : pt_(pt) {
    }
    ~PointHolder() {
        //attempting free on address which was not malloc()-ed
        delete pt_;
    }
    void Print() const {
        cout << "("s << pt_->x << ", "s << pt_->y << ")"s << endl;
    }

private:
    Point* pt_;
};

int main() {
    {
        PointHolder holder(new Point{10, 15});
        holder.Print();
    }  // OK
    {
        Point pt{10, 15};
        PointHolder holder(&pt);
        holder.Print();
    }  // ошибка: попытка удалить адрес в стеке
}
