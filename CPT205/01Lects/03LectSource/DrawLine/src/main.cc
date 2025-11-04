#include <GL/freeglut.h>
// #include <GL/glut.h>  // WARNING:  ❌ 删掉，避免冲突

#include <PixelLine.hpp>
#include <iostream>

int W = 300, H = 300;

PixelSet pixel_set;

// 例子：从 (0,0) 到 (100,100)
Line2D line({0, 0}, {200, 100}, Color4f(1.0, 1.0, 1.0, 1.0));  // alpha 改成 1.0

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    // renderPixelSet(pixel_set);
    line.renderShitDDA();
    glutSwapBuffers();  // ✅ 双缓冲
}

void reshape(int w, int h) {
    W = w;
    H = h;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // 整数坐标当像素边界：左下(0,0) 右上(W,H)
    glOrtho(0.0, (double)w, 0.0, (double)h, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv) {
    std::cout << "Hello Here\n";

    // 1) 初始化 GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);  // ✅ 双缓冲 + RGBA
    glutInitWindowSize(W, H);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Lines");

    // 2) OpenGL 状态
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_MULTISAMPLE);
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_POINT_SMOOTH);
    glClearColor(0.1f, 0.1f, 0.1f, 1.f);

    // 3) 计算一次像素集合（逻辑坐标直接用像素坐标系）
    line.calcPixelDDA(pixel_set);

    // 4) 回调
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);

    glutMainLoop();
    return 0;
}
