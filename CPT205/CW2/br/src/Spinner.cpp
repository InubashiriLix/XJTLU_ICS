#include <GL/freeglut.h>
#include <cmath>
#include <cstdlib>

const float PI = 3.14159265f;

// ---------------- Camera ----------------
float camX = 0.0f;
float camY = 35.0f;
float camZ = 120.0f;
float yaw = 0.0f;     // Yaw left/right
float pitch = -15.0f; // Pitch up/down

const float MOVE_SPEED = 2.5f;
const float ROT_SPEED = 2.0f;
const float SPINNER_MOVE_SPEED = 3.0f;
const float SPINNER_ASCEND_SPEED = 1.5f;
const float SPINNER_CAMERA_DISTANCE = 32.0f;
const float SPINNER_CAMERA_HEIGHT = 12.0f;

// Spinner camera toggle state
static bool g_spinnerCameraAttached = false;
static float g_savedCamX = camX;
static float g_savedCamY = camY;
static float g_savedCamZ = camZ;
static float g_savedYaw = yaw;
static float g_savedPitch = pitch;

// ---------------- Spinner class ----------------
class Spinner {
public:
  float x, y, z;
  float yawDeg;     // Heading
  float hoverPhase; // Hover phase
  float sirenPhase; // Siren phase

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

    // Gentle vertical bob while hovering
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
    // Main body: chunky armored block
    float hullLen = 30.0f; // Along Z
    float hullW = 14.0f;   // Along X
    float hullH = 4.0f;    // Along Y

