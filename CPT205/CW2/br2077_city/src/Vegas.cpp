#include <GL/freeglut.h>
#include <cmath>
#include <cstdlib>
#include <vector>

const float PI = 3.14159265f;

// ---------------- Camera ----------------
float camX = 0.0f;
float camY = 50.0f;
float camZ = 200.0f;
float yaw = 0.0f;     // 左右转动
float pitch = -15.0f; // 上下俯仰

const float MOVE_SPEED = 3.0f;
const float ROT_SPEED = 2.0f;

// ----------- Casino 场景参数 -----------
const float CASINO_PODIUM_HEIGHT = 12.0f;
const float CASINO_PODIUM_HALF_W = 60.0f;
const float CASINO_PODIUM_HALF_D = 40.0f;

const float HOTEL_HEIGHT = 120.0f;
const float HOTEL_HALF_W = 25.0f;
const float HOTEL_HALF_D = 15.0f;

// 赌场入口前广场范围
const float PLAZA_HALF_SIZE = 120.0f;

// 雾气（很淡，可以直接改这一行调节；设为 0 就等于关雾）
float g_fogDensity = 0.003f;

// ---------------- 小工具函数 ----------------
float frand(float a, float b) {
  return a + (b - a) * (std::rand() / (float)RAND_MAX);
}

// ---------------- 场景元素结构体 ----------------
struct SlotMachine {
  float x, z;
  float rotDeg;
};

struct Table {
  float x, z;
  float rotDeg;
};

struct Chair {
  float x, z;
  float rotDeg;
};

std::vector<SlotMachine> g_slots;
std::vector<Table> g_tables;
std::vector<Chair> g_chairs;

// ---------------- Camera helpers ----------------
void getForward2D(float &fx, float &fz) {
  float radYaw = yaw * PI / 180.0f;
  fx = std::sin(radYaw);
  fz = -std::cos(radYaw);
}

void moveForward(float amount) {
  float fx, fz;
  getForward2D(fx, fz);
  camX += fx * amount;
  camZ += fz * amount;
}

void strafe(float amount) {
  float radYaw = yaw * PI / 180.0f;
  float rx = std::cos(radYaw);
  float rz = std::sin(radYaw);
  camX += rx * amount;
  camZ += rz * amount;
}

void resetCamera() {
  camX = 0.0f;
  camY = 50.0f;
  camZ = 200.0f;
  yaw = 0.0f;
  pitch = -15.0f;
}

// ---------------- 场景初始化 ----------------
void initScene() {
  std::srand(2025);

  // 在赌场前广场随机散落老虎机 / 桌子 / 椅子
  for (int i = 0; i < 25; ++i) {
    SlotMachine s;
    s.x = frand(-PLAZA_HALF_SIZE * 0.8f, PLAZA_HALF_SIZE * 0.8f);
    s.z = frand(10.0f, PLAZA_HALF_SIZE * 0.9f);
    s.rotDeg = frand(0.0f, 360.0f);
    g_slots.push_back(s);
  }

  for (int i = 0; i < 10; ++i) {
    Table t;
    t.x = frand(-PLAZA_HALF_SIZE * 0.6f, PLAZA_HALF_SIZE * 0.6f);
    t.z = frand(-10.0f, PLAZA_HALF_SIZE * 0.7f);
    t.rotDeg = frand(0.0f, 360.0f);
    g_tables.push_back(t);
  }

  for (int i = 0; i < 30; ++i) {
    Chair c;
    c.x = frand(-PLAZA_HALF_SIZE * 0.8f, PLAZA_HALF_SIZE * 0.8f);
    c.z = frand(-10.0f, PLAZA_HALF_SIZE * 0.9f);
    c.rotDeg = frand(0.0f, 360.0f);
    g_chairs.push_back(c);
  }
}

