
#include <GL/freeglut.h>
#include <cmath>
#include <cstdlib>

const float PI = 3.14159265f;

// --------------------- 简单向量工具 ---------------------
struct Vec3 {
  float x, y, z;
  Vec3(float xx = 0.0f, float yy = 0.0f, float zz = 0.0f)
      : x(xx), y(yy), z(zz) {}
};

Vec3 operator-(const Vec3 &a, const Vec3 &b) {
  return Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vec3 cross(const Vec3 &a, const Vec3 &b) {
  return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
              a.x * b.y - a.y * b.x);
}

Vec3 normalize(const Vec3 &v) {
  float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
  if (len < 1e-6f)
    return Vec3(0.0f, 1.0f, 0.0f);
  return Vec3(v.x / len, v.y / len, v.z / len);
}

// --------------------- 相机 ---------------------
float camX = 0.0f;
float camY = 30.0f;
float camZ = 150.0f;
float yaw = 180.0f;   // 左右转动（度）
float pitch = -10.0f; // 上下俯仰（度）

const float MOVE_SPEED = 2.0f;
const float ROT_SPEED = 2.0f;

void clampPitch() {
  if (pitch > 89.0f)
    pitch = 89.0f;
  if (pitch < -89.0f)
    pitch = -89.0f;
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

void resetCamera() {
  camX = 0.0f;
  camY = 30.0f;
  camZ = 150.0f;
  yaw = 180.0f;
  pitch = -10.0f;
}

// --------------------- 海浪高度函数 ---------------------
float waveHeight(float x, float z, float t) {
  // 多重正弦波叠加，做出暴风海浪的混乱感
  float h1 = std::sin(0.04f * x + 1.3f * t) * 2.2f;
  float h2 = std::cos(0.07f * z + 1.7f * t) * 1.8f;
  float h3 = std::sin(0.025f * (x + z) + 0.8f * t) * 1.4f;
  float base = h1 + h2 + h3; // 范围大致在 [-5, 5] 附近

  // 靠近海堤处（z 接近 OCEAN_NEAR_Z）稍微衰减，避免穿墙
  const float OCEAN_NEAR_Z = -80.0f;
  if (z > OCEAN_NEAR_Z - 30.0f) {
    float tBlend = (z - (OCEAN_NEAR_Z - 30.0f)) / 30.0f; // 0..1
    base *= (1.0f - 0.25f * tBlend);
  }

  // 反射/回波：靠近海堤叠加一个相位反转的微波，制造拍墙感
  if (z > OCEAN_NEAR_Z - 20.0f) {
    float rBlend = (z - (OCEAN_NEAR_Z - 20.0f)) / 20.0f; // 0..1
    float reflected = std::sin(0.06f * x - 1.5f * t + 1.2f) * 1.3f;
    base += reflected * (0.25f + 0.35f * rBlend);
  }
  return base;
}

// --------------------- 基础几何：无纹理盒子 ---------------------
void drawBox(float w, float h, float d) {
  float hw = w * 0.5f;
  float hh = h * 0.5f;
  float hd = d * 0.5f;

  glBegin(GL_QUADS);
  // Front
  glNormal3f(0.0f, 0.0f, 1.0f);
  glVertex3f(-hw, -hh, hd);
  glVertex3f(hw, -hh, hd);
  glVertex3f(hw, hh, hd);
  glVertex3f(-hw, hh, hd);
  // Back
  glNormal3f(0.0f, 0.0f, -1.0f);
  glVertex3f(hw, -hh, -hd);
  glVertex3f(-hw, -hh, -hd);
  glVertex3f(-hw, hh, -hd);
  glVertex3f(hw, hh, -hd);
  // Left
  glNormal3f(-1.0f, 0.0f, 0.0f);
  glVertex3f(-hw, -hh, -hd);
  glVertex3f(-hw, -hh, hd);
  glVertex3f(-hw, hh, hd);
  glVertex3f(-hw, hh, -hd);
  // Right
  glNormal3f(1.0f, 0.0f, 0.0f);
  glVertex3f(hw, -hh, hd);
  glVertex3f(hw, -hh, -hd);
  glVertex3f(hw, hh, -hd);
  glVertex3f(hw, hh, hd);
  // Top
  glNormal3f(0.0f, 1.0f, 0.0f);
  glVertex3f(-hw, hh, hd);
  glVertex3f(hw, hh, hd);
  glVertex3f(hw, hh, -hd);
  glVertex3f(-hw, hh, -hd);
  // Bottom
  glNormal3f(0.0f, -1.0f, 0.0f);
  glVertex3f(-hw, -hh, -hd);
  glVertex3f(hw, -hh, -hd);
  glVertex3f(hw, -hh, hd);
  glVertex3f(-hw, -hh, hd);
  glEnd();
}

// --------------------- 海面绘制 ---------------------
void drawOcean(float timeSec) {
  // 海在海堤外侧：z 负方向
  const float OCEAN_HALF_X = 600.0f;
  const float OCEAN_NEAR_Z = -80.0f; // 距离海堤不远
  const float DX = 20.0f;
  const float DZ = 20.0f;
  const int COLS = 60; // x 方向网格数
  const int ROWS = 60; // z 方向网格数

  float baseY = -3.0f; // 海平面基准（波谷在更低）

  glColor3f(0.12f, 0.28f, 0.40f); // 更亮的蓝绿色海面

  glBegin(GL_TRIANGLES);
  for (int ix = 0; ix < COLS; ++ix) {
    float x0 = -OCEAN_HALF_X + ix * DX;
    float x1 = x0 + DX;

    for (int iz = 0; iz < ROWS; ++iz) {
      float z0 = OCEAN_NEAR_Z - iz * DZ;
      float z1 = z0 - DZ;

      float h00 = baseY + 0.7f * waveHeight(x0, z0, timeSec);
      float h10 = baseY + 0.7f * waveHeight(x1, z0, timeSec);
      float h11 = baseY + 0.7f * waveHeight(x1, z1, timeSec);
      float h01 = baseY + 0.7f * waveHeight(x0, z1, timeSec);

      Vec3 p00(x0, h00, z0);
      Vec3 p10(x1, h10, z0);
      Vec3 p11(x1, h11, z1);
      Vec3 p01(x0, h01, z1);

      // 三角形 1: p00, p10, p01
      Vec3 n1 = normalize(cross(p10 - p00, p01 - p00));
      glNormal3f(n1.x, n1.y, n1.z);
      glVertex3f(p00.x, p00.y, p00.z);
      glVertex3f(p10.x, p10.y, p10.z);
      glVertex3f(p01.x, p01.y, p01.z);

      // 三角形 2: p10, p11, p01
      Vec3 n2 = normalize(cross(p11 - p10, p01 - p10));
      glNormal3f(n2.x, n2.y, n2.z);
      glVertex3f(p10.x, p10.y, p10.z);
      glVertex3f(p11.x, p11.y, p11.z);
      glVertex3f(p01.x, p01.y, p01.z);
    }
  }
  glEnd();
}

// --------------------- 海堤绘制 ---------------------
const float WALL_HEIGHT = 60.0f;
const float WALL_SEG_LENGTH = 120.0f; // 缩短单段长度以整体减小海堤水平延展
const int WALL_SEG_COUNT = 3;       // 每侧 3 段，合计 7 段，整体更短
const float CITY_SIDE_Z = 10.0f;    // 城市侧垂直面 z
const float TOP_WALK_Z = -5.0f;     // 顶部平台靠海侧位置
const float OCEAN_SLOPE_Z = -60.0f; // 海侧坡脚位置
const float CITY_SLIDE_OUT = 42.0f; // 城市侧滑坡延伸距离

// 城市侧装饰：细分缝、装饰条
void drawCitySideDetail(float centerX) {
  float halfLen = WALL_SEG_LENGTH * 0.5f;
  float H = WALL_HEIGHT;

  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // 细分缝（竖线）
  glLineWidth(2.0f);
  glColor4f(0.80f, 0.83f, 0.87f, 0.6f);
  glBegin(GL_LINES);
  float x1 = centerX - halfLen * 0.35f;
  float x2 = centerX + halfLen * 0.35f;
  glVertex3f(x1, 0.0f, CITY_SIDE_Z + 0.05f);
  glVertex3f(x1, H, CITY_SIDE_Z + 0.05f);
  glVertex3f(x2, 0.0f, CITY_SIDE_Z + 0.05f);
  glVertex3f(x2, H, CITY_SIDE_Z + 0.05f);
  glEnd();

  // 装饰条（水平）
  glLineWidth(3.0f);
  glColor4f(0.86f, 0.88f, 0.92f, 0.65f);
  glBegin(GL_LINES);
  float bandY = H * 0.65f;
  glVertex3f(centerX - halfLen, bandY, CITY_SIDE_Z + 0.06f);
  glVertex3f(centerX + halfLen, bandY, CITY_SIDE_Z + 0.06f);
  glEnd();

  glDisable(GL_BLEND);
  glEnable(GL_LIGHTING);
}

// 城市侧大坡面（像滑坡/缓冲带）
void drawCitySlide(float centerX) {
  float halfLen = WALL_SEG_LENGTH * 0.5f;
  float H = WALL_HEIGHT;

  // 坡从墙中上部缓缓向城市地面下降
  Vec3 v0(centerX - halfLen, H * 0.65f, CITY_SIDE_Z);
  Vec3 v1(centerX + halfLen, H * 0.65f, CITY_SIDE_Z);
  Vec3 v2(centerX + halfLen, 0.0f, CITY_SIDE_Z + CITY_SLIDE_OUT);
  Vec3 v3(centerX - halfLen, 0.0f, CITY_SIDE_Z + CITY_SLIDE_OUT);

  Vec3 n = normalize(cross(v1 - v0, v3 - v0));

  glColor3f(0.70f, 0.74f, 0.78f);
  glBegin(GL_QUADS);
  glNormal3f(n.x, n.y, n.z);
  glVertex3f(v0.x, v0.y, v0.z);
  glVertex3f(v1.x, v1.y, v1.z);
  glVertex3f(v2.x, v2.y, v2.z);
  glVertex3f(v3.x, v3.y, v3.z);
  glEnd();

  // 侧边封板
  glBegin(GL_QUADS);
  Vec3 nL = normalize(cross(v3 - v0, Vec3(0, 1, 0)));
  glNormal3f(nL.x, nL.y, nL.z);
  glVertex3f(v0.x, v0.y, v0.z);
  glVertex3f(v3.x, v3.y, v3.z);
  glVertex3f(v3.x, v3.y, v3.z + 0.1f);
  glVertex3f(v0.x, v0.y, v0.z + 0.1f);

  Vec3 nR = normalize(cross(Vec3(0, 1, 0), v2 - v1));
  glNormal3f(nR.x, nR.y, nR.z);
  glVertex3f(v1.x, v1.y, v1.z);
  glVertex3f(v1.x, v1.y, v1.z + 0.1f);
  glVertex3f(v2.x, v2.y, v2.z + 0.1f);
  glVertex3f(v2.x, v2.y, v2.z);
  glEnd();
}

void drawWallSegment(float centerX) {
  float halfLen = WALL_SEG_LENGTH * 0.5f;
  float H = WALL_HEIGHT;

  // ---- 城市侧垂直面 ----
  glColor3f(0.60f, 0.64f, 0.68f); // 亮一些的混凝土
  glBegin(GL_QUADS);
  glNormal3f(0.0f, 0.0f, 1.0f);
  glVertex3f(centerX - halfLen, 0.0f, CITY_SIDE_Z);
  glVertex3f(centerX + halfLen, 0.0f, CITY_SIDE_Z);
  glVertex3f(centerX + halfLen, H, CITY_SIDE_Z);
  glVertex3f(centerX - halfLen, H, CITY_SIDE_Z);
  glEnd();

  // ---- 顶部平台（从城市侧到稍微靠海一点）----
  glColor3f(0.50f, 0.54f, 0.58f);
  glBegin(GL_QUADS);
  glNormal3f(0.0f, 1.0f, 0.0f);
  glVertex3f(centerX - halfLen, H, CITY_SIDE_Z);
  glVertex3f(centerX + halfLen, H, CITY_SIDE_Z);
  glVertex3f(centerX + halfLen, H, TOP_WALK_Z);
  glVertex3f(centerX - halfLen, H, TOP_WALK_Z);
  glEnd();

  // ---- 海侧大斜坡 ----
  glColor3f(0.55f, 0.58f, 0.63f);
  glBegin(GL_QUADS);

  // 计算坡面的法线：用左下->右下，左下->左上 两个向量叉乘
  Vec3 v0(centerX - halfLen, 0.0f, OCEAN_SLOPE_Z);
  Vec3 v1(centerX + halfLen, 0.0f, OCEAN_SLOPE_Z);
  Vec3 v2(centerX + halfLen, H, TOP_WALK_Z);
  Vec3 v3(centerX - halfLen, H, TOP_WALK_Z);
  Vec3 n = normalize(cross(v1 - v0, v3 - v0));
  glNormal3f(n.x, n.y, n.z);

  glVertex3f(v0.x, v0.y, v0.z);
  glVertex3f(v1.x, v1.y, v1.z);
  glVertex3f(v2.x, v2.y, v2.z);
  glVertex3f(v3.x, v3.y, v3.z);
  glEnd();

  // ---- 底部厚度（把平台和坡连成实体）----
  glColor3f(0.46f, 0.50f, 0.54f);
  glBegin(GL_QUADS);
  // 底面（连接城市侧脚线与海侧坡脚）
  glNormal3f(0.0f, -1.0f, 0.0f);
  glVertex3f(centerX - halfLen, 0.0f, CITY_SIDE_Z);
  glVertex3f(centerX + halfLen, 0.0f, CITY_SIDE_Z);
  glVertex3f(centerX + halfLen, 0.0f, OCEAN_SLOPE_Z);
  glVertex3f(centerX - halfLen, 0.0f, OCEAN_SLOPE_Z);
  glEnd();

  // 左侧侧面
  glBegin(GL_QUADS);
  Vec3 a(centerX - halfLen, 0.0f, CITY_SIDE_Z);
  Vec3 b(centerX - halfLen, 0.0f, OCEAN_SLOPE_Z);
  Vec3 c(centerX - halfLen, H, TOP_WALK_Z);
  Vec3 d(centerX - halfLen, H, CITY_SIDE_Z);
  Vec3 nSideL = normalize(cross(b - a, d - a));
  glNormal3f(nSideL.x, nSideL.y, nSideL.z);
  glVertex3f(a.x, a.y, a.z);
  glVertex3f(b.x, b.y, b.z);
  glVertex3f(c.x, c.y, c.z);
  glVertex3f(d.x, d.y, d.z);
  glEnd();

  // 右侧侧面
  glBegin(GL_QUADS);
  Vec3 a2(centerX + halfLen, 0.0f, CITY_SIDE_Z);
  Vec3 b2(centerX + halfLen, 0.0f, OCEAN_SLOPE_Z);
  Vec3 c2(centerX + halfLen, H, TOP_WALK_Z);
  Vec3 d2(centerX + halfLen, H, CITY_SIDE_Z);
  Vec3 nSideR = normalize(cross(d2 - a2, b2 - a2));
  glNormal3f(nSideR.x, nSideR.y, nSideR.z);
  glVertex3f(a2.x, a2.y, a2.z);
  glVertex3f(d2.x, d2.y, d2.z);
  glVertex3f(c2.x, c2.y, c2.z);
  glVertex3f(b2.x, b2.y, b2.z);
  glEnd();

  drawCitySideDetail(centerX);
  drawCitySlide(centerX);
}

// 顶部维护灯
void drawTopLights() {
  float H = WALL_HEIGHT;
  float halfLen = WALL_SEG_LENGTH * 0.5f;

  glDisable(GL_LIGHTING);
  glColor3f(0.9f, 0.85f, 0.6f); // 暖黄小灯

  // 把灯柱基座抬起一点，避免与顶面穿插
  float yBase = H + 0.15f;
  float poleH = 4.0f;
  float lampLift = 1.0f;

  // 将灯放在顶平台的中线上，略向海侧，分三排
  float zCenterTop = 0.5f * (CITY_SIDE_Z + TOP_WALK_Z);
  float zStep = 3.0f;

  for (int i = -WALL_SEG_COUNT; i <= WALL_SEG_COUNT; ++i) {
    float cx = i * WALL_SEG_LENGTH;
    for (int j = -1; j <= 1; ++j) {
      float z = zCenterTop + j * zStep;
      float x = cx - halfLen * 0.7f + (j + 1) * halfLen * 0.7f;

      // 小柱子
      glBegin(GL_QUADS);
      float w = 0.5f;
      float y0 = yBase;
      float y1 = yBase + poleH;

      // 四个面（简单直立柱）
      // Front
      glVertex3f(x - w, y0, z + w);
      glVertex3f(x + w, y0, z + w);
      glVertex3f(x + w, y1, z + w);
      glVertex3f(x - w, y1, z + w);
      // Back
      glVertex3f(x + w, y0, z - w);
      glVertex3f(x - w, y0, z - w);
      glVertex3f(x - w, y1, z - w);
      glVertex3f(x + w, y1, z - w);
      // Left
      glVertex3f(x - w, y0, z - w);
      glVertex3f(x - w, y0, z + w);
      glVertex3f(x - w, y1, z + w);
      glVertex3f(x - w, y1, z - w);
      // Right
      glVertex3f(x + w, y0, z + w);
      glVertex3f(x + w, y0, z - w);
      glVertex3f(x + w, y1, z - w);
      glVertex3f(x + w, y1, z + w);
      glEnd();

      // 顶部灯头
      glBegin(GL_QUADS);
      float ly0 = y1;
      float ly1 = ly0 + lampLift;
      float lw = 1.2f;
      glVertex3f(x - lw, ly0, z - lw);
      glVertex3f(x + lw, ly0, z - lw);
      glVertex3f(x + lw, ly1, z - lw);
      glVertex3f(x - lw, ly1, z - lw);

      glVertex3f(x - lw, ly0, z + lw);
      glVertex3f(x + lw, ly0, z + lw);
      glVertex3f(x + lw, ly1, z + lw);
      glVertex3f(x - lw, ly1, z + lw);

      glVertex3f(x - lw, ly0, z - lw);
      glVertex3f(x - lw, ly0, z + lw);
      glVertex3f(x - lw, ly1, z + lw);
      glVertex3f(x - lw, ly1, z - lw);

      glVertex3f(x + lw, ly0, z + lw);
      glVertex3f(x + lw, ly0, z - lw);
      glVertex3f(x + lw, ly1, z - lw);
      glVertex3f(x + lw, ly1, z + lw);
      glEnd();
    }
  }

  glEnable(GL_LIGHTING);
}

// 城市侧服务路（地面上沿着海堤）
void drawServiceRoad() {
  float roadWidth = 14.0f;
  float roadZ0 = CITY_SIDE_Z + 2.0f;
  float roadZ1 = roadZ0 + roadWidth;
  float totalHalfX = WALL_SEG_LENGTH * (WALL_SEG_COUNT + 0.5f);

  glColor3f(0.16f, 0.17f, 0.19f);
  glBegin(GL_QUADS);
  glNormal3f(0.0f, 1.0f, 0.0f);
  glVertex3f(-totalHalfX, 0.01f, roadZ0);
  glVertex3f(totalHalfX, 0.01f, roadZ0);
  glVertex3f(totalHalfX, 0.01f, roadZ1);
  glVertex3f(-totalHalfX, 0.01f, roadZ1);
  glEnd();
}

// 通往顶部的平台楼梯（设在 x=0）
void drawStairs() {
  const int STEPS = 22;
  const float totalRise = WALL_HEIGHT;
  // 起点放在城市侧滑坡的末端，使楼梯与滑坡无缝衔接
  const float startZ = CITY_SIDE_Z + CITY_SLIDE_OUT;
  const float endZ = CITY_SIDE_Z + 2.0f;
  const float width = 16.0f;

  float stepH = totalRise / STEPS;
  float stepD = (startZ - endZ) / STEPS;

  // 楼梯稍微提亮，便于辨认
  glColor3f(0.52f, 0.56f, 0.60f);

  for (int i = 0; i < STEPS; ++i) {
    float zFront = startZ - i * stepD;
    float zBack = zFront - stepD;
    float zCenter = 0.5f * (zFront + zBack);

    // 让每一级台阶延伸到地面：高度随级数累积
    float stepTop = (i + 1) * stepH;
    float yCenter = stepTop * 0.5f;

    glPushMatrix();
    glTranslatef(0.0f, yCenter, zCenter);
    drawBox(width, stepTop, std::fabs(stepD));
    glPopMatrix();
  }

  // 顶部连接平台，把楼梯接到海堤顶
  float landingD = 6.0f;
  float landingH = stepH;
  float landingZCenter = endZ - landingD * 0.5f;
  float landingYCenter = WALL_HEIGHT - landingH * 0.5f;
  glPushMatrix();
  glTranslatef(0.0f, landingYCenter, landingZCenter);
  drawBox(width, landingH, landingD);
  glPopMatrix();

  // 侧向连接栏杆：让楼梯与海堤平台视觉上绑定在一起
  glDisable(GL_LIGHTING);
  glLineWidth(4.0f);
  glColor3f(0.82f, 0.85f, 0.88f);
  float railLift = 3.6f; // 扶手离踏步的高度

  for (int side = -1; side <= 1; side += 2) {
    float x = side * (width * 0.5f + 0.6f);

    // 立柱：每个踏步前缘一个，再加平台前缘一个
    glBegin(GL_LINES);
    for (int i = 0; i <= STEPS; ++i) {
      float yBase = i * stepH;
      float zBase = startZ - i * stepD;
      glVertex3f(x, yBase, zBase);
      glVertex3f(x, yBase + railLift, zBase);
    }
    glVertex3f(x, totalRise, endZ - landingD);
    glVertex3f(x, totalRise + railLift, endZ - landingD);
    glEnd();

    // 扶手：连接所有立柱顶部，贯穿到平台前缘
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= STEPS; ++i) {
      float zBase = startZ - i * stepD;
      float yTop = i * stepH + railLift;
      glVertex3f(x, yTop, zBase);
    }
    glVertex3f(x, totalRise + railLift, endZ - landingD);
    glEnd();
  }
  glEnable(GL_LIGHTING);
}

