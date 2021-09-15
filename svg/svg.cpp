#include "svg.h"

namespace svg {

using namespace std::literals;

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\" "sv;
    out << "/>"sv;
}

// ---------- Polyline ------------------

Polyline& Polyline::AddPoint(Point point) {
    points_.push_back(point);
    return *this;
}

void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<polyline points=\""sv;
    for (auto it = points_.begin(); it != points_.end(); ++it) {
        out << it->x << ","sv << it->y;
        if (it + 1 != points_.end()) {
            out << " "sv;
        }
    }
    out << "\""sv;
    out << "/>"sv;
}

// ---------- Text ------------------

Text& Text::SetPosition(Point pos) {
    pos_ = pos;
    return *this;
}

Text& Text::SetOffset(Point offset) {
    offset_ = offset;
    return *this;
}

Text& Text::SetFontSize(uint32_t size) {
    font_size_ = size;
    return *this;
}

Text& Text::SetFontFamily(std::string font_family) {
    font_family_ = font_family;
    return *this;
}

Text& Text::SetFontWeight(std::string font_weight) {
    font_weight_ = font_weight;
    return *this;
}

Text& Text::SetData(std::string data) {
    data_ = data;
    return *this;
}

void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<text"sv;
    out << " x=\""sv << pos_.x
        << "\" y=\""sv << pos_.y << "\" "sv
        << "dx=\""sv << offset_.x
        << "\" dy=\""sv << offset_.y << "\" "sv
        << "font-size=\""sv << font_size_ << "\""sv;
    if (font_family_) {
        out << " font-family=\""sv << *font_family_ << "\""sv;
    }
    if (font_weight_) {
        out << " font-weight=\""sv << *font_weight_ << "\""sv;
    }
    out << ">"sv;
    for (char c : data_) {
        switch (c) {
        case '\"':
            out << "&quot;"sv;
            break;
        case '\'':
            out << "&apos;"sv;
            break;
        case '<':
            out << "&lt;"sv;
            break;
        case '>':
            out << "&gt;"sv;
            break;
        case '&':
            out << "&amp;"sv;
            break;
        default:
            out << c;
            break;
        }
    }
    out << "</text>"sv;
}

// ---------- Document ------------------

void Document::AddPtr(std::unique_ptr<Object>&& obj) {
    objects_.emplace_back(move(obj));
}

void Document::Render(std::ostream& out) const {
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"sv;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n"sv;
    for (const auto& obj : objects_) {
        obj->Render(RenderContext(out, 2, 2));
    }
    out << "</svg>"sv;
}

}  // namespace svg