// ---------------- 绘制函数 ----------------
void drawGround() {
  // 远处是辐射沙漠 + 城市废墟的地基
  glDisable(GL_LIGHTING);
  glBegin(GL_QUADS);
  glColor3f(0.55f, 0.40f, 0.25f); // 橙褐色沙土
  float size = 1000.0f;
  float y = -5.0f;
  glVertex3f(-size, y, -size);
  glVertex3f(size, y, -size);
  glVertex3f(size, y, size);
  glVertex3f(-size, y, size);
  glEnd();

  // 赌场前的混凝土广场
  glBegin(GL_QUADS);
  glColor3f(0.60f, 0.50f, 0.40f);
  glVertex3f(-PLAZA_HALF_SIZE, 0.0f, PLAZA_HALF_SIZE);
  glVertex3f(PLAZA_HALF_SIZE, 0.0f, PLAZA_HALF_SIZE);
  glVertex3f(PLAZA_HALF_SIZE, 0.0f, -PLAZA_HALF_SIZE);
  glVertex3f(-PLAZA_HALF_SIZE, 0.0f, -PLAZA_HALF_SIZE);
  glEnd();

  glEnable(GL_LIGHTING);
}

void drawCasinoPodium() {
  // 前方赌场大厅（低矮、长条体量）
  glPushMatrix();
  float centerY = CASINO_PODIUM_HEIGHT * 0.5f;
  glTranslatef(0.0f, centerY, 0.0f);
  glScalef(CASINO_PODIUM_HALF_W * 2.0f, CASINO_PODIUM_HEIGHT,
           CASINO_PODIUM_HALF_D * 2.0f);
  glColor3f(0.55f, 0.48f, 0.42f);
  glutSolidCube(1.0);
  glPopMatrix();

  // 入口檐口
  glPushMatrix();
  glTranslatef(0.0f, CASINO_PODIUM_HEIGHT + 2.0f, CASINO_PODIUM_HALF_D + 4.0f);
  glScalef(CASINO_PODIUM_HALF_W * 1.8f, 4.0f, 8.0f);
  glColor3f(0.50f, 0.42f, 0.36f);
  glutSolidCube(1.0);
  glPopMatrix();
}

void drawHotelTower() {
  // 后方高酒店塔楼
  glPushMatrix();
  float baseY = CASINO_PODIUM_HEIGHT;
  float centerY = baseY + HOTEL_HEIGHT * 0.5f;
  glTranslatef(0.0f, centerY, -CASINO_PODIUM_HALF_D + HOTEL_HALF_D);

  glScalef(HOTEL_HALF_W * 2.0f, HOTEL_HEIGHT, HOTEL_HALF_D * 2.0f);
  glColor3f(0.65f, 0.58f, 0.52f);
  glutSolidCube(1.0);
  glPopMatrix();

  // 顶部一些残破结构（简单几个偏移的方块）
  glPushMatrix();
  float topY = baseY + HOTEL_HEIGHT;
  glTranslatef(0.0f, topY + 4.0f, -CASINO_PODIUM_HALF_D + HOTEL_HALF_D);
  glScalef(HOTEL_HALF_W * 1.6f, 8.0f, HOTEL_HALF_D * 1.6f);
  glColor3f(0.50f, 0.42f, 0.36f);
  glutSolidCube(1.0);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(HOTEL_HALF_W * 0.8f, topY + 10.0f,
               -CASINO_PODIUM_HALF_D + HOTEL_HALF_D + 4.0f);
  glRotatef(-25.0f, 0.0f, 0.0f, 1.0f);
  glScalef(20.0f, 6.0f, 8.0f);
  glColor3f(0.40f, 0.34f, 0.30f);
  glutSolidCube(1.0);
  glPopMatrix();
}

