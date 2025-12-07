#include <GL/freeglut.h>
#include <cmath>
#include <cstdlib>
#include <vector>

const float PI = 3.14159265f;

// ---------------- Camera ----------------
float camX = 0.0f;
float camY = 80.0f;
float camZ = 260.0f;
float yaw = 0.0f;     // Yaw left/right
float pitch = -20.0f; // Pitch up/down

const float MOVE_SPEED = 5.0f;
const float ROT_SPEED = 2.0f;

// ---------------- City grid ----------------
const int GRID_N = 25;         // 25×25 grid
const float CELL_SIZE = 32.0f; // Cell size
const int GRID_CENTER = GRID_N / 2;
const float CITY_HALF_SIZE = GRID_N * CELL_SIZE * 0.5f;

bool g_isRoadRow[GRID_N];
bool g_isRoadCol[GRID_N];

// Base window color (cool teal)
const float WINDOW_BASE_COLOR[3] = {0.25f, 0.85f, 1.0f};

struct Building {
  float x, z;        // World-space center
  float w, d, h;     // Front block width/depth/height (windows stick to this face)
  float rotationDeg; // Rotate around Y (0/90/180/270)
  float tint;        // Small overall color tweak
  int footprintType; // 0: box, 1: slab (+rear extension), 2: L-ish (rear/side extension)
  int windowStyle;   // 0: none, 1: vertical strips, 2: grid
};

std::vector<Building> g_buildings;

// ---------------- Small helpers ----------------
float frand(float a, float b) {
  return a + (b - a) * (std::rand() / (float)RAND_MAX);
}

float gridToWorld(int idx) { return (idx - GRID_CENTER) * CELL_SIZE; }

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
  camY = 80.0f;
  camZ = 260.0f;
  yaw = 0.0f;
  pitch = -20.0f;
}

// ---------------- Grid layout: roads ----------------
void initRoadGrid() {
  for (int i = 0; i < GRID_N; ++i) {
    g_isRoadRow[i] = false;
    g_isRoadCol[i] = false;
  }

  int c = GRID_CENTER;

  // Three horizontal arterials (center plus one above and below)
  int rows[] = {c - 6, c, c + 6};
  // Three vertical arterials
  int cols[] = {c - 6, c, c + 6};

  for (int r : rows) {
    if (r >= 0 && r < GRID_N)
      g_isRoadRow[r] = true;
  }
  for (int col : cols) {
    if (col >= 0 && col < GRID_N)
      g_isRoadCol[col] = true;
  }
}

// ---------------- City init: buildings ----------------
void initCity() {
  std::srand(2077);
  initRoadGrid();
  g_buildings.clear();

  float jitter = CELL_SIZE * 0.2f; // Small per-cell offset

  for (int i = 0; i < GRID_N; ++i) {
    for (int j = 0; j < GRID_N; ++j) {
      // Skip road tiles
      if (g_isRoadCol[i] || g_isRoadRow[j])
        continue;

      // Reserve central plaza for landmarks
      if (std::abs(i - GRID_CENTER) <= 2 && std::abs(j - GRID_CENTER) <= 2)
        continue;

      Building b;

      b.x = gridToWorld(i) + frand(-jitter, jitter);
      b.z = gridToWorld(j) + frand(-jitter, jitter);

      // w,d are the front-face block dimensions (define window extent)
      b.w = frand(14.0f, 28.0f);
      b.d = frand(12.0f, 30.0f);
      b.h = frand(25.0f, 80.0f);

      b.rotationDeg = 90.0f * (std::rand() % 4);
      b.tint = frand(0.9f, 1.1f);

      float rFoot = frand(0.0f, 1.0f);
      if (rFoot < 0.65f)
        b.footprintType = 0; // Single box
      else if (rFoot < 0.90f)
        b.footprintType = 1; // Slab with rear extension
      else
        b.footprintType = 2; // L-ish (rear + side)

      float rWin = frand(0.0f, 1.0f);
      if (rWin < 0.45f)
        b.windowStyle = 1;
      else if (rWin < 0.85f)
        b.windowStyle = 2;
      else
        b.windowStyle = 0;

      g_buildings.push_back(b);
    }
  }
}

