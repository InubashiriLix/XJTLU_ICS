#include <GL/freeglut.h>
#include <cmath>
#include <cstdlib>
#include <vector>

const float PI = 3.14159265f;

// ---------------- Camera ----------------
float camX = 0.0f;
float camY = 60.0f;
float camZ = 220.0f;
float yaw = 0.0f;     // 左右转动
float pitch = -15.0f; // 上下俯仰

const float MOVE_SPEED = 3.0f;
const float ROT_SPEED = 2.0f;

// ---------------- Trash Mesa 参数 ----------------
const float MESA_TOP_Y = 8.0f;        // 垃圾高地顶部高度
const float MESA_THICKNESS = 16.0f;   // Mesa 厚度
const float MESA_HALF_SIZE = 260.0f;  // 高地半宽（扩大覆盖范围）
const float MESA_BUMP_HEIGHT = 60.0f; // Mesa mound 额外高度
const float DUNE_BASE_Y = -30.0f;     // 沙面基准高度

// ---------------- 雾气参数（你可以直接改这里） ----------------
float g_fogDensity = 0.002f; // 越大雾越浓，改一个数字就行

// ---------------- 小工具函数 ----------------
float frand(float a, float b) {
  return a + (b - a) * (std::rand() / (float)RAND_MAX);
}

// Base ground height (flattened — no surrounding dunes)
float getDuneHeight(float x, float z) {
  (void)x;
  (void)z;
  return DUNE_BASE_Y;
}

// Helper to get the height of the central Mesa Dune at (x, z)
float getMesaY(float x, float z) {
  float base = getDuneHeight(x, z);
  float dist = std::sqrt(x * x + z * z);
  float maxR = MESA_HALF_SIZE; // 180.0f

  if (dist > maxR) {
    // Outside Mesa radius, follow the dunes completely
    return base;
  }

  // Cosine hill shape: Peak at center, smooth drop to edge
  float normalizedPos = dist / maxR;                                 // 0 to 1
  float heightFactor = (1.0f + std::cos(normalizedPos * PI)) * 0.5f; // 1 to 0

  // Raise the dune by an extra bump, keeping continuity at the border
  return base + MESA_BUMP_HEIGHT * heightFactor;
}

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

// ---------------- 垃圾场类 ----------------
class TrashYard {
public:
  TrashYard() = default;

  void init() {
    std::srand(1337);

    initDebris();
    initCars();
    // initCranes(); // Removed Cranes
    initCrushers();
    initShips();
  }

  void update(float dt) {
    // 垃圾船绕场转圈
    for (auto &s : ships_) {
      s.angleDeg += s.speedDeg * dt * 60.0f; // 把 dt 大致折算回“帧”
      if (s.angleDeg > 360.0f)
        s.angleDeg -= 360.0f;
    }
  }

  void draw() {
    drawDunes(); // Replaced drawFloodedSea with drawDunes
    drawMesa();

    for (const auto &d : debris_)
      drawDebrisChunk(d);
    for (const auto &c : cars_)
      drawCarWreck(c);
    // for (const auto &cr : cranes_) // Removed Cranes
    //   drawCrane(cr);
    for (const auto &cru : crushers_)
      drawCrusher(cru);
    for (const auto &s : ships_)
      drawShip(s);
  }

private:
  struct DebrisChunk {
    float x, z;
    float sx, sy, sz;
    float rotY;
    float gray; // 用来稍微变一下颜色
  };

  struct CarWreck {
    float x, z;
    float rotDeg;
    float scale;
  };

  struct Crane {
    float x, z;
    float height;
  };

  struct Crusher {
    float x, z;
    float width;
    float depth;
  };

  struct Ship {
    float angleDeg; // 绕中心旋转角
    float radius;   // 轨道半径
    float height;   // 基础高度
    float speedDeg; // 每帧角速度
  };

  std::vector<DebrisChunk> debris_;
  std::vector<CarWreck> cars_;
  std::vector<Crane> cranes_;
  std::vector<Crusher> crushers_;
  std::vector<Ship> ships_;

  // -------------- Utility --------------
  bool tooClose(const std::vector<DebrisChunk> &arr, float x, float z,
                float minDist2) {
    for (const auto &d : arr) {
      float dx = x - d.x;
      float dz = z - d.z;
      if (dx * dx + dz * dz < minDist2)
        return true;
    }
    return false;
  }

