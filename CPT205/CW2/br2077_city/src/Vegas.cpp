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

// ----------- Glass + Concrete Building 参数 -----------
const int NUM_FLOORS = 6;
const float FLOOR_HEIGHT = 12.0f;
const float BUILDING_HEIGHT = NUM_FLOORS * FLOOR_HEIGHT;
const float GLASS_BUILDING_HALF_W = 60.0f;
const float GLASS_BUILDING_HALF_D = 40.0f;
const float GLASS_ALPHA = 0.30f; // 玻璃透明度（越小越透）
const float SLAB_THICKNESS = 1.0f;
const float COLUMN_SIZE = 2.5f;

// 建筑前的混凝土广场范围
const float PLAZA_HALF_SIZE = 120.0f;

// 雾气（很淡，可以直接改这一行调节；设为 0 就等于关雾）
float g_fogDensity = 0.003f;

// ---------------- 小工具函数 ----------------
float frand(float a, float b) {
  return a + (b - a) * (std::rand() / (float)RAND_MAX);
}

// ---------------- 场景元素结构体 ----------------
struct SlotMachine {
  float x, z, y;
  float rotDeg;
};

struct Table {
  float x, z, y;
  float rotDeg;
};

struct Chair {
  float x, z, y;
  float rotDeg;
};

struct BarCounter {
  float x, z, y;
  float rotDeg;
};

