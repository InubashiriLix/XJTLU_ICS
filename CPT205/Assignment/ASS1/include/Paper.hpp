/**
 * @file Paper.hpp
 * @brief Implements the paper sheet renderer with shading, folding, and interaction helpers.
 */

#pragma once

#include <GL/freeglut.h>

#include <algorithm>
#include <cmath>
#include <vector>

struct Col {
    float r, g, b, a;
};
struct V2 {
    float x, y;
};

struct PaperCfg {
    float cx = 600.f, cy = 400.f;
    float width = 520.f, height = 360.f;
    float rotDeg = -15.f;
    float persp = 0.16f;
    float curl = 34.f;

    bool drawEdge = true;
    bool drawShadow = true;
    bool drawFold = false;

    Col paper{0.98f, 0.98f, 0.965f, 1.0f};
    Col edge{0.70f, 0.70f, 0.72f, 1.0f};
    Col back{0.92f, 0.92f, 0.90f, 1.0f};
    Col foldS{0.0f, 0.0f, 0.0f, 0.10f};
    Col shadow{0.0f, 0.0f, 0.0f, 0.18f};
};

class Paper {
   public:
    explicit Paper(PaperCfg cfg) : C(cfg) {}

    void setCenter(float x, float y) {
        C.cx = x;
        C.cy = y;
    }
    void setSize(float w, float h) {
        C.width = w;
        C.height = h;
    }
    void setRotation(float deg) { C.rotDeg = deg; }
    void setPersp(float p) { C.persp = std::clamp(p, 0.f, 0.4f); }
    void setCurl(float c) { C.curl = std::max(0.f, c); }
    bool containsPoint(float x_, float y_) {
        return (x_ > C.cx - C.width / 2.f) && (x_ < C.cx + C.width / 2.f) &&
               (y_ < C.cy + C.height / 2.f) && (y_ > C.cy - C.height / 2.f);
    }

    void render() const {
        if (C.drawShadow)
            rotEllipseFilled(C.cx, C.cy - C.height * 0.06f, C.width * 0.55f, C.height * 0.18f,
                             C.rotDeg, C.shadow, 72);

        float w = C.width, h = C.height;
        float topK = std::max(0.f, 1.f - std::clamp(C.persp, 0.f, 0.4f));
        V2 TL = {-w * 0.5f * topK, +h * 0.5f};
        V2 TR = {+w * 0.5f * topK, +h * 0.5f};
        V2 BR = {+w * 0.5f, -h * 0.5f};
        V2 BL = {-w * 0.5f, -h * 0.5f};

        TL = rot(TL, C.rotDeg);
        TR = rot(TR, C.rotDeg);
        BR = rot(BR, C.rotDeg);
        BL = rot(BL, C.rotDeg);
        translate(TL, C.cx, C.cy);
        translate(TR, C.cx, C.cy);
        translate(BR, C.cx, C.cy);
        translate(BL, C.cx, C.cy);

        quad(TL, TR, BR, BL, C.paper);

        float curl = C.curl;
        V2 t_local = {+w * 0.5f * topK, +h * 0.5f};
        V2 pB_local = {t_local.x - curl, t_local.y};
        V2 pC_local = {t_local.x, t_local.y - curl};
        V2 T = world(t_local), PB = world(pB_local), PC = world(pC_local);

        triFan({T, PB, PC}, C.back);

        float r = curl * 0.9f;
        V2 arcC_local = {t_local.x - r * 0.7071f, t_local.y - r * 0.7071f};
        V2 arcC = world(arcC_local);
        std::vector<V2> arc;
        arc.reserve(20);
        arc.push_back(arcC);
        for (int i = 0; i <= 18; ++i) {
            float ang = 3.1415926535f * 0.25f * (i / 18.f);
            float x = arcC_local.x + r * std::cos(ang);
            float y = arcC_local.y + r * std::sin(ang);
            V2 q = world({x, y});
            arc.push_back(q);
        }
        triFan(arc, {1, 1, 1, 0.35f});

        if (C.drawFold) {
            V2 n{PC.y - PB.y, -(PC.x - PB.x)};
            norm(n);
            float wThin = 2.0f;
            quad(PB, PC, {PC.x + n.x * wThin, PC.y + n.y * wThin},
                 {PB.x + n.x * wThin, PB.y + n.y * wThin}, C.foldS);
        }

        if (C.drawEdge) lineLoop({TL, TR, BR, BL}, 1.2f, C.edge);
    }

   private:
    PaperCfg C;

    static V2 rot(const V2& p, float deg) {
        float t = deg * 3.14159265358979323846f / 180.f, c = std::cos(t), s = std::sin(t);
        return {p.x * c - p.y * s, p.x * s + p.y * c};
    }
    static void translate(V2& p, float x, float y) {
        p.x += x;
        p.y += y;
    }
    V2 world(const V2& local) const {
        V2 r = rot(local, C.rotDeg);
        r.x += C.cx;
        r.y += C.cy;
        return r;
    }

    static void quad(const V2& a, const V2& b, const V2& c, const V2& d, Col col) {
        glColor4f(col.r, col.g, col.b, col.a);
        glBegin(GL_QUADS);
        glVertex2f(a.x, a.y);
        glVertex2f(b.x, b.y);
        glVertex2f(c.x, c.y);
        glVertex2f(d.x, d.y);
        glEnd();
    }
    static void triFan(const std::vector<V2>& pts, Col col) {
        if (pts.size() < 3) return;
        glColor4f(col.r, col.g, col.b, col.a);
        glBegin(GL_TRIANGLE_FAN);
        for (auto& p : pts) glVertex2f(p.x, p.y);
        glEnd();
    }
    static void lineLoop(const std::vector<V2>& pts, float w, Col col) {
        glLineWidth(std::max(1.f, w));
        glColor4f(col.r, col.g, col.b, col.a);
        glBegin(GL_LINE_LOOP);
        for (auto& p : pts) glVertex2f(p.x, p.y);
        glEnd();
    }
    static void rotEllipseFilled(float cx, float cy, float rx, float ry, float deg, Col col,
                                 int seg) {
        glColor4f(col.r, col.g, col.b, col.a);
        float t = deg * 3.14159265358979323846f / 180.f, c = std::cos(t), s = std::sin(t);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);
        for (int i = 0; i <= seg; ++i) {
            float a = 2.f * 3.1415926535f * (i / (float)seg);
            float x = rx * std::cos(a), y = ry * std::sin(a);
            float X = x * c - y * s, Y = x * s + y * c;
            glVertex2f(cx + X, cy + Y);
        }
        glEnd();
    }
    static void norm(V2& v) {
        float L = std::sqrt(v.x * v.x + v.y * v.y);
        if (L > 1e-6f) {
            v.x /= L;
            v.y /= L;
        }
    }
};
