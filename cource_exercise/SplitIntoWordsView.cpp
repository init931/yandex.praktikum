#include <vector>
#include <string>
#include <string_view>
#include <iostream>
#include "../search_server/log_duration.h"

using namespace std;

vector<string_view> SplitIntoWordsView(string_view str) {
    vector<string_view> result;
    int64_t pos = 0;
    const int64_t pos_end = str.npos;
    while (true) {
        int64_t space = str.find(' ', pos);
        result.push_back(space == pos_end ? str.substr(pos) : str.substr(0, space - pos));
        if (space == pos_end) {
            break;
        } else {
            str.remove_prefix(space + 1);
        }
    }

    return result;
}

// создадим очень длинную строку,
// состоящую из слов из ста 'a'
string GenerateText() {
    const int SIZE = 10000000;
    string text(SIZE, 'a');
    for (int i = 100; i < SIZE; i += 100) {
        text[i] = ' ';
    }
    return text;
}

int main() {
    const string text = GenerateText();
    {
        //LOG_DURATION("string");
        //const auto words = SplitIntoWords(text);
        // выведем первое слово
        //cout << words[0] << "\n";
    }
    {
        //LOG_DURATION("string_view");
        const auto words = SplitIntoWordsView(text);
        // выведем первое слово
        cout << words[0] << "\n";
    }

    return 0;
}