    // Chassis
    glPushMatrix();
    glTranslatef(0.0f, hullH * 0.5f, 0.0f);
    glScalef(hullW, hullH, hullLen);
    glColor3f(0.15f, 0.16f, 0.18f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Wedge nose in the front half, faked with a tilted block
    glPushMatrix();
    glTranslatef(0.0f, hullH * 0.9f, hullLen * 0.20f);
    glRotatef(-18.0f, 1.0f, 0.0f, 0.0f); // Tilt downward
    glScalef(hullW * 0.9f, hullH * 1.1f, hullLen * 0.7f);
    glColor3f(0.19f, 0.20f, 0.22f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Side skirt armor
    glPushMatrix();
    glTranslatef(0.0f, hullH * 0.4f, 0.0f);
    glScalef(hullW * 1.1f, hullH * 0.5f, hullLen * 0.9f);
    glColor3f(0.12f, 0.13f, 0.15f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Front lower bumper block
    glPushMatrix();
    glTranslatef(0.0f, hullH * 0.25f, hullLen * 0.55f);
    glScalef(hullW * 0.7f, hullH * 0.4f, hullLen * 0.15f);
    glColor3f(0.10f, 0.11f, 0.13f);
    glutSolidCube(1.0);
    glPopMatrix();
  }

  void drawCabin() {
    // Cockpit canopy + side windows
    float hullLen = 30.0f;
    float hullH = 4.0f;

    // Canopy body (slightly transparent deep blue/gray)
    glPushMatrix();
    glTranslatef(0.0f, hullH + 2.0f, -hullLen * 0.05f);
    glScalef(10.0f, 3.0f, 12.0f);
    glColor3f(0.12f, 0.18f, 0.22f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Rounded top of canopy using a squashed sphere
    glPushMatrix();
    glTranslatef(0.0f, hullH + 3.0f, -hullLen * 0.05f);
    glScalef(6.5f, 2.5f, 8.0f);
    glColor3f(0.10f, 0.15f, 0.20f);
    glutSolidSphere(1.0, 22, 16);
    glPopMatrix();

    // Window outline lines (suggesting scissor-door contour)
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(0.0f, hullH + 2.0f, -hullLen * 0.05f);
    glColor3f(0.35f, 0.45f, 0.90f);
    glLineWidth(2.0f);

    float w = 5.0f;
    float h = 2.0f;
    float z = 5.0f;

    glBegin(GL_LINE_LOOP); // Left side
    glVertex3f(-w, h, z);
    glVertex3f(-w, -h, z);
    glVertex3f(-w, -h, -z);
    glVertex3f(-w, h, -z * 0.2f);
    glEnd();

    glBegin(GL_LINE_LOOP); // Right side
    glVertex3f(w, h, z);
    glVertex3f(w, -h, z);
    glVertex3f(w, -h, -z);
    glVertex3f(w, h, -z * 0.2f);
    glEnd();

    glPopMatrix();
    glEnable(GL_LIGHTING);
  }

  void drawSideEnginePod(float sideSign) {
    // Side engine pod (sideSign = +1 left / -1 right)
    float hullLen = 30.0f;
    float hullW = 14.0f;

    glPushMatrix();
    glTranslatef(sideSign * (hullW * 0.7f), 4.5f, -hullLen * 0.05f);

    // Pod body
    glPushMatrix();
    glScalef(4.0f, 4.0f, 14.0f);
    glColor3f(0.14f, 0.15f, 0.17f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Front intake ring
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

    // Rear nozzle with glow
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -7.3f);
    glColor3f(0.10f, 0.11f, 0.13f);
    glutSolidSphere(1.3f, 16, 12);
    glPopMatrix();

    // Glow ring
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
    // Main rear thruster
    float hullLen = 30.0f;

    glPushMatrix();
    glTranslatef(0.0f, 3.5f, -hullLen * 0.6f);

    glPushMatrix();
    glScalef(8.0f, 4.0f, 4.0f);
    glColor3f(0.16f, 0.17f, 0.19f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Central nozzle
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, -2.4f);
    glColor3f(0.10f, 0.11f, 0.13f);
    glutSolidSphere(1.6f, 18, 14);
    glPopMatrix();

    // Blue exhaust flame
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
    // Bottom anti-gravity thrusters (four)
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

      // Thruster body
      glColor3f(0.12f, 0.12f, 0.14f);
      glutSolidSphere(0.7f, 10, 8);

      // Downward glow ring
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
    // Roof light-bar module
    float hullLen = 30.0f;
    float hullH = 4.0f;

    glPushMatrix();
    glTranslatef(0.0f, hullH + 4.5f, -hullLen * 0.25f);

    // Light bar base
    glPushMatrix();
    glScalef(6.0f, 1.0f, 2.0f);
    glColor3f(0.12f, 0.12f, 0.14f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Two lamp covers
    float blink = std::sin(sirenPhase);

    // Left: red
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

    // Right: blue
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
    // Top ID plate
    float hullLen = 30.0f;
    float hullH = 4.0f;

    glPushMatrix();
    glTranslatef(0.0f, hullH + 3.0f, -hullLen * 0.4f);
    glScalef(8.0f, 1.2f, 4.0f);
    glColor3f(0.18f, 0.18f, 0.19f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Left-side lettering removed
  }
};

// Global Spinner instance
Spinner g_spinner;

// Keep the chase camera locked to the spinner when attached
static void updateSpinnerFollowCamera() {
  if (!g_spinnerCameraAttached)
    return;

  // Keep spinner heading in sync with the view direction
  g_spinner.yawDeg = yaw;

  float radYaw = yaw * PI / 180.0f;
  float forwardX = std::sin(radYaw);
  float forwardZ = -std::cos(radYaw);

  camX = g_spinner.x - forwardX * SPINNER_CAMERA_DISTANCE;
  camZ = g_spinner.z - forwardZ * SPINNER_CAMERA_DISTANCE;
  camY = g_spinner.y + SPINNER_CAMERA_HEIGHT;
}

static void attachCameraToSpinner() {
  if (g_spinnerCameraAttached)
    return;

  g_savedCamX = camX;
  g_savedCamY = camY;
  g_savedCamZ = camZ;
  g_savedYaw = yaw;
  g_savedPitch = pitch;

  g_spinnerCameraAttached = true;
  yaw = g_spinner.yawDeg;   // Match spinner heading
  pitch = -12.0f;           // Slight downward angle
  updateSpinnerFollowCamera();
}

static void detachCameraFromSpinner() {
  if (!g_spinnerCameraAttached)
    return;

  g_spinnerCameraAttached = false;
  camX = g_savedCamX;
  camY = g_savedCamY;
  camZ = g_savedCamZ;
  yaw = g_savedYaw;
  pitch = g_savedPitch;
}

static void toggleSpinnerView() {
  if (g_spinnerCameraAttached)
    detachCameraFromSpinner();
  else
    attachCameraToSpinner();
}

// ---------------- Environment: ground + simple city block ----------------
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

  // Simple road markings
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

// ---------------- GLUT callbacks ----------------
void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  updateSpinnerFollowCamera();
  applyCamera();

  // Lighting: cool but fairly strong
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
  bool shouldRedisplay = true;

  // Toggle chase camera on/off
  if (key == '\\') {
    toggleSpinnerView();
    glutPostRedisplay();
    return;
  }

  if (g_spinnerCameraAttached) {
    float radYaw = g_spinner.yawDeg * PI / 180.0f;
    float fx = std::sin(radYaw);
    float fz = -std::cos(radYaw);
    float rx = std::cos(radYaw);
    float rz = std::sin(radYaw);

    switch (key) {
    case 27: // ESC
      std::exit(0);
      break;
    case 'w':
    case 'W':
      g_spinner.x += fx * SPINNER_MOVE_SPEED;
      g_spinner.z += fz * SPINNER_MOVE_SPEED;
      break;
    case 's':
    case 'S':
      g_spinner.x -= fx * SPINNER_MOVE_SPEED;
      g_spinner.z -= fz * SPINNER_MOVE_SPEED;
      break;
    case 'a':
    case 'A':
      g_spinner.x -= rz * SPINNER_MOVE_SPEED;
      g_spinner.z += rx * SPINNER_MOVE_SPEED;
      break;
    case 'd':
    case 'D':
      g_spinner.x += rz * SPINNER_MOVE_SPEED;
      g_spinner.z -= rx * SPINNER_MOVE_SPEED;
      break;
    case 'q':
    case 'Q':
      g_spinner.y -= SPINNER_ASCEND_SPEED;
      if (g_spinner.y < 2.0f)
        g_spinner.y = 2.0f; // keep above ground
      break;
    case 'e':
    case 'E':
      g_spinner.y += SPINNER_ASCEND_SPEED;
      break;
    case 'r':
    case 'R':
      detachCameraFromSpinner();
      camX = 0.0f;
      camY = 35.0f;
      camZ = 120.0f;
      yaw = 0.0f;
      pitch = -15.0f;
      break;
    default:
      shouldRedisplay = false;
      break;
    }

    updateSpinnerFollowCamera();
  } else {
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
      shouldRedisplay = false;
      break;
    }
  }

  if (shouldRedisplay)
    glutPostRedisplay();
}

void special(int key, int, int) {
  switch (key) {
  case GLUT_KEY_LEFT:
    yaw -= ROT_SPEED;
    if (g_spinnerCameraAttached)
      g_spinner.yawDeg = yaw;
    break;
  case GLUT_KEY_RIGHT:
    yaw += ROT_SPEED;
    if (g_spinnerCameraAttached)
      g_spinner.yawDeg = yaw;
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
  if (g_spinnerCameraAttached)
    updateSpinnerFollowCamera();
  glutPostRedisplay();
}

void timer(int) {
  g_spinner.update(0.016f); // ~60 FPS
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

  // Night sky in bright cyber blue
  glClearColor(0.03f, 0.04f, 0.08f, 1.0f);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
