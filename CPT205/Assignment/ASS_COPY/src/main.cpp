#include <GL/freeglut.h>

#include <climits>
#include <cmath>
#include <ctime>
#include <functional>
#include <string>
#include <utility>
#include <vector>

struct Color4f {
    float r;
    float g;
    float b;
    float a;

    Color4f() = default;

    Color4f(float red, float green, float blue, float alpha = 1.0f)
        : r(red), g(green), b(blue), a(alpha) {}

    Color4f(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255)
        : r(static_cast<float>(r) / 255.0f),
          g(static_cast<float>(g) / 255.0f),
          b(static_cast<float>(b) / 255.0f),
          a(static_cast<float>(a) / 255.0f) {}

    void render() { return glColor4f(r, g, b, a); }

    template <typename T>
    static T clamp(T value, T min, T max) {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }

    Color4f operator*(float scalar) const {
        return Color4f(clamp(r * scalar, 0.0f, 1.0f), clamp(g * scalar, 0.0f, 1.0f),
                       clamp(b * scalar, 0.0f, 1.0f), a);
    }

    Color4f operator+(const Color4f& other) const {
        return Color4f(r + other.r, g + other.g, b + other.b, a + other.a);
    }

    Color4f operator-(const Color4f& other) const {
        return Color4f(r - other.r, g - other.g, b - other.b, a - other.a);
    }
};

namespace COLORS {
const Color4f RED{1.0f, 0.0f, 0.0f, 1.0f};
const Color4f GREEN{0.0f, 1.0f, 0.0f, 1.0f};
const Color4f BLUE{0.0f, 0.0f, 1.0f, 1.0f};
const Color4f YELLOW{1.0f, 1.0f, 0.0f, 1.0f};
const Color4f CYAN{0.0f, 1.0f, 1.0f, 1.0f};
const Color4f MAGENTA{1.0f, 0.0f, 1.0f, 1.0f};
const Color4f WHITE{1.0f, 1.0f, 1.0f, 1.0f};
const Color4f BLACK{0.0f, 0.0f, 0.0f, 1.0f};
const Color4f BROWN{0.6f, 0.4f, 0.2f, 1.0f};
}  // namespace COLORS

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
                tmp_.push_back(Vec2{C.originX + p.x * sx, C.originY + (1.0f - p.y) * sy});
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

            float ub = (float)std::rand() / RAND_MAX;  // 0..1
            float bend_span = 25.f;
            float base_bend = (ub * 2.f - 1.f) * bend_span;  // -25..25
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

struct GrassFieldCfg {};

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

enum class SCENE_MODE {
    BG_ONLY,
    CONGETS,
};

static SCENE_MODE gSceneMode = SCENE_MODE::BG_ONLY;

static int gW = 1100, gH = 800;
static Sky* gSky = nullptr;
static Sun* gSun = nullptr;
static GrassField* gField = nullptr;
static CentralBuilding* gCB = nullptr;
static Paper* gPaper = nullptr;
static TextLine* gTextBar = nullptr;
static PaperWithText* gPaperText = nullptr;

// the decorations
static std::function<void(int, int, int, int, Balloon*)> g_reset_b_on_ex_up_cb =
    [](int x_lb, int y_lb, int w, int h, Balloon* b) {
        if (y_lb > gH) b->resetStatus();
    };
static std::function<void(int, int, int, int, Balloon*)> g_empty_b_cb;
static std::function<void(int, int, int, int, Balloon*)> g_suiside_b_cb =
    [](int x_lb, int y_lb, int w, int g, Balloon* b) {
        if (y_lb > gH) {
            b->killyourself();
        }
    };

static std::vector<Balloon> gRandomBalloonsVec;

static std::vector<Tree> gForest;

static bool gPause = false;
static float gTime = 0.f;

static void reshape(int w, int h) {
    // Force window size to stay at gW x gH
    if (w != gW || h != gH) {
        glutReshapeWindow(gW, gH);
        return;
    }

    glViewport(0, 0, gW, gH);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, gW, 0.0, gH, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if (gSky) gSky->setViewport((float)gW, (float)gH);
}
static void keyboard(unsigned char key, int, int) {
    switch (key) {
        case 'q': {
            exit(0);
        }
    }
}