// ---------------- Ground + Roads ----------------
void drawGroundAndRoads() {
  glDisable(GL_LIGHTING);

  // Overall ground plane
  glBegin(GL_QUADS);
  glColor3f(0.02f, 0.03f, 0.06f);
  glVertex3f(-CITY_HALF_SIZE, 0.0f, -CITY_HALF_SIZE);
  glVertex3f(CITY_HALF_SIZE, 0.0f, -CITY_HALF_SIZE);
  glVertex3f(CITY_HALF_SIZE, 0.0f, CITY_HALF_SIZE);
  glVertex3f(-CITY_HALF_SIZE, 0.0f, CITY_HALF_SIZE);
  glEnd();

  glBegin(GL_QUADS);
  glColor3f(0.08f, 0.09f, 0.11f);

  float roadHalf = CELL_SIZE * 0.4f;

  // Horizontal roads
  for (int j = 0; j < GRID_N; ++j) {
    if (!g_isRoadRow[j])
      continue;
    float z = gridToWorld(j);
    glVertex3f(-CITY_HALF_SIZE - 10.0f, 0.01f, z - roadHalf);
    glVertex3f(CITY_HALF_SIZE + 10.0f, 0.01f, z - roadHalf);
    glVertex3f(CITY_HALF_SIZE + 10.0f, 0.01f, z + roadHalf);
    glVertex3f(-CITY_HALF_SIZE - 10.0f, 0.01f, z + roadHalf);
  }

  // Vertical roads
  for (int i = 0; i < GRID_N; ++i) {
    if (!g_isRoadCol[i])
      continue;
    float x = gridToWorld(i);
    glVertex3f(x - roadHalf, 0.01f, -CITY_HALF_SIZE - 10.0f);
    glVertex3f(x + roadHalf, 0.01f, -CITY_HALF_SIZE - 10.0f);
    glVertex3f(x + roadHalf, 0.01f, CITY_HALF_SIZE + 10.0f);
    glVertex3f(x - roadHalf, 0.01f, CITY_HALF_SIZE + 10.0f);
  }

  glEnd();

  // Central plaza (space to place LAPD or other landmark)
  glBegin(GL_QUADS);
  glColor3f(0.05f, 0.07f, 0.10f);
  float plaza = 90.0f;
  glVertex3f(-plaza, 0.015f, -plaza);
  glVertex3f(plaza, 0.015f, -plaza);
  glVertex3f(plaza, 0.015f, plaza);
  glVertex3f(-plaza, 0.015f, plaza);
  glEnd();

  glEnable(GL_LIGHTING);
}