std::vector<SlotMachine> g_slots;
std::vector<Table> g_tables;
std::vector<Chair> g_chairs;
std::vector<BarCounter> g_barCounters;

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
  // 让摆放更有秩序：组合单元，留足安全间距避免重叠
  float marginX = GLASS_BUILDING_HALF_W * 0.78f;
  float marginZ = GLASS_BUILDING_HALF_D * 0.78f;

  // -------- 底层：赌场区（牌桌+4椅+老虎机组合） --------
  float casinoY = 1.0f;
  int clusterCols = 2, clusterRows = 2;
  float clusterSpanX = (marginX * 1.2f) / (clusterCols - 1);
  float clusterSpanZ = (marginZ * 1.2f) / (clusterRows - 1);
  float clusterZStart = -marginZ * 0.30f;

  for (int r = 0; r < clusterRows; ++r) {
    for (int c = 0; c < clusterCols; ++c) {
      float baseX = -marginX * 0.7f + c * clusterSpanX;
      float baseZ = clusterZStart + r * clusterSpanZ;

      // 桌子
      Table t;
      t.x = baseX;
      t.z = baseZ;
      t.y = casinoY;
      t.rotDeg = (r % 2 == 0) ? 0.0f : 90.0f;
      g_tables.push_back(t);

      // 椅子四把，朝向桌心
      float radius = 7.0f; // 增加半径保证不碰撞桌子
      for (int i = 0; i < 4; ++i) {
        float angle = 90.0f * i;
        float rad = angle * PI / 180.0f;
        Chair ch;
        ch.x = baseX + radius * std::cos(rad);
        ch.z = baseZ + radius * std::sin(rad);
        ch.y = casinoY;
        ch.rotDeg = angle + 180.0f;
        g_chairs.push_back(ch);
      }

      // 老虎机：在桌子一侧排两台，远离椅子
      for (int i = 0; i < 2; ++i) {
        SlotMachine s;
        float offset = (i == 0) ? -3.0f : 3.0f;
        s.x = baseX + offset;
        s.z = baseZ - radius - 6.0f;
        s.y = casinoY;
        s.rotDeg = 0.0f;
        g_slots.push_back(s);
      }
    }
  }

  // 补充少量散落老虎机，靠近入口但不重叠
  for (int i = 0; i < 4; ++i) {
    SlotMachine s;
    s.x = frand(-marginX * 0.7f, marginX * 0.7f);
    s.z = frand(-marginZ * 0.95f, -marginZ * 0.6f);
    s.y = casinoY;
    s.rotDeg = 0.0f;
    g_slots.push_back(s);
  }

  // -------- 二层：酒吧休息层（吧台+凳子组合） --------
  float barY = FLOOR_HEIGHT + 1.0f;

  // 两条长吧台
  for (int i = 0; i < 2; ++i) {
    BarCounter b;
    b.x = (i == 0) ? -marginX * 0.35f : marginX * 0.35f;
    b.z = 0.0f;
    b.y = barY;
    b.rotDeg = (i == 0) ? 90.0f : -90.0f;
    g_barCounters.push_back(b);

    // 在吧台前沿排布高脚凳
    int stools = 5;
    float span = 16.0f / (stools - 1);
    for (int k = 0; k < stools; ++k) {
      Chair stool;
      stool.x = b.x;
      stool.z = -6.0f + k * span;
      stool.y = barY;
      stool.rotDeg = (i == 0) ? 90.0f : -90.0f; // 面向吧台
      g_chairs.push_back(stool);
    }
  }

  // 休息沙发组合：2 个小组，每组 2 把面向中心点的椅子（不放咖啡桌以免体积冲突）
  for (int g = 0; g < 2; ++g) {
    float centerX = -marginX * 0.35f + g * (marginX * 0.7f);
    float centerZ = marginZ * 0.45f;
    for (int i = 0; i < 2; ++i) {
      Chair seat;
      seat.x = centerX + (i == 0 ? -4.5f : 4.5f);
      seat.z = centerZ;
      seat.y = barY;
      seat.rotDeg = (i == 0) ? 0.0f : 180.0f;
      g_chairs.push_back(seat);
    }
  }

  // -------- 酒店客房层（3-6层）：床+双灯组合 --------
  float roomMarginX = GLASS_BUILDING_HALF_W * 0.70f;
  float roomMarginZ = GLASS_BUILDING_HALF_D * 0.70f;
  int roomCols = 3, roomRows = 2;
  float cellX = (roomMarginX * 2.0f) / roomCols;
  float cellZ = (roomMarginZ * 2.0f) / roomRows;

  for (int floor = 2; floor < NUM_FLOORS; ++floor) {
    float roomY = floor * FLOOR_HEIGHT + 1.0f;
    for (int r = 0; r < roomRows; ++r) {
      for (int c = 0; c < roomCols; ++c) {
        // 留出中轴走廊
        if (c == 1 && std::abs(-roomMarginZ + (r + 0.5f) * cellZ) < 4.0f)
          continue;

        float px = -roomMarginX + (c + 0.5f) * cellX;
        float pz = -roomMarginZ + (r + 0.5f) * cellZ;

        Table bed; // 床体
        bed.x = px;
        bed.z = pz;
        bed.y = roomY;
        bed.rotDeg = (c < 1) ? 0.0f : 180.0f;
        g_tables.push_back(bed);

        // 床头灯两盏，靠床头一侧
        for (int i = -1; i <= 1; i += 2) {
          Chair lamp;
          lamp.x = px + 1.8f * i;
          lamp.z = pz + ((c < 1) ? 2.0f : -2.0f);
          lamp.y = roomY + 1.5f;
          lamp.rotDeg = 0.0f;
          g_chairs.push_back(lamp);
        }
      }
    }
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

  // 建筑前的混凝土广场
  glBegin(GL_QUADS);
  glColor3f(0.60f, 0.50f, 0.40f);
  glVertex3f(-PLAZA_HALF_SIZE, 0.0f, PLAZA_HALF_SIZE);
  glVertex3f(PLAZA_HALF_SIZE, 0.0f, PLAZA_HALF_SIZE);
  glVertex3f(PLAZA_HALF_SIZE, 0.0f, -PLAZA_HALF_SIZE);
  glVertex3f(-PLAZA_HALF_SIZE, 0.0f, -PLAZA_HALF_SIZE);
  glEnd();

  glEnable(GL_LIGHTING);
}

