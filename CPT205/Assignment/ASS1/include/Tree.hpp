/**
 * @file Tree.hpp
 * @brief Provides animated tree generation with configurable branches and fruits.
 */

#pragma once

// stl
#include <algorithm>  // std::max, std::min
#include <climits>
#include <cmath>
#include <cstdlib>
#include <utility>
#include <vector>

// opengl lib
#include <GL/freeglut.h>

// helper classes
#include <BasicColors.hpp>

struct TreeCfg {
    std::pair<int, int> pos_root{0, 0};
    bool motive = false;

    struct fruit_info_t {
        bool has_fruits = false;
        bool randomlize = true;
        Color4f fruit_color = COLORS::RED;
    } fruit_info;

    struct branch_info_t {
        bool randomlize = true;
        Color4f root_color = COLORS::BROWN;
        Color4f leaf_color = COLORS::GREEN;
    } branch_info;

    static float randomFloat(float lower, float upper) {
        return lower + static_cast<float>(rand()) / RAND_MAX * (upper - lower);
    }

    void randomlizeFruitColor() {
        fruit_info.fruit_color = Color4f(randomFloat(0.75f, 1.00f),  // R
                                         randomFloat(0.20f, 0.65f),  // G
                                         randomFloat(0.05f, 0.30f),  // B
                                         1.0f);
    }

    void randomlizeBrachColor() {
        float k1 = randomFloat(0.85f, 1.15f);
        float k2 = randomFloat(0.85f, 1.15f);
        branch_info.root_color = branch_info.root_color * k1;
        branch_info.leaf_color = branch_info.leaf_color * k2;
    }

    TreeCfg() {
        if (fruit_info.has_fruits && fruit_info.randomlize) randomlizeFruitColor();
        if (branch_info.randomlize) randomlizeBrachColor();
    }
};

namespace TREE_CFG_PRELOADS {

inline const TreeCfg AUTUMN = [] {
    TreeCfg c;
    c.motive = true;

    c.branch_info.randomlize = false;
    c.branch_info.root_color = Color4f(0.55f, 0.33f, 0.15f, 1.0f);
    c.branch_info.leaf_color = Color4f(0.95f, 0.60f, 0.18f, 1.0f);

    c.fruit_info.has_fruits = true;
    c.fruit_info.randomlize = false;
    c.fruit_info.fruit_color = Color4f(0.90f, 0.20f, 0.20f, 1.0f);

    return c;
}();

inline const TreeCfg SPRING = [] {
    TreeCfg c;
    c.motive = true;

    c.branch_info.randomlize = false;
    c.branch_info.root_color = COLORS::BROWN;
    c.branch_info.leaf_color = Color4f(0.35f, 0.78f, 0.36f, 1.0f);

    c.fruit_info.has_fruits = false;
    c.fruit_info.randomlize = false;

    return c;
}();

inline const TreeCfg KANADE = [] {
    TreeCfg c;
    c.motive = true;

    c.branch_info.randomlize = false;
    c.branch_info.root_color = Color4f(0.50f, 0.34f, 0.22f, 1.0f);
    c.branch_info.leaf_color = Color4f(0.98f, 0.78f, 0.86f, 1.0f);

    c.fruit_info.has_fruits = false;
    c.fruit_info.randomlize = false;
    c.fruit_info.fruit_color = Color4f(0.95f, 0.55f, 0.70f, 1.0f);

    return c;
}();

}  // namespace TREE_CFG_PRELOADS

class Tree {
   public:
    explicit Tree(TreeCfg cfg) : _cfg(std::move(cfg)) {
        const float s = 0.90f + (rand() % 21) / 100.0f;  // 0.90~1.10
        _trunk_h = int(160 * s);
        _trunk_w = std::max(8, int(16 * s));
        _canopy_r = int(60 * s);

        _sway_amp_deg = 2.5f + (rand() % 16) / 10.0f;
        _sway_speed = 0.012f + (rand() % 10) / 2000.0f;  // 0.012~0.017
        _phase0 = (rand() % 6283) / 1000.0f;

        gen_leaves_();
        if (_cfg.fruit_info.has_fruits) gen_fruits_();
    }

    Tree& setPosition(int x, int y) {
        this->_cfg.pos_root = {x, y};
        return *this;
    }

