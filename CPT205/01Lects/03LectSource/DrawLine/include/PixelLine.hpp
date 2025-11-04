#include <GL/freeglut.h>
#include <GL/glut.h>

#include <cmath>
#include <set>
#include <unordered_set>
#include <utility>

struct Color4f {
    Color4f(double r, double g, double b, double a) : _r(r), _g(g), _b(b), _a(a) {}
    double _r;
    double _g;
    double _b;
    double _a;

    bool operator==(Color4f const& other) const {
        return _r == other._r && _g == other._g && _b == other._b && _a == other._a;
    }
};

struct Pixel2D {
    int x;
    int y;
    Color4f color;
    Pixel2D(int x, int y, Color4f color) : x(x), y(y), color(color) {}

    bool operator==(Pixel2D const& other) const {
        return x == other.x && y == other.y && color == other.color;
    }
};

// Provide a hash function for Pixel2D
struct Pixel2DHash {
    std::size_t operator()(const Pixel2D& p) const {
        return std::hash<int>()(p.x) ^ std::hash<int>()(p.y);  // Add color if needed
    }
};

using PixelSet = std::unordered_set<Pixel2D, Pixel2DHash>;

void renderPixelSet(PixelSet& pixel_set) {
    glBegin(GL_QUADS);
    for (auto& ele : pixel_set) {
        glColor4d(ele.color._r, ele.color._g, ele.color._b, ele.color._a);
        // 以整数为像素“边界”
        glVertex2i(ele.x, ele.y);
        glVertex2i(ele.x + 1, ele.y);
        glVertex2i(ele.x + 1, ele.y + 1);
        glVertex2i(ele.x, ele.y + 1);
    }
    glEnd();
}

void renderSinglePixel(int x, int y, Color4f color) {
    glBegin(GL_QUADS);
    glColor4d(color._r, color._g, color._b, color._a);
    // 以整数为像素“边界”
    glVertex2i(x, y);
    glVertex2i(x + 1, y);
    glVertex2i(x + 1, y + 1);
    glVertex2i(x, y + 1);
    glEnd();
}

struct Line2D {
    Line2D(std::pair<double, double> p1, std::pair<double, double> p2, Color4f color)
        : _p1(p1), _p2(p2), _color(color) {}

    std::pair<double, double> _p1;  // the start point of the line
    std::pair<double, double> _p2;  // the end point of the line
    Color4f _color;                 // the color of the line

    void render() {
        glBegin(GL_LINES);
        glColor4d(_color._r, _color._g, _color._b, _color._a);
        glVertex2d(_p1.first, _p1.second);
        glVertex2d(_p2.first, _p2.second);
        glEnd();
    }

    [[nodiscard]] static inline int ifloor(double v) { return (int)std::floor(v); }
    [[nodiscard]] static inline double frac(double v) { return v - std::floor(v); }
    void calcPixelDDA(PixelSet& pixel_set) {
        double x0 = _p1.first, y0 = _p1.second;
        double x1 = _p2.first, y1 = _p2.second;

        int ix = ifloor(x0), iy = ifloor(y0);
        int ix1 = ifloor(x1), iy1 = ifloor(y1);

        pixel_set.insert(Pixel2D(ix, iy, _color));

        double dx = x1 - x0, dy = y1 - y0;

        int stepX, stepY;
        stepX = (dx > 0) ? 1 : ((dx < 0) ? -1 : 0);
        stepY = (dy > 0) ? 1 : ((dy < 0) ? -1 : 0);

        // shortcut: if the start and end are the same (no move),
        // return
        if (stepX == 0 && stepY == 0) return;

        // if the stepX is 0, then it is totally vertical line
        if (stepX == 0) {
            int end = iy1;
            int s = (stepY > 0) ? 1 : -1;
            while (iy != end) {
                iy += s;
                pixel_set.insert(Pixel2D(ix, iy, _color));
            }
            return;
        }

        // if the stepY is 0, then it is totally horizontal line
        if (stepY == 0) {
            int end = ix1;
            int s = (stepX > 0) ? 1 : -1;
            while (ix != end) {
                ix += s;
                pixel_set.insert(Pixel2D(ix, iy, _color));
            }
            return;
        }

        double invAbsDx = 1.0 / std::abs(dx);
        double invAbsDy = 1.0 / std::abs(dy);

        double tMaxX = (stepX > 0) ? (1.0 - frac(x0)) * invAbsDx : frac(x0) * invAbsDx;
        double tMaxY = stepY > 0 ? (1.0 - frac(y0)) * invAbsDy : frac(y0) * invAbsDy;

        double tDeltaX = invAbsDx;
        double tDeltaY = invAbsDy;

        while (ix != ix1 || iy != iy1) {
            if (tMaxX < tMaxY) {
                ix += stepX;
                tMaxX += tDeltaX;
            } else if (tMaxY < tMaxX) {
                iy += stepY;
                tMaxY += tDeltaY;
            } else {
                // tMaxX == tMaxY：穿过一个“格点”（角点）
                // 如果你想做
                // supercover（不漏缝），可以把两个都加；
                // 否则选一种稳定顺序也行。
                ix += stepX;
                iy += stepY;
                tMaxX += tDeltaX;
                tMaxY += tDeltaY;
            }
            pixel_set.insert(Pixel2D(ix, iy, _color));
        }
    }

    void renderShitDDA() {
        double dx = _p2.first - _p1.first, dy = _p2.second - _p1.second;
        double steps = std::max(std::abs(dx), std::abs(dy));
        double x0 = _p1.first, y0 = _p1.second;

        for (int i = 0; i < steps; i++) {
            renderSinglePixel((int)std::lround(x0), (int)std::lround(y0), _color);
            x0 += dx / steps;
            y0 += dy / steps;
        }
    }
    void renderBresenham(double x0, double y0, double x1, double y1, Color4f color) {
        double dx = std::abs(x1 - x0), dy = std::abs(y1 - y0);
        int sign_x = x0 < x1 ? 1 : -1;
        int sign_y = y0 < y1 ? 1 : -1;
        int err = dx - dy;  // dy(x - x0) = dx(y - y0)

        for (;;) {
        }
    }

    void renderBresenham(int x0, int y0, int x1, int y1) {
        int del_x = std::abs(x1 - x0), del_y = std::abs(y1 - y0);

        // according the y = kx + b
        // k = del_y / del_x
        // y = del_y / del_x * x + b
        // del_x * y = del_y * x + b * del_x
        // and
        // x + 1 => y + del_y / del_x
        // y + 1 => x + del_x / del_y
    }
};
