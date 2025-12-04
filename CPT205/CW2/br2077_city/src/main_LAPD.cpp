#include <GL/freeglut.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>

const float PI = 3.14159265f;

GLuint texConcrete;

GLuint loadBMP(const char *imagepath) {
  unsigned char header[54];
  unsigned int dataPos;
  unsigned int width, height;
  unsigned int imageSize;
  unsigned char *data;

  FILE *file = fopen(imagepath, "rb");
  if (!file) {
    std::cerr << "Image could not be opened: " << imagepath << std::endl;
    return 0;
  }
  if (fread(header, 1, 54, file) != 54) {
    std::cerr << "Not a correct BMP file" << std::endl;
    fclose(file);
    return 0;
  }
  if (header[0] != 'B' || header[1] != 'M') {
    std::cerr << "Not a correct BMP file" << std::endl;
    fclose(file);
    return 0;
  }

  dataPos = *(int *)&(header[0x0A]);
  imageSize = *(int *)&(header[0x22]);
  width = *(int *)&(header[0x12]);
  height = *(int *)&(header[0x16]);

  if (imageSize == 0)
    imageSize = width * height * 3;
  if (dataPos == 0)
    dataPos = 54;

  data = new unsigned char[imageSize];
  fread(data, 1, imageSize, file);
  fclose(file);

  GLuint textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR,
               GL_UNSIGNED_BYTE, data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  delete[] data;
  return textureID;
}

// ---------------- Camera ----------------
float camX = 0.0f;
float camY = 40.0f;
float camZ = 140.0f;
float yaw = 0.0f;     // 左右转动
float pitch = -10.0f; // 上下俯仰

const float MOVE_SPEED = 2.0f;
const float ROT_SPEED = 2.5f;

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
  camY = 40.0f;
  camZ = 140.0f;
  yaw = 0.0f;
  pitch = -10.0f;
}

// ---------------- Scene ----------------
void drawGround() {
  glDisable(GL_LIGHTING);

  // 夜晚深色地面
  glBegin(GL_QUADS);
  glColor3f(0.06f, 0.06f, 0.08f); // 深灰偏蓝
  float size = 400.0f;
  glVertex3f(-size, 0.0f, -size);
  glVertex3f(size, 0.0f, -size);
  glVertex3f(size, 0.0f, size);
  glVertex3f(-size, 0.0f, size);
  glEnd();

  // 通往大楼的道路（比地面稍亮一点）
  glBegin(GL_QUADS);
  glColor3f(0.16f, 0.17f, 0.20f);
  glVertex3f(-12.0f, 0.01f, 160.0f);
  glVertex3f(12.0f, 0.01f, 160.0f);
  glVertex3f(12.0f, 0.01f, 0.0f);
  glVertex3f(-12.0f, 0.01f, 0.0f);
  glEnd();

  glEnable(GL_LIGHTING);
}

void drawTexturedBox(float w, float h, float d) {
  float hw = w * 0.5f;
  float hh = h * 0.5f;
  float hd = d * 0.5f;

  glBegin(GL_QUADS);
  // Front
  glNormal3f(0.0f, 0.0f, 1.0f);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(-hw, -hh, hd);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(hw, -hh, hd);
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(hw, hh, hd);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(-hw, hh, hd);
  // Back
  glNormal3f(0.0f, 0.0f, -1.0f);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(hw, -hh, -hd);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(-hw, -hh, -hd);
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(-hw, hh, -hd);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(hw, hh, -hd);
  // Left
  glNormal3f(-1.0f, 0.0f, 0.0f);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(-hw, -hh, -hd);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(-hw, -hh, hd);
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(-hw, hh, hd);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(-hw, hh, -hd);
  // Right
  glNormal3f(1.0f, 0.0f, 0.0f);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(hw, -hh, hd);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(hw, -hh, -hd);
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(hw, hh, -hd);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(hw, hh, hd);
  // Top
  glNormal3f(0.0f, 1.0f, 0.0f);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(-hw, hh, hd);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(hw, hh, hd);
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(hw, hh, -hd);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(-hw, hh, -hd);
  // Bottom
  glNormal3f(0.0f, -1.0f, 0.0f);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(-hw, -hh, -hd);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(hw, -hh, -hd);
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(hw, -hh, hd);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(-hw, -hh, hd);
  glEnd();
}

class LAPDBuilding {
private:
  // ---------------- LAPD building 参数 ----------------
  // 基座（podium）
  const float PODIUM_HEIGHT = 20.0f;
  const float PODIUM_HALF_W = 30.0f;
  const float PODIUM_HALF_D = 30.0f;

  // 竖直塔身
  const float TOWER_HEIGHT = 80.0f;
  const float TOWER_HALF_W = 20.0f;
  const float TOWER_HALF_D = 20.0f;

