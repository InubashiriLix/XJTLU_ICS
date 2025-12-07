#include <GL/freeglut.h>
#include <cmath>
#include <cstdlib>
#include <vector>

const float PI = 3.14159265f;

// ---------------- Camera ----------------
float camX = 0.0f;
float camY = 40.0f;
float camZ = 160.0f;
float yaw = 0.0f;     // Yaw left/right
float pitch = -15.0f; // Pitch up/down

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

// Random helper
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
    s.y = frand(100.0f, 250.0f); // Raised from 10-120 to 100-250
    s.vy = frand(0.4f, 1.2f);
    g_snow.push_back(s);
  }
}

void updateSnow(float dt) {
  for (auto &s : g_snow) {
    s.y -= s.vy; // Simple downward drift
    if (s.y < 0.5f) {
      // Respawn above the scene
      s.y = frand(150.0f, 250.0f); // Raised from 60-130 to 150-250
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
  // Large snowy/concrete field
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

// Forecourt hardscape connecting ground to steps
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

// ---------------- Stelline memory lab ----------------

// Wide steps
void drawStairs() {
  const float startZ = 44.0f; // Front edge of lowest step is around z≈46

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
    glColor3f(0.82f, 0.82f, 0.84f); // Snow-covered concrete
    glutSolidCube(1.0);
    glPopMatrix();
  }

  // Gentle ramped platform from stair top to the facade
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

// Main concrete block (brutalist mass)
void drawMainBuilding() {
  float baseY = FOUNDATION_H; // Stair top and foundation are unified
  float bodyH = 22.0f;
  float bodyW = 60.0f;
  float bodyD = 40.0f;

  // Single monolithic block (foundation + upper) to avoid seams
  glPushMatrix();
  float totalH = FOUNDATION_H + bodyH;
  glTranslatef(0.0f, totalH * 0.5f, -bodyD * 0.3f);
  glScalef(bodyW, totalH, bodyD);
  glColor3f(0.60f, 0.60f, 0.62f);
  glutSolidCube(1.0);
  glPopMatrix();

  // Small offset block on top for extra brutalist character
  glPushMatrix();
  glTranslatef(-10.0f, baseY + bodyH + 5.0f, -bodyD * 0.6f);
  glScalef(bodyW * 0.6f, 10.0f, bodyD * 0.7f);
  glColor3f(0.68f, 0.68f, 0.70f);
  glutSolidCube(1.0);
  glPopMatrix();

  // Rooftop equipment box
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

// Curtain wall + entry
void drawFrontFacade() {
  float baseY = FOUNDATION_H;
  float bodyD = 40.0f;
  float bodyFaceZ =
      -bodyD * 0.3f + bodyD * 0.5f; // World Z of front facade (~8)

  // Curtain wall: cool gray-blue, semi-transparent, facing the steps
  glPushMatrix();
  glTranslatef(0.0f, baseY + 8.0f, bodyFaceZ + 0.05f);
  glScalef(34.0f, 14.0f, 0.6f);
  glColor4f(0.75f, 0.80f, 0.85f, 0.5f);
  glutSolidCube(1.0);
  glPopMatrix();
  glEnable(GL_LIGHTING);

  // Vertical mullions
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

  // Horizontal mullions
  for (int j = 0; j <= 2; ++j) {
    float y = y0 + j * 6.5f;
    glBegin(GL_LINES);
    glVertex3f(-17.0f, y, bodyFaceZ + 0.15f);
    glVertex3f(17.0f, y, bodyFaceZ + 0.15f);
    glEnd();
  }

  // Entry doors: double glass leaves with dark frame
  float doorW = 2.6f;
  float doorH = 4.4f;
  float doorZ = bodyFaceZ + 0.45f; // Protrudes in front of the glazing
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

// Low concrete annex buildings around the core
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

// Annex window bands
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

// Annex entrances
void drawAnnexDoors() {
  // Service doors on the two frontmost blocks
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

// Short bollard lights on the plaza
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

// Simple side-elevation window bands
void drawSideWindows() {
  float baseY = FOUNDATION_H;
  float bodyD = 40.0f;
  float bodyH = 22.0f;
  glDisable(GL_LIGHTING);
  glColor3f(0.32f, 0.38f, 0.46f);
  float yStart = baseY + 4.0f;
  float yEnd = baseY + bodyH - 3.0f;
  // Two horizontal bands on each side
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

// Entry canopy
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

// Warm facade light strips for night
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

// Simplified fallen figure on the steps
void drawFallenMan() {
  // Place near the middle of the staircase
  const int stepIndex = 5;
  const float stepH = STEP_H;
  const float stepDepth = STEP_DEPTH;
  const float startZ = 44.0f;

  float stepY = (stepIndex + 1) * stepH -
                0.25f; // sink slightly into tread so body sits lower
  float stepZ = startZ - stepIndex * stepDepth - stepDepth * 0.5f;
  float slopeDeg = std::atan(STEP_H / STEP_DEPTH) * 180.0f / PI; // ≈11.3°

  glPushMatrix();
  // place slightly off-center in x so he occupies one side of the stair
  glTranslatef(-1.2f, stepY, stepZ);
  // align body with stair run along -Z (toward building), no yaw needed
  // tilt to stair slope: head higher toward building (negative z direction)
  glRotatef(slopeDeg, 1.0f, 0.0f, 0.0f);

  // Body (long coat) aligned along z
  glPushMatrix();
  glScalef(1.4f, 0.55f, 6.0f); // flatter profile
  glColor3f(0.20f, 0.20f, 0.22f);
  glutSolidCube(1.0);
  glPopMatrix();

  // Legs
  glPushMatrix();
  glTranslatef(0.0f, -0.25f, 2.6f); // Feet point downhill (+z)
  glScalef(1.2f, 0.55f, 3.2f);
  glColor3f(0.18f, 0.18f, 0.20f);
  glutSolidCube(1.0);
  glPopMatrix();

  // Head faces the building at the uphill end
  glPushMatrix();
  glTranslatef(0.0f, 0.45f, -3.4f); // head toward building (-z)
  glColor3f(0.22f, 0.22f, 0.24f);
  glutSolidSphere(0.6f, 14, 10);
  glPopMatrix();

  glPopMatrix();
}

// Whole building assembly
void drawStelline() {
  drawPlaza();
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

// ---------------- GLUT callbacks ----------------
void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  applyCamera();

  // Cool diffuse skylight under a gray-white sky
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
  updateSnow(0.016f); // ~60 FPS
  glutPostRedisplay();
  glutTimerFunc(16, timer, 0);
}

// ---------------- OpenGL initialization ----------------
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

  // Night sky: deep blue-gray
  glClearColor(0.08f, 0.10f, 0.12f, 1.0f);

  // Light fog to gently fade distant scenery
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
