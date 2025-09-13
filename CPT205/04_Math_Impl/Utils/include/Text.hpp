#pragma once
#include <GL/glut.h>

#include <cassert>
#include <cmath>
#include <string>
#include <utility>

#include "basic/Color.hpp"
#include "basic/Pos.hpp"
#include "utils/clamp.hpp"

using size2D = std::pair<float, float>;

// Easing helpers
inline float lerp(float a, float b, float t) { return a + (b - a) * t; }
inline float smoothstep01(float t) {
    t = std::fmax(0.f, std::fmin(1.f, t));
    return t * t * (3.f - 2.f * t);
}

// Default stroke-font nominal height for GLUT (approx). GLUT stroke units are ~100..120 tall.
constexpr float GLUT_STROKE_NOMINAL_HEIGHT = 100.f;

// Decide if the pointer is a GLUT stroke font (we rely on a flag; caller should set use_stroke=true
// when passing stroke fonts)
inline bool isBitmapFont(void* font_ptr) {
    return (font_ptr == GLUT_BITMAP_8_BY_13) || (font_ptr == GLUT_BITMAP_9_BY_15) ||
           (font_ptr == GLUT_BITMAP_HELVETICA_10) || (font_ptr == GLUT_BITMAP_HELVETICA_12) ||
           (font_ptr == GLUT_BITMAP_HELVETICA_18) || (font_ptr == GLUT_BITMAP_TIMES_ROMAN_10) ||
           (font_ptr == GLUT_BITMAP_TIMES_ROMAN_24);
}

// Pick a best-fit GLUT bitmap font by desired pixel height (coarse approximation)
inline void* pickBitmapFontByPx(float px) {
    // Order from small to large
    if (px <= 11) return GLUT_BITMAP_HELVETICA_10;
    if (px <= 13) return GLUT_BITMAP_8_BY_13;
    if (px <= 15) return GLUT_BITMAP_9_BY_15;
    if (px <= 18) return GLUT_BITMAP_HELVETICA_12;
    if (px <= 20) return GLUT_BITMAP_TIMES_ROMAN_10;
    if (px <= 24) return GLUT_BITMAP_HELVETICA_18;
    return GLUT_BITMAP_TIMES_ROMAN_24;  // largest
}

class Text2D {
   public:
    // When use_stroke=true, pass GLUT_STROKE_ROMAN or GLUT_STROKE_MONO_ROMAN in font
    Text2D(Pos2D position, void* font, const std::string& content, size2D size, RGBA rgba,
           bool use_stroke = true)
        : position_(position),
          content_(content),
          font_(font),
          size_(size),
          rgba_(rgba),
          use_stroke_(use_stroke) {
        // Initialize scale (for stroke) based on requested "size"
        // Interpret size as {scale_x, scale_y} for stroke mode; for bitmap it's pixel size hint
        // (height)
        if (use_stroke_) {
            current_scale_x_ = std::max(0.01f, size_.first);
            current_scale_y_ = std::max(0.01f, size_.second);
        } else {
            bitmap_px_height_ = std::max(8.f, (size_.first + size_.second) * 0.5f);
            font_ = pickBitmapFontByPx(bitmap_px_height_);
        }
    }

    void render() const {
        glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glColor4f(color_r_, color_g_, color_b_, color_a_);

        if (use_stroke_) {
            glPushMatrix();
            glTranslatef(position_.x, position_.y, 0.f);

            // Stroke fonts are outline paths in an abstract unit; scale maps to screen space
            glScalef(current_scale_x_, current_scale_y_, 1.f);
            glLineWidth(stroke_line_width_);

            const unsigned char* s = reinterpret_cast<const unsigned char*>(content_.c_str());
            for (; *s != '\0'; ++s) {
                glutStrokeCharacter(reinterpret_cast<void*>(font_), *s);
            }
            glPopMatrix();
        } else {
            // Bitmap path (no true scaling; approximate by swapping fonts)
            glRasterPos2f(position_.x, position_.y);
            void* chosen = pickBitmapFontByPx(bitmap_px_height_);
            const unsigned char* s = reinterpret_cast<const unsigned char*>(content_.c_str());
            for (; *s != '\0'; ++s) {
                glutBitmapCharacter(chosen, *s);
            }
        }

        glPopAttrib();
    }

    void update(float dt_seconds) {
        if (!animating_) return;
        elapsed_ += std::max(0.f, dt_seconds);
        float t = (anim_duration_ <= 0.f) ? 1.f : std::min(1.f, elapsed_ / anim_duration_);
        float e = smoothstep01(t);
        if (use_stroke_) {
            current_scale_x_ = lerp(start_scale_x_, target_scale_x_, e);
            current_scale_y_ = lerp(start_scale_y_, target_scale_y_, e);
        } else {
            bitmap_px_height_ = lerp(start_bitmap_px_, target_bitmap_px_, e);
        }
        if (t >= 1.f) animating_ = false;
    }

    // Instant position set
    bool setPosition(Pos2D newPos) {
        position_ = newPos;
        return true;
    }