  bool tooClose(const std::vector<CarWreck> &arr, float x, float z,
                float minDist2) {
    for (const auto &c : arr) {
      float dx = x - c.x;
      float dz = z - c.z;
      if (dx * dx + dz * dz < minDist2)
        return true;
    }
    return false;
  }

  bool tooCloseAny(float x, float z, float minDist2) {
    if (tooClose(debris_, x, z, minDist2))
      return true;
    if (tooClose(cars_, x, z, minDist2))
      return true;
    return false;
  }

  // ---------- 初始化 ----------
  void initDebris() {
    // 散落在 Mesa 顶部的碎块
    const int NUM_DEBRIS = 650;
    debris_.reserve(NUM_DEBRIS);
    for (int i = 0; i < NUM_DEBRIS; ++i) {
      DebrisChunk d;
      int tries = 0;
      const float minDist = 3.0f;
      bool placed = false;
      while (!placed && tries < 300) {
        d.x = frand(-MESA_HALF_SIZE * 0.95f, MESA_HALF_SIZE * 0.95f);
        d.z = frand(-MESA_HALF_SIZE * 0.95f, MESA_HALF_SIZE * 0.95f);
        float minDist2 = (minDist * minDist);
        if (!tooClose(debris_, d.x, d.z, minDist2))
          placed = true;
        ++tries;
      }
      d.sx = frand(1.0f, 4.5f);
      d.sy = frand(0.4f, 2.5f);
      d.sz = frand(1.0f, 4.0f);
      d.rotY = frand(0.0f, 360.0f);
      d.gray = frand(0.22f, 0.58f);
      debris_.push_back(d);
    }
  }

  void initCars() {
    const int NUM_CARS = 28;
    for (int i = 0; i < NUM_CARS; ++i) {
      CarWreck c;
      int tries = 0;
      const float minDist = 10.0f;
      bool placed = false;
      while (!placed && tries < 300) {
        c.x = frand(-MESA_HALF_SIZE * 0.8f, MESA_HALF_SIZE * 0.8f);
        c.z = frand(-MESA_HALF_SIZE * 0.8f, MESA_HALF_SIZE * 0.8f);
        float minDist2 = (minDist * minDist);
        if (!tooClose(cars_, c.x, c.z, minDist2) &&
            !tooClose(debris_, c.x, c.z, minDist2 * 0.7f))
          placed = true;
        ++tries;
      }
      c.rotDeg = frand(0.0f, 360.0f);
      c.scale = frand(0.8f, 1.3f);
      cars_.push_back(c);
    }
  }

  void initCranes() {
    Crane c1{-MESA_HALF_SIZE * 0.6f, 40.0f, 70.0f};
    Crane c2{MESA_HALF_SIZE * 0.4f, -50.0f, 80.0f};
    Crane c3{0.0f, MESA_HALF_SIZE * 0.7f, 65.0f};
    cranes_.push_back(c1);
    cranes_.push_back(c2);
    cranes_.push_back(c3);
  }

  void initCrushers() {
    Crusher a{-40.0f, 20.0f, 40.0f, 26.0f};
    Crusher b{50.0f, -10.0f, 32.0f, 20.0f};
    crushers_.push_back(a);
    crushers_.push_back(b);
  }

  void initShips() {
    for (int i = 0; i < 3; ++i) {
      Ship s;
      s.angleDeg = frand(0.0f, 360.0f);
      s.radius =
          (MESA_HALF_SIZE + 60.0f) + i * 40.0f; // stay outside enlarged mesa
      s.height = 90.0f + i * 10.0f;
      s.speedDeg = 0.12f + 0.04f * i;
      ships_.push_back(s);
    }
  }

  // ---------- 绘制 ----------
  void drawDunes() {
    glDisable(GL_LIGHTING);
    glColor3f(0.76f, 0.70f, 0.50f); // Sand color

    float size = 800.0f;
    int steps = 100;
    float stepSize = size * 2.0f / steps;

    for (int z = 0; z < steps; ++z) {
      glBegin(GL_TRIANGLE_STRIP);
      for (int x = 0; x <= steps; ++x) {
        float xPos = -size + x * stepSize;
        float zPos = -size + z * stepSize;
        float zPosNext = -size + (z + 1) * stepSize;

        // Flat ground at base height
        float y1 = DUNE_BASE_Y;
        float y2 = DUNE_BASE_Y;

        // Normal straight up for flat ground
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(xPos, y1, zPos);
        glVertex3f(xPos, y2, zPosNext);
      }
      glEnd();
    }
    glEnable(GL_LIGHTING);
  }