  // 顶部倒梯台
  const float FRUSTUM_HEIGHT = 40.0f;
  const float FRUSTUM_BOTTOM_HALF_W = TOWER_HALF_W;
  const float FRUSTUM_BOTTOM_HALF_D = TOWER_HALF_D;
  const float FRUSTUM_TOP_HALF_W = 40.0f;
  const float FRUSTUM_TOP_HALF_D = 40.0f;
  const float FRUSTUM_TOP_OFFSET_Z = 10.0f; // 顶部整体向前悬挑

  // 一些派生高度
  const float PODIUM_Y0 = 0.0f;
  const float PODIUM_Y1 = PODIUM_Y0 + PODIUM_HEIGHT;

  const float TOWER_Y0 = PODIUM_Y1;
  const float TOWER_Y1 = TOWER_Y0 + TOWER_HEIGHT;

  const float FRUSTUM_Y0 = TOWER_Y1;
  const float FRUSTUM_Y1 = FRUSTUM_Y0 + FRUSTUM_HEIGHT;

  void drawPodium() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texConcrete);
    glPushMatrix();
    glTranslatef(0.0f, (PODIUM_Y0 + PODIUM_Y1) * 0.5f, 0.0f);
    glColor3f(0.9f, 0.9f, 0.9f);
    drawTexturedBox(PODIUM_HALF_W * 2.0f, PODIUM_HEIGHT, PODIUM_HALF_D * 2.0f);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    // 大门
    glDisable(GL_LIGHTING);
    glColor3f(0.02f, 0.02f, 0.03f); // 深色门洞背景

    float doorW = 16.0f;
    float doorH = 12.0f;
    float doorZ = PODIUM_HALF_D + 0.1f;
    float doorY = PODIUM_Y0;

    glBegin(GL_QUADS);
    glVertex3f(-doorW / 2, doorY, doorZ);
    glVertex3f(doorW / 2, doorY, doorZ);
    glVertex3f(doorW / 2, doorY + doorH, doorZ);
    glVertex3f(-doorW / 2, doorY + doorH, doorZ);
    glEnd();

    // 门上的玻璃条
    glColor3f(0.6f, 0.85f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    for (float x = -doorW / 2; x <= doorW / 2; x += 2.0f) {
      glVertex3f(x, doorY, doorZ + 0.05f);
      glVertex3f(x, doorY + doorH, doorZ + 0.05f);
    }
    glEnd();
    glEnable(GL_LIGHTING);
  }

  void drawTowerShaft() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texConcrete);
    glPushMatrix();
    glTranslatef(0.0f, (TOWER_Y0 + TOWER_Y1) * 0.5f, 0.0f);
    glColor3f(0.85f, 0.85f, 0.9f);
    drawTexturedBox(TOWER_HALF_W * 2.0f, TOWER_HEIGHT, TOWER_HALF_D * 2.0f);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    // 夜景发光窗带（不用光照，让它自发光）
    glDisable(GL_LIGHTING);
    glColor3f(0.75f, 0.9f, 1.0f); // 亮蓝白

    float winH = 2.0f;
    float gapY = 4.0f;

    // Front & Back
    for (float y = TOWER_Y0 + 2.0f; y < TOWER_Y1 - 2.0f; y += gapY) {
      // Front
      glBegin(GL_QUADS);
      glVertex3f(-TOWER_HALF_W + 1.0f, y, TOWER_HALF_D + 0.05f);
      glVertex3f(TOWER_HALF_W - 1.0f, y, TOWER_HALF_D + 0.05f);
      glVertex3f(TOWER_HALF_W - 1.0f, y + winH, TOWER_HALF_D + 0.05f);
      glVertex3f(-TOWER_HALF_W + 1.0f, y + winH, TOWER_HALF_D + 0.05f);
      glEnd();

      // Back
      glBegin(GL_QUADS);
      glVertex3f(-TOWER_HALF_W + 1.0f, y, -TOWER_HALF_D - 0.05f);
      glVertex3f(TOWER_HALF_W - 1.0f, y, -TOWER_HALF_D - 0.05f);
      glVertex3f(TOWER_HALF_W - 1.0f, y + winH, -TOWER_HALF_D - 0.05f);
      glVertex3f(-TOWER_HALF_W + 1.0f, y + winH, -TOWER_HALF_D - 0.05f);
      glEnd();
    }

    // Left & Right
    for (float y = TOWER_Y0 + 2.0f; y < TOWER_Y1 - 2.0f; y += gapY) {
      // Right
      glBegin(GL_QUADS);
      glVertex3f(TOWER_HALF_W + 0.05f, y, -TOWER_HALF_D + 1.0f);
      glVertex3f(TOWER_HALF_W + 0.05f, y, TOWER_HALF_D - 1.0f);
      glVertex3f(TOWER_HALF_W + 0.05f, y + winH, TOWER_HALF_D - 1.0f);
      glVertex3f(TOWER_HALF_W + 0.05f, y + winH, -TOWER_HALF_D + 1.0f);
      glEnd();

      // Left
      glBegin(GL_QUADS);
      glVertex3f(-TOWER_HALF_W - 0.05f, y, -TOWER_HALF_D + 1.0f);
      glVertex3f(-TOWER_HALF_W - 0.05f, y, TOWER_HALF_D - 1.0f);
      glVertex3f(-TOWER_HALF_W - 0.05f, y + winH, TOWER_HALF_D - 1.0f);
      glVertex3f(-TOWER_HALF_W - 0.05f, y + winH, -TOWER_HALF_D + 1.0f);
      glEnd();
    }

    glEnable(GL_LIGHTING);
  }

  void drawTopFrustum() {
    float y0 = FRUSTUM_Y0;
    float y1 = FRUSTUM_Y1;
    float bw = FRUSTUM_BOTTOM_HALF_W;
    float bd = FRUSTUM_BOTTOM_HALF_D;
    float tw = FRUSTUM_TOP_HALF_W;
    float td = FRUSTUM_TOP_HALF_D;
    float oz = FRUSTUM_TOP_OFFSET_Z;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texConcrete);
    glColor3f(0.9f, 0.9f, 0.95f);

    glBegin(GL_QUADS);
    // Front
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-bw, y0, bd);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(bw, y0, bd);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(tw, y1, oz + td);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-tw, y1, oz + td);

    // Back
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(bw, y0, -bd);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-bw, y0, -bd);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-tw, y1, oz - td);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(tw, y1, oz - td);

    // Left
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-bw, y0, -bd);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-bw, y0, bd);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-tw, y1, oz + td);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-tw, y1, oz - td);

    // Right
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(bw, y0, bd);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(bw, y0, -bd);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(tw, y1, oz - td);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(tw, y1, oz + td);

    // Top face
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-tw, y1, oz - td);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(tw, y1, oz - td);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(tw, y1, oz + td);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-tw, y1, oz + td);
    glEnd();

    glDisable(GL_TEXTURE_2D);
  }

  void drawHelipad() {
    // 顶部六边形停机坪
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glColor3f(0.0f, 0.95f, 0.45f); // 霓虹绿

    glTranslatef(0.0f, FRUSTUM_Y1 + 0.1f, FRUSTUM_TOP_OFFSET_Z);

    glLineWidth(6.0f);
    glBegin(GL_LINE_LOOP);
    float radius = 18.0f;
    for (int i = 0; i < 6; ++i) {
      float angle = i * 60.0f * PI / 180.0f;
      glVertex3f(radius * std::cos(angle), 0.0f, radius * std::sin(angle));
    }
    glEnd();

    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    float radius2 = 16.0f;
    for (int i = 0; i < 6; ++i) {
      float angle = i * 60.0f * PI / 180.0f;
      glVertex3f(radius2 * std::cos(angle), 0.0f, radius2 * std::sin(angle));
    }
    glEnd();

    glPopMatrix();
    glEnable(GL_LIGHTING);
  }

  void drawAntiCollisionLights() {
    float tw = FRUSTUM_TOP_HALF_W;
    float td = FRUSTUM_TOP_HALF_D;
    float oz = FRUSTUM_TOP_OFFSET_Z;
    float y = FRUSTUM_Y1 + 1.0f;

    int t = glutGet(GLUT_ELAPSED_TIME);
    bool on = (t % 1000) < 500; // 1s 闪烁周期

    glDisable(GL_LIGHTING);

    // 支柱
    glColor3f(0.6f, 0.6f, 0.6f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex3f(tw, y, oz + td);
    glVertex3f(tw, FRUSTUM_Y1, oz + td);
    glVertex3f(-tw, y, oz + td);
    glVertex3f(-tw, FRUSTUM_Y1, oz + td);
    glVertex3f(tw, y, oz - td);
    glVertex3f(tw, FRUSTUM_Y1, oz - td);
    glVertex3f(-tw, y, oz - td);
    glVertex3f(-tw, FRUSTUM_Y1, oz - td);
    glVertex3f(0.0f, y + 0.5f, oz + td + 1.0f);
    glVertex3f(0.0f, FRUSTUM_Y1, oz + td + 1.0f);
    glEnd();

    if (on)
      glColor3f(1.0f, 0.15f, 0.15f);
    else
      glColor3f(0.25f, 0.05f, 0.05f);

    glPushMatrix();
    glTranslatef(tw, y, oz + td);
    glutSolidSphere(0.7, 12, 12);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-tw, y, oz + td);
    glutSolidSphere(0.7, 12, 12);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(tw, y, oz - td);
    glutSolidSphere(0.7, 12, 12);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-tw, y, oz - td);
    glutSolidSphere(0.7, 12, 12);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, y + 0.5f, oz + td + 1.0f);
    glutSolidSphere(0.5, 12, 12);
    glPopMatrix();

    glEnable(GL_LIGHTING);
  }

  class LAPDLogo {
    const LAPDBuilding &b;

  public:
    LAPDLogo(const LAPDBuilding &building) : b(building) {}
    void draw() {
      float bottomY = b.FRUSTUM_Y0;
      float topY = b.FRUSTUM_Y1;
      float frontZb = b.FRUSTUM_BOTTOM_HALF_D;
      float frontZt = b.FRUSTUM_TOP_OFFSET_Z + b.FRUSTUM_TOP_HALF_D;

      float centerY = 0.5f * (bottomY + topY);
      float centerZ = 0.5f * (frontZb + frontZt);

      float tiltRad = std::atan2(frontZt - frontZb, topY - bottomY);
      float tiltDeg = tiltRad * 180.0f / PI;

      glDisable(GL_LIGHTING);
      glPushMatrix();
      glColor3f(0.6f, 0.9f, 1.0f);
      glLineWidth(3.0f);

      glTranslatef(0.0f, centerY, centerZ + 0.05f);
      glRotatef(tiltDeg, 1.0f, 0.0f, 0.0f);
      glScalef(0.1f, 0.1f, 0.1f);
      glTranslatef(-170.0f, -80.0f, 0.0f);

      const char *text = "LAPD";
      for (const char *p = text; *p; ++p) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
      }

      glPopMatrix();
      glEnable(GL_LIGHTING);
    }
  };

  LAPDLogo lapdLogo;

