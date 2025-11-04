/**
 * @file TextLine.hpp
 * @brief Displays an animated stroke-based text line using GLUT primitives.
 */

#pragma once
#include <GL/freeglut.h>

#include <cmath>
#include <string>
#include <utility>

class TextLine {
   public:
    // x,y are in window pixels; size_px is approximate pixel height of the text
    TextLine(std::string text, float x, float y, float size_px = 32.0f)
        : _text(std::move(text)), _x(x), _y(y), _size_px(size_px) {}

    void setPosition(float x, float y) {
        _x = x;
        _y = y;
    }
    void setText(std::string text) { _text = std::move(text); }
    void setSize(float size_px) { _size_px = size_px; }

    void render() {
        _t += 0.016f;

        glPushMatrix();

        // Render at specified position (requires orthographic projection in pixel coords)
        glTranslatef(_x, _y, 0.0f);

        // Subtle breathing and rotation
        float breath = 1.0f + 0.06f * sinf(_t * 2.0f);
        float angle = 3.0f * sinf(_t * 1.5f);

        // Convert GLUT stroke units (~119 for ROMAN) to pixels
        constexpr float kGlutStrokeRomanUnits = 119.05f;
        float s = (_size_px / kGlutStrokeRomanUnits) * breath;

        // Color
        float r = 0.85f + 0.15f * sinf(_t);
        float g = 0.65f + 0.20f * sinf(_t + 2.0f);
        float b = 0.40f + 0.15f * sinf(_t + 4.0f);
        glColor3f(r, g, b);

        glRotatef(angle, 0.0f, 0.0f, 1.0f);
        glScalef(s, s, 1.0f);

        // Center horizontally around anchor
        float width_units = getFontStringWidth(_text);
        glTranslatef(-width_units * 0.5f, 0.0f, 0.0f);

        // Make strokes thicker for visibility
        glLineWidth(4.0f);

        renderStrokeString(_text);

        glPopMatrix();
    }

   private:
    static float getFontStringWidth(const std::string& line) {
        float w = 0.0f;
        for (unsigned char c : line) w += glutStrokeWidth(GLUT_STROKE_ROMAN, c);
        return w;
    }

    static void renderStrokeString(const std::string& line) {
        for (unsigned char c : line) glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
    }

   private:
    std::string _text;
    float _x;
    float _y;
    float _size_px;
    float _t{0.f};
};