// ---------------- Windows (uniform palette & flush to wall) ----------------
void drawBuildingWindows(const Building &b) {
  if (b.windowStyle == 0)
    return;

  glPushMatrix();
  glTranslatef(b.x, 0.0f, b.z);
  glRotatef(b.rotationDeg, 0.0f, 1.0f, 0.0f);

  // Front face plane of the primary block
  float zFront = b.d * 0.5f;
  float yBottom = 2.5f;
  float yTop = b.h - 2.5f;

  glDisable(GL_LIGHTING);
  glEnable(GL_POLYGON_OFFSET_FILL);
  // Nudge slightly toward camera to avoid z-fighting while staying coplanar
  glPolygonOffset(-1.0f, -1.0f);

  if (b.windowStyle == 1) {
    // Vertical strip lights/windows (cool teal, vary brightness only)
    float usableWidth = b.w * 0.6f;
    float x1 = -usableWidth * 0.5f;
    float x2 = usableWidth * 0.5f;
    float xMid = 0.0f;

    float stripW = usableWidth * 0.22f;
    float stripH = (yTop - yBottom);

    glBegin(GL_QUADS);

    auto drawStrip = [&](float xc, float brightness) {
      float r = WINDOW_BASE_COLOR[0] * brightness * b.tint;
      float g = WINDOW_BASE_COLOR[1] * brightness * b.tint;
      float bb = WINDOW_BASE_COLOR[2] * brightness * b.tint;

      float xL = xc - stripW * 0.5f;
      float xR = xc + stripW * 0.5f;

      glColor3f(r, g, bb);
      glVertex3f(xL, yBottom, zFront);
      glVertex3f(xR, yBottom, zFront);
      glVertex3f(xR, yTop, zFront);
      glVertex3f(xL, yTop, zFront);
    };

    drawStrip(x1, 0.85f);
    drawStrip(xMid, 1.05f);
    drawStrip(x2, 0.9f);

    glEnd();
  } else if (b.windowStyle == 2) {
    // Grid windows
    int nCols = 4;
    int nRows = (int)((yTop - yBottom) / 6.0f);
    if (nRows < 3)
      nRows = 3;
    if (nRows > 10)
      nRows = 10;

    float usableWidth = b.w * 0.8f;
    float colSpacing = usableWidth / (float)nCols;
    float rowSpacing = (yTop - yBottom) / (float)nRows;
    float xStart = -usableWidth * 0.5f + colSpacing * 0.5f;

    glBegin(GL_QUADS);
    for (int r = 0; r < nRows; ++r) {
      float yc = yBottom + (r + 0.5f) * rowSpacing;
      float winH = rowSpacing * 0.5f;
      for (int c = 0; c < nCols; ++c) {
        float xc = xStart + c * colSpacing;
        float winW = colSpacing * 0.35f;

        float key = (float)(r * 7 + c * 11);
        float brightness = 0.7f + 0.3f * std::sin(key * 0.37f);

        float rr = WINDOW_BASE_COLOR[0] * brightness * b.tint;
        float gg = WINDOW_BASE_COLOR[1] * brightness * b.tint;
        float bb = WINDOW_BASE_COLOR[2] * brightness * b.tint;

        glColor3f(rr, gg, bb);

        glVertex3f(xc - winW, yc - winH, zFront);
        glVertex3f(xc + winW, yc - winH, zFront);
        glVertex3f(xc + winW, yc + winH, zFront);
        glVertex3f(xc - winW, yc + winH, zFront);
      }
    }
    glEnd();
  }

  glDisable(GL_POLYGON_OFFSET_FILL);
  glEnable(GL_LIGHTING);
  glPopMatrix();
}

