#include <iostream>
#include <string>
#include <list>

using namespace std;

class Editor {
public:
    Editor() : text_({}) {
        cursor_ = text_.begin();
    }

    // сдвинуть курсор влево
    void Left() {
        if (cursor_ != text_.begin()) {
            cursor_--;
        }
    }

    // сдвинуть курсор вправо 
    void Right() {
        if (cursor_ != text_.end()) {
            cursor_++;
        }
    }

    // вставить символ token
    void Insert(char token) {
        text_.insert(cursor_, token);
    }

    // вырезать не более tokens символов, начиная с текущей позиции курсора
    void Cut(size_t tokens = 1) {
        list<char>::iterator start = cursor_;
        list<char>::iterator end = cursor_;
        while (end != text_.end() && tokens > 0) {
            end++;
            cursor_++;
            tokens--;
        }
        buffer_.assign(start, end);
        text_.erase(start, end);
    }

    // cкопировать не более tokens символов, начиная с текущей позиции курсора
    void Copy(size_t tokens = 1) {
        list<char>::iterator end = cursor_;
        while (end != text_.end() && tokens > 0) {
            end++;
            tokens--;
        }
        buffer_.assign(cursor_, end);
    }

    // вставить содержимое буфера в текущую позицию курсора
    void Paste() {
        text_.insert(cursor_, buffer_.begin(), buffer_.end());
    }

    // получить текущее содержимое текстового редактора
    string GetText() const {
        string ret;
        ret.reserve(text_.size());
        for (const char& item : text_) {
            ret += item;
        }
        return ret;
    }
private:
    list<char> text_;
    list<char>::iterator cursor_;
    list<char> buffer_;
};

int main() {
    Editor editor;
    const string text = "hello, world"s;
    for (char c : text) {
        editor.Insert(c);
    }
    // Текущее состояние редактора: `hello, world|`
    for (size_t i = 0; i < text.size(); ++i) {
        editor.Left();
    }
    // Текущее состояние редактора: `|hello, world`
    editor.Cut(7);
    // Текущее состояние редактора: `|world`
    // в буфере обмена находится текст `hello, `
    for (size_t i = 0; i < 5; ++i) {
        editor.Right();
    }
    // Текущее состояние редактора: `world|`
    editor.Insert(',');
    editor.Insert(' ');
    // Текущее состояние редактора: `world, |`
    editor.Paste();
    // Текущее состояние редактора: `world, hello, |`
    editor.Left();
    editor.Left();
    //Текущее состояние редактора: `world, hello|, `
    editor.Cut(3);  // Будут вырезаны 2 символа
    // Текущее состояние редактора: `world, hello|`
    cout << editor.GetText() << endl;
    return 0;
}