void drawSeaWall() {
  // 多段海堤拼成近乎无限的墙体
  for (int i = -WALL_SEG_COUNT; i <= WALL_SEG_COUNT; ++i) {
    float cx = i * WALL_SEG_LENGTH;
    drawWallSegment(cx);
  }
  drawTopLights();
  drawServiceRoad();

  // 在不同 x 位置布置多组楼梯，覆盖缩短后的墙长
  const float stairPositions[] = {-WALL_SEG_LENGTH * 1.5f, 0.0f,
                                  WALL_SEG_LENGTH * 1.5f};
  const int stairCount = sizeof(stairPositions) / sizeof(stairPositions[0]);
  for (int i = 0; i < stairCount; ++i) {
    glPushMatrix();
    glTranslatef(stairPositions[i], 0.0f, 0.0f);
    drawStairs();
    glPopMatrix();
  }
}

// 顶部护栏和玻璃挡板，提升精致感
void drawGlassRail() {
  float totalHalfX = WALL_SEG_LENGTH * (WALL_SEG_COUNT + 0.5f);
  float railZ = TOP_WALK_Z - 0.6f;
  float baseY = WALL_HEIGHT + 0.4f;
  float glassH = 5.5f;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_LIGHTING);

  // 玻璃面板
  glColor4f(0.70f, 0.86f, 0.95f, 0.26f);
  glBegin(GL_QUADS);
  glNormal3f(0.0f, 0.0f, 1.0f);
  glVertex3f(-totalHalfX, baseY, railZ);
  glVertex3f(totalHalfX, baseY, railZ);
  glVertex3f(totalHalfX, baseY + glassH, railZ);
  glVertex3f(-totalHalfX, baseY + glassH, railZ);
  glEnd();

  // 立柱
  glColor4f(0.85f, 0.88f, 0.90f, 0.9f);
  const float postW = 0.6f;
  const float postStep = 18.0f;
  glBegin(GL_QUADS);
  for (float x = -totalHalfX; x <= totalHalfX; x += postStep) {
    float x0 = x - postW;
    float x1 = x + postW;
    float y0 = baseY;
    float y1 = baseY + glassH + 0.8f;
    // Front
    glVertex3f(x0, y0, railZ + postW);
    glVertex3f(x1, y0, railZ + postW);
    glVertex3f(x1, y1, railZ + postW);
    glVertex3f(x0, y1, railZ + postW);
    // Back
    glVertex3f(x1, y0, railZ - postW);
    glVertex3f(x0, y0, railZ - postW);
    glVertex3f(x0, y1, railZ - postW);
    glVertex3f(x1, y1, railZ - postW);
    // Left
    glVertex3f(x0, y0, railZ - postW);
    glVertex3f(x0, y0, railZ + postW);
    glVertex3f(x0, y1, railZ + postW);
    glVertex3f(x0, y1, railZ - postW);
    // Right
    glVertex3f(x1, y0, railZ + postW);
    glVertex3f(x1, y0, railZ - postW);
    glVertex3f(x1, y1, railZ - postW);
    glVertex3f(x1, y1, railZ + postW);
  }
  glEnd();

  // 顶部扶手
  glColor4f(0.92f, 0.93f, 0.95f, 1.0f);
  float barY0 = baseY + glassH + 0.8f;
  float barY1 = barY0 + 0.6f;
  float barZ0 = railZ - 0.7f;
  float barZ1 = railZ + 0.7f;
  glBegin(GL_QUADS);
  glVertex3f(-totalHalfX, barY0, barZ1);
  glVertex3f(totalHalfX, barY0, barZ1);
  glVertex3f(totalHalfX, barY1, barZ1);
  glVertex3f(-totalHalfX, barY1, barZ1);

  glVertex3f(totalHalfX, barY0, barZ0);
  glVertex3f(-totalHalfX, barY0, barZ0);
  glVertex3f(-totalHalfX, barY1, barZ0);
  glVertex3f(totalHalfX, barY1, barZ0);

  glVertex3f(-totalHalfX, barY0, barZ0);
  glVertex3f(-totalHalfX, barY0, barZ1);
  glVertex3f(-totalHalfX, barY1, barZ1);
  glVertex3f(-totalHalfX, barY1, barZ0);

  glVertex3f(totalHalfX, barY0, barZ1);
  glVertex3f(totalHalfX, barY0, barZ0);
  glVertex3f(totalHalfX, barY1, barZ0);
  glVertex3f(totalHalfX, barY1, barZ1);
  glEnd();

  glEnable(GL_LIGHTING);
  glDisable(GL_BLEND);
}