void drawNeonSign() {
  // 赌场前的竖立霓虹牌（略微破损）
  float signHeight = 40.0f;
  float signWidth = 18.0f;
  float signThickness = 3.0f;

  // 牌身
  glPushMatrix();
  glTranslatef(-CASINO_PODIUM_HALF_W - 15.0f, signHeight * 0.5f + 5.0f,
               CASINO_PODIUM_HALF_D * 0.4f);
  glRotatef(-10.0f, 0.0f, 0.0f, 1.0f);
  glScalef(signWidth, signHeight, signThickness);
  glColor3f(0.35f, 0.30f, 0.28f);
  glutSolidCube(1.0);
  glPopMatrix();

  // 边缘霓虹框（只用线条）
  glDisable(GL_LIGHTING);
  glPushMatrix();
  glTranslatef(-CASINO_PODIUM_HALF_W - 15.0f, signHeight * 0.5f + 5.0f,
               CASINO_PODIUM_HALF_D * 0.4f + signThickness * 0.6f);
  glRotatef(-10.0f, 0.0f, 0.0f, 1.0f);
  glScalef(signWidth * 0.8f, signHeight * 0.8f, 1.0f);

  glLineWidth(2.0f);
  glColor3f(0.95f, 0.60f, 0.25f); // 橙色霓虹
  glBegin(GL_LINE_LOOP);
  glVertex3f(-0.5f, -0.5f, 0.0f);
  glVertex3f(0.5f, -0.5f, 0.0f);
  glVertex3f(0.5f, 0.5f, 0.0f);
  glVertex3f(-0.5f, 0.5f, 0.0f);
  glEnd();

  // 中间 "CASINO" 字样
  glScalef(0.06f, 0.06f, 1.0f);
  glTranslatef(-350.0f, -50.0f, 0.0f);
  const char *text = "CASINO";
  for (const char *p = text; *p; ++p) {
    glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
  }

  glPopMatrix();
  glEnable(GL_LIGHTING);
}

void drawBrokenBillboard() {
  // 半塌的广告牌 / 雕塑：一个大矩形板倾斜躺倒
  glPushMatrix();
  glTranslatef(CASINO_PODIUM_HALF_W + 35.0f, 25.0f,
               CASINO_PODIUM_HALF_D * 0.2f);
  glRotatef(70.0f, 0.0f, 0.0f, 1.0f); // 倒下
  glRotatef(15.0f, 0.0f, 1.0f, 0.0f);
  glScalef(50.0f, 30.0f, 2.0f);
  glColor3f(0.40f, 0.33f, 0.30f);
  glutSolidCube(1.0);
  glPopMatrix();

  // 支撑架子几根残留钢梁
  glDisable(GL_LIGHTING);
  glBegin(GL_LINES);
  glColor3f(0.6f, 0.55f, 0.5f);
  glVertex3f(CASINO_PODIUM_HALF_W + 15.0f, 0.0f, CASINO_PODIUM_HALF_D * 0.1f);
  glVertex3f(CASINO_PODIUM_HALF_W + 35.0f, 20.0f, CASINO_PODIUM_HALF_D * 0.2f);

  glVertex3f(CASINO_PODIUM_HALF_W + 20.0f, 0.0f, CASINO_PODIUM_HALF_D * 0.4f);
  glVertex3f(CASINO_PODIUM_HALF_W + 35.0f, 20.0f, CASINO_PODIUM_HALF_D * 0.2f);
  glEnd();
  glEnable(GL_LIGHTING);
}

void drawSlotMachine(const SlotMachine &s) {
  glPushMatrix();
  glTranslatef(s.x, 0.0f, s.z);
  glRotatef(s.rotDeg, 0.0f, 1.0f, 0.0f);

  // 底座
  glPushMatrix();
  glTranslatef(0.0f, 1.0f, 0.0f);
  glScalef(3.0f, 2.0f, 2.0f);
  glColor3f(0.35f, 0.35f, 0.37f);
  glutSolidCube(1.0);
  glPopMatrix();

  // 上半机身
  glPushMatrix();
  glTranslatef(0.0f, 3.5f, 0.0f);
  glScalef(2.5f, 4.0f, 1.8f);
  glColor3f(0.40f, 0.40f, 0.42f);
  glutSolidCube(1.0);
  glPopMatrix();

  // 屏幕位置（用深一点的灰）
  glPushMatrix();
  glTranslatef(0.0f, 4.0f, 1.0f);
  glScalef(1.6f, 1.4f, 0.1f);
  glColor3f(0.10f, 0.10f, 0.12f);
  glutSolidCube(1.0);
  glPopMatrix();

  glPopMatrix();
}

