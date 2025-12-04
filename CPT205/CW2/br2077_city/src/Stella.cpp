#include <GL/freeglut.h>
#include <cmath>
#include <cstdlib>
#include <vector>

const float PI = 3.14159265f;

// ---------------- Camera ----------------
float camX = 0.0f;
float camY = 40.0f;
float camZ = 160.0f;
float yaw = 0.0f;     // 左右转
float pitch = -15.0f; // 上下俯仰

const float MOVE_SPEED = 3.0f;
const float ROT_SPEED = 2.0f;

// ---------------- Shared building dims ----------------
const int NUM_STEPS = 9;
const float STEP_H = 0.8f;
const float STEP_DEPTH = 4.0f;
const float STEP_W = 40.0f;
const float FOUNDATION_H = NUM_STEPS * STEP_H; // tie building base to ground

// ---------------- Snow ----------------
struct Snow {
  float x, y, z;
  float vy;
};

std::vector<Snow> g_snow;
const int SNOW_COUNT = 900;

// 随机工具
float frand(float a, float b) {
  return a + (b - a) * (std::rand() / (float)RAND_MAX);
}

// ---------------- Camera helpers ----------------
void moveForward(float amount) {
  float rad = yaw * PI / 180.0f;
  camX += std::sin(rad) * amount;
  camZ += -std::cos(rad) * amount;
}

void strafe(float amount) {
  float rad = yaw * PI / 180.0f;
  camX += std::cos(rad) * amount;
  camZ += std::sin(rad) * amount;
}

void resetCamera() {
  camX = 0.0f;
  camY = 40.0f;
  camZ = 160.0f;
  yaw = 0.0f;
  pitch = -15.0f;
}

// ---------------- Snow init/update/draw ----------------
void initSnow() {
  g_snow.clear();
  g_snow.reserve(SNOW_COUNT);
  for (int i = 0; i < SNOW_COUNT; ++i) {
    Snow s;
    s.x = frand(-200.0f, 200.0f);
    s.z = frand(-200.0f, 200.0f);
    s.y = frand(10.0f, 120.0f);
    s.vy = frand(0.4f, 1.2f);
    g_snow.push_back(s);
  }
}

void updateSnow(float dt) {
  for (auto &s : g_snow) {
    s.y -= s.vy; // 简单向下飘
    if (s.y < 0.5f) {
      // 重新从天上刷下来
      s.y = frand(60.0f, 130.0f);
      s.x = frand(-200.0f, 200.0f);
      s.z = frand(-200.0f, 200.0f);
      s.vy = frand(0.4f, 1.2f);
    }
  }
}

void drawSnow() {
  glDisable(GL_LIGHTING);
  glPointSize(2.0f);
  glBegin(GL_POINTS);
  glColor3f(0.90f, 0.92f, 0.98f);
  for (const auto &s : g_snow) {
    glVertex3f(s.x, s.y, s.z);
  }
  glEnd();
  glEnable(GL_LIGHTING);
}

// ---------------- Ground & environment ----------------
void drawGround() {
  // 大片雪地/混凝土场
  glDisable(GL_LIGHTING);
  glBegin(GL_QUADS);
  glColor3f(0.28f, 0.30f, 0.33f);
  float size = 600.0f;
  glVertex3f(-size, 0.0f, -size);
  glVertex3f(size, 0.0f, -size);
  glVertex3f(size, 0.0f, size);
  glVertex3f(-size, 0.0f, size);
  glEnd();
  glEnable(GL_LIGHTING);
}

// 近建筑雪场，带柔和渐隐
void drawSnowField() {
  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  float inner = 170.0f; // 纯雪区域
  float outer = 260.0f; // 渐隐到无

  auto v = [](float x, float z, float a) {
    glColor4f(0.82f, 0.84f, 0.88f, a);
    glVertex3f(x, 0.04f, z);
  };

  glBegin(GL_TRIANGLE_STRIP);
  v(-outer, -outer, 0.0f);
  v(-inner, -inner, 0.85f);
  v(outer, -outer, 0.0f);
  v(inner, -inner, 0.85f);
  v(outer, outer, 0.0f);
  v(inner, inner, 0.85f);
  v(-outer, outer, 0.0f);
  v(-inner, inner, 0.85f);
  v(-outer, -outer, 0.0f);
  v(-inner, -inner, 0.85f);
  glEnd();

  glEnable(GL_LIGHTING);
}

