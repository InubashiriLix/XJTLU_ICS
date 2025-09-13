#include <GL/freeglut.h>
#include <GL/glut.h>

#include "Polygon.hpp"
#include "Quad.hpp"
#include "Text.hpp"
#include "Triangle.hpp"
#include "basic/Color.hpp"

int gWinW = 400;
int gWinH = 400;

// 你的三角形：逻辑单位（0..10）
Triangle2D test(std::make_tuple(Pos2D(0, 0), Pos2D(0, 40), Pos2D(30, 0)), RGBA(1, 0, 0, 1));
// fucking shit
Quad2D quad(std::make_tuple(Pos2D(50, 50), Pos2D(50, 150), Pos2D(150, 150), Pos2D(150, 50)),
            RGBA(0, 1, 0, 1));

Polygon2D poly(std::vector<Pos2D>{Pos2D(200, 200), Pos2D(250, 250), Pos2D(300, 200),
                                  Pos2D(275, 150), Pos2D(225, 150)},
               RGBA(0, 0, 1, 1));
Text2D text(Pos2D(300, 300), GLUT_BITMAP_HELVETICA_18, "Hello, Text!", size2D(1, 18),
            RGBA(1, 1, 0, 1), false);

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // 画三角形
    test.render();
    quad.render();
    poly.render();
    text.render();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    gWinW = w;
    gWinH = h;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // —— 关键改动：用“逻辑坐标”投影 ——
    // 左上为 (0,0)，右下为 (10,10)
    glOrtho(0, 400, 400, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(0.08f, 0.09f, 0.10f, 1.0f);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(gWinW, gWinH);
    glutCreateWindow("triangle");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    // 如果需要连续刷新：glutIdleFunc([](){ glutPostRedisplay(); });

    glutMainLoop();
    return 0;
}