void drawTable(const Table &t) {
  glPushMatrix();
  glTranslatef(t.x, 0.0f, t.z);
  glRotatef(t.rotDeg, 0.0f, 1.0f, 0.0f);

  // 桌面
  glPushMatrix();
  glTranslatef(0.0f, 2.0f, 0.0f);
  glScalef(10.0f, 1.0f, 6.0f);
  glColor3f(0.32f, 0.30f, 0.30f);
  glutSolidCube(1.0);
  glPopMatrix();

  // 桌脚（四个小立方体）
  glColor3f(0.18f, 0.18f, 0.18f);
  float legX = 4.0f, legZ = 2.5f;
  float legH = 2.0f;

  for (int sx = -1; sx <= 1; sx += 2) {
    for (int sz = -1; sz <= 1; sz += 2) {
      glPushMatrix();
      glTranslatef(sx * legX, legH * 0.5f, sz * legZ);
      glScalef(1.0f, legH, 1.0f);
      glutSolidCube(1.0);
      glPopMatrix();
    }
  }
  glPopMatrix();
}

void drawChair(const Chair &c) {
  glPushMatrix();
  glTranslatef(c.x, 0.0f, c.z);
  glRotatef(c.rotDeg, 0.0f, 1.0f, 0.0f);

  // 坐垫
  glPushMatrix();
  glTranslatef(0.0f, 1.0f, 0.0f);
  glScalef(2.0f, 0.8f, 2.0f);
  glColor3f(0.28f, 0.28f, 0.30f);
  glutSolidCube(1.0);
  glPopMatrix();

  // 椅背
  glPushMatrix();
  glTranslatef(0.0f, 2.4f, -0.6f);
  glScalef(2.0f, 3.0f, 0.5f);
  glColor3f(0.30f, 0.30f, 0.32f);
  glutSolidCube(1.0);
  glPopMatrix();

  glPopMatrix();
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

  // 强烈的橙色阳光（脏弹后的雾天但光很猛）
  GLfloat sunDir[] = {-0.4f, 0.8f, 0.1f, 0.0f};
  GLfloat sunDiffuse[] = {1.0f, 0.85f, 0.65f, 1.0f};
  GLfloat sunAmbient[] = {0.55f, 0.40f, 0.30f, 1.0f};
  GLfloat sunSpecular[] = {0.4f, 0.35f, 0.30f, 1.0f};
  glLightfv(GL_LIGHT0, GL_POSITION, sunDir);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, sunDiffuse);
  glLightfv(GL_LIGHT0, GL_AMBIENT, sunAmbient);
  glLightfv(GL_LIGHT0, GL_SPECULAR, sunSpecular);

  GLfloat globalAmb[] = {0.25f, 0.20f, 0.18f, 1.0f};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmb);

  drawGround();
  drawCasinoPodium();
  drawHotelTower();
  drawNeonSign();
  drawBrokenBillboard();

  for (const auto &s : g_slots)
    drawSlotMachine(s);
  for (const auto &t : g_tables)
    drawTable(t);
  for (const auto &c : g_chairs)
    drawChair(c);

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
  float farZ = 1000.0f;
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
  case 'W':
    moveForward(MOVE_SPEED);
    break;
  case 's':
  case 'S':
    moveForward(-MOVE_SPEED);
    break;
  case 'a':
  case 'A':
    strafe(-MOVE_SPEED);
    break;
  case 'd':
  case 'D':
    strafe(MOVE_SPEED);
    break;
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
    resetCamera();
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
  // 暂时没有动画，这里只是驱动重绘；以后要加飞舞尘土可以从这里下手
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

  GLfloat specular[] = {0.35f, 0.30f, 0.28f, 1.0f};
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);

  // 橙色放射性沙尘天空
  GLfloat fogColor[] = {0.80f, 0.60f, 0.40f, 1.0f};
  glClearColor(fogColor[0], fogColor[1], fogColor[2], fogColor[3]);

  // 很淡的雾 - 想更淡就把 g_fogDensity 调得更小或设为 0
  glEnable(GL_FOG);
  glFogfv(GL_FOG_COLOR, fogColor);
  glFogi(GL_FOG_MODE, GL_EXP2);
  glFogf(GL_FOG_DENSITY, g_fogDensity);
  glHint(GL_FOG_HINT, GL_NICEST);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  initScene();
}

// ---------------- main ----------------
int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(1280, 720);
  glutCreateWindow("Radioactive Las Vegas Casino Ruins (FreeGLUT)");

  initGL();

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(special);
  glutTimerFunc(16, timer, 0);

  glutMainLoop();
  return 0;
}
