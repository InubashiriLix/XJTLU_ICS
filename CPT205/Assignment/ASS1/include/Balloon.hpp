/**
 * @file Balloon.hpp
 * @brief Defines balloon configuration and rendering helpers for animated balloons.
 */

#pragma once

// stl
#include <climits>
#include <cmath>
#include <functional>
#include <utility>

// hal
#include <GL/freeglut.h>

// helper classes
#include <BasicColors.hpp>

struct BalloonCfg {
    enum class MOTION_TYPE { NO_MOTION, ONE_WAY, TWO_WAY, CUSTOMIZED };

    float scale_size = 1.0f;
    Color4f color = Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    std::pair<int, int> period_frame_idx = {0, INT_MAX};

    struct MotionInfo {
        MOTION_TYPE use_motion = MOTION_TYPE::NO_MOTION;
        std::pair<int, int> start_pos{0, 0};
        std::pair<int, int> end_pos{0, 0};
        int step_per_frame = 1;
        std::function<void(std::pair<int, int>&, int)> custom_motion_func;
    } motion_info;

    static BalloonCfg makeRandomOneWayVertical(int gw, int gh) {
        BalloonCfg cfg;
        cfg.scale_size = 1.0f + static_cast<float>(rand()) / RAND_MAX;  // [1,2]
        cfg.color =
            Color4f(static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX,
                    static_cast<float>(rand()) / RAND_MAX,
                    0.5f + static_cast<float>(rand()) / (2.0f * RAND_MAX)  // [0.5,1]
            );
        int x = rand() % gw;
        cfg.motion_info.use_motion = MOTION_TYPE::ONE_WAY;
        cfg.motion_info.start_pos = {x, 0};
        cfg.motion_info.end_pos = {x, gh + 200};
        cfg.motion_info.step_per_frame = 1 + rand() % 5;
        cfg.period_frame_idx = {0, INT_MAX};
        return cfg;
    }
};

class Balloon {
   public:
    explicit Balloon(BalloonCfg cfg) : _cfg(std::move(cfg)), _cur_pos(_cfg.motion_info.start_pos) {}

    ~Balloon() = default;

    void resetStatus() {
        _cur_pos = _cfg.motion_info.start_pos;
        _t = 0.f;
    }

    void killyourself() { _alive = false; }

    bool isAlive() const { return _alive; }

    void render(std::function<void(int, int, int, int, Balloon*)>& cb) {
        if (!_alive) return;

        if (_frame_idx < _cfg.period_frame_idx.first || _frame_idx > _cfg.period_frame_idx.second) {
            ++_frame_idx;
            return;
        }

        update_motion_();

        const int radius = std::max(4, static_cast<int>(24.0f * _cfg.scale_size));
        const int w = radius * 2;
        const int h = radius * 2 + radius * 2.2f;  // the sum of circle and the tail length
        const int x_lb = _cur_pos.first - radius;
        const int y_lb = _cur_pos.second - radius - radius * 2.2f;

        if (cb) cb(x_lb, y_lb, w, h, this);

        glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LINE_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glColor4f(_cfg.color.r, _cfg.color.g, _cfg.color.b, _cfg.color.a);
        draw_circle_filled_(static_cast<float>(_cur_pos.first), static_cast<float>(_cur_pos.second),
                            static_cast<float>(radius), 48);

        glColor4f(0.f, 0.f, 0.f, 1.f);
        draw_circle_wire_(static_cast<float>(_cur_pos.first), static_cast<float>(_cur_pos.second),
                          static_cast<float>(radius), 48, 2.0f);

        glColor4f(1.f, 1.f, 1.f, 0.9f);
        draw_circle_filled_(_cur_pos.first - radius * 0.30f, _cur_pos.second + radius * 0.30f,
                            radius * 0.18f, 20);

        glEnable(GL_LINE_SMOOTH);
        glLineWidth(2.0f);
        glColor4f(0.f, 0.f, 0.f, 1.f);
        glBegin(GL_LINE_STRIP);
        const float line_len = radius * 2.2f;
        for (int i = 0; i <= 20; ++i) {
            float t = i / 20.f;
            float yy = _cur_pos.second - radius - t * line_len;
            float xx =
                _cur_pos.first + std::sin((_phase + t) * 6.2831853f) * radius * 0.15f * (1.0f - t);
            glVertex2f(xx, yy);
        }
        glEnd();
        glDisable(GL_LINE_SMOOTH);

        glPopAttrib();

        ++_frame_idx;
    }

   private:
    void update_motion_() {
        if (!_alive) return;

        using MT = BalloonCfg::MOTION_TYPE;
        auto& mi = _cfg.motion_info;

        switch (mi.use_motion) {
            case MT::NO_MOTION:
                break;
            case MT::ONE_WAY:
                step_along_line_(/*pingpong=*/false);
                break;
            case MT::TWO_WAY:
                step_along_line_(/*pingpong=*/true);
                break;
            case MT::CUSTOMIZED:
                if (mi.custom_motion_func) {
                    mi.custom_motion_func(_cur_pos, _frame_idx);
                }
                break;
        }

        _phase += 0.03f;
        if (_phase > 6.2831853f) _phase -= 6.2831853f;
    }

    void step_along_line_(bool pingpong) {
        auto& mi = _cfg.motion_info;

        const float sx = static_cast<float>(mi.start_pos.first);
        const float sy = static_cast<float>(mi.start_pos.second);
        const float ex = static_cast<float>(mi.end_pos.first);
        const float ey = static_cast<float>(mi.end_pos.second);

        const float dx = ex - sx;
        const float dy = ey - sy;
        const float L = std::sqrt(dx * dx + dy * dy);
        if (L < 1e-4f) return;

        const int step_px = std::max(0, mi.step_per_frame);
        if (step_px == 0) return;
        const float dt = static_cast<float>(step_px) / L;

        if (pingpong) {
            _t += dt * _t_dir;
            if (_t >= 1.f) {
                _t = 1.f;
                _t_dir = -1.f;
            }
            if (_t <= 0.f) {
                _t = 0.f;
                _t_dir = 1.f;
            }
        } else {
            if (_t < 1.f) _t = std::min(1.f, _t + dt);
        }

        const float nx = sx + dx * _t;
        const float ny = sy + dy * _t;
        _cur_pos = {static_cast<int>(std::lround(nx)), static_cast<int>(std::lround(ny))};
    }


    static void draw_circle_filled_(float cx, float cy, float r, int seg) {
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);
        for (int i = 0; i <= seg; ++i) {
            float a = (2.0f * 3.1415926535f) * (i / static_cast<float>(seg));
            glVertex2f(cx + std::cos(a) * r, cy + std::sin(a) * r);
        }
        glEnd();
    }
    static void draw_circle_wire_(float cx, float cy, float r, int seg, float width) {
        glLineWidth(width);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < seg; ++i) {
            float a = (2.0f * 3.1415926535f) * (i / static_cast<float>(seg));
            glVertex2f(cx + std::cos(a) * r, cy + std::sin(a) * r);
        }
        glEnd();
    }

   private:
    BalloonCfg _cfg;
    std::pair<int, int> _cur_pos{0, 0};

    int _frame_idx = 0;
    float _t = 0.f;
    float _t_dir = 1.f;
    float _phase = 0.f;

    bool _alive = true;
};