// 海浪拍打海堤形成的泡沫带
void drawWaveFoam(float timeSec) {
  const float OCEAN_HALF_X = 600.0f;
  const float OCEAN_NEAR_Z = -80.0f;
  const float CONTACT_Z = OCEAN_SLOPE_Z + 0.4f; // 紧贴海侧斜坡面
  float baseY = -3.0f;

  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(0.92f, 0.97f, 1.0f, 0.45f);

  glBegin(GL_TRIANGLE_STRIP);
  const float DX = 12.0f;
  for (float x = -OCEAN_HALF_X; x <= OCEAN_HALF_X; x += DX) {
    float h = baseY + 0.7f * waveHeight(x, OCEAN_NEAR_Z, timeSec);
    float jitter = std::sin(0.6f * timeSec + 0.15f * x) * 0.4f;
    glVertex3f(x, h + 0.8f + jitter, CONTACT_Z);
    glVertex3f(x, h + 1.6f + jitter, CONTACT_Z + 0.4f);
  }
  glEnd();

  glDisable(GL_BLEND);
  glEnable(GL_LIGHTING);
}

// 海水拍打墙体的竖向溅射（与波高相关）
void drawSplashLine(float timeSec) {
  const float OCEAN_HALF_X = 600.0f;
  const float OCEAN_NEAR_Z = -80.0f;
  const float CONTACT_Z = OCEAN_SLOPE_Z + 0.35f;

  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glBegin(GL_QUADS);
  for (float x = -OCEAN_HALF_X; x <= OCEAN_HALF_X; x += 16.0f) {
    float h = 0.7f * waveHeight(x, OCEAN_NEAR_Z, timeSec) + 1.5f;
    float height = 4.0f + std::fabs(h) * 0.8f;
    float jitter = std::sin(0.9f * timeSec + 0.12f * x) * 0.4f;

    glColor4f(0.86f, 0.92f, 0.98f, 0.32f + 0.08f * std::fabs(h));
    glVertex3f(x - 3.0f, height * 0.2f + jitter, CONTACT_Z);
    glVertex3f(x + 3.0f, height * 0.2f + jitter, CONTACT_Z);
    glColor4f(0.86f, 0.92f, 0.98f, 0.05f);
    glVertex3f(x + 3.0f, height + jitter, CONTACT_Z);
    glVertex3f(x - 3.0f, height + jitter, CONTACT_Z);
  }
  glEnd();

  glDisable(GL_BLEND);
  glEnable(GL_LIGHTING);
}