public:
  LAPDBuilding() : lapdLogo(*this) {}

  void draw() {
    drawPodium();
    drawTowerShaft();
    drawTopFrustum();
    drawHelipad();
    drawAntiCollisionLights();
    lapdLogo.draw();
  }
};

LAPDBuilding lapdBuilding;

// ---------------- Camera transform ----------------
void applyCamera() {
  glRotatef(-pitch, 1.0f, 0.0f, 0.0f);
  glRotatef(-yaw, 0.0f, 1.0f, 0.0f);
  glTranslatef(-camX, -camY, -camZ);
}

// ---------------- GLUT callbacks ----------------
void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  applyCamera();

  // ---- 夜晚光照：月光 + 城市环境光 (Brightened) ----
  // Directional moonlight
  GLfloat moonDir[] = {-0.3f, -1.0f, -0.4f, 0.0f};   // 来自左上后方
  GLfloat moonDiffuse[] = {0.8f, 0.8f, 0.9f, 1.0f};  // Brighter diffuse
  GLfloat moonAmbient[] = {0.3f, 0.3f, 0.4f, 1.0f};  // Brighter ambient
  GLfloat moonSpecular[] = {0.5f, 0.5f, 0.6f, 1.0f}; // Brighter specular
  glLightfv(GL_LIGHT0, GL_POSITION, moonDir);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, moonDiffuse);
  glLightfv(GL_LIGHT0, GL_AMBIENT, moonAmbient);
  glLightfv(GL_LIGHT0, GL_SPECULAR, moonSpecular);

  // 全局环境光稍微有点城市泛光的感觉 (Brightened)
  GLfloat globalAmbient[] = {0.4f, 0.4f, 0.5f, 1.0f};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

  drawGround();
  lapdBuilding.draw();

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

// ---------------- Init ----------------
void initGL() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);
  glShadeModel(GL_SMOOTH);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

  GLfloat specular[] = {0.3f, 0.3f, 0.35f, 1.0f};
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);

  // 夜晚深蓝天空 (Lightened)
  glClearColor(0.15f, 0.15f, 0.25f, 1.0f);

  // ---- 雾气设置（指数雾）----
  GLfloat fogColor[4] = {0.15f, 0.15f, 0.25f, 1.0f};
  glEnable(GL_FOG);
  glFogfv(GL_FOG_COLOR, fogColor);
  glFogi(GL_FOG_MODE, GL_EXP2);
  glFogf(GL_FOG_DENSITY, 0.002f); // 数值越大雾越浓 (Reduced density)
  glHint(GL_FOG_HINT, GL_NICEST); // 要求比较好看的雾

  texConcrete = loadBMP("../textures/concrete.bmp");
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(1280, 720);
  glutCreateWindow("LAPD Brutalist Tower - Night & Fog (FreeGLUT)");

  initGL();

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(special);

  glutMainLoop();
  return 0;
}