void drawGlassBuildingBase() {
  // 深色地坪，方便看清玻璃内的装饰物
  glPushMatrix();
  glDisable(GL_LIGHTING);
  glBegin(GL_QUADS);
  glColor3f(0.20f, 0.20f, 0.22f);
  glVertex3f(-GLASS_BUILDING_HALF_W, 0.05f, -GLASS_BUILDING_HALF_D);
  glVertex3f(GLASS_BUILDING_HALF_W, 0.05f, -GLASS_BUILDING_HALF_D);
  glVertex3f(GLASS_BUILDING_HALF_W, 0.05f, GLASS_BUILDING_HALF_D);
  glVertex3f(-GLASS_BUILDING_HALF_W, 0.05f, GLASS_BUILDING_HALF_D);
  glEnd();
  glEnable(GL_LIGHTING);
  glPopMatrix();
}

void drawConcreteFrameAndSlabs() {
  glColor3f(0.45f, 0.44f, 0.42f); // 混凝土灰

  // 四角立柱
  auto drawColumn = [](float x, float z) {
    glPushMatrix();
    glTranslatef(x, BUILDING_HEIGHT * 0.5f, z);
    glScalef(COLUMN_SIZE, BUILDING_HEIGHT, COLUMN_SIZE);
    glutSolidCube(1.0);
    glPopMatrix();
  };

  drawColumn(-GLASS_BUILDING_HALF_W, -GLASS_BUILDING_HALF_D);
  drawColumn(GLASS_BUILDING_HALF_W, -GLASS_BUILDING_HALF_D);
  drawColumn(GLASS_BUILDING_HALF_W, GLASS_BUILDING_HALF_D);
  drawColumn(-GLASS_BUILDING_HALF_W, GLASS_BUILDING_HALF_D);

  // 底部混凝土裙座，加厚基座视觉
  glPushMatrix();
  glTranslatef(0.0f, 3.0f, 0.0f);
  glScalef(GLASS_BUILDING_HALF_W * 2.0f + 6.0f, 6.0f,
           GLASS_BUILDING_HALF_D * 2.0f + 6.0f);
  glColor3f(0.42f, 0.40f, 0.38f);
  glutSolidCube(1.0);
  glPopMatrix();

  // 中轴电梯核心
  glPushMatrix();
  glTranslatef(0.0f, BUILDING_HEIGHT * 0.5f, 0.0f);
  glScalef(12.0f, BUILDING_HEIGHT, 12.0f);
  glColor3f(0.35f, 0.34f, 0.33f);
  glutSolidCube(1.0);
  glPopMatrix();

  // 每层楼板
  for (int i = 0; i <= NUM_FLOORS; ++i) {
    float y = i * FLOOR_HEIGHT;
    glPushMatrix();
    glTranslatef(0.0f, y, 0.0f);
    glScalef(GLASS_BUILDING_HALF_W * 2.0f + 2.0f, SLAB_THICKNESS,
             GLASS_BUILDING_HALF_D * 2.0f + 2.0f);
    glColor3f(0.50f, 0.49f, 0.47f);
    glutSolidCube(1.0);
    glPopMatrix();
  }

  // 楼层腰线：在每层上方加混凝土外檐
  for (int i = 1; i <= NUM_FLOORS; ++i) {
    float y = i * FLOOR_HEIGHT - 0.5f;
    glPushMatrix();
    glTranslatef(0.0f, y, 0.0f);
    glScalef(GLASS_BUILDING_HALF_W * 2.0f + 4.0f, 0.8f,
             GLASS_BUILDING_HALF_D * 2.0f + 4.0f);
    glColor3f(0.46f, 0.45f, 0.44f);
    glutSolidCube(1.0);
    glPopMatrix();
  }

  // 垂直混凝土肋板：前后立面各 5 片
  auto drawRibStrip = [](float x, float zSign) {
    for (int i = 0; i < 5; ++i) {
      float offset = -GLASS_BUILDING_HALF_W + (i + 0.5f) *
                                         (GLASS_BUILDING_HALF_W * 2.0f / 5.0f);
      glPushMatrix();
      glTranslatef(offset, BUILDING_HEIGHT * 0.5f, zSign * (GLASS_BUILDING_HALF_D + 0.6f));
      glScalef(3.0f, BUILDING_HEIGHT, 1.2f);
      glColor3f(0.43f, 0.42f, 0.41f);
      glutSolidCube(1.0);
      glPopMatrix();
    }
  };
  drawRibStrip(0.0f, 1.0f);
  drawRibStrip(0.0f, -1.0f);

  // 侧面竖向翼墙各两片，提升混凝土存在感
  for (int side = -1; side <= 1; side += 2) {
    for (int i = 0; i < 2; ++i) {
      float z = -GLASS_BUILDING_HALF_D + (i + 0.5f) *
                                    (GLASS_BUILDING_HALF_D * 2.0f / 2.0f);
      glPushMatrix();
      glTranslatef(side * (GLASS_BUILDING_HALF_W + 0.6f), BUILDING_HEIGHT * 0.5f, z);
      glScalef(1.2f, BUILDING_HEIGHT, 6.0f);
      glColor3f(0.43f, 0.42f, 0.41f);
      glutSolidCube(1.0);
      glPopMatrix();
    }
  }

  // 屋顶女儿墙
  glPushMatrix();
  glTranslatef(0.0f, BUILDING_HEIGHT + 1.0f, 0.0f);
  glScalef(GLASS_BUILDING_HALF_W * 2.0f + 3.0f, 2.0f,
           GLASS_BUILDING_HALF_D * 2.0f + 3.0f);
  glColor3f(0.44f, 0.43f, 0.42f);
  glutSolidCube(1.0);
  glPopMatrix();

  // 外环梁线（简单线条强调层次）
  glDisable(GL_LIGHTING);
  glColor3f(0.60f, 0.58f, 0.55f);
  glBegin(GL_LINES);
  for (int i = 0; i <= NUM_FLOORS; ++i) {
    float y = i * FLOOR_HEIGHT;
    glVertex3f(-GLASS_BUILDING_HALF_W, y, -GLASS_BUILDING_HALF_D);
    glVertex3f(GLASS_BUILDING_HALF_W, y, -GLASS_BUILDING_HALF_D);

    glVertex3f(GLASS_BUILDING_HALF_W, y, -GLASS_BUILDING_HALF_D);
    glVertex3f(GLASS_BUILDING_HALF_W, y, GLASS_BUILDING_HALF_D);

    glVertex3f(GLASS_BUILDING_HALF_W, y, GLASS_BUILDING_HALF_D);
    glVertex3f(-GLASS_BUILDING_HALF_W, y, GLASS_BUILDING_HALF_D);

    glVertex3f(-GLASS_BUILDING_HALF_W, y, GLASS_BUILDING_HALF_D);
    glVertex3f(-GLASS_BUILDING_HALF_W, y, -GLASS_BUILDING_HALF_D);
  }
  glEnd();
  glEnable(GL_LIGHTING);
}