// 前广场硬质铺装，衔接地面与台阶
void drawPlaza() {
  glDisable(GL_LIGHTING);
  glBegin(GL_QUADS);
  glColor3f(0.32f, 0.34f, 0.36f);
  float w = 80.0f;
  float d = 70.0f;
  float zCenter = 70.0f;
  glVertex3f(-w, 0.02f, zCenter - d);
  glVertex3f(w, 0.02f, zCenter - d);
  glVertex3f(w, 0.02f, zCenter + d * 0.2f);
  glVertex3f(-w, 0.02f, zCenter + d * 0.2f);
  glEnd();
  glEnable(GL_LIGHTING);
}

// ---------------- Stelline 记忆实验楼 ----------------

// 宽台阶
void drawStairs() {
  const float startZ = 44.0f; // 最下面那级台阶前缘大约在 z≈46

  // Solid cheek walls to visually anchor stairs to ground
  float wallThickness = 1.4f;
  float wallH = FOUNDATION_H;
  float wallZSpan = startZ - (NUM_STEPS - 1) * STEP_DEPTH - STEP_DEPTH;

  glPushMatrix();
  glTranslatef(-(STEP_W * 0.5f + wallThickness * 0.5f), wallH * 0.5f,
               (startZ + wallZSpan) * 0.5f);
  glScalef(wallThickness, wallH, std::fabs(startZ - wallZSpan));
  glColor3f(0.55f, 0.55f, 0.57f);
  glutSolidCube(1.0);
  glPopMatrix();

  glPushMatrix();
  glTranslatef((STEP_W * 0.5f + wallThickness * 0.5f), wallH * 0.5f,
               (startZ + wallZSpan) * 0.5f);
  glScalef(wallThickness, wallH, std::fabs(startZ - wallZSpan));
  glColor3f(0.55f, 0.55f, 0.57f);
  glutSolidCube(1.0);
  glPopMatrix();

  for (int i = 0; i < NUM_STEPS; ++i) {
    float hCenter = (i + 0.5f) * STEP_H;
    float zCenter = startZ - i * STEP_DEPTH - STEP_DEPTH * 0.5f;

    glPushMatrix();
    glTranslatef(0.0f, hCenter, zCenter);
    glScalef(STEP_W, STEP_H, STEP_DEPTH);
    glColor3f(0.82f, 0.82f, 0.84f); // 覆雪混凝土
    glutSolidCube(1.0);
    glPopMatrix();
  }

  // 台阶顶部到建筑前的缓坡平台
  float topY = NUM_STEPS * STEP_H;
  glPushMatrix();
  glTranslatef(0.0f, topY + 0.01f, 4.0f);
  glScalef(STEP_W * 1.05f, 0.2f, 8.0f);
  glColor3f(0.86f, 0.86f, 0.88f);
  glutSolidCube(1.0);
  glPopMatrix();

  // Slim handrails for realism
  glDisable(GL_LIGHTING);
  glLineWidth(3.0f);
  glColor3f(0.45f, 0.45f, 0.47f);
  for (int side = -1; side <= 1; side += 2) {
    float x = side * (STEP_W * 0.5f - 1.5f);
    glBegin(GL_LINES);
    glVertex3f(x, 0.2f, startZ + 0.5f);
    glVertex3f(x, FOUNDATION_H + 0.5f, startZ - NUM_STEPS * STEP_DEPTH - 4.0f);
    glEnd();
  }
  glEnable(GL_LIGHTING);
}

