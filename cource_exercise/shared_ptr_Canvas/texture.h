#pragma once
#include "common.h"

class Texture {
public:
    explicit Texture(Image image)
        : image_(std::move(image)) {
    }

    Size GetSize() const {
        // Заглушка. Реализуйте метод самостоятельно
        //return {0, 0};

        Size size = GetImageSize(image_);
        return size;
    }

    char GetPixelColor(Point p) const {
        (void) p;
        // Заглушка. Реализуйте метод самостоятельно
        //return ' ';

        if (image_.empty()) {
            return '.';
        }
        auto s = GetImageSize(image_);
        if (s.height <= p.y 
                || s.width <= p.x) {

            return '.';
        }
        return image_[p.y][p.x];
    }

private:
    Image image_;
};