void drawCurtainWall() {
  glPushMatrix();
  glColor4f(0.55f, 0.82f, 1.0f, GLASS_ALPHA);
  glDepthMask(GL_FALSE); // 避免完全遮挡内部装饰

  int colsLong = 8;
  int colsShort = 6;
  float panelH = FLOOR_HEIGHT - 1.5f;
  float spanX = GLASS_BUILDING_HALF_W * 2.0f;
  float spanZ = GLASS_BUILDING_HALF_D * 2.0f;

  // 正反面（沿 x）
  for (int side = -1; side <= 1; side += 2) {
    for (int floor = 0; floor < NUM_FLOORS; ++floor) {
      float y0 = floor * FLOOR_HEIGHT + 0.5f;
      float y1 = y0 + panelH;
      for (int c = 0; c < colsLong; ++c) {
        float x0 = -GLASS_BUILDING_HALF_W + (spanX / colsLong) * c + 0.4f;
        float x1 = -GLASS_BUILDING_HALF_W + (spanX / colsLong) * (c + 1) - 0.4f;
        float z = side * GLASS_BUILDING_HALF_D;
        glBegin(GL_QUADS);
        glVertex3f(x0, y0, z);
        glVertex3f(x1, y0, z);
        glVertex3f(x1, y1, z);
        glVertex3f(x0, y1, z);
        glEnd();
      }
    }
  }

  // 侧面（沿 z）
  for (int side = -1; side <= 1; side += 2) {
    for (int floor = 0; floor < NUM_FLOORS; ++floor) {
      float y0 = floor * FLOOR_HEIGHT + 0.5f;
      float y1 = y0 + panelH;
      for (int c = 0; c < colsShort; ++c) {
        float z0 = -GLASS_BUILDING_HALF_D + (spanZ / colsShort) * c + 0.4f;
        float z1 = -GLASS_BUILDING_HALF_D + (spanZ / colsShort) * (c + 1) - 0.4f;
        float x = side * GLASS_BUILDING_HALF_W;
        glBegin(GL_QUADS);
        glVertex3f(x, y0, z0);
        glVertex3f(x, y0, z1);
        glVertex3f(x, y1, z1);
        glVertex3f(x, y1, z0);
        glEnd();
      }
    }
  }

  glDepthMask(GL_TRUE);
  glPopMatrix();
}