    void render_branchs() {
        glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LINE_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glColor4f(_cfg.branch_info.root_color.r, _cfg.branch_info.root_color.g,
                  _cfg.branch_info.root_color.b, _cfg.branch_info.root_color.a);
        const float x0 = -_trunk_w * 0.5f;
        const float x1 = _trunk_w * 0.5f;
        const float y0 = 0.0f;
        const float y1 = float(_trunk_h);
        glBegin(GL_QUADS);
        glVertex2f(x0, y0);
        glVertex2f(x1, y0);
        glVertex2f(x1, y1);
        glVertex2f(x0, y1);
        glEnd();

        const float fork_y = y1 * 0.72f;

        glEnable(GL_LINE_SMOOTH);
        glLineWidth(std::max(2.f, _trunk_w * 0.6f));

        glBegin(GL_LINE_STRIP);
        draw_branch_polyline_(-_trunk_w * 0.1f, fork_y, -_canopy_r * 0.6f, y1 + _canopy_r * 0.1f,
                              -0.6f);
        glEnd();

        glBegin(GL_LINE_STRIP);
        draw_branch_polyline_(_trunk_w * 0.1f, fork_y, _canopy_r * 0.7f, y1 + _canopy_r * 0.2f,
                              0.6f);
        glEnd();

        glBegin(GL_LINE_STRIP);
        draw_branch_polyline_(-_trunk_w * 0.2f, fork_y * 0.85f, -_canopy_r * 0.8f,
                              fork_y + _canopy_r * 0.2f, -0.3f);
        glEnd();

        glBegin(GL_LINE_STRIP);
        draw_branch_polyline_(_trunk_w * 0.2f, fork_y * 0.85f, _canopy_r * 0.8f,
                              fork_y + _canopy_r * 0.25f, 0.3f);
        glEnd();

        glPopAttrib();
    }

    void render_leaves() {
        glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LINE_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        const float cx = 0.0f;
        const float cy = float(_trunk_h) + _canopy_r * 0.12f;

        for (const auto& L : _leaves) {
            glColor4f(L.c.r, L.c.g, L.c.b, L.c.a);
            if (L.is_circle) {
                draw_circle_filled_(cx + L.x, cy + L.y, L.rx, 32);
            } else {
                draw_ellipse_filled_(cx + L.x, cy + L.y, L.rx, L.ry, 32);
            }
        }

        if (!_fruits.empty()) {
            glColor4f(_fruit_color.r, _fruit_color.g, _fruit_color.b, _fruit_color.a);
            for (const auto& f : _fruits) {
                draw_circle_filled_(cx + f.x, cy + f.y, f.r, 20);

                glColor4f(1.f, 1.f, 1.f, 0.9f);
                draw_circle_filled_(cx + f.x - f.r * 0.25f, cy + f.y + f.r * 0.25f, f.r * 0.25f,
                                    12);
                glColor4f(_fruit_color.r, _fruit_color.g, _fruit_color.b, _fruit_color.a);
            }
        }

        glPopAttrib();
    }

    void render() {
        glPushMatrix();
        glTranslatef(static_cast<float>(_cfg.pos_root.first),
                     static_cast<float>(_cfg.pos_root.second), 0.0f);

        float deg = 0.0f;
        if (_cfg.motive) deg = _sway_amp_deg * std::sin(_phase0 + _frame * _sway_speed);
        glRotatef(deg, 0.f, 0.f, 1.f);

        render_branchs();
        render_leaves();

        glPopMatrix();
        ++_frame;
    }

   private:
    struct Leaf {
        float x, y;
        float rx, ry;
        bool is_circle;
        Color4f c;
    };
    struct Fruit {
        float x, y, r;
    };

