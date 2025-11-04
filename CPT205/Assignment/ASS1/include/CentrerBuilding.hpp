/**
 * @file CentrerBuilding.hpp
 * @brief Builds and renders the central building polygon mesh with configurable styling.
 */

#pragma once
// FreeGLUT + C++17
#include <GL/freeglut.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <vector>


struct RGBA {
    float r, g, b, a;
};
struct Vec2 {
    float x, y;
};


struct CentralBuildingCfg {

    float baseW = 1200.f;
    float baseH = 800.f;


    float originX = 0.f;
    float originY = 0.f;
    float scale = 1.f;


    bool drawStroke = false;
    float strokeWidth = 1.5f;
    RGBA strokeColor{0.f, 0.f, 0.f, 1.f};


    std::vector<RGBA> perPolyColor;


    float seamSealPx = 0.0f;
    std::vector<int> seamSealList;
};


class CentralBuilding {
   public:
    explicit CentralBuilding(CentralBuildingCfg cfg = {}) : C(std::move(cfg)) {
        buildDefaultPalette_();
    }

    void setOrigin(float x, float y) {
        C.originX = x;
        C.originY = y;
    }
    void setScale(float s) { C.scale = std::max(0.001f, s); }
    void setStroke(bool on, float w = 1.5f, RGBA c = {0, 0, 0, 1}) {
        C.drawStroke = on;
        C.strokeWidth = w;
        C.strokeColor = c;
    }

    void setPolyColor(size_t idx, RGBA col) {
        if (idx >= palette_.size()) return;
        palette_[idx] = col;
    }


    void render() const {
        glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LINE_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        const float sx = C.scale * C.baseW;
        const float sy = C.scale * C.baseH;


        for (size_t i = 0; i < POLYS_NORM.size(); ++i) {
            const auto& polyN = POLYS_NORM[i];


            tmp_.clear();
            tmp_.reserve(polyN.size());
            for (auto& p : polyN) {
                tmp_.push_back(Vec2{
                    C.originX + p.x * sx,
                    C.originY + (1.0f - p.y) * sy
                });
            }


            tris_.clear();
            triangulate_(tmp_, tris_);


            const RGBA col = palette_[i];
            glColor4f(col.r, col.g, col.b, col.a);
            glBegin(GL_TRIANGLES);
            for (size_t k = 0; k < tris_.size(); k += 3) {
                const Vec2& a = tmp_[tris_[k + 0]];
                const Vec2& b = tmp_[tris_[k + 1]];
                const Vec2& c = tmp_[tris_[k + 2]];
                glVertex2f(a.x, a.y);
                glVertex2f(b.x, b.y);
                glVertex2f(c.x, c.y);
            }
            glEnd();


            if (C.drawStroke) {
                glLineWidth(std::max(1.f, C.strokeWidth));
                glColor4f(C.strokeColor.r, C.strokeColor.g, C.strokeColor.b, C.strokeColor.a);
                glBegin(GL_LINE_LOOP);
                for (auto& v : tmp_) glVertex2f(v.x, v.y);
                glEnd();
            }
            if (C.seamSealPx > 0.0f &&
                (C.seamSealList.empty() || std::find(C.seamSealList.begin(), C.seamSealList.end(),
                                                     (int)i) != C.seamSealList.end())) {
                glLineWidth(std::max(1.f, C.seamSealPx));
                glColor4f(col.r, col.g, col.b, 1.0f);
                glBegin(GL_LINE_LOOP);
                for (auto& v : tmp_) glVertex2f(v.x, v.y);
                glEnd();
            }
        }

        glPopAttrib();
    }

   private:
    CentralBuildingCfg C;
    mutable std::vector<Vec2> tmp_;
    mutable std::vector<int> tris_;


