#include <cmath>
#include <numbers>

#include "PixelLine.hpp"

class Circle2D {
   public:
    Circle2D(double x, double y, double r) : _x(x), _y(y), _r(r) {}

    void renderSinglePixel(int x, int y, Color4f color) {
        glBegin(GL_QUADS);
        glColor4d(color._r, color._g, color._b, color._a);
        // 以整数为像素“边界”
        glVertex2i(x, y);
        glVertex2i(x + 1, y);
        glVertex2i(x + 1, y + 1);
        glVertex2i(x, y + 1);

        // for (;;) {
        // }

        glEnd();
    }

    void drawCircle(float cx, float cy, float r, int num_segments) {
        glBegin(GL_LINE_LOOP);  // Use GL_POLYGON for filled circle
        for (int i = 0; i < num_segments; ++i) {
            float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);
            float x = r * cosf(theta);
            float y = r * sinf(theta);
            glVertex2f(cx + x, cy + y);
        }
        glEnd();
    }

    void drawCicle() { glBegin(GL_LINE_LOOP); }

   private:
    double _x, _y, _r;
};
