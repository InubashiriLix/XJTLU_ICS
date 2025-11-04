/**
 * @file Sun.hpp
 * @brief Renders an animated sun with pulsing rays and optional outline.
 */

#pragma once

#include <GL/freeglut.h>

#include <algorithm>
#include <cmath>
#include <utility>

#include "BasicColors.hpp"

struct SunCfg {
    std::pair<float, float> center{320.f, 240.f};
    float radius = 50.f;
    int rays = 16;
    float ray_length = 40.f;
    float pulse_amp = 15.f;
    float pulse_speed = 2.0f;
    float rotate_deg_per_sec = 20.f;
    float line_thickness = 3.0f;
    Color4f core_color = Color4f(1.0f, 0.85f, 0.25f, 1.0f);
    Color4f ray_color = Color4f(1.0f, 0.75f, 0.10f, 1.0f);
    bool outline = false;
};

class Sun {
   public:
    explicit Sun(const SunCfg& cfg) : C(cfg) {}

    void render() {
        time_sec_ += 0.016f;

        glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LINE_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glLineWidth(std::max(1.f, C.line_thickness));
        glDisable(GL_TEXTURE_2D);

        glPushMatrix();
        glTranslatef(C.center.first, C.center.second, 0.f);

        const float rot_rad = deg2rad_(C.rotate_deg_per_sec) * time_sec_;
        glColor4f(C.ray_color.r, C.ray_color.g, C.ray_color.b, C.ray_color.a);
        glEnable(GL_LINE_SMOOTH);

        glBegin(GL_LINES);
        for (int i = 0; i < C.rays; ++i) {
            float base_a = (2.0f * PI_) * (i / static_cast<float>(C.rays));
            float a = base_a + rot_rad;

            float phase = base_a * 0.5f;
            float extend = C.ray_length + C.pulse_amp * std::sin(C.pulse_speed * time_sec_ + phase);

            float r0 = C.radius * 1.05f;
            float r1 = r0 + std::max(5.f, extend);

            float x0 = std::cos(a) * r0;
            float y0 = std::sin(a) * r0;
            float x1 = std::cos(a) * r1;
            float y1 = std::sin(a) * r1;

            glVertex2f(x0, y0);
            glVertex2f(x1, y1);
        }
        glEnd();

        draw_filled_circle_(0.f, 0.f, C.radius, 48, C.core_color);

        if (C.outline) {
            glColor4f(0.f, 0.f, 0.f, 1.f);
            draw_circle_wire_(0.f, 0.f, C.radius, 48, 2.2f);
        }

        glPopMatrix();
        glPopAttrib();
    }

    void setCenter(float x, float y) { C.center = {x, y}; }
    void setRadius(float r) { C.radius = std::max(1.f, r); }
    void setRayCount(int n) { C.rays = std::max(4, n); }

   private:
    SunCfg C;
    float time_sec_ = 0.f;

    static constexpr float PI_ = 3.14159265358979323846f;
    static float deg2rad_(float d) { return d * (PI_ / 180.f); }

    static void draw_filled_circle_(float cx, float cy, float r, int seg, const Color4f& c) {
        glColor4f(c.r, c.g, c.b, c.a);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);
        for (int i = 0; i <= seg; ++i) {
            float a = 2.f * PI_ * (i / static_cast<float>(seg));
            glVertex2f(cx + std::cos(a) * r, cy + std::sin(a) * r);
        }
        glEnd();
    }
    static void draw_circle_wire_(float cx, float cy, float r, int seg, float width) {
        glLineWidth(width);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < seg; ++i) {
            float a = 2.f * PI_ * (i / static_cast<float>(seg));
            glVertex2f(cx + std::cos(a) * r, cy + std::sin(a) * r);
        }
        glEnd();
    }
};