static void mouse(int button, int state, int x, int y) {
    int flipped_y = gH - y;

    if ((button == GLUT_LEFT_BUTTON || button == GLUT_RIGHT_BUTTON) && state == GLUT_DOWN) {
        if (gSceneMode == SCENE_MODE::BG_ONLY) {
            if (gPaper && gPaper->containsPoint(x, flipped_y)) {
                gSceneMode = SCENE_MODE::CONGETS;
            } else {
                gRandomBalloonsVec.emplace_back(BalloonCfg::makeRandomOneWayVertical(gW, gH));
            }
        } else {
            if (gPaperText && !gPaperText->containsPoint(x, flipped_y)) {
                gSceneMode = SCENE_MODE::BG_ONLY;
            }
        }
    }
}

static void drawGround() {
    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT);
    glColor4f(0.82f, 0.95f, 0.78f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(gW, 0);
    glVertex2f(gW, 140);
    glVertex2f(0, 140);
    glEnd();
    glPopAttrib();
}

static void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (gSky) {
        if (gPause) gSky->setTime(gTime);
        gSky->render();
        gSun->render();
    }

    drawGround();
    if (gCB) gCB->render();
    for (auto& t : gForest) t.render();
    if (gField) gField->render();

    gPaper->render();

    if (!gRandomBalloonsVec.empty()) {
        for (auto& b : gRandomBalloonsVec) {
            b.render(g_suiside_b_cb);
        }
        // it will remove the dead balloons (exceeded the screen)
        gRandomBalloonsVec.erase(
            std::remove_if(gRandomBalloonsVec.begin(), gRandomBalloonsVec.end(),
                           [](const Balloon& b) { return !b.isAlive(); }),
            gRandomBalloonsVec.end());
    }

    gTextBar->render();

    if (gSceneMode == SCENE_MODE::CONGETS && gPaperText) gPaperText->render();

    glutSwapBuffers();
}