// 主体混凝土楼（粗野主义方块）
void drawMainBuilding() {
  float baseY = FOUNDATION_H; // 台阶顶高度与基础一体
  float bodyH = 22.0f;
  float bodyW = 60.0f;
  float bodyD = 40.0f;

  // 单一整体方块（含基础与上部），避免拼接感
  glPushMatrix();
  float totalH = FOUNDATION_H + bodyH;
  glTranslatef(0.0f, totalH * 0.5f, -bodyD * 0.3f);
  glScalef(bodyW, totalH, bodyD);
  glColor3f(0.60f, 0.60f, 0.62f);
  glutSolidCube(1.0);
  glPopMatrix();

  // 顶部偏移的小体块，增加粗野感
  glPushMatrix();
  glTranslatef(-10.0f, baseY + bodyH + 5.0f, -bodyD * 0.6f);
  glScalef(bodyW * 0.6f, 10.0f, bodyD * 0.7f);
  glColor3f(0.68f, 0.68f, 0.70f);
  glutSolidCube(1.0);
  glPopMatrix();

  // 屋顶设备箱体
  glPushMatrix();
  glTranslatef(12.0f, baseY + bodyH + 2.0f, -bodyD * 0.2f);
  glScalef(14.0f, 3.0f, 8.0f);
  glColor3f(0.48f, 0.48f, 0.50f);
  glutSolidCube(1.0);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(-18.0f, baseY + bodyH + 1.5f, -bodyD * 0.15f);
  glScalef(10.0f, 2.5f, 6.0f);
  glColor3f(0.50f, 0.50f, 0.52f);
  glutSolidCube(1.0);
  glPopMatrix();
}

// 玻璃幕墙 + 入口
void drawFrontFacade() {
  float baseY = FOUNDATION_H;
  float bodyD = 40.0f;
  float bodyFaceZ =
      -bodyD * 0.3f + bodyD * 0.5f; // 主体前立面世界坐标（约 z=8）

  // 玻璃幕墙：偏冷灰蓝、半透明，正对台阶
  glPushMatrix();
  glTranslatef(0.0f, baseY + 8.0f, bodyFaceZ + 0.05f);
  glScalef(34.0f, 14.0f, 0.6f);
  glColor4f(0.75f, 0.80f, 0.85f, 0.5f);
  glutSolidCube(1.0);
  glPopMatrix();
  glEnable(GL_LIGHTING);

  // 幕墙竖向分格线
  glDisable(GL_LIGHTING);
  glLineWidth(1.5f);
  glColor3f(0.6f, 0.6f, 0.65f);
  float y0 = baseY + 1.5f;
  float y1 = baseY + 14.5f;
  for (int i = -3; i <= 3; ++i) {
    float x = i * 5.0f;
    glBegin(GL_LINES);
    glVertex3f(x, y0, bodyFaceZ + 0.15f);
    glVertex3f(x, y1, bodyFaceZ + 0.15f);
    glEnd();
  }

  // 横向分格
  for (int j = 0; j <= 2; ++j) {
    float y = y0 + j * 6.5f;
    glBegin(GL_LINES);
    glVertex3f(-17.0f, y, bodyFaceZ + 0.15f);
    glVertex3f(17.0f, y, bodyFaceZ + 0.15f);
    glEnd();
  }

  // 入口门：双扇玻璃门 + 深色框
  float doorW = 2.6f;
  float doorH = 4.4f;
  float doorZ = bodyFaceZ + 0.45f; // 外凸于幕墙，显式在玻璃之外
  // Frame
  glLineWidth(6.0f);
  glColor3f(0.30f, 0.30f, 0.32f);
  glBegin(GL_LINE_LOOP);
  glVertex3f(-doorW * 0.5f, baseY + 1.0f, doorZ);
  glVertex3f(doorW * 0.5f, baseY + 1.0f, doorZ);
  glVertex3f(doorW * 0.5f, baseY + 1.0f + doorH, doorZ);
  glVertex3f(-doorW * 0.5f, baseY + 1.0f + doorH, doorZ);
  glEnd();
  // Glass leaves
  glLineWidth(2.5f);
  glColor3f(0.72f, 0.78f, 0.84f);
  // left leaf
  glBegin(GL_LINE_LOOP);
  glVertex3f(-doorW * 0.5f + 0.25f, baseY + 1.1f, doorZ);
  glVertex3f(0.0f, baseY + 1.1f, doorZ);
  glVertex3f(0.0f, baseY + 1.1f + doorH - 0.6f, doorZ);
  glVertex3f(-doorW * 0.5f + 0.25f, baseY + 1.1f + doorH - 0.6f, doorZ);
  glEnd();
  // right leaf
  glBegin(GL_LINE_LOOP);
  glVertex3f(0.0f, baseY + 1.1f, doorZ);
  glVertex3f(doorW * 0.5f - 0.25f, baseY + 1.1f, doorZ);
  glVertex3f(doorW * 0.5f - 0.25f, baseY + 1.1f + doorH - 0.6f, doorZ);
  glVertex3f(0.0f, baseY + 1.1f + doorH - 0.6f, doorZ);
  glEnd();
  // handles
  glLineWidth(3.0f);
  glColor3f(0.90f, 0.90f, 0.92f);
  glBegin(GL_LINES);
  glVertex3f(-0.55f, baseY + 2.6f, doorZ + 0.05f);
  glVertex3f(-0.55f, baseY + 3.3f, doorZ + 0.05f);
  glVertex3f(0.55f, baseY + 2.6f, doorZ + 0.05f);
  glVertex3f(0.55f, baseY + 3.3f, doorZ + 0.05f);
  glEnd();
  glEnable(GL_LIGHTING);
}

