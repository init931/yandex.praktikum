#include <iostream>
#include <string>
#include <string_view>
#include <execution>


using namespace std;

int CountWords(string_view str) {
    // подсчитайте количество слов
    return count(execution::par, str.begin(), str.end(), ' ') + 1;
}

int main() {
    cout << CountWords("pretty little octopus"sv) << endl;
    // должно вывести 3

    return 0;
}