static void timer_cb(int) {
    if (!gPause) gTime += 0.016f;  // ~60 fps
    glutPostRedisplay();
    glutTimerFunc(16, timer_cb, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
    glutInitWindowSize(gW, gH);
    glutCreateWindow("Happy 20th Anniversary, XJTLU");

    // the SUN
    SunCfg sun_cfg = {.center = {70.f, 700.f},
                      .radius = 60.f,
                      .rays = 20,
                      .ray_length = 55.f,
                      .pulse_amp = 18.f,
                      .pulse_speed = 2.2f,
                      .rotate_deg_per_sec = 25.f,
                      .line_thickness = 4.f,
                      .core_color = Color4f(1.0f, 0.9f, 0.35f, 1.0f),
                      .ray_color = Color4f(1.0f, 0.80f, 0.15f, 1.0f)};
    gSun = new Sun(sun_cfg);

    // initialize the sky
    SkyCfg sky_cfg = {.width = (float)gW,
                      .height = (float)gH,

                      .bottom_color = Color4f(0.90f, 0.97f, 1.00f, 1.0f),
                      .top_color = Color4f(0.70f, 0.85f, 1.00f, 1.0f),

                      .layers = 3,
                      .clouds_per_layer = 5,
                      .wind_to_right = true,
                      .speed_min = 12.f,
                      .speed_max = 36.f,
                      .scale_min = 0.8f,
                      .scale_max = 1.6f,
                      .cloud_color = Color4f(1.0f, 1.0f, 1.0f, 0.93f),

                      .motive = true,
                      .speed_scale = 1,

                      .seed = -1};
    gSky = new Sky(sky_cfg);

    CentralBuildingCfg gCbCfg;
    gCbCfg.baseW = 1200;
    gCbCfg.baseH = 800;
    gCbCfg.originX = 0;
    gCbCfg.originY = 0;
    gCbCfg.scale = 1.0f;
    gCbCfg.seamSealPx = 3.0f;
    gCbCfg.seamSealList = {1, 5, 6, 7, 10, 11, 12};
    gCB = new CentralBuilding(gCbCfg);

    gField = new GrassField();
    GrassPatchCfg gGfCfg1;
    gGfCfg1.motive = true;
    gGfCfg1.speed = 2;
    gGfCfg1.color = COLORS::GREEN;
    gGfCfg1.win_dir_right = true;
    gField->addPatch(gGfCfg1)
        .setOrigin(100.f, 80.f)
        .setSize(420.f, 100.f)
        .setBladeCount(400)
        .setSegments(10)
        .setWindAmplitude(14.f);

    GrassPatchCfg gGfCfg2;
    gGfCfg2.motive = true;
    gGfCfg2.speed = 1;
    gGfCfg2.color = Color4f(0.50f, 0.75f, 0.30f, 1.0f);
    gGfCfg2.win_dir_right = false;
    gField->addPatch(gGfCfg2)
        .setOrigin(560.f, 70.f)
        .setSize(360.f, 160.f)
        .setBladeCount(480)
        .setSegments(9)
        .setWindAmplitude(10.f);

    GrassPatchCfg gGfCfg3;
    gGfCfg3.motive = true;
    gGfCfg3.speed = 1;
    gGfCfg3.color = Color4f(0.50f, 0.85f, 0.30f, 1.0f);
    gGfCfg3.win_dir_right = false;
    gField->addPatch(gGfCfg3)
        .setOrigin(20.f, 30.f)
        .setSize(360.f, 120.f)
        .setBladeCount(480)
        .setSegments(9)
        .setWindAmplitude(10.f);

    GrassPatchCfg gGfCfg4;
    gGfCfg4.motive = true;
    gGfCfg4.speed = 1;
    gGfCfg4.color = Color4f(0.50f, 0.75f, 0.30f, 1.0f);
    gGfCfg4.win_dir_right = false;
    gField->addPatch(gGfCfg4)
        .setOrigin(300.f, 20.f)
        .setSize(360.f, 160.f)
        .setBladeCount(480)
        .setSegments(4)
        .setWindAmplitude(10.f);

    GrassPatchCfg gGfCfg5;
    gGfCfg5.motive = true;
    gGfCfg5.speed = 1;
    gGfCfg5.color = Color4f(0.50f, 0.75f, 0.30f, 1.0f);
    gGfCfg5.win_dir_right = false;
    gField->addPatch(gGfCfg5)
        .setOrigin(700.f, 20.f)
        .setSize(360.f, 160.f)
        .setBladeCount(480)
        .setSegments(4)
        .setWindAmplitude(10.f);

    for (int i = 0; i < 10; i++) {
        gRandomBalloonsVec.emplace_back(BalloonCfg::makeRandomOneWayVertical(gW, gH));
    }

    gForest.emplace_back(TREE_CFG_PRELOADS::SPRING).setPosition(220, 110);
    gForest.emplace_back(TREE_CFG_PRELOADS::AUTUMN).setPosition(520, 110);
    gForest.emplace_back(TREE_CFG_PRELOADS::AUTUMN).setPosition(720, 110);
    gForest.emplace_back(TREE_CFG_PRELOADS::AUTUMN).setPosition(920, 110);
    gForest.emplace_back(TREE_CFG_PRELOADS::AUTUMN).setPosition(850, 110);
    gForest.emplace_back(TREE_CFG_PRELOADS::KANADE).setPosition(100, 110);

    PaperCfg gPCfg;
    gPCfg.cx = 500;
    gPCfg.cy = 80;
    gPCfg.width = 520.f;
    gPCfg.height = 360.f;
    gPCfg.rotDeg = -15.f;
    gPCfg.persp = 0.16f;
    gPCfg.curl = 34.f;
    gPCfg.drawEdge = true;
    gPCfg.drawShadow = true;
    gPaper = new Paper(gPCfg);
    gPaper->setSize(80.f, 80.f);

    gTextBar = new TextLine("Congratulations XJTLU 20th Anniversary", gW / 2, 500);

    PaperWithTextCfg gPTCfg;
    gPaperText = new PaperWithText(gPTCfg);

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutTimerFunc(16, timer_cb, 0);
    glClearColor(sky_cfg.bottom_color.r, sky_cfg.bottom_color.g, sky_cfg.bottom_color.b, 1.0f);
    glutMainLoop();
    return 0;
}