  void drawMesa() {
    // Draw the central Mesa as a large dune
    glDisable(GL_LIGHTING);
    // Slightly darker/different sand color for the mesa to distinguish it
    glColor3f(0.65f, 0.60f, 0.45f);

    int steps = 60;
    float stepSize = MESA_HALF_SIZE * 2.0f / steps;
    float start = -MESA_HALF_SIZE;

    for (int z = 0; z < steps; ++z) {
      glBegin(GL_TRIANGLE_STRIP);
      for (int x = 0; x <= steps; ++x) {
        float xPos = start + x * stepSize;
        float zPos = start + z * stepSize;
        float zPosNext = start + (z + 1) * stepSize;

        // Calculate heights
        float y1 = getMesaY(xPos, zPos);
        float y2 = getMesaY(xPos, zPosNext);

        // Calculate normals (approximate by gradient)
        // For a proper lighting, we'd calculate derivatives, but up-vector is
        // okay for soft dunes Or simple central-difference normal
        float delta = 1.0f;
        float hL = getMesaY(xPos - delta, zPos);
        float hR = getMesaY(xPos + delta, zPos);
        float hD = getMesaY(xPos, zPos - delta);
        float hU = getMesaY(xPos, zPos + delta);

        glNormal3f(hL - hR, 2.0f * delta, hD - hU); // Very rough normal
        glVertex3f(xPos, y1, zPos);

        float hL2 = getMesaY(xPos - delta, zPosNext);
        float hR2 = getMesaY(xPos + delta, zPosNext);
        float hD2 = getMesaY(xPos, zPosNext - delta);
        float hU2 = getMesaY(xPos, zPosNext + delta);

        glNormal3f(hL2 - hR2, 2.0f * delta, hD2 - hU2);
        glVertex3f(xPos, y2, zPosNext);
      }
      glEnd();
    }
    glEnable(GL_LIGHTING);
  }

  void drawDebrisChunk(const DebrisChunk &d) {
    glPushMatrix();
    float y = getMesaY(d.x, d.z);
    // small lift to avoid z-fighting/shaking with ground
    glTranslatef(d.x, y + d.sy * 0.5f + 0.2f, d.z);
    glRotatef(d.rotY, 0.0f, 1.0f, 0.0f);
    float g = d.gray;
    // 70% chance: box; 30% chance: crushed cylinder look
    float choice = frand(0.0f, 1.0f);
    if (choice < 0.7f) {
      glScalef(d.sx, d.sy, d.sz);
      glColor3f(g * 1.05f, g * 0.95f, g * 0.9f);
      glutSolidCube(1.0);
    } else {
      glColor3f(g * 1.0f, g * 0.8f, g * 0.85f);
      glScalef(d.sx * 0.8f, d.sy, d.sz * 0.8f);
      glutSolidCylinder(0.6, 1.4, 8, 1);
      glTranslatef(0.0f, d.sy * 0.3f, 0.0f);
      glRotatef(15.0f, 0.0f, 0.0f, 1.0f);
      glutSolidCylinder(0.5, 0.9, 8, 1);
    }
    glPopMatrix();
  }