    // For stroke mode: interpret size as scales; for bitmap: treat as pixel-height hint
    bool setSize(size2D newSize, bool bound = true) {
        if (!bound) {
            assert(newSize.first > 0 && newSize.second > 0 && "size must be positive");
            size_ = newSize;
        } else {
            size_ = clampPair(newSize, size2D(0.01f, 0.01f), size2D(1000.f, 1000.f));
        }

        if (use_stroke_) {
            current_scale_x_ = size_.first;
            current_scale_y_ = size_.second;
        } else {
            bitmap_px_height_ = std::max(8.f, (size_.first + size_.second) * 0.5f);
            font_ = pickBitmapFontByPx(bitmap_px_height_);
        }
        return true;
    }

    // Smoothly animate to a new size over duration (seconds)
    void animateToSize(size2D newSize, float duration_sec, bool bound = true) {
        if (bound) newSize = clampPair(newSize, size2D(0.01f, 0.01f), size2D(1000.f, 1000.f));
        if (use_stroke_) {
            start_scale_x_ = current_scale_x_;
            start_scale_y_ = current_scale_y_;
            target_scale_x_ = newSize.first;
            target_scale_y_ = newSize.second;
        } else {
            start_bitmap_px_ = bitmap_px_height_;
            target_bitmap_px_ = std::max(8.f, (newSize.first + newSize.second) * 0.5f);
        }
        anim_duration_ = std::max(0.f, duration_sec);
        elapsed_ = 0.f;
        animating_ = true;
    }

    // Switch between stroke and bitmap
    void setUseStroke(bool s, void* newFont = nullptr) {
        use_stroke_ = s;
        if (newFont) font_ = newFont;
    }

    // Color and stroke thickness (stroke fonts only)
    void setColor(float r, float g, float b, float a = 1.f) {
        color_r_ = r;
        color_g_ = g;
        color_b_ = b;
        color_a_ = a;
    }
    void setStrokeLineWidth(float w) { stroke_line_width_ = std::max(1.f, w); }

    // Content setters
    void setText(const std::string& s) { content_ = s; }
    const std::string& text() const { return content_; }

    // Optional: text width estimation in stroke units (useful for alignment)
    float estimateStrokeTextWidth() const {
        if (!use_stroke_) return 0.f;
        float w = 0.f;
        for (const unsigned char* s = (const unsigned char*)content_.c_str(); *s; ++s) {
            w += glutStrokeWidth(reinterpret_cast<void*>(font_), *s);
        }
        return w * current_scale_x_;
    }

    // Optional: height estimation in pixels for bitmap mode (approx)
    float bitmapPixelHeight() const { return bitmap_px_height_; }

    // Optional: center alignment helpers
    void renderCentered(float cx, float cy) const {
        if (use_stroke_) {
            float w_units = 0.f;
            for (const unsigned char* s = (const unsigned char*)content_.c_str(); *s; ++s) {
                w_units += glutStrokeWidth(reinterpret_cast<void*>(font_), *s);
            }
            float w = w_units * current_scale_x_;
            float h = GLUT_STROKE_NOMINAL_HEIGHT * current_scale_y_;

            glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT);
            glDisable(GL_LIGHTING);
            glDisable(GL_TEXTURE_2D);
            glColor4f(color_r_, color_g_, color_b_, color_a_);

            glPushMatrix();
            glTranslatef(cx - w * 0.5f, cy - h * 0.5f, 0.f);
            glScalef(current_scale_x_, current_scale_y_, 1.f);
            glLineWidth(stroke_line_width_);
            const unsigned char* s = reinterpret_cast<const unsigned char*>(content_.c_str());
            for (; *s != '\0'; ++s) glutStrokeCharacter(reinterpret_cast<void*>(font_), *s);
            glPopMatrix();
            glPopAttrib();
        } else {
            // For bitmap, center roughly by shifting raster pos
            // Note: GLUT bitmap has no width query across all fonts; we do a crude pass
            int w = 0;
            for (const unsigned char* s = (const unsigned char*)content_.c_str(); *s; ++s) {
                w += glutBitmapWidth(font_, *s);
            }
            float h = bitmap_px_height_;
            glPushAttrib(GL_CURRENT_BIT);
            glColor4f(color_r_, color_g_, color_b_, color_a_);
            glRasterPos2f(cx - w * 0.5f, cy + h * 0.35f);  // tweak baseline
            const unsigned char* s = reinterpret_cast<const unsigned char*>(content_.c_str());
            for (; *s != '\0'; ++s) glutBitmapCharacter(font_, *s);
            glPopAttrib();
        }
    }

   private:
    Pos2D position_;
    std::string content_;
    void* font_{nullptr};
    size2D size_{1.f, 1.f};
    RGBA rgba_;

    // Mode and appearance
    bool use_stroke_{true};
    float color_r_{1.f}, color_g_{1.f}, color_b_{1.f}, color_a_{1.f};
    float stroke_line_width_{2.f};

    // Stroke scaling state
    float current_scale_x_{1.f}, current_scale_y_{1.f};

    // Bitmap size hint (px). We cannot scale bitmaps; we swap fonts as a coarse approximation.
    float bitmap_px_height_{12.f};

    // Animation state
    bool animating_{false};
    float anim_duration_{0.f};
    float elapsed_{0.f};

    // Stroke targets
    float start_scale_x_{1.f}, start_scale_y_{1.f};
    float target_scale_x_{1.f}, target_scale_y_{1.f};

    // Bitmap targets
    float start_bitmap_px_{12.f}, target_bitmap_px_{12.f};
};