void drawSlotMachine(const SlotMachine &s) {
  glPushMatrix();
  glTranslatef(s.x, s.y, s.z);
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
  glTranslatef(t.x, t.y, t.z);
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
  glTranslatef(c.x, c.y, c.z);
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

void drawBarCounter(const BarCounter &b) {
  glPushMatrix();
  glTranslatef(b.x, b.y, b.z);
  glRotatef(b.rotDeg, 0.0f, 1.0f, 0.0f);

  // 吧台主体
  glPushMatrix();
  glTranslatef(0.0f, 2.5f, 0.0f);
  glScalef(18.0f, 5.0f, 3.5f);
  glColor3f(0.26f, 0.23f, 0.22f);
  glutSolidCube(1.0);
  glPopMatrix();

  // 台面亮色
  glPushMatrix();
  glTranslatef(0.0f, 5.0f, 0.0f);
  glScalef(18.5f, 0.8f, 3.8f);
  glColor3f(0.85f, 0.76f, 0.60f);
  glutSolidCube(1.0);
  glPopMatrix();

  // 背景瓶架（用小立方体模拟）
  for (int i = -3; i <= 3; ++i) {
    glPushMatrix();
    glTranslatef(i * 2.2f, 6.0f, -2.0f);
    glScalef(0.6f, 1.8f, 0.6f);
    glColor3f(0.6f, 0.8f, 0.9f);
    glutSolidCube(1.0);
    glPopMatrix();
  }

  // 高脚凳
  for (int i = -3; i <= 3; ++i) {
    glPushMatrix();
    glTranslatef(i * 2.6f, 0.0f, 2.8f);
    glScalef(1.0f, 3.0f, 1.0f);
    glColor3f(0.22f, 0.22f, 0.24f);
    glutSolidCube(1.0);
    glPopMatrix();
  }

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
  drawGlassBuildingBase();
  drawConcreteFrameAndSlabs();

  for (const auto &s : g_slots)
    drawSlotMachine(s);
  for (const auto &t : g_tables)
    drawTable(t);
  for (const auto &c : g_chairs)
    drawChair(c);
  for (const auto &b : g_barCounters)
    drawBarCounter(b);

  // 最后绘制玻璃外壳，确保内部内容透过玻璃可见
  drawCurtainWall();

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
  glutCreateWindow("Radioactive Las Vegas Glass Tower (FreeGLUT)");

  initGL();

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(special);
  glutTimerFunc(16, timer, 0);

  glutMainLoop();
  return 0;
}