// 斜坡表面的贴附泡沫（沿着坡面分布的小块）
void drawSlopeFoam(float timeSec) {
  const float OCEAN_HALF_X = 600.0f;
  // 在坡面下部 1/3 区域铺一些贴附泡沫
  const float Z_MIN = OCEAN_SLOPE_Z + 2.0f;
  const float Z_MAX = OCEAN_SLOPE_Z + (TOP_WALK_Z - OCEAN_SLOPE_Z) * 0.35f;

  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glBegin(GL_QUADS);
  for (float z = Z_MIN; z <= Z_MAX; z += 5.0f) {
    float tSlope = (z - OCEAN_SLOPE_Z) / (TOP_WALK_Z - OCEAN_SLOPE_Z);
    float yBase = WALL_HEIGHT * tSlope;
    for (float x = -OCEAN_HALF_X; x <= OCEAN_HALF_X; x += 22.0f) {
      float jitter = std::sin(0.7f * timeSec + 0.18f * x + 0.3f * z) * 0.35f;
      float h = 0.6f + 0.8f * (0.5f + 0.5f * std::sin(0.4f * timeSec + 0.11f * x));
      glColor4f(0.90f, 0.95f, 1.0f, 0.16f);
      glVertex3f(x - 2.5f, yBase + jitter, z);
      glVertex3f(x + 2.5f, yBase + jitter, z);
      glColor4f(0.90f, 0.95f, 1.0f, 0.04f);
      glVertex3f(x + 2.5f, yBase + h + jitter, z + 0.1f);
      glVertex3f(x - 2.5f, yBase + h + jitter, z + 0.1f);
    }
  }
  glEnd();

  glDisable(GL_BLEND);
  glEnable(GL_LIGHTING);
}

