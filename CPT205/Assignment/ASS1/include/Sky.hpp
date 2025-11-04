/**
 * @file Sky.hpp
 * @brief Generates the sky, clouds, and animation controls.
 */

#pragma once

#include <GL/freeglut.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <utility>
#include <vector>

#include "BasicColors.hpp"

struct SkyCfg {
    float width = 1024.f;
    float height = 640.f;

    Color4f bottom_color = Color4f(0.90f, 0.97f, 1.00f, 1.0f);
    Color4f top_color = Color4f(0.70f, 0.85f, 1.00f, 1.0f);

    int layers = 3;
    int clouds_per_layer = 5;
    bool wind_to_right = true;
    float speed_min = 12.f;  // px/s
    float speed_max = 36.f;
    float scale_min = 0.8f;
    float scale_max = 1.6f;
    Color4f cloud_color = Color4f(1.f, 1.f, 1.f, 0.93f);

    bool motive = true;
    int speed_scale = 1;

    int seed = -1;
};

class Sky {
   public:
    explicit Sky(const SkyCfg& cfg) : C(cfg) {
        if (C.seed < 0)
            std::srand((unsigned)std::time(nullptr));
        else
            std::srand((unsigned)C.seed);
        buildClouds_();
    }

    void setViewport(float w, float h) {
        C.width = w;
        C.height = h;
    }

    void setTime(float t) {
        time_sec_ = t;
        use_external_time_ = true;
    }

    void render() {
        if (C.motive && !use_external_time_) {
            time_sec_ += 0.016f * std::max(1, C.speed_scale);
        }

        glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LINE_BIT);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (int layer = 0; layer < std::max(1, C.layers); ++layer) {
            float depthK = (C.layers <= 1) ? 1.f : (layer / float(C.layers - 1));  // 0..1
            float parallax = 0.5f + 0.7f * depthK;
            float alphaMul = 0.75f + 0.25f * (1.f - depthK);

            for (auto& cl : clouds_)
                if (cl.layer == layer) {
                    float dt = 0.016f;
                    if (use_external_time_) {
                        dt = std::max(0.f, time_sec_ - cl.last_t);
                        cl.last_t = time_sec_;
                    }

                    float dir = C.wind_to_right ? 1.f : -1.f;
                    float v = cl.speed * parallax * std::max(1, C.speed_scale);  // px/s
                    cl.x += dir * v * dt;

                    float margin = 120.f * cl.scale;
                    if (C.wind_to_right) {
                        if (cl.x > C.width + margin) cl.x = -margin;
                    } else {
                        if (cl.x < -margin) cl.x = C.width + margin;
                    }

                    glPushMatrix();
                    glTranslatef(cl.x, cl.y, 0.f);
                    glScalef(cl.scale, cl.scale, 1.f);
                    drawCloud_(cl, alphaMul);
                    glPopMatrix();
                }
        }

        glPopAttrib();
    }

   private:
    struct Puff {
        float ox, oy, rx, ry;
    };
    struct Cloud {
        int layer = 0;
        float x = 0.f, y = 0.f;
        float scale = 1.f;
        float speed = 20.f;  // px/s
        float last_t = 0.f;
        std::vector<Puff> puffs;
    };

    SkyCfg C;
    std::vector<Cloud> clouds_;
    float time_sec_ = 0.f;
    bool use_external_time_ = false;

    static constexpr float PI_ = 3.14159265358979323846f;

    void buildClouds_() {
        clouds_.clear();
        int total = std::max(1, C.layers) * std::max(1, C.clouds_per_layer);
        clouds_.reserve(total);

        for (int layer = 0; layer < std::max(1, C.layers); ++layer) {
            float depthK = (C.layers <= 1) ? 1.f : (layer / float(C.layers - 1));  // 0..1

            float y_lo = C.height * (0.60f + 0.20f * (1.f - depthK));  // 0.60~0.80H
            float y_hi = C.height * (0.78f + 0.17f * (1.f - depthK));  // 0.78~0.95H

            for (int i = 0; i < std::max(1, C.clouds_per_layer); ++i) {
                Cloud cl;
                cl.layer = layer;
                float ux = (float)std::rand() / RAND_MAX;
                float uy = (float)std::rand() / RAND_MAX;
                cl.x = ux * C.width;
                cl.y = y_lo + uy * (y_hi - y_lo);

                float us = (float)std::rand() / RAND_MAX;
                float u2 = (float)std::rand() / RAND_MAX;
                cl.scale = C.scale_min + us * (C.scale_max - C.scale_min);
                cl.speed = C.speed_min + u2 * (C.speed_max - C.speed_min);
                cl.last_t = time_sec_;

                int puffs = 3 + (std::rand() % 4);
                cl.puffs.reserve(puffs);
                for (int k = 0; k < puffs; ++k) {
                    float a = ((float)std::rand() / RAND_MAX) * 2.f * PI_;
                    float r = 18.f + 26.f * ((float)std::rand() / RAND_MAX);  // 18~44
                    float ox = std::cos(a) * (12.f + 22.f * ((float)std::rand() / RAND_MAX));
                    float oy = std::sin(a) * (6.f + 12.f * ((float)std::rand() / RAND_MAX));
                    float rx = r * (0.9f + 0.3f * ((float)std::rand() / RAND_MAX));
                    float ry = r * (0.6f + 0.3f * ((float)std::rand() / RAND_MAX));
                    cl.puffs.push_back(Puff{ox, oy, rx, ry});
                }
                clouds_.push_back(std::move(cl));
            }
        }
    }

    static void drawEllipseFilled_(float cx, float cy, float rx, float ry, int seg = 28) {
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);
        for (int i = 0; i <= seg; ++i) {
            float a = 2.f * PI_ * (i / float(seg));
            glVertex2f(cx + std::cos(a) * rx, cy + std::sin(a) * ry);
        }
        glEnd();
    }

    void drawCloud_(const Cloud& cl, float alphaMul) const {
        Color4f col = C.cloud_color;
        col.a *= alphaMul;

        glColor4f(col.r, col.g, col.b, col.a * 0.45f);
        drawEllipseFilled_(0.f, 0.f, 56.f, 32.f, 32);

        glColor4f(col.r, col.g, col.b, col.a);
        for (const auto& p : cl.puffs) {
            drawEllipseFilled_(p.ox, p.oy, p.rx, p.ry, 28);
        }
    }
};