    void gen_leaves_() {
        _leaves.clear();
        _leaves.reserve(24);

        auto base = _cfg.branch_info.leaf_color;
        auto clamp = [](float v) { return std::max(0.f, std::min(1.f, v)); };

        {
            Leaf L;
            L.x = 0.f;
            L.y = _canopy_r * 0.00f;
            L.rx = _canopy_r * 0.95f;
            L.ry = _canopy_r * 0.78f;
            L.is_circle = false;
            L.c = Color4f(base.r, base.g, base.b, 0.25f);
            _leaves.push_back(L);
        }
        for (int i = 0; i < 4; ++i) {
            float ang = (rand() % 6283) / 1000.0f;
            Leaf L;
            L.x = std::cos(ang) * _canopy_r * 0.28f;
            L.y = std::sin(ang) * _canopy_r * 0.18f - _canopy_r * 0.05f;
            L.rx = _canopy_r * (0.32f + (rand() % 20) / 100.0f);
            L.ry = L.rx * (0.70f + (rand() % 20) / 100.0f);
            L.is_circle = false;
            L.c = Color4f(base.r, base.g, base.b, 0.22f);
            _leaves.push_back(L);
        }

        const int blobs = 14 + rand() % 7;  // 14~20
        for (int i = 0; i < blobs; ++i) {
            float a = (rand() % 6283) / 1000.0f;
            float u = (rand() % 1000) / 1000.0f;
            float rr = std::sqrt(u);
            float ox = std::cos(a) * rr * _canopy_r * 0.62f;
            float oy = std::sin(a) * rr * _canopy_r * 0.48f - _canopy_r * 0.03f;

            float r0 = _canopy_r * (0.16f + (rand() % 28) / 100.0f);  // 0.16~0.44
            float rx = r0 * (1.00f + (rand() % 25) / 100.0f);         // 1.00~1.25
            float ry = r0 * (0.80f + (rand() % 20) / 100.0f);         // 0.80~1.00

            float j = (rand() % 13 - 6) / 255.0f;  // -0.023 ~ +0.023
            Color4f c = Color4f(clamp(base.r + j * 0.6f), clamp(base.g + j * 1.0f),
                                clamp(base.b + j * 0.6f), 0.90f);

            bool use_circle = (i % 3 == 0);
            _leaves.push_back(
                Leaf{ox, oy, (use_circle ? r0 : rx), (use_circle ? r0 : ry), use_circle, c});
        }
    }

    void gen_fruits_() {
        _fruit_color = _cfg.fruit_info.fruit_color;

        const int n = 6 + rand() % 11;  // 6~16
        _fruits.reserve(n);
        for (int i = 0; i < n; ++i) {
            float a = (rand() % 6283) / 1000.0f;
            float u = (rand() % 1000) / 1000.0f;
            float r = std::sqrt(u) * (_canopy_r * 0.80f);
            float x = std::cos(a) * r * 0.85f;
            float y = std::sin(a) * r * 0.65f + _canopy_r * 0.02f;
            float rr = 3.0f + (rand() % 4);  // 3~6
            _fruits.push_back({x, y, rr});
        }
    }

    static void draw_circle_filled_(float cx, float cy, float r, int seg) {
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);
        for (int i = 0; i <= seg; ++i) {
            float a = 2.0f * 3.1415926535f * (i / static_cast<float>(seg));
            glVertex2f(cx + std::cos(a) * r, cy + std::sin(a) * r);
        }
        glEnd();
    }
    static void draw_ellipse_filled_(float cx, float cy, float rx, float ry, int seg) {
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);
        for (int i = 0; i <= seg; ++i) {
            float a = 2.0f * 3.1415926535f * (i / static_cast<float>(seg));
            glVertex2f(cx + std::cos(a) * rx, cy + std::sin(a) * ry);
        }
        glEnd();
    }
    static void draw_branch_polyline_(float x0, float y0, float x1, float y1, float curv) {
        const int segments = 12;
        glVertex2f(x0, y0);
        for (int i = 1; i <= segments; ++i) {
            float t = i / static_cast<float>(segments);
            float mx = (x0 + x1) * 0.5f + (y1 - y0) * 0.15f * curv;
            float my = (y0 + y1) * 0.5f - (x1 - x0) * 0.15f * curv;
            float xa = x0 + (mx - x0) * t;
            float ya = y0 + (my - y0) * t;
            float xb = mx + (x1 - mx) * t;
            float yb = my + (y1 - my) * t;
            float x = xa + (xb - xa) * t;
            float y = ya + (yb - ya) * t;
            glVertex2f(x, y);
        }
    }

   private:
    TreeCfg _cfg;

    int _trunk_h = 160;
    int _trunk_w = 16;
    int _canopy_r = 60;

    int _frame = 0;
    float _sway_amp_deg = 3.2f;
    float _sway_speed = 0.015f;
    float _phase0 = 0.0f;

    std::vector<Leaf> _leaves;
    std::vector<Fruit> _fruits;
    Color4f _fruit_color = COLORS::RED;
};