// --------------------- 地面 & 远处平面（让城市侧不至于一片空）
// ---------------------
void drawCityGround() {
  float size = 800.0f;
  glColor3f(0.13f, 0.14f, 0.16f);
  glBegin(GL_QUADS);
  glNormal3f(0.0f, 1.0f, 0.0f);
  glVertex3f(-size, 0.0f, 0.0f);
  glVertex3f(size, 0.0f, 0.0f);
  glVertex3f(size, 0.0f, 400.0f);
  glVertex3f(-size, 0.0f, 400.0f);
  glEnd();
}

// --------------------- 相机变换 ---------------------
void applyCamera() {
  glRotatef(-pitch, 1.0f, 0.0f, 0.0f);
  glRotatef(-yaw, 0.0f, 1.0f, 0.0f);
  glTranslatef(-camX, -camY, -camZ);
}

// --------------------- GLUT 回调 ---------------------
void display() {
  float timeSec = glutGet(GLUT_ELAPSED_TIME) * 0.001f;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  applyCamera();

  // ---- 灰蓝风暴天的光照 ----
  GLfloat dirPos[] = {-0.4f, 1.0f, 0.2f, 0.0f}; // 类似风暴云层透下来的光
  GLfloat dirDiffuse[] = {0.60f, 0.65f, 0.78f, 1.0f};
  GLfloat dirAmbient[] = {0.14f, 0.16f, 0.20f, 1.0f};
  GLfloat dirSpecular[] = {0.48f, 0.52f, 0.68f, 1.0f};

  glLightfv(GL_LIGHT0, GL_POSITION, dirPos);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, dirDiffuse);
  glLightfv(GL_LIGHT0, GL_AMBIENT, dirAmbient);
  glLightfv(GL_LIGHT0, GL_SPECULAR, dirSpecular);

  GLfloat globalAmbient[] = {0.12f, 0.13f, 0.15f, 1.0f};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

  // 场景
  drawCityGround();
  drawSeaWall();
  drawOcean(timeSec);
  drawGlassRail();
  drawWaveFoam(timeSec);
  drawSplashLine(timeSec);
  drawSlopeFoam(timeSec);

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
  float farZ = 2000.0f;
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
    clampPitch();
    break;
  case GLUT_KEY_DOWN:
    pitch -= ROT_SPEED;
    clampPitch();
    break;
  default:
    break;
  }
  glutPostRedisplay();
}

void idle() {
  glutPostRedisplay(); // 不断重绘，实现海浪动画
}

// --------------------- 初始化 ---------------------
void initGL() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);
  glShadeModel(GL_SMOOTH);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

  GLfloat specular[] = {0.55f, 0.58f, 0.62f, 1.0f};
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 48.0f);

  // 让天空更明亮清爽的蓝色
  glClearColor(0.35f, 0.48f, 0.65f, 1.0f);

  // 雾气：更浅的蓝灰，密度略低
  GLfloat fogColor[4] = {0.35f, 0.48f, 0.65f, 1.0f};
  glEnable(GL_FOG);
  glFogfv(GL_FOG_COLOR, fogColor);
  glFogi(GL_FOG_MODE, GL_EXP2);
  glFogf(GL_FOG_DENSITY, 0.00012f); // 数值越大雾越浓
  glHint(GL_FOG_HINT, GL_NICEST);

  resetCamera();
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(1280, 720);
  glutCreateWindow("Sepulveda Sea Wall - Stormy Ocean (FreeGLUT)");

  initGL();

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(special);
  glutIdleFunc(idle);

  glutMainLoop();
  return 0;
}