// ---------------- Building masses ----------------
void drawBuildingBody(const Building &b) {
  glPushMatrix();
  glTranslatef(b.x, b.h * 0.5f, b.z);
  glRotatef(b.rotationDeg, 0.0f, 1.0f, 0.0f);

  float base = 0.12f * b.tint;

  // All shapes share this front block (b.w, b.h, b.d); windows attach here.
  // Extra masses extend backward/sideways so they never cover the front face.

  // Front block
  glPushMatrix();
  glScalef(b.w, b.h, b.d);
  glColor3f(base + 0.03f, base + 0.05f, base + 0.08f);
  glutSolidCube(1.0);
  glPopMatrix();

  if (b.footprintType == 1) {
    // Slab: extend a mass behind
    glPushMatrix();
    glTranslatef(0.0f, -b.h * 0.05f, -b.d * 0.5f); // Pull backward
    glScalef(b.w * 0.8f, b.h * 0.6f, b.d * 0.9f);
    glColor3f(base + 0.05f, base + 0.07f, base + 0.10f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Rooftop mechanical block (also set back)
    glPushMatrix();
    glTranslatef(0.0f, b.h * 0.3f, -b.d * 0.15f);
    glScalef(b.w * 0.5f, b.h * 0.25f, b.d * 0.5f);
    glColor3f(base + 0.06f, base + 0.08f, base + 0.12f);
    glutSolidCube(1.0);
    glPopMatrix();
  } else if (b.footprintType == 2) {
    // L-ish: extend backward/right
    // Wing sits rear-right, never ahead of the front face
    glPushMatrix();
    glTranslatef(b.w * 0.35f, -b.h * 0.05f, -b.d * 0.25f);
    glScalef(b.w * 0.6f, b.h * 0.75f, b.d * 0.7f);
    glColor3f(base + 0.04f, base + 0.06f, base + 0.10f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Small rear block
    glPushMatrix();
    glTranslatef(-b.w * 0.25f, 0.0f, -b.d * 0.5f);
    glScalef(b.w * 0.5f, b.h * 0.5f, b.d * 0.8f);
    glColor3f(base + 0.02f, base + 0.04f, base + 0.07f);
    glutSolidCube(1.0);
    glPopMatrix();
  }

  glPopMatrix();
}

void drawBuildings() {
  for (const auto &b : g_buildings) {
    drawBuildingBody(b);
    drawBuildingWindows(b);
  }
}

// ---------------- Arched overpass (away from center) ----------------
void drawOverpass() {
  // Pick an arterial away from the plaza: GRID_CENTER + 6
  int row = GRID_CENTER + 6;
  if (row >= GRID_N)
    return;
  float z = gridToWorld(row);

  int colStart = 3;
  int colEnd = GRID_N - 4;
  if (colEnd <= colStart)
    return;

  float xStart = gridToWorld(colStart);
  float xEnd = gridToWorld(colEnd);
  float deckWidth = CELL_SIZE * 0.7f;
  float deckThick = 2.5f;
  float archH = 18.0f; // Arch height at center
  int segments = 80;   // More segments = smoother

  auto centerY = [&](float t) {
    // t in [0,1]; ends on ground, highest at center
    // Keep y(t) base at 0, top from 0 to archH+deckThick
    float mid = deckThick * 0.5f + archH * std::sin(PI * t);
    return mid;
  };

  float zL = z - deckWidth * 0.5f;
  float zR = z + deckWidth * 0.5f;

  glColor3f(0.07f, 0.08f, 0.10f);
  glBegin(GL_QUADS);

  for (int i = 0; i < segments; ++i) {
    float t0 = (float)i / (float)segments;
    float t1 = (float)(i + 1) / (float)segments;

    float x0 = xStart + (xEnd - xStart) * t0;
    float x1 = xStart + (xEnd - xStart) * t1;

    float cy0 = centerY(t0);
    float cy1 = centerY(t1);

    float y0Top = cy0 + deckThick * 0.5f;
    float y0Bot = cy0 - deckThick * 0.5f;
    float y1Top = cy1 + deckThick * 0.5f;
    float y1Bot = cy1 - deckThick * 0.5f;

    // Top face
    glVertex3f(x0, y0Top, zL);
    glVertex3f(x1, y1Top, zL);
    glVertex3f(x1, y1Top, zR);
    glVertex3f(x0, y0Top, zR);

    // Bottom face
    glVertex3f(x0, y0Bot, zR);
    glVertex3f(x1, y1Bot, zR);
    glVertex3f(x1, y1Bot, zL);
    glVertex3f(x0, y0Bot, zL);

    // Left face
    glVertex3f(x0, y0Bot, zL);
    glVertex3f(x1, y1Bot, zL);
    glVertex3f(x1, y1Top, zL);
    glVertex3f(x0, y0Top, zL);

    // Right face
    glVertex3f(x0, y0Bot, zR);
    glVertex3f(x1, y1Bot, zR);
    glVertex3f(x1, y1Top, zR);
    glVertex3f(x0, y0Top, zR);

    // Start endcap
    if (i == 0) {
      glVertex3f(x0, y0Bot, zL);
      glVertex3f(x0, y0Bot, zR);
      glVertex3f(x0, y0Top, zR);
      glVertex3f(x0, y0Top, zL);
    }
    // End endcap
    if (i == segments - 1) {
      glVertex3f(x1, y1Bot, zR);
      glVertex3f(x1, y1Bot, zL);
      glVertex3f(x1, y1Top, zL);
      glVertex3f(x1, y1Top, zR);
    }
  }

  glEnd();

  // Piers: place columns periodically through the middle span
  int pillarCount = 10;
  for (int k = 1; k <= pillarCount; ++k) {
    float t = (float)k / (float)(pillarCount + 1); // Avoid both ends
    float x = xStart + (xEnd - xStart) * t;
    float cy = centerY(t);
    float yBot = cy - deckThick * 0.5f;

    // Try to land bases near y=0
    float pillarH = yBot;
    if (pillarH < 1.0f)
      continue;

    glPushMatrix();
    glTranslatef(x, pillarH * 0.5f, z);
    glScalef(4.0f, pillarH, 4.0f);
    glColor3f(0.10f, 0.11f, 0.13f);
    glutSolidCube(1.0f);
    glPopMatrix();
  }

  // Thicker abutments at both ground ends
  float endYCenter0 = centerY(0.0f);
  float endYCenter1 = centerY(1.0f);
  float baseH0 = endYCenter0 + deckThick * 0.5f;
  float baseH1 = endYCenter1 + deckThick * 0.5f;

  float abDepth = deckWidth * 1.4f;
  float abWidth = CELL_SIZE * 0.9f;

  glPushMatrix();
  glTranslatef(xStart, baseH0 * 0.5f, z);
  glScalef(abWidth, baseH0, abDepth);
  glColor3f(0.11f, 0.12f, 0.15f);
  glutSolidCube(1.0f);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(xEnd, baseH1 * 0.5f, z);
  glScalef(abWidth, baseH1, abDepth);
  glColor3f(0.11f, 0.12f, 0.15f);
  glutSolidCube(1.0f);
  glPopMatrix();
}

// ---------------- Communication towers / infrastructure ----------------
void drawTowers() {
  struct Tower {
    float x, z, h;
  } ts[] = {
      {-260.0f, -260.0f, 110.0f},
      {260.0f, -240.0f, 130.0f},
      {-260.0f, 230.0f, 95.0f},
      {240.0f, 260.0f, 120.0f},
  };

  for (auto &t : ts) {
    // Main tower shaft
    glPushMatrix();
    glTranslatef(t.x, t.h * 0.5f, t.z);
    glScalef(8.0f, t.h, 8.0f);
    glColor3f(0.06f, 0.07f, 0.09f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Thickened top segment
    glPushMatrix();
    glTranslatef(t.x, t.h + 6.0f, t.z);
    glScalef(5.0f, 12.0f, 5.0f);
    glColor3f(0.10f, 0.12f, 0.15f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Glowing sphere at the tip (the only sphere in this blocky world)
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(t.x, t.h + 14.0f, t.z);
    glColor3f(0.25f, 0.85f, 1.0f);
    glutSolidSphere(2.5, 20, 16);
    glPopMatrix();
    glEnable(GL_LIGHTING);
  }
}

// ---------------- Whole city ----------------
void drawCity() {
  drawGroundAndRoads();
  drawBuildings();
  drawOverpass();
  drawTowers();

  // The central plaza at (0,0) is reserved for landmarks like LAPD/Stelline/Spinner
  // Insert drawLAPD() etc. here if needed
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

  // Cool-tone key light for the futuristic city
  GLfloat sunDir[] = {-0.5f, 0.8f, -0.3f, 0.0f};
  GLfloat sunDiffuse[] = {0.9f, 0.95f, 1.0f, 1.0f};
  GLfloat sunAmbient[] = {0.25f, 0.28f, 0.30f, 1.0f};
  GLfloat sunSpecular[] = {0.5f, 0.55f, 0.6f, 1.0f};
  glLightfv(GL_LIGHT0, GL_POSITION, sunDir);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, sunDiffuse);
  glLightfv(GL_LIGHT0, GL_AMBIENT, sunAmbient);
  glLightfv(GL_LIGHT0, GL_SPECULAR, sunSpecular);

  GLfloat globalAmb[] = {0.18f, 0.18f, 0.20f, 1.0f};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmb);

  drawCity();

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

  GLfloat specular[] = {0.4f, 0.4f, 0.45f, 1.0f};
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);

  // Dark blue futuristic city sky
  glClearColor(0.02f, 0.03f, 0.06f, 1.0f);

  // Light fog
  GLfloat fogColor[] = {0.02f, 0.03f, 0.06f, 1.0f};
  glEnable(GL_FOG);
  glFogfv(GL_FOG_COLOR, fogColor);
  glFogi(GL_FOG_MODE, GL_EXP2);
  glFogf(GL_FOG_DENSITY, 0.0025f);
  glHint(GL_FOG_HINT, GL_NICEST);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  initCity();
}
