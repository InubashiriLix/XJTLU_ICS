#include <GL/freeglut.h>
#include <cmath>
#include <cstdlib>

const float PI = 3.14159265f;

// ---------------- Camera ----------------
float camX = 0.0f;
float camY = 35.0f;
float camZ = 120.0f;
float yaw = 0.0f;     // 左右转
float pitch = -15.0f; // 上下俯仰

const float MOVE_SPEED = 2.5f;
const float ROT_SPEED = 2.0f;

// ---------------- Spinner 类 ----------------
class Spinner {
public:
  float x, y, z;
  float yawDeg;     // 车头朝向
  float hoverPhase; // 悬停相位
  float sirenPhase; // 警灯相位

  Spinner()
      : x(0.0f), y(6.0f), z(0.0f), yawDeg(0.0f), hoverPhase(0.0f),
        sirenPhase(0.0f) {}

  void update(float dt) {
    hoverPhase += dt * 2.0f;
    sirenPhase += dt * 6.0f;
    if (hoverPhase > 2.0f * PI)
      hoverPhase -= 2.0f * PI;
    if (sirenPhase > 2.0f * PI)
      sirenPhase -= 2.0f * PI;
  }

  void draw() {
    glPushMatrix();

    // 悬停轻微上下起伏
    float hoverOffset = std::sin(hoverPhase) * 0.6f;
    glTranslatef(x, y + hoverOffset, z);
    glRotatef(yawDeg, 0.0f, 1.0f, 0.0f);

    drawBody();
    drawCabin();
    drawSideEngines();
    drawRearEngine();
    drawBottomThrusters();
    drawLightBar();
    drawDetails();

    glPopMatrix();
  }

private:
  void drawBody() {
    // 主车身底盘：厚重装甲块
    float hullLen = 30.0f; // Z 向
    float hullW = 14.0f;   // X 向
    float hullH = 4.0f;    // Y 向

    // 底盘
    glPushMatrix();
    glTranslatef(0.0f, hullH * 0.5f, 0.0f);
    glScalef(hullW, hullH, hullLen);
    glColor3f(0.15f, 0.16f, 0.18f);
    glutSolidCube(1.0);
    glPopMatrix();

    // 楔形车头（在前半部分，用倾斜方块模拟）
    glPushMatrix();
    glTranslatef(0.0f, hullH * 0.9f, hullLen * 0.20f);
    glRotatef(-18.0f, 1.0f, 0.0f, 0.0f); // 朝下倾斜
    glScalef(hullW * 0.9f, hullH * 1.1f, hullLen * 0.7f);
    glColor3f(0.19f, 0.20f, 0.22f);
    glutSolidCube(1.0);
    glPopMatrix();

    // 车身侧裙装甲
    glPushMatrix();
    glTranslatef(0.0f, hullH * 0.4f, 0.0f);
    glScalef(hullW * 1.1f, hullH * 0.5f, hullLen * 0.9f);
    glColor3f(0.12f, 0.13f, 0.15f);
    glutSolidCube(1.0);
    glPopMatrix();

    // 车头前下方防撞块
    glPushMatrix();
    glTranslatef(0.0f, hullH * 0.25f, hullLen * 0.55f);
    glScalef(hullW * 0.7f, hullH * 0.4f, hullLen * 0.15f);
    glColor3f(0.10f, 0.11f, 0.13f);
    glutSolidCube(1.0);
    glPopMatrix();
  }

  void drawCabin() {
    // 驾驶舱舱盖 + 侧窗
    float hullLen = 30.0f;
    float hullH = 4.0f;

    // 舱盖主体（稍微透一点的深蓝/灰）
    glPushMatrix();
    glTranslatef(0.0f, hullH + 2.0f, -hullLen * 0.05f);
    glScalef(10.0f, 3.0f, 12.0f);
    glColor3f(0.12f, 0.18f, 0.22f);
    glutSolidCube(1.0);
    glPopMatrix();

    // 舱盖顶部圆滑一点，用球压扁
    glPushMatrix();
    glTranslatef(0.0f, hullH + 3.0f, -hullLen * 0.05f);
    glScalef(6.5f, 2.5f, 8.0f);
    glColor3f(0.10f, 0.15f, 0.20f);
    glutSolidSphere(1.0, 22, 16);
    glPopMatrix();

    // 侧窗区域线条（用线表示剪刀门轮廓）
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(0.0f, hullH + 2.0f, -hullLen * 0.05f);
    glColor3f(0.35f, 0.45f, 0.90f);
    glLineWidth(2.0f);

    float w = 5.0f;
    float h = 2.0f;
    float z = 5.0f;

    glBegin(GL_LINE_LOOP); // 左侧
    glVertex3f(-w, h, z);
    glVertex3f(-w, -h, z);
    glVertex3f(-w, -h, -z);
    glVertex3f(-w, h, -z * 0.2f);
    glEnd();

    glBegin(GL_LINE_LOOP); // 右侧
    glVertex3f(w, h, z);
    glVertex3f(w, -h, z);
    glVertex3f(w, -h, -z);
    glVertex3f(w, h, -z * 0.2f);
    glEnd();

    glPopMatrix();
    glEnable(GL_LIGHTING);
  }

