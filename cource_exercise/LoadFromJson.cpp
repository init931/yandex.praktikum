// не меняйте файлы json.h и json.cpp
//#pragma once

// #include <istream>
// #include <map>
// #include <string>
// #include <unordered_map>
// #include <vector>

// class Node {
// public:
//     explicit Node(std::vector<Node> array);
//     explicit Node(std::map<std::string, Node> map);
//     explicit Node(int value);
//     explicit Node(std::string value);

//     const std::vector<Node>& AsArray() const;
//     const std::map<std::string, Node>& AsMap() const;
//     int AsInt() const;
//     const std::string& AsString() const;

// private:
//     std::vector<Node> as_array_;
//     std::map<std::string, Node> as_map_;
//     int as_int_;
//     std::string as_string_;
// };

// class Document {
// public:
//     explicit Document(Node root);

//     const Node& GetRoot() const;

// private:
//     Node root_;
// };

// Document Load(std::istream& input);

// //
// using namespace std;

// Node::Node(vector<Node> array)
//     : as_array_(move(array)) {
// }

// Node::Node(map<string, Node> map)
//     : as_map_(move(map)) {
// }

// Node::Node(int value)
//     : as_int_(value) {
// }

// Node::Node(string value)
//     : as_string_(move(value)) {
// }

// const vector<Node>& Node::AsArray() const {
//     return as_array_;
// }

// const map<string, Node>& Node::AsMap() const {
//     return as_map_;
// }

// int Node::AsInt() const {
//     return as_int_;
// }

// const string& Node::AsString() const {
//     return as_string_;
// }

// Document::Document(Node root)
//     : root_(move(root)) {
// }

// const Node& Document::GetRoot() const {
//     return root_;
// }

// Node LoadNode(istream& input);

// Node LoadArray(istream& input) {
//     vector<Node> result;

//     for (char c; input >> c && c != ']';) {
//         if (c != ',') {
//             input.putback(c);
//         }
//         result.push_back(LoadNode(input));
//     }

//     return Node(move(result));
// }

// Node LoadInt(istream& input) {
//     int result = 0;
//     while (isdigit(input.peek())) {
//         result *= 10;
//         result += input.get() - '0';
//     }
//     return Node(result);
// }

// Node LoadString(istream& input) {
//     string line;
//     getline(input, line, '"');
//     return Node(move(line));
// }

// Node LoadDict(istream& input) {
//     map<string, Node> result;

//     for (char c; input >> c && c != '}';) {
//         if (c == ',') {
//             input >> c;
//         }

//         string key = LoadString(input).AsString();
//         input >> c;
//         result.insert({move(key), LoadNode(input)});
//     }

//     return Node(move(result));
// }

// Node LoadNode(istream& input) {
//     char c;
//     input >> c;

//     if (c == '[') {
//         return LoadArray(input);
//     } else if (c == '{') {
//         return LoadDict(input);
//     } else if (c == '"') {
//         return LoadString(input);
//     } else {
//         input.putback(c);
//         return LoadInt(input);
//     }
// }

// Document Load(istream& input) {
//     return Document{LoadNode(input)};
// }
//

//
//#pragma once

#include <istream>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

class Node {
public:
    Node(std::string name, std::unordered_map<std::string, std::string> attrs);

    const std::vector<Node>& Children() const;
    void AddChild(Node node);
    std::string_view Name() const;

    template <typename T>
    T AttributeValue(const std::string& name) const;

private:
    std::string name_;
    std::vector<Node> children_;
    std::unordered_map<std::string, std::string> attrs_;
};

class Document {
public:
    explicit Document(Node root);

    const Node& GetRoot() const;

private:
    Node root_;
};

Document Load(std::istream& input);

template <typename T>
inline T Node::AttributeValue(const std::string& name) const {
    std::istringstream attr_input(attrs_.at(name));
    T result;
    attr_input >> result;
    return result;
}
//
//#include "xml.h"