    static float cross(const Vec2& a, const Vec2& b, const Vec2& c) {
        return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    }
    static bool pointInTri(const Vec2& p, const Vec2& a, const Vec2& b, const Vec2& c) {
        float c1 = cross(a, b, p), c2 = cross(b, c, p), c3 = cross(c, a, p);
        bool hasNeg = (c1 < 0) || (c2 < 0) || (c3 < 0);
        bool hasPos = (c1 > 0) || (c2 > 0) || (c3 > 0);
        return !(hasNeg && hasPos);
    }
    static float signedArea(const std::vector<Vec2>& P) {
        double A = 0;
        for (size_t i = 0; i < P.size(); ++i) {
            const Vec2& p = P[i];
            const Vec2& q = P[(i + 1) % P.size()];
            A += p.x * q.y - p.y * q.x;
        }
        return float(0.5 * A);
    }
    static void triangulate_(const std::vector<Vec2>& P, std::vector<int>& out) {
        const int n = (int)P.size();
        if (n < 3) return;
        std::vector<int> V(n);
        if (signedArea(P) > 0) {
            for (int i = 0; i < n; ++i) V[i] = i;
        }  // CCW
        else {
            for (int i = 0; i < n; ++i) V[i] = n - 1 - i;
        }  // make CCW

        int nv = n;
        int count = 2 * nv;
        for (int m = 0, v = nv - 1; nv > 2;) {
            if ((count--) <= 0) break;
            int u = v;
            if (++v >= nv) v = 0;
            int w = v;
            if (++v >= nv) v = 0;

            const Vec2& A = P[V[u]];
            const Vec2& B = P[V[w]];
            const Vec2& Cc = P[V[v]];

            if (cross(A, B, Cc) <= 0) continue;

            bool ear = true;
            for (int i = 0; i < nv; ++i) {
                if (i == u || i == w || i == v) continue;
                if (pointInTri(P[V[i]], A, B, Cc)) {
                    ear = false;
                    break;
                }
            }
            if (!ear) continue;

            out.push_back(V[u]);
            out.push_back(V[w]);
            out.push_back(V[v]);
            for (int i = w; i < nv - 1; ++i) V[i] = V[i + 1];
            nv--;
            count = 2 * nv;
        }
    }


    std::vector<RGBA> palette_;
    void buildDefaultPalette_() {
        palette_.assign(POLYS_NORM.size(), RGBA{0.86f, 0.86f, 0.88f, 1.0f});

        auto mark = [&](std::initializer_list<size_t> idxs, RGBA c) {
            for (auto i : idxs)
                if (i < palette_.size()) palette_[i] = c;
        };
        mark({3, 4, 10}, RGBA{0.30f, 0.28f, 0.32f, 1.0f});  // notch/dark bands

        mark({13, 14}, RGBA{0.f, 0.f, 0.f, 0.15f});

        palette_[3] = RGBA{0.80f, 0.82f, 0.88f, 1.0f};
        palette_[6] = RGBA{0.80f, 0.82f, 0.88f, 1.0f};
        palette_[7] = RGBA{0.80f, 0.82f, 0.88f, 1.0f};
        palette_[8] = RGBA{0.80f, 0.82f, 0.88f, 1.0f};
        palette_[9] = RGBA{0.80f, 0.82f, 0.88f, 1.0f};
        palette_[10] = RGBA{0.80f, 0.82f, 0.88f, 1.0f};
        palette_[12] = RGBA{0.80f, 0.82f, 0.88f, 1.0f};
        palette_[13] = RGBA{0.80f, 0.82f, 0.88f, 1.0f};
    }



    static const std::vector<std::vector<Vec2>> POLYS_NORM;
};


