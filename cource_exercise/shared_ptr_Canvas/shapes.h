#pragma once
#include "texture.h"

#include <memory>
#include <stdexcept>

// Поддерживаемые виды фигур: прямоугольник и эллипс
enum class ShapeType { RECTANGLE, ELLIPSE};

class Shape {
public:
    // Фигура после создания имеет нулевые координаты и размер,
    // а также не имеет текстуры
    explicit Shape(ShapeType type)
        : type_(type)  {

        // Заглушка. Реализуйте конструктор самостоятельно
        //(void) type;
    }

    void SetPosition(Point pos) {
        //(void) pos;
        // Заглушка. Реализуйте метод самостоятельно

        position_ = pos;
    }

    void SetSize(Size size) {
        //(void) size;
        // Заглушка. Реализуйте метод самостоятельно

        size_ = size;
    }

    void SetTexture(std::shared_ptr<Texture> texture) {
        //(void) texture;
        // Заглушка. Реализуйте метод самостоятельно

        texture_ = std::move(texture);
    }

    // Рисует фигуру на указанном изображении
	// В зависимости от типа фигуры должен рисоваться либо эллипс, либо прямоугольник
    // Пиксели фигуры, выходящие за пределы текстуры, а также в случае, когда текстура не задана,
    // должны отображаться с помощью символа точка '.'
    // Части фигуры, выходящие за границы объекта image, должны отбрасываться.
    void Draw(Image& image) const {
        //(void) image;
        // Заглушка. Реализуйте метод самостоятельно

        switch (type_) {
            case ShapeType::RECTANGLE:
                for (int x = position_.x, w = 0; w < size_.width; ++x, ++w) {
                    for (int y = position_.y, h = 0; h < size_.height; ++y, ++h) {
                        if (texture_ == nullptr) {
                            image[y][x] = '.';
                        }
                        else if ((x < GetImageSize(image).width && y < GetImageSize(image).height)) {
                            image[y][x] = texture_->GetPixelColor({ w, h });
                        }
                    }
                }
                break;
            case ShapeType::ELLIPSE:
                for (int x = position_.x, w = 0; w < size_.width; ++x, ++w) {
                    for (int y = position_.y, h = 0; h < size_.height; ++y, ++h) {
                        if (texture_ == nullptr) {
                            image[y][x] = '.';
                        }
                        else if ((x < GetImageSize(image).width && y < GetImageSize(image).height) && IsPointInEllipse({ w, h }, size_)) {
                            image[y][x] = texture_->GetPixelColor({ w, h });
                        }
                    }
                }
                break;
            default:
                throw std::logic_error("Error");
                break;
        }
    }

private:
    ShapeType type_;
    Point position_;
    Size size_;
    std::shared_ptr<Texture> texture_;
};