using namespace std;

pair<string_view, string_view> Split(string_view line, char by) {
    size_t pos = line.find(by);
    string_view left = line.substr(0, pos);

    if (pos < line.size() && pos + 1 < line.size()) {
        return {left, line.substr(pos + 1)};
    } else {
        return {left, string_view()};
    }
}

string_view Lstrip(string_view line) {
    while (!line.empty() && isspace(line[0])) {
        line.remove_prefix(1);
    }
    return line;
}

string_view Unquote(string_view value) {
    if (!value.empty() && value.front() == '"') {
        value.remove_prefix(1);
    }
    if (!value.empty() && value.back() == '"') {
        value.remove_suffix(1);
    }
    return value;
}

Node LoadNode(istream& input) {
    string root_name;
    getline(input, root_name);

    Node root(root_name.substr(1, root_name.size() - 2), {});
    for (string line; getline(input, line) && line[1] != '/';) {
        auto [node_name, attrs] = Split(Lstrip(line), ' ');
        attrs = Split(attrs, '>').first;
        unordered_map<string, string> node_attrs;
        while (!attrs.empty()) {
            const auto [head, tail] = Split(attrs, ' ');
            const auto [name, value] = Split(head, '=');
            if (!name.empty() && !value.empty()) {
                node_attrs[string(Unquote(name))] = string(Unquote(value));
            }
            attrs = tail;
        }

        root.AddChild(Node(string(node_name.substr(1)), move(node_attrs)));
    }
    return root;
}

Document Load(istream& input) {
    return Document{LoadNode(input)};
}

Node::Node(string name, unordered_map<string, string> attrs)
    : name_(move(name))
    , attrs_(move(attrs)) {
}

const vector<Node>& Node::Children() const {
    return children_;
}

Document::Document(Node root)
    : root_(move(root)) {
}

const Node& Document::GetRoot() const {
    return root_;
}

void Node::AddChild(Node node) {
    children_.push_back(move(node));
}

string_view Node::Name() const {
    return name_;
}
//

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string_view>
#include <vector>

using namespace std;

struct Spending {
    string category;
    int amount;
};

int CalculateTotalSpendings(const vector<Spending>& spendings) {
    int result = 0;
    for (const Spending& s : spendings) {
        result += s.amount;
    }
    return result;
}

string FindMostExpensiveCategory(const vector<Spending>& spendings) {
    auto compare_by_amount = [](const Spending& lhs, const Spending& rhs) {
        return lhs.amount < rhs.amount;
    };
    return max_element(begin(spendings), end(spendings), compare_by_amount)->category;
}

// vector<Spending> LoadFromJson(istream& input) {
//     // место для вашей реализации
//     // пример корректного JSON-документа в условии

//     const Document doc = Load(input);
//     const Node& rootNode = doc.GetRoot();
//     const vector<Node>& items = rootNode.AsArray();

//     vector<Spending> ret;
//     for (const auto& item : items) {
//         ret.push_back({ 
//             item.AsMap().at("category").AsString(), 
//             item.AsMap().at("amount").AsInt() 
//         });
//     }
//     return ret;
// }

vector<Spending> LoadFromXml(istream& input) {
    // место для вашей реализации
    // пример корректного XML-документа в условии

    const Document doc = Load(input);
    const Node& rootNode = doc.GetRoot();
    const vector<Node>& items = rootNode.Children();

    vector<Spending> ret;
    for (const auto& item : items) {
        ret.push_back({ 
            item.AttributeValue<string>("category"), 
            item.AttributeValue<int>("amount")
        });
    }
    return ret;
}

int main() {
    // не меняйте main
    //const vector<Spending> spendings = LoadFromJson(cin);
    const vector<Spending> spendings = LoadFromXml(cin);
    cout << "Total "sv << CalculateTotalSpendings(spendings) << '\n';
    cout << "Most expensive is "sv << FindMostExpensiveCategory(spendings) << '\n';
}