const std::vector<std::vector<Vec2>> CentralBuilding::POLYS_NORM = {
    {{0.20f, 0.5f}, {0.4f, 0.3f}, {0.63f, 0.55f}, {0.63f, 0.85f}, {0.19f, 0.8f}},

    // poly_001
    {{0.19579621f, 0.51326831f}, {0.18797755f, 0.69993886f}, {0.19318999f, 0.70091619f},
     {0.19709932f, 0.75271483f}, {0.18797755f, 0.76053349f}, {0.18471977f, 0.86608543f},
     {0.24335974f, 0.86510809f}, {0.31046992f, 0.86413076f}, {0.39842987f, 0.86022143f},
     {0.40038453f, 0.63738956f}, {0.28114994f, 0.68918820f}, {0.23293486f, 0.74685083f},
     {0.21143354f, 0.75271483f}, {0.21338820f, 0.68234687f}, {0.21403976f, 0.67550554f},
     {0.33653213f, 0.60122825f}, {0.33653213f, 0.60122825f}, {0.40038453f, 0.50838164f},
     {0.40559697f, 0.26795779f}, {0.27072505f, 0.42433103f}, {0.26616417f, 0.50447231f},
     {0.25313307f, 0.51620030f}, {0.24140507f, 0.46537900f}, {0.19775088f, 0.51131364f}},
    // poly_002
    {{0.26861682f, 0.45254421f},
     {0.25689723f, 0.46426379f},
     {0.24624306f, 0.45840400f},
     {0.24837389f, 0.48104411f},
     {0.25210285f, 0.50395058f},
     {0.25476639f, 0.51007672f},
     {0.26186917f, 0.50421693f},
     {0.26435515f, 0.50261881f}},
    // poly_003
    {{0.40737455f, 0.56583401f},
     {0.40701697f, 0.60069816f},
     {0.47603011f, 0.63717143f},
     {0.47710285f, 0.60606188f}},
    // poly_004
    {{0.21782558f, 0.68403779f},
     {0.21568009f, 0.74196593f},
     {0.23177124f, 0.73874770f},
     {0.27968710f, 0.67974682f},
     {0.40162224f, 0.61967320f},
     {0.40197982f, 0.62771878f},
     {0.28004468f, 0.68725602f},
     {0.23212882f, 0.74518415f},
     {0.21353460f, 0.75054787f},
     {0.21246186f, 0.75162061f},
     {0.21460735f, 0.68189231f},
     {0.33654249f, 0.60358206f},
     {0.39947675f, 0.51668986f},
     {0.40233740f, 0.51347163f},
     {0.40233740f, 0.52634455f},
     {0.34548201f, 0.60680029f},
     {0.23999560f, 0.67223762f},
     {0.21711041f, 0.68511054f}},
    // poly_005
    {{0.35804205f, 0.62829430f}, {0.40454264f, 0.60168830f}, {0.40598080f, 0.56753194f}},
    // poly_006
    {{0.21919098f, 0.68629292f},
     {0.34433298f, 0.61602254f},
     {0.40376510f, 0.53425362f},
     {0.53189198f, 0.61481825f},
     {0.54121704f, 0.60784180f},
     {0.54319299f, 0.56990374f},
     {0.57085616f, 0.59361503f},
     {0.56489711f, 0.46801022f},
     {0.57401888f, 0.46410089f},
     {0.58835310f, 0.70647941f},
     {0.52374180f, 0.67451847f},
     {0.40136263f, 0.61535308f},
     {0.28106772f, 0.67652776f},
     {0.23140678f, 0.73716712f},
     {0.21678567f, 0.74126424f}},
    // poly_007
    {{0.40709642f, 0.26752570f},
     {0.40418444f, 0.50873425f},
     {0.53425263f, 0.60288809f},
     {0.53910593f, 0.59706414f},
     {0.54072369f, 0.42137502f},
     {0.40677286f, 0.26606971f}},
    // poly_008
    {{0.54371650f, 0.44416990f},
     {0.56412594f, 0.46771925f},
     {0.56922830f, 0.58428857f},
     {0.54332401f, 0.56456599f}},
    // poly_009
    {{0.53409864f, 0.68784514f},
     {0.57292496f, 0.70774364f},
     {0.58748484f, 0.71308226f},
     {0.59201457f, 0.73783404f},
     {0.59395589f, 0.74026069f},
     {0.59848563f, 0.74074601f},
     {0.60301536f, 0.74220200f},
     {0.63375287f, 0.74511398f},
     {0.63537064f, 0.81111873f}},
    // poly_010
    {{0.40357416f, 0.62048034f},
     {0.51743820f, 0.67383377f},
     {0.52070200f, 0.67619296f},
     {0.63782808f, 0.82033570f},
     {0.64721111f, 0.82276234f},
     {0.64979953f, 0.82130636f},
     {0.64979953f, 0.83004228f},
     {0.63976940f, 0.82664498f},
     {0.52070200f, 0.68201691f},
     {0.40487012f, 0.62863071f},
     {0.40325236f, 0.62717473f}},
    // poly_011
    {{0.40411733f, 0.51422300f},
     {0.53285522f, 0.60398438f},
     {0.53924772f, 0.60238626f},
     {0.53963854f, 0.60678455f},
     {0.53466659f, 0.61131257f},
     {0.40805954f, 0.53060723f},
     {0.40367182f, 0.52878633f}},
    // poly_012
    {{0.40392682f, 0.63502705f},
     {0.40217024f, 0.85855130f},
     {0.64755713f, 0.86196333f},
     {0.64649752f, 0.83136708f},
     {0.52093371f, 0.68474351f}},
    // poly_013
    {{0.57648412f, 0.46394750f},
     {0.59259859f, 0.71147632f},
     {0.59436479f, 0.72266226f},
     {0.59946715f, 0.73590878f},
     {0.60437327f, 0.73767498f},
     {0.64735085f, 0.74150175f},
     {0.64735085f, 0.73796934f},
     {0.64538840f, 0.73561441f},
     {0.64128748f, 0.53894292f}},
    // poly_014
    {{0.20575763f, 0.86709160f},
     {0.20720465f, 0.88287720f},
     {0.40057024f, 0.88871100f},
     {0.39898082f, 0.86407506f}},
    // poly_015
    {{0.62437589f, 0.86535594f},
     {0.62411099f, 0.88363421f},
     {0.40556637f, 0.88959452f},
     {0.40477167f, 0.86336917f}},
    {{0.25435515f, 0.49261881f}, {0.19f, 0.70f}, {0.33, 0.62}, {0.4051f, 0.51f}, {0.406f, 0.27f}},
    {{0.19, 0.77f}, {0.32f, 0.89f}, {0.40f, 0.63f}},
};