// 周围低矮混凝土附属建筑
void drawAnnexBuildings() {
  struct Block {
    float x, z;
    float w, d, h;
  } blocks[] = {{-60.0f, -40.0f, 50.0f, 30.0f, 10.0f},
                {65.0f, -30.0f, 45.0f, 28.0f, 12.0f},
                {-50.0f, 30.0f, 40.0f, 25.0f, 9.0f},
                {55.0f, 35.0f, 35.0f, 22.0f, 11.0f}};

  for (auto &b : blocks) {
    glPushMatrix();
    glTranslatef(b.x, b.h * 0.5f, b.z);
    glScalef(b.w, b.h, b.d);
    // Annexes in the same gray palette as the main mass
    glColor3f(0.60f, 0.60f, 0.62f);
    glutSolidCube(1.0);
    glPopMatrix();
  }
}

// 附属楼窗带
void drawAnnexWindows() {
  struct Block {
    float x, z;
    float w, d, h;
  } blocks[] = {{-60.0f, -40.0f, 50.0f, 30.0f, 10.0f},
                {65.0f, -30.0f, 45.0f, 28.0f, 12.0f},
                {-50.0f, 30.0f, 40.0f, 25.0f, 9.0f},
                {55.0f, 35.0f, 35.0f, 22.0f, 11.0f}};

  glDisable(GL_LIGHTING);
  glColor3f(0.26f, 0.34f, 0.44f);
  float winH = 1.3f;
  float margin = 3.5f;
  for (auto &b : blocks) {
    float colSpan = b.w - margin * 2.0f;
    if (colSpan <= 6.0f)
      continue;
    float winW = 5.0f;
    int cols = 3;
    float step = (colSpan - winW) / cols;

    for (int side = -1; side <= 1; side += 2) {
      float z = b.z + side * (b.d * 0.5f + 0.02f);
      for (int row = 0; row < 2; ++row) {
        float y = 2.0f + row * 3.2f;
        if (y + winH >= b.h)
          break;
        for (int c = 0; c <= cols; ++c) {
          float xL = b.x - b.w * 0.5f + margin + c * step;
          float xR = xL + winW;
          glBegin(GL_QUADS);
          glVertex3f(xL, y, z);
          glVertex3f(xR, y, z);
          glVertex3f(xR, y + winH, z);
          glVertex3f(xL, y + winH, z);
          glEnd();
        }
      }
    }
  }
  glEnable(GL_LIGHTING);
}

