#include <iostream>
#include <string>
#include <string_view>
#include <cassert>
#include <vector>
#include <map>

using namespace std;

class Translator {
public:
    Translator() = default;

    void Add(string_view source, string_view target) {
        string str_source { source };
        string str_target { target };

        dict_source_[str_source] = str_target;
        dict_target_[str_target] = str_source;
    }

    string_view TranslateForward(string_view source) const {
        string str { source };
        if (dict_source_.count(str) == 0) {
            return ""s;
        }
        return dict_source_.at(str);
    }

    string_view TranslateBackward(string_view target) const {
        string str { target };
        if (dict_target_.count(str) == 0) {
            return ""s;
        }
        return dict_target_.at(str);
    }

private:
    map<string, string> dict_source_;
    map<string, string> dict_target_;
};

void TestSimple() {
      Translator translator;
    translator.Add(string("okno"s), string("window"s));
    translator.Add(string("stol"s), string("table"s));

    assert(translator.TranslateForward("okno"s) == "window"s);
    assert(translator.TranslateBackward("table"s) == "stol"s);
    assert(translator.TranslateForward("table"s) == ""s);
}

int main() {
    TestSimple();
    return 0;
}
