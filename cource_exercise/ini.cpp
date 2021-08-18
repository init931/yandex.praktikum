#include "ini.h"

using namespace std;

// место для реализаций методов и функций библиотеки ini
// не забудьте, что они должны быть помещены в namespace ini

namespace ini {
    Section& Document::AddSection(string name) {
        return sections_[name];
    }

    const Section& Document::GetSection(const string& name) const {
        if (sections_.find(name) != sections_.end()) {
            return sections_.at(name);
        }
        else {
            return *(new Section());
        }
    }

    size_t Document::GetSectionCount() const {
        return sections_.size();
    }

    Document Load(istream& input) {
        Document ret;
        Section* ptr = nullptr;

        string line;
        while (!input.eof()) {
            getline(input, line);
            line.erase(0, line.find_first_not_of(' '));
            line.erase(line.find_last_not_of(' ') + 1);

            if (line.size() == 0) {
                continue;
            }
            else if (line[0] == '[') {
                line.erase(0, line.find_first_not_of('['));
                line.erase(line.find_last_not_of(']') + 1);
                ptr = &ret.AddSection(line);
            }
            else {
                vector<string> spt;
                while (true) {
                    string l;
                    string r;
                    auto cpos = line.find('=');
                    if (cpos == line.npos) {
                        l = line;
                        r = {};
                    }
                    else {
                        l = line.substr(0, cpos);
                        r = line.substr(cpos + 1);
                    }

                    l.erase(0, l.find_first_not_of(' '));
                    l.erase(l.find_last_not_of(' ') + 1);
                    spt.push_back(l);
                    if (r.size() == 0) {
                        break;
                    }
                    line = r;
                }

                ptr->emplace(spt[0], spt[1]);
            }
        }
        return ret;
    }
}
