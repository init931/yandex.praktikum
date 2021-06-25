//Утечка памяти
//нужно определить системную переменную окружения ASAN_OPTIONS=detect_leaks=1, 
//либо использовать -fsanitize=leak при компиляции.

class IntArray {
public:
    IntArray()
        : ptr(nullptr) {
    }
    IntArray(int n)
        : ptr(new int[n]) {
    }
    ~IntArray() {
        delete[] ptr;
    }

private:
    int* ptr;
};

int main() {
    IntArray a1(5); //detected memory leaks
    IntArray a2;
    a1 = a2;
    return 0;
}