  void drawCarWreck(const CarWreck &c) {
    glPushMatrix();
    float y = getMesaY(c.x, c.z);
    // slight lift to avoid ground z-fighting
    glTranslatef(c.x, y + 1.1f, c.z);
    glRotatef(c.rotDeg, 0.0f, 1.0f, 0.0f);
    // Tilt the car slightly to match terrain slope?
    // For simplicity, just keep upright or random tilt.

    glScalef(c.scale, c.scale, c.scale);

    // Rusty/Wrecked color
    float bodyR = 0.55f;
    float bodyG = 0.35f;
    float bodyB = 0.25f;

    // Main Body (Lower)
    glPushMatrix();
    glScalef(6.0f, 1.6f, 3.0f);
    glColor3f(bodyR, bodyG, bodyB);
    glutSolidCube(1.0);
    glPopMatrix();

    // Cabin (Upper) - slightly smaller and offset
    glPushMatrix();
    glTranslatef(0.0f, 1.4f, 0.0f);
    glScalef(3.5f, 1.4f, 2.5f);
    glColor3f(bodyR * 0.9f, bodyG * 0.9f, bodyB * 0.9f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Hood (Front)
    glPushMatrix();
    glTranslatef(2.0f, 0.5f, 0.0f);
    glScalef(2.0f, 0.1f, 2.8f);
    glColor3f(bodyR * 0.8f, bodyG * 0.8f, bodyB * 0.8f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Trunk (Back)
    glPushMatrix();
    glTranslatef(-2.0f, 0.5f, 0.0f);
    glScalef(2.0f, 0.1f, 2.8f);
    glColor3f(bodyR * 0.8f, bodyG * 0.8f, bodyB * 0.8f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Wheels (Torus for better look)
    glColor3f(0.1f, 0.1f, 0.1f);

    // Front Left
    glPushMatrix();
    glTranslatef(2.0f, -0.2f, 1.5f);
    glutSolidTorus(0.3, 0.6, 8, 16);
    glPopMatrix();

    // Front Right
    glPushMatrix();
    glTranslatef(2.0f, -0.2f, -1.5f);
    glutSolidTorus(0.3, 0.6, 8, 16);
    glPopMatrix();

    // Back Left
    glPushMatrix();
    glTranslatef(-2.0f, -0.2f, 1.5f);
    glutSolidTorus(0.3, 0.6, 8, 16);
    glPopMatrix();

    // Back Right
    glPushMatrix();
    glTranslatef(-2.0f, -0.2f, -1.5f);
    glutSolidTorus(0.3, 0.6, 8, 16);
    glPopMatrix();

    // Debris/Damage details (random lines)
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    glColor3f(0.2f, 0.2f, 0.2f);
    glVertex3f(-1.0f, 2.2f, 1.3f);
    glVertex3f(1.0f, 2.2f, 1.3f); // Roof rack?
    glVertex3f(-1.0f, 2.2f, -1.3f);
    glVertex3f(1.0f, 2.2f, -1.3f);

    // Broken windshield
    glColor3f(0.6f, 0.7f, 0.8f);
    glVertex3f(1.5f, 1.0f, 1.0f);
    glVertex3f(1.8f, 2.0f, 0.0f);
    glVertex3f(1.5f, 1.0f, -1.0f);
    glVertex3f(1.8f, 2.0f, 0.0f);
    glEnd();
    glEnable(GL_LIGHTING);

    glPopMatrix();
  }

  void drawCrane(const Crane &c) {
    float baseY = MESA_TOP_Y;

    // 塔柱（灰沉色）
    glPushMatrix();
    glTranslatef(c.x, baseY + c.height * 0.5f, c.z);
    glScalef(2.2f, c.height, 2.2f);
    glColor3f(0.40f, 0.40f, 0.42f);
    glutSolidCube(1.0);
    glPopMatrix();

    // 水平臂
    glPushMatrix();
    float armY = baseY + c.height + 2.0f;
    glTranslatef(c.x, armY, c.z);
    glScalef(40.0f, 1.6f, 1.6f);
    glColor3f(0.45f, 0.45f, 0.47f);
    glutSolidCube(1.0);
    glPopMatrix();

    // 吊钩钢缆
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    glColor3f(0.25f, 0.25f, 0.25f);
    float cableX = c.x + 20.0f;
    glVertex3f(cableX, armY, c.z);
    glVertex3f(cableX, MESA_TOP_Y + 12.0f, c.z);
    glEnd();
    glEnable(GL_LIGHTING);
  }

  void drawCrusher(const Crusher &cr) {
    float y = getMesaY(cr.x, cr.z);

    // 机体（粉碎箱）- main body
    glPushMatrix();
    glTranslatef(cr.x, y + 6.0f, cr.z);
    glScalef(cr.width, 12.0f, cr.depth);
    glColor3f(0.32f, 0.33f, 0.35f);
    glutSolidCube(1.0);
    glPopMatrix();

    // 顶部护栏
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    glColor3f(0.75f, 0.75f, 0.75f);
    float railHalfW = cr.width * 0.5f;
    float railHalfD = cr.depth * 0.5f;
    float railY = y + 12.0f;
    glVertex3f(cr.x - railHalfW, railY, cr.z - railHalfD);
    glVertex3f(cr.x + railHalfW, railY, cr.z - railHalfD);
    glVertex3f(cr.x - railHalfW, railY, cr.z + railHalfD);
    glVertex3f(cr.x + railHalfW, railY, cr.z + railHalfD);
    glVertex3f(cr.x - railHalfW, railY, cr.z - railHalfD);
    glVertex3f(cr.x - railHalfW, railY, cr.z + railHalfD);
    glVertex3f(cr.x + railHalfW, railY, cr.z - railHalfD);
    glVertex3f(cr.x + railHalfW, railY, cr.z + railHalfD);
    glEnd();
    glEnable(GL_LIGHTING);

    // 进料口
    glPushMatrix();
    glTranslatef(cr.x, y + 5.0f, cr.z - cr.depth * 0.6f);
    glScalef(cr.width * 0.8f, 6.0f, cr.depth * 0.6f);
    glColor3f(0.26f, 0.26f, 0.28f);
    glutSolidCube(1.0);
    glPopMatrix();

    // 内部牙齿线条 + 侧面铆钉
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    glColor3f(0.85f, 0.85f, 0.85f);
    int teeth = 12;
    for (int i = 0; i < teeth; ++i) {
      float t = (i / (float)(teeth - 1) - 0.5f) * cr.width * 0.7f;
      glVertex3f(cr.x + t, y + 2.0f, cr.z - cr.depth * 0.6f);
      glVertex3f(cr.x + t, y + 8.0f, cr.z - cr.depth * 0.6f - 3.0f);
    }
    glEnd();

    glPointSize(3.0f);
    glBegin(GL_POINTS);
    glColor3f(0.7f, 0.7f, 0.7f);
    for (int i = 0; i < 16; ++i) {
      float t = (i / 15.0f - 0.5f) * cr.width * 0.9f;
      glVertex3f(cr.x + t, y + 6.0f, cr.z + cr.depth * 0.5f + 0.1f);
      glVertex3f(cr.x + t, y + 6.0f, cr.z - cr.depth * 0.5f - 0.1f);
    }
    glEnd();
    glEnable(GL_LIGHTING);

    // 排料传送带
    glPushMatrix();
    glTranslatef(cr.x, y + 1.5f, cr.z + cr.depth * 0.6f);
    glRotatef(-18.0f, 1.0f, 0.0f, 0.0f);
    glScalef(cr.width * 0.8f, 1.5f, cr.depth * 0.9f);
    glColor3f(0.20f, 0.21f, 0.22f);
    glutSolidCube(1.0);
    glPopMatrix();
  }

  void drawShip(const Ship &s) {
    float rad = s.angleDeg * PI / 180.0f;
    float x = std::cos(rad) * s.radius;
    float z = std::sin(rad) * s.radius;
    float y = s.height + std::sin(rad * 0.7f) * 2.0f; // 轻微上下起伏

    // 是否正在倾倒垃圾
    bool dumping = std::sin(rad * 2.0f) > 0.6f;

    glPushMatrix();
    glTranslatef(x, y, z);
    // Forward tangent along circular path
    float sign = (s.speedDeg >= 0.0f) ? 1.0f : -1.0f;
    float dirX = -std::sin(rad) * sign;
    float dirZ = std::cos(rad) * sign;
    float headingRad = std::atan2(-dirZ, dirX); // map to model's +X forward
    float headingDeg = headingRad * 180.0f / PI;
    glRotatef(headingDeg, 0.0f, 1.0f, 0.0f);

    float shipLen = 50.0f;
    float shipHeight = 10.0f;
    float shipWidth = 16.0f;

    // 船壳（分层收腰）
    glPushMatrix();
    glScalef(shipLen, shipHeight * 0.65f, shipWidth);
    glColor3f(0.28f, 0.29f, 0.31f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, shipHeight * 0.35f, 0.0f);
    glScalef(shipLen * 0.9f, shipHeight * 0.4f, shipWidth * 0.92f);
    glColor3f(0.30f, 0.31f, 0.33f);
    glutSolidCube(1.0);
    glPopMatrix();

    // 甲板货舱
    glPushMatrix();
    glTranslatef(-5.0f, shipHeight * 0.4f, 0.0f);
    glScalef(shipLen * 0.7f, shipHeight * 0.6f, shipWidth * 0.8f);
    glColor3f(0.35f, 0.36f, 0.38f);
    glutSolidCube(1.0);
    glPopMatrix();

    // 驾驶楼
    glPushMatrix();
    glTranslatef(-shipLen * 0.15f, shipHeight * 0.9f, 0.0f);
    glScalef(shipLen * 0.25f, shipHeight * 0.8f, shipWidth * 0.6f);
    glColor3f(0.40f, 0.41f, 0.43f);
    glutSolidCube(1.0);
    glPopMatrix();

    // 烟囱
    glPushMatrix();
    glTranslatef(-shipLen * 0.05f, shipHeight * 1.4f, 0.0f);
    glColor3f(0.45f, 0.20f, 0.18f);
    glutSolidCylinder(1.4, 5.5, 10, 1);
    glTranslatef(0.0f, 5.5f, 0.0f);
    glColor3f(0.12f, 0.12f, 0.12f);
    glutSolidCylinder(1.2, 1.2, 10, 1);
    glPopMatrix();

    // （移除装饰线条，保持简洁）

    // 船首斜切
    glPushMatrix();
    glTranslatef(shipLen * 0.5f, shipHeight * 0.25f, 0.0f);
    glRotatef(20.0f, 0.0f, 1.0f, 0.0f);
    glScalef(shipLen * 0.12f, shipHeight * 0.9f, shipWidth * 0.85f);
    glColor3f(0.26f, 0.27f, 0.29f);
    glutSolidCube(1.0);
    glPopMatrix();

    // 倾倒垃圾：一串小方块
    if (dumping) {
      glPushMatrix();
      glTranslatef(0.0f, -shipHeight * 0.5f, 0.0f); // 船底为原点
      int pieces = 14;
      for (int i = 0; i < pieces; ++i) {
        glPushMatrix();
        float drop = (i + 1) * 3.0f;
        float offsetX = ((i % 3) - 1) * 1.8f;
        float offsetZ = 2.0f;
        glTranslatef(offsetX, -drop, offsetZ);
        glScalef(1.4f, 1.4f, 1.4f);
        glColor3f(0.45f, 0.40f, 0.32f);
        glutSolidCube(1.0);
        glPopMatrix();
      }
      glPopMatrix();
    }

    // 防撞灯
    glColor3f(1.0f, 0.2f, 0.2f);
    glutSolidSphere(1.0, 12, 12); // 船头
    glPushMatrix();
    glTranslatef(shipLen * 0.25f, shipHeight * 0.7f, shipWidth * 0.4f);
    glutSolidSphere(0.8, 12, 12);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(shipLen * 0.25f, shipHeight * 0.7f, -shipWidth * 0.4f);
    glutSolidSphere(0.8, 12, 12);
    glPopMatrix();

    glEnable(GL_LIGHTING);
    glPopMatrix();
  }
};

// 全局垃圾场实例
TrashYard g_trashYard;

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

  // 阴沉的灰黄雾天光照
  GLfloat sunDir[] = {-0.4f, 0.8f, 0.3f, 0.0f};
  GLfloat sunDiffuse[] = {0.8f, 0.75f, 0.6f, 1.0f};
  GLfloat sunAmbient[] = {0.35f, 0.32f, 0.25f, 1.0f};
  GLfloat sunSpecular[] = {0.3f, 0.28f, 0.25f, 1.0f};
  glLightfv(GL_LIGHT0, GL_POSITION, sunDir);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, sunDiffuse);
  glLightfv(GL_LIGHT0, GL_AMBIENT, sunAmbient);
  glLightfv(GL_LIGHT0, GL_SPECULAR, sunSpecular);

  GLfloat globalAmb[] = {0.18f, 0.17f, 0.14f, 1.0f};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmb);

  g_trashYard.draw();

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
    camX = 0.0f;
    camY = 60.0f;
    camZ = 220.0f;
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
  g_trashYard.update(0.016f); // 简单当成 60 FPS
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

  GLfloat specular[] = {0.25f, 0.25f, 0.25f, 1.0f};
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20.0f);

  // 阴沉灰黄天空
  GLfloat fogColor[] = {0.45f, 0.42f, 0.32f, 1.0f};
  glClearColor(fogColor[0], fogColor[1], fogColor[2], fogColor[3]);

  // 雾使用全局变量 g_fogDensity
  glEnable(GL_FOG);
  glFogfv(GL_FOG_COLOR, fogColor);
  glFogi(GL_FOG_MODE, GL_EXP2);
  glFogf(GL_FOG_DENSITY, g_fogDensity);
  glHint(GL_FOG_HINT, GL_NICEST);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  g_trashYard.init();
}

// ---------------- main ----------------
int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(1280, 720);
  glutCreateWindow("Trash Mesa - Flooded San Diego (FreeGLUT)");

  initGL();

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(special);
  glutTimerFunc(16, timer, 0);

  glutMainLoop();
  return 0;
}