  void drawSideEnginePod(float sideSign) {
    // 侧面发动机舱（sideSign = +1 左 / -1 右）
    float hullLen = 30.0f;
    float hullW = 14.0f;

    glPushMatrix();
    glTranslatef(sideSign * (hullW * 0.7f), 4.5f, -hullLen * 0.05f);

    // 舱体
    glPushMatrix();
    glScalef(4.0f, 4.0f, 14.0f);
    glColor3f(0.14f, 0.15f, 0.17f);
    glutSolidCube(1.0);
    glPopMatrix();

    // 前端进气圈
    glDisable(GL_LIGHTING);
    glColor3f(0.40f, 0.42f, 0.45f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    int seg = 24;
    float r = 2.2f;
    for (int i = 0; i < seg; ++i) {
      float ang = 2.0f * PI * i / seg;
      float x = 0.0f;
      float y = r * std::cos(ang);
      float z = r * std::sin(ang);
      glVertex3f(x - sideSign * 0.2f, y, z + 7.0f);
    }
    glEnd();
    glEnable(GL_LIGHTING);

    // 后端喷口 + 发光
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -7.3f);
    glColor3f(0.10f, 0.11f, 0.13f);
    glutSolidSphere(1.3f, 16, 12);
    glPopMatrix();

    // 发光圈
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -7.4f);
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(0.2f, 0.9f, 1.0f, 0.85f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    for (int i = 0; i <= seg; ++i) {
      float ang = 2.0f * PI * i / seg;
      float y = 1.8f * std::cos(ang);
      float z = 1.8f * std::sin(ang);
      glVertex3f(0.0f, y, z);
    }
    glEnd();
    glPopMatrix();
    glEnable(GL_LIGHTING);

    glPopMatrix();
  }

  void drawSideEngines() {
    drawSideEnginePod(+1.0f);
    drawSideEnginePod(-1.0f);
  }

  void drawRearEngine() {
    // 尾部主喷口
    float hullLen = 30.0f;

    glPushMatrix();
    glTranslatef(0.0f, 3.5f, -hullLen * 0.6f);

    glPushMatrix();
    glScalef(8.0f, 4.0f, 4.0f);
    glColor3f(0.16f, 0.17f, 0.19f);
    glutSolidCube(1.0);
    glPopMatrix();

    // 中央喷口
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, -2.4f);
    glColor3f(0.10f, 0.11f, 0.13f);
    glutSolidSphere(1.6f, 18, 14);
    glPopMatrix();