// 附属楼出入口
void drawAnnexDoors() {
  // 在最靠前的两栋做服务门
  struct DoorTarget {
    float x, z, w, d, h;
  } doors[] = {{55.0f, 35.0f, 35.0f, 22.0f, 11.0f},
               {-50.0f, 30.0f, 40.0f, 25.0f, 9.0f}};

  glDisable(GL_LIGHTING);
  for (auto &d : doors) {
    float doorW = 3.0f;
    float doorH = 4.2f;
    float zFace = d.z + d.d * 0.5f + 0.03f;
    glColor3f(0.20f, 0.22f, 0.25f);
    glBegin(GL_QUADS);
    glVertex3f(d.x - doorW * 0.5f, 0.5f, zFace);
    glVertex3f(d.x + doorW * 0.5f, 0.5f, zFace);
    glVertex3f(d.x + doorW * 0.5f, 0.5f + doorH, zFace);
    glVertex3f(d.x - doorW * 0.5f, 0.5f + doorH, zFace);
    glEnd();

    glLineWidth(2.5f);
    glColor3f(0.70f, 0.72f, 0.74f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(d.x - doorW * 0.5f, 0.5f, zFace);
    glVertex3f(d.x + doorW * 0.5f, 0.5f, zFace);
    glVertex3f(d.x + doorW * 0.5f, 0.5f + doorH, zFace);
    glVertex3f(d.x - doorW * 0.5f, 0.5f + doorH, zFace);
    glEnd();
  }
  glEnable(GL_LIGHTING);
}

// 广场上的矮柱灯
void drawBollardLights() {
  float z = 80.0f; // move away from stairs/building
  glDisable(GL_LIGHTING);
  for (float x = -48.0f; x <= 48.0f; x += 16.0f) {
    glPushMatrix();
    glTranslatef(x, 0.9f, z);
    glScalef(1.2f, 1.8f, 1.2f);
    glColor3f(0.20f, 0.22f, 0.24f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(x, 1.8f, z);
    glScalef(1.0f, 0.4f, 1.0f);
    glColor3f(1.0f, 0.86f, 0.55f);
    glutSolidCube(1.0);
    glPopMatrix();
  }
  glEnable(GL_LIGHTING);
}

// 简单侧立面窗带
void drawSideWindows() {
  float baseY = FOUNDATION_H;
  float bodyD = 40.0f;
  float bodyH = 22.0f;
  glDisable(GL_LIGHTING);
  glColor3f(0.32f, 0.38f, 0.46f);
  float yStart = baseY + 4.0f;
  float yEnd = baseY + bodyH - 3.0f;
  // 左右两侧各画两条水平窗带
  for (int side = -1; side <= 1; side += 2) {
    float x = side * 30.0f;
    for (int band = 0; band < 2; ++band) {
      float y = yStart + band * 6.0f;
      glBegin(GL_QUADS);
      glVertex3f(x, y, -bodyD * 0.8f);
      glVertex3f(x, y + 1.4f, -bodyD * 0.8f);
      glVertex3f(x, y + 1.4f, bodyD * 0.2f);
      glVertex3f(x, y, bodyD * 0.2f);
      glEnd();
    }
  }
  glEnable(GL_LIGHTING);
}

// 入口雨棚
void drawCanopy() {
  float baseY = FOUNDATION_H;
  float bodyD = 40.0f;
  glDisable(GL_LIGHTING);
  glPushMatrix();
  glTranslatef(0.0f, baseY + 10.0f, -bodyD * 0.75f);
  glScalef(22.0f, 0.5f, 6.5f);
  glColor3f(0.58f, 0.60f, 0.62f);
  glutSolidCube(1.0);
  glPopMatrix();
  glEnable(GL_LIGHTING);
}

// 夜间暖光的立面灯带
void drawStripLights() {
  float baseY = FOUNDATION_H;
  float bodyD = 40.0f;
  glDisable(GL_LIGHTING);
  glColor3f(1.0f, 0.85f, 0.55f);
  glBegin(GL_LINES);
  for (int i = -2; i <= 2; ++i) {
    float x = i * 7.0f;
    glVertex3f(x - 3.0f, baseY + 2.2f, -bodyD * 0.6f);
    glVertex3f(x + 3.0f, baseY + 2.2f, -bodyD * 0.6f);
  }
  glEnd();
  glEnable(GL_LIGHTING);
}

  // 台阶上的躺倒人影（超简化剪影）
void drawFallenMan() {
  // 选择台阶中段位置
  const int stepIndex = 5;
  const float stepH = STEP_H;
  const float stepDepth = STEP_DEPTH;
  const float startZ = 44.0f;

  float stepY = (stepIndex + 1) * stepH - 0.25f; // sink slightly into tread so body sits lower
  float stepZ = startZ - stepIndex * stepDepth - stepDepth * 0.5f;
  float slopeDeg = std::atan(STEP_H / STEP_DEPTH) * 180.0f / PI; // ≈11.3°

  glPushMatrix();
  // place slightly off-center in x so he occupies one side of the stair
  glTranslatef(-1.2f, stepY, stepZ);
  // align body with stair run along -Z (toward building), no yaw needed
  // tilt to stair slope: head higher toward building (negative z direction)
  glRotatef(slopeDeg, 1.0f, 0.0f, 0.0f);

  // 身体（长大衣）沿 z 方向
  glPushMatrix();
  glScalef(1.4f, 0.55f, 6.0f); // flatter profile
  glColor3f(0.20f, 0.20f, 0.22f);
  glutSolidCube(1.0);
  glPopMatrix();

  // 腿
  glPushMatrix();
  glTranslatef(0.0f, -0.25f, 2.6f); // feet朝下坡方向（+z）
  glScalef(1.2f, 0.55f, 3.2f);
  glColor3f(0.18f, 0.18f, 0.20f);
  glutSolidCube(1.0);
  glPopMatrix();

  // 头（朝向建筑，位于上坡端）
  glPushMatrix();
  glTranslatef(0.0f, 0.45f, -3.4f); // head toward building (-z)
  glColor3f(0.22f, 0.22f, 0.24f);
  glutSolidSphere(0.6f, 14, 10);
  glPopMatrix();

  glPopMatrix();
}

// 整体建筑
void drawStelline() {
  drawPlaza();
  drawSnowField();
  drawBollardLights();
  drawStairs();
  drawMainBuilding();
  drawAnnexBuildings();
  drawAnnexWindows();
  drawAnnexDoors();
  drawSideWindows();
  drawCanopy();
  drawStripLights();
  drawFrontFacade();
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

  // 灰白天空下、略偏冷的散射光
  GLfloat sunDir[] = {-0.3f, 0.9f, 0.2f, 0.0f};
  GLfloat sunDiffuse[] = {0.45f, 0.50f, 0.65f, 1.0f};
  GLfloat sunAmbient[] = {0.12f, 0.14f, 0.18f, 1.0f};
  GLfloat sunSpecular[] = {0.22f, 0.22f, 0.24f, 1.0f};
  glLightfv(GL_LIGHT0, GL_POSITION, sunDir);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, sunDiffuse);
  glLightfv(GL_LIGHT0, GL_AMBIENT, sunAmbient);
  glLightfv(GL_LIGHT0, GL_SPECULAR, sunSpecular);

  GLfloat globalAmb[] = {0.20f, 0.22f, 0.26f, 1.0f};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmb);

  drawGround();
  drawStelline();
  drawSnow();

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
  updateSnow(0.016f); // 约 60 FPS
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

  GLfloat specular[] = {0.3f, 0.3f, 0.32f, 1.0f};
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 24.0f);

  // 夜色天空：深蓝灰
  glClearColor(0.08f, 0.10f, 0.12f, 1.0f);

  // 轻微雾，让远处慢慢消失
  GLfloat fogColor[] = {0.08f, 0.10f, 0.12f, 1.0f};
  glEnable(GL_FOG);
  glFogfv(GL_FOG_COLOR, fogColor);
  glFogi(GL_FOG_MODE, GL_EXP2);
  glFogf(GL_FOG_DENSITY, 0.0025f);
  glHint(GL_FOG_HINT, GL_NICEST);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  initSnow();
}

// ---------------- main ----------------
int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(1280, 720);
  glutCreateWindow("Stelline Memory Lab - Snowfield (FreeGLUT)");

  initGL();

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(special);
  glutTimerFunc(16, timer, 0);

  glutMainLoop();
  return 0;
}
