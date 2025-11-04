/**
 * @file PaperWithText.hpp
 * @brief Extends the paper renderer with ribbons and text layout utilities.
 */

#pragma once
#include <GL/freeglut.h>

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include "Paper.hpp"

static inline float rad(float d) { return d * 3.14159265358979323846f / 180.f; }

namespace flat {

inline void filledQuad(const V2& a, const V2& b, const V2& c, const V2& d, Col col) {
    glColor4f(col.r, col.g, col.b, col.a);
    glBegin(GL_QUADS);
    glVertex2f(a.x, a.y);
    glVertex2f(b.x, b.y);
    glVertex2f(c.x, c.y);
    glVertex2f(d.x, d.y);
    glEnd();
}

inline void filledRect(float x0, float y0, float x1, float y1, Col col) {
    filledQuad({x0, y0}, {x1, y0}, {x1, y1}, {x0, y1}, col);
}

inline void rectStroke(float x0, float y0, float x1, float y1, float w, Col c) {
    glLineWidth(std::max(1.f, w));
    glColor4f(c.r, c.g, c.b, c.a);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x0, y0);
    glVertex2f(x1, y0);
    glVertex2f(x1, y1);
    glVertex2f(x0, y1);
    glEnd();
}

inline float strokeTextWidth(void* font, const std::string& s) {
    float w = 0.f;
    for (unsigned char ch : s) w += glutStrokeWidth(font, ch);
    return w;
}

inline std::vector<std::string> wrapStrokeText(void* font, const std::string& text,
                                               float maxUnits) {
    std::vector<std::string> lines;
    std::string cur;
    float curW = 0.f;

    auto flush = [&] {
        if (!cur.empty()) {
            lines.push_back(cur);
            cur.clear();
            curW = 0.f;
        }
    };

    std::string word;
    for (size_t i = 0; i <= text.size(); ++i) {
        char ch = (i < text.size() ? text[i] : ' ');
        if (ch == ' ' || ch == '\n' || i == text.size()) {
            if (!word.empty()) {
                float w = strokeTextWidth(font, word + " ");
                if (curW + w > maxUnits && !cur.empty()) flush();
                cur += word + " ";
                curW += w;
                word.clear();
            }
            if (ch == '\n') flush();
        } else {
            word.push_back(ch);
        }
    }
    flush();
    return lines;
}

inline void drawStrokeLine(void* font, const std::string& s, float px, float py, float scale,
                           Col col) {
    glPushMatrix();
    glTranslatef(px, py, 0.f);
    glScalef(scale, scale, 1.f);
    glColor4f(col.r, col.g, col.b, col.a);
    for (unsigned char ch : s) glutStrokeCharacter(font, ch);
    glPopMatrix();
}

}  // namespace flat

struct PaperWithTextCfg {
    float x = 100.f, y = 150.f, w = 880.f, h = 520.f, radius = 0.f;
    Col paper{0.985f, 0.985f, 0.97f, 1.0f};
    Col border{0.72f, 0.72f, 0.75f, 1.0f};
    bool showRibbon = false;
    Col ribbon{0.84f, 0.12f, 0.20f, 1.0f};
    Col ribbonDark{0.68f, 0.10f, 0.16f, 1.0f};
    float ribbonH = 36.f, bowW = 140.f, bowH = 74.f;

    std::string headline = "XJTLU 20th Anniversary";
    std::string body =
        "Founded in 2006, Xi'an Jiaotong-Liverpool University celebrates two decades of curiosity, "
        "collaboration and impact. From Suzhou to the world, our community has grown into a "
        "vibrant hub for research-led learning and innovation. Thank you to every student, "
        "alumnus, "
        "staff member and partner who made the journey possible. Here's to the next chapter "
        "together.";

    float marginX = 56.f, marginTop = 120.f, marginBottom = 48.f;
    float headlinePx = 42.f, bodyPx = 22.f, lineGap = 8.f;
    float borderWidth = 1.4f;

    bool coverRightRect = false;
    float coverRectStartFrac = 0.58f;
};

class PaperWithText {
   public:
    explicit PaperWithText(PaperWithTextCfg cfg) : C(std::move(cfg)) {}

    bool containsPoint(float x, float y) {
        return (x >= C.x && x <= C.x + C.w && y >= C.y && y <= C.y + C.h);
    }

    void render() const {
        flat::filledRect(C.x, C.y, C.x + C.w, C.y + C.h, C.paper);

        if (C.coverRightRect) {
            float xL = C.x + C.w * std::clamp(C.coverRectStartFrac, 0.f, 1.f);
            flat::filledRect(xL, C.y, C.x + C.w, C.y + C.h, C.paper);
        }

        if (C.borderWidth > 0.f) {
            flat::rectStroke(C.x, C.y, C.x + C.w, C.y + C.h, C.borderWidth, C.border);
        }

        drawText();
    }

   private:
    PaperWithTextCfg C;

    void drawText() const {
        float x0 = C.x + C.marginX, x1 = C.x + C.w - C.marginX;
        float yTop = C.y + C.h - C.marginTop, yBot = C.y + C.marginBottom;
        float availW = std::max(0.f, x1 - x0);
        if (availW <= 1.f) return;

        void* font = GLUT_STROKE_ROMAN;
        const float unitH = 120.f;

        float headScale = C.headlinePx / unitH;
        float headPxW = flat::strokeTextWidth(font, C.headline) * headScale;
        float hx = C.x + (C.w - headPxW) * 0.5f, hy = yTop - C.headlinePx;
        flat::drawStrokeLine(font, C.headline, hx, hy, headScale, Col{0.12f, 0.12f, 0.16f, 1});

        float bodyScale = C.bodyPx / unitH;
        float maxUnits = availW / bodyScale;
        auto lines = flat::wrapStrokeText(font, C.body, maxUnits);

        float y = hy - 24.f;
        for (auto& ln : lines) {
            y -= (C.bodyPx + C.lineGap);
            if (y < yBot) break;
            flat::drawStrokeLine(font, ln, x0, y, bodyScale, Col{0.18f, 0.18f, 0.20f, 1});
        }
    }
};
