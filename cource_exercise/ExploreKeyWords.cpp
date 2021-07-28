#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <future>

using namespace std;

struct Stats {
    map<string, int> word_frequences;

    void operator+=(const Stats& other) {
        // сложить частоты
        for (const auto& [w, f] : other.word_frequences) {
            word_frequences[w] += f;
        }
    }
};

using KeyWords = set<string, less<>>;

Stats getStatFromLine(const KeyWords& key_words, string line) {
    string_view sw = line;
    Stats ret;
    while (true) {
        const size_t space = sw.find(' ');
        if (space != 0 && !sw.empty()) {
            string_view word = sw.substr(0, space);
            if (key_words.count(word) > 0) {
                ++ret.word_frequences[string(word)];
            }
        }
        if (space == sw.npos) {
            break;
        }
        else {
            sw.remove_prefix(space + 1);
        }
    }
    return ret;
}

Stats ExploreKeyWords(const KeyWords& key_words, istream& input) {
    // ...
    Stats ret;
    vector<future<Stats>> tasks;
    while (true) {
        string line;
        getline(input, line);
        if (line.size() == 0) {
            break;
        }
        tasks.push_back(
            async(getStatFromLine, ref(key_words), move(line))
        );
    }

    for (auto& task : tasks) {
        ret += task.get();
    }
    return ret;
}

int main() {
    const KeyWords key_words = {"yangle", "rocks", "sucks", "all"};

    stringstream ss;
    ss << "this new yangle service really rocks\n";
    ss << "It sucks when yangle isn't available\n";
    ss << "10 reasons why yangle is the best IT company\n";
    ss << "yangle rocks others suck\n";
    ss << "Goondex really sucks, but yangle rocks. Use yangle\n";

    for (const auto& [word, frequency] : ExploreKeyWords(key_words, ss).word_frequences) {
        cout << word << " " << frequency << endl;
    }

    return 0;
}