    // 蓝色尾焰
    int seg = 28;
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, -2.8f);
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(0.25f, 0.8f, 1.0f, 0.85f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    for (int i = 0; i <= seg; ++i) {
      float ang = 2.0f * PI * i / seg;
      float x = 1.6f * std::cos(ang);
      float y = 1.6f * std::sin(ang);
      glVertex3f(x, y, -2.0f);
    }
    glEnd();
    glPopMatrix();
    glEnable(GL_LIGHTING);

    glPopMatrix();
  }

  void drawBottomThrusters() {
    // 底部反重力喷口（四个）
    float hullLen = 30.0f;
    float hullW = 14.0f;

    float positions[4][3] = {
        {hullW * 0.35f, 1.0f, hullLen * 0.20f},
        {-hullW * 0.35f, 1.0f, hullLen * 0.20f},
        {hullW * 0.35f, 1.0f, -hullLen * 0.15f},
        {-hullW * 0.35f, 1.0f, -hullLen * 0.15f},
    };

    float glow = (std::sin(hoverPhase * 1.8f) + 1.0f) * 0.5f; // 0~1

    for (int i = 0; i < 4; ++i) {
      glPushMatrix();
      glTranslatef(positions[i][0], positions[i][1], positions[i][2]);

      // 喷口本体
      glColor3f(0.12f, 0.12f, 0.14f);
      glutSolidSphere(0.7f, 10, 8);

      // 向下的发光圈
      glDisable(GL_LIGHTING);
      glBegin(GL_TRIANGLE_FAN);
      glColor4f(0.3f * (0.5f + 0.5f * glow), 0.9f * (0.7f + 0.3f * glow), 1.0f,
                0.8f);
      glVertex3f(0.0f, -0.1f, 0.0f);
      int seg = 16;
      float r = 1.0f;
      for (int j = 0; j <= seg; ++j) {
        float ang = 2.0f * PI * j / seg;
        float x = r * std::cos(ang);
        float z = r * std::sin(ang);
        glVertex3f(x, -1.8f, z);
      }
      glEnd();
      glEnable(GL_LIGHTING);

      glPopMatrix();
    }
  }

  void drawLightBar() {
    // 车顶警灯模块
    float hullLen = 30.0f;
    float hullH = 4.0f;

    glPushMatrix();
    glTranslatef(0.0f, hullH + 4.5f, -hullLen * 0.25f);

    // 灯条底座
    glPushMatrix();
    glScalef(6.0f, 1.0f, 2.0f);
    glColor3f(0.12f, 0.12f, 0.14f);
    glutSolidCube(1.0);
    glPopMatrix();

    // 左右两块灯罩
    float blink = std::sin(sirenPhase);

    // 左：红
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(-1.6f, 0.6f, 0.0f);
    glScalef(2.4f, 1.0f, 1.4f);
    if (blink > 0.0f)
      glColor4f(1.0f, 0.2f, 0.2f, 0.9f);
    else
      glColor4f(0.4f, 0.1f, 0.1f, 0.4f);
    glutSolidCube(1.0);
    glPopMatrix();

    // 右：蓝
    glPushMatrix();
    glTranslatef(1.6f, 0.6f, 0.0f);
    glScalef(2.4f, 1.0f, 1.4f);
    if (blink <= 0.0f)
      glColor4f(0.2f, 0.5f, 1.0f, 0.9f);
    else
      glColor4f(0.1f, 0.2f, 0.4f, 0.4f);
    glutSolidCube(1.0);
    glPopMatrix();
    glEnable(GL_LIGHTING);

    glPopMatrix();
  }

  void drawDetails() {
    // 车身顶部编号块
    float hullLen = 30.0f;
    float hullH = 4.0f;

    glPushMatrix();
    glTranslatef(0.0f, hullH + 3.0f, -hullLen * 0.4f);
    glScalef(8.0f, 1.2f, 4.0f);
    glColor3f(0.18f, 0.18f, 0.19f);
    glutSolidCube(1.0);
    glPopMatrix();

    // 左侧文字已移除
  }
};

// 全局 Spinner 实例
Spinner g_spinner;

// ---------------- 环境：地面 + 简单城市块 ----------------
void drawGround() {
  glDisable(GL_LIGHTING);
  glBegin(GL_QUADS);
  glColor3f(0.05f, 0.06f, 0.08f);
  float size = 400.0f;
  glVertex3f(-size, 0.0f, -size);
  glVertex3f(size, 0.0f, -size);
  glVertex3f(size, 0.0f, size);
  glVertex3f(-size, 0.0f, size);
  glEnd();

  // 简单路面线条
  glLineWidth(1.5f);
  glColor3f(0.4f, 0.4f, 0.4f);
  glBegin(GL_LINES);
  for (int i = -3; i <= 3; ++i) {
    glVertex3f(i * 20.0f, 0.01f, -120.0f);
    glVertex3f(i * 20.0f, 0.01f, 120.0f);
  }
  glEnd();

  glEnable(GL_LIGHTING);
}

// ---------------- Camera transform ----------------
void applyCamera() {
  glRotatef(-pitch, 1.0f, 0.0f, 0.0f);
  glRotatef(-yaw, 0.0f, 1.0f, 0.0f);
  glTranslatef(-camX, -camY, -camZ);
}

