/**
 * @file BasicColors.hpp
 * @brief Provides RGBA helpers and predefined color constants.
 */

#pragma once

#include <GL/freeglut.h>

struct Color4f {
    float r;
    float g;
    float b;
    float a;

    Color4f() = default;

    Color4f(float red, float green, float blue, float alpha = 1.0f)
        : r(red), g(green), b(blue), a(alpha) {}

    Color4f(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255)
        : r(static_cast<float>(r) / 255.0f),
          g(static_cast<float>(g) / 255.0f),
          b(static_cast<float>(b) / 255.0f),
          a(static_cast<float>(a) / 255.0f) {}

    void render() { return glColor4f(r, g, b, a); }

    template <typename T>
    static T clamp(T value, T min, T max) {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }

    Color4f operator*(float scalar) const {
        return Color4f(clamp(r * scalar, 0.0f, 1.0f), clamp(g * scalar, 0.0f, 1.0f),
                       clamp(b * scalar, 0.0f, 1.0f), a);
    }

    Color4f operator+(const Color4f& other) const {
        return Color4f(r + other.r, g + other.g, b + other.b, a + other.a);
    }

    Color4f operator-(const Color4f& other) const {
        return Color4f(r - other.r, g - other.g, b - other.b, a - other.a);
    }
};

namespace COLORS {
const Color4f RED{1.0f, 0.0f, 0.0f, 1.0f};
const Color4f GREEN{0.0f, 1.0f, 0.0f, 1.0f};
const Color4f BLUE{0.0f, 0.0f, 1.0f, 1.0f};
const Color4f YELLOW{1.0f, 1.0f, 0.0f, 1.0f};
const Color4f CYAN{0.0f, 1.0f, 1.0f, 1.0f};
const Color4f MAGENTA{1.0f, 0.0f, 1.0f, 1.0f};
const Color4f WHITE{1.0f, 1.0f, 1.0f, 1.0f};
const Color4f BLACK{0.0f, 0.0f, 0.0f, 1.0f};
const Color4f BROWN{0.6f, 0.4f, 0.2f, 1.0f};
}  // namespace COLORS
