/**
 * @file Grass.hpp
 * @brief Implements animated grass patches and fields with configurable wind effects.
 */

#pragma once

// stl
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <utility>
#include <vector>

// opengl
#include <GL/freeglut.h>

// cus
#include "BasicColors.hpp"


struct GrassPatchCfg {
    bool motive = false;
    int speed = 1;
    Color4f color = COLORS::GREEN;
    bool win_dir_right = true;
};

class GrassPatch {
   public:
    explicit GrassPatch(GrassPatchCfg cfg) : _cfg(std::move(cfg)) { regenerate_(); }



    GrassPatch& setOrigin(float x, float y) {
        _ox = x;
        _oy = y;
        return *this;
    }

    GrassPatch& setSize(float w, float h) {
        _pw = std::max(1.f, w);
        _ph = std::max(1.f, h);
        regenerate_();
        return *this;
    }

    GrassPatch& setBladeCount(int n) {
        _blades = std::max(1, n);
        regenerate_();
        return *this;
    }

    GrassPatch& setSegments(int seg) {
        _segments = std::max(3, seg);
        return *this;
    }

    GrassPatch& setWindAmplitude(float amp) {
        _wind_amp = std::max(0.f, amp);
        return *this;
    }

    GrassPatch& setTime(float t) {
        _time = t;
        return *this;
    }


    void render() {

        if (_cfg.motive) {

            const float dt = 0.016f * std::max(1, _cfg.speed);
            _time += dt;
        }

        glPushMatrix();
        glTranslatef(_ox, _oy, 0.0f);

        glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LINE_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (const auto& b : _data) drawBlade_(b);

        glPopAttrib();
        glPopMatrix();
    }

   private:

    struct Blade {
        float x0;
        float height;
        float width;
        float bend;
        float phase;
        float speedK;
        float ampK;
    };


    void regenerate_() {
        _data.clear();
        _data.reserve(_blades);
        for (int i = 0; i < _blades; ++i) {
            Blade b{};
            float u = (float)std::rand() / RAND_MAX;
            b.x0 = u * _pw;


            float uh = (float)std::rand() / RAND_MAX;
            b.height = std::max(8.f, _ph * (0.5f + 0.5f * uh));


            b.width = 2.0f + 3.0f * ((float)std::rand() / RAND_MAX);


            float ub = (float)std::rand() / RAND_MAX;                 // 0..1
            float bend_span = 25.f;
            float base_bend = (ub * 2.f - 1.f) * bend_span;           // -25..25
            b.bend = base_bend * (0.7f + 0.6f * ((b.height / _ph)));


            b.phase = ((float)std::rand() / RAND_MAX) * 6.2831853f;
            b.speedK = 0.6f + 0.8f * ((float)std::rand() / RAND_MAX);  // 0.6~1.4
            b.ampK = 0.8f + 0.4f * ((float)std::rand() / RAND_MAX);    // 0.8~1.2

            _data.push_back(b);
        }
    }


    void drawBlade_(const Blade& b) const {
        const int seg = _segments;
        const float dir = _cfg.win_dir_right ? 1.f : -1.f;


        auto clamp01 = [](float v) { return std::max(0.f, std::min(1.f, v)); };
        const Color4f base = _cfg.color;
        Color4f tip;
        tip.r = clamp01(base.r * 1.10f + 0.10f);
        tip.g = clamp01(base.g * 1.10f + 0.10f);
        tip.b = clamp01(base.b * 1.05f + 0.05f);
        tip.a = base.a;


        float prev_cx = b.x0;
        float prev_cy = (b.height / seg);

        glBegin(GL_TRIANGLE_STRIP);
        for (int i = 0; i <= seg; ++i) {
            float t = (float)i / seg;  // 0..1 from root to tip
            float y = t * b.height;


            float global_speed = (0.9f + 0.1f * _cfg.speed);
            float w = global_speed * 1.6f * b.speedK;
            float local_amp = _wind_amp * t * (0.25f + 0.75f * t);
            float sway = dir * local_amp * b.ampK * std::sin(w * _time + b.phase);


            float curve = b.bend * t * t;

            float cx = b.x0 + curve + sway;
            float cy = y;


            float dx = cx - prev_cx;
            float dy = cy - prev_cy;
            float len = std::sqrt(dx * dx + dy * dy);
            if (len < 1e-5f) {
                dx = 0.f;
                dy = 1.f;
                len = 1.f;
            }
            dx /= len;
            dy /= len;
            float nx = -dy, ny = dx;


            float width = b.width * std::pow(1.0f - t, 1.5f);


            Color4f c;
            c.r = base.r + (tip.r - base.r) * t;
            c.g = base.g + (tip.g - base.g) * t;
            c.b = base.b + (tip.b - base.b) * t;
            c.a = base.a + (tip.a - base.a) * t;
            glColor4f(c.r, c.g, c.b, c.a);


            glVertex2f(cx + nx * width, cy + ny * width);
            glVertex2f(cx - nx * width, cy - ny * width);

            prev_cx = cx;
            prev_cy = cy;
        }
        glEnd();
    }

   private:
    GrassPatchCfg _cfg;


    float _ox = 0.f, _oy = 0.f;
    float _pw = 600.f, _ph = 100.f;
    int _blades = 400;
    int _segments = 8;
    float _wind_amp = 12.f;
    float _time = 0.f;

    std::vector<Blade> _data;
};


struct GrassFieldCfg {
};

class GrassField {
   public:
    GrassField() = default;


    GrassPatch& addPatch(const GrassPatchCfg& cfg) {
        _patches.emplace_back(cfg);
        return _patches.back();
    }


    void addPatch(const GrassPatch& p) { _patches.push_back(p); }


    void render() {
        for (auto& p : _patches) p.render();
    }


    void moveAll(float dx, float dy) {
        for (auto& p : _patches) {



            (void)dx;
            (void)dy;
        }
    }

   private:
    std::vector<GrassPatch> _patches;
};