// ---------------- GLUT 回调 ----------------
void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  applyCamera();

  // 灯光：偏冷、但强度高一点
  GLfloat sunDir[] = {-0.4f, 0.8f, -0.3f, 0.0f};
  GLfloat sunDiffuse[] = {0.9f, 0.95f, 1.0f, 1.0f};
  GLfloat sunAmbient[] = {0.25f, 0.27f, 0.30f, 1.0f};
  GLfloat sunSpecular[] = {0.6f, 0.6f, 0.6f, 1.0f};
  glLightfv(GL_LIGHT0, GL_POSITION, sunDir);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, sunDiffuse);
  glLightfv(GL_LIGHT0, GL_AMBIENT, sunAmbient);
  glLightfv(GL_LIGHT0, GL_SPECULAR, sunSpecular);

  GLfloat globalAmb[] = {0.15f, 0.16f, 0.18f, 1.0f};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmb);

  drawGround();
  g_spinner.draw();

  glutSwapBuffers();
}

void reshape(int w, int h) {
  if (h == 0)
    h = 1;
  glViewport(0, 0, w, h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  float aspect = w / (float)h;
  float fovY = 60.0f;
  float nearZ = 0.5f;
  float farZ = 800.0f;
  float top = std::tan(fovY * PI / 360.0f) * nearZ;
  float bottom = -top;
  float right = top * aspect;
  float left = -right;
  glFrustum(left, right, bottom, top, nearZ, farZ);

  glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int, int) {
  switch (key) {
  case 27: // ESC
    std::exit(0);
    break;
  case 'w':
  case 'W': {
    float radYaw = yaw * PI / 180.0f;
    camX += std::sin(radYaw) * MOVE_SPEED;
    camZ += -std::cos(radYaw) * MOVE_SPEED;
  } break;
  case 's':
  case 'S': {
    float radYaw = yaw * PI / 180.0f;
    camX -= std::sin(radYaw) * MOVE_SPEED;
    camZ -= -std::cos(radYaw) * MOVE_SPEED;
  } break;
  case 'a':
  case 'A': {
    float radYaw = yaw * PI / 180.0f;
    camX += std::cos(radYaw) * -MOVE_SPEED;
    camZ += std::sin(radYaw) * -MOVE_SPEED;
  } break;
  case 'd':
  case 'D': {
    float radYaw = yaw * PI / 180.0f;
    camX += std::cos(radYaw) * MOVE_SPEED;
    camZ += std::sin(radYaw) * MOVE_SPEED;
  } break;
  case 'q':
  case 'Q':
    camY -= MOVE_SPEED;
    break;
  case 'e':
  case 'E':
    camY += MOVE_SPEED;
    break;
  case 'r':
  case 'R':
    camX = 0.0f;
    camY = 35.0f;
    camZ = 120.0f;
    yaw = 0.0f;
    pitch = -15.0f;
    break;
  default:
    break;
  }
  glutPostRedisplay();
}

void special(int key, int, int) {
  switch (key) {
  case GLUT_KEY_LEFT:
    yaw -= ROT_SPEED;
    break;
  case GLUT_KEY_RIGHT:
    yaw += ROT_SPEED;
    break;
  case GLUT_KEY_UP:
    pitch += ROT_SPEED;
    if (pitch > 89.0f)
      pitch = 89.0f;
    break;
  case GLUT_KEY_DOWN:
    pitch -= ROT_SPEED;
    if (pitch < -89.0f)
      pitch = -89.0f;
    break;
  default:
    break;
  }
  glutPostRedisplay();
}

void timer(int) {
  g_spinner.update(0.016f); // ~60 FPS
  glutPostRedisplay();
  glutTimerFunc(16, timer, 0);
}

// ---------------- OpenGL 初始化 ----------------
void initGL() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);
  glShadeModel(GL_SMOOTH);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

  GLfloat specular[] = {0.4f, 0.4f, 0.45f, 1.0f};
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);

  // 夜里但光比较足的赛博蓝
  glClearColor(0.03f, 0.04f, 0.08f, 1.0f);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// ---------------- main ----------------
int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(1280, 720);
  glutCreateWindow("Police Spinner - FreeGLUT");

  initGL();

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(special);
  glutTimerFunc(16, timer, 0);

  glutMainLoop();
  return 0;
}
