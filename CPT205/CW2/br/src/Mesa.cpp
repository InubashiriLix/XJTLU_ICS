#include <GL/freeglut.h>
#include <cmath>
#include <cstdlib>
#include <vector>

const float PI = 3.14159265f;

// ---------------- Camera ----------------
float camX = 0.0f;
float camY = 60.0f;
float camZ = 220.0f;
float yaw = 0.0f;     // Yaw left/right
float pitch = -15.0f; // Pitch up/down

const float MOVE_SPEED = 3.0f;
const float ROT_SPEED = 2.0f;

// ---------------- Trash Mesa parameters ----------------
const float MESA_TOP_Y = 8.0f;       // Top height of trash mesa
const float MESA_THICKNESS = 16.0f;  // Mesa thickness
const float MESA_HALF_SIZE = 200.0f; // Half-width of the raised area
const float MESA_BUMP_HEIGHT =
    70.0f; // Extra bump height for the central mound
const float DUNE_BASE_Y =
    30.0f; // Base sand height (raised for more dramatic terrain)

// ---------------- Fog parameters (tweak here) ----------------
float g_fogDensity = 0.002f; // Higher = thicker fog

// ---------------- Small helpers ----------------
float frand(float a, float b) {
  return a + (b - a) * (std::rand() / (float)RAND_MAX);
}

// Perlin-like noise for dune variation
float noiseHash(int x, int z) {
  int n = x * 374761393 + z * 668265263;
  n = (n << 13) ^ n;
  return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) /
                     1073741824.0f);
}

float smoothNoise(float x, float z) {
  int ix = (int)std::floor(x);
  int iz = (int)std::floor(z);
  float fx = x - ix;
  float fz = z - iz;

  // Smooth interpolation
  fx = fx * fx * (3.0f - 2.0f * fx);
  fz = fz * fz * (3.0f - 2.0f * fz);

  float a = noiseHash(ix, iz);
  float b = noiseHash(ix + 1, iz);
  float c = noiseHash(ix, iz + 1);
  float d = noiseHash(ix + 1, iz + 1);

  return a * (1.0f - fx) * (1.0f - fz) + b * fx * (1.0f - fz) +
         c * (1.0f - fx) * fz + d * fx * fz;
}

float duneNoise(float x, float z) {
  float scale1 = 0.02f;
  float scale2 = 0.05f;
  float scale3 = 0.1f;

  return smoothNoise(x * scale1, z * scale1) * 8.0f +
         smoothNoise(x * scale2, z * scale2) * 4.0f +
         smoothNoise(x * scale3, z * scale3) * 2.0f;
}

// Base ground height with realistic dune variation and beach transition
float getDuneHeight(float x, float z) {
  float baseHeight = DUNE_BASE_Y + duneNoise(x, z);

  // Create beach transition: gradually lower height toward the ocean side
  // Beach should touch ocean edge. Desert extends deeper away from ocean
  const float desertEdge = 100.0f; // Desert interior edge (moved from 85.0f)
  const float beachWidth =
      30.0f; // Beach transition width (increased from 10.0f)
  const float beachEnd =
      desertEdge + beachWidth; // Outer beach edge at z=130 (touches ocean)

  if (z < desertEdge) {
    // Interior desert - full height
    return baseHeight;
  } else if (z < beachEnd) {
    // Beach transition zone - smoothly decrease to just above sea level
    float t = (z - desertEdge) / beachWidth; // 0..1
    t = t * t * (3.0f - 2.0f * t);           // Smoothstep for natural curve

    const float seaLevel = 5.0f;               // Updated from -3.0f
    const float beachHeight = seaLevel + 2.0f; // Beach at Y≈7

    return baseHeight * (1.0f - t) + beachHeight * t;
  } else {
    // Beyond beach - at sea level
    return 7.0f; // Updated from -1.0f
  }
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

// ---------------- Trash yard class ----------------
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
    // Garbage barges circle the yard
    for (auto &s : ships_) {
      s.angleDeg += s.speedDeg * dt * 60.0f; // Roughly convert dt into "frames"
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

  void drawDunes() {
    glDisable(GL_LIGHTING);
    glColor3f(0.76f, 0.70f, 0.50f); // Sand color

    // Desert should be smaller and not overlap ocean
    // In local coords: ocean is at positive Z, so desert extends into negative
    // Z
    float sizeX = 450.0f;   // Half-width in X: -400 to +400 (total 800)
    float zStart = -400.0f; // Start from -400 (away from ocean)
    float zEnd = 150.0f;    // End at +150 (closer to ocean, was 100.0f)

    int stepsX = 80;
    int stepsZ = 50;
    float stepSizeX = (sizeX * 2.0f) / stepsX;
    float stepSizeZ = (zEnd - zStart) / stepsZ;

    for (int z = 0; z < stepsZ; ++z) {
      glBegin(GL_TRIANGLE_STRIP);
      for (int x = 0; x <= stepsX; ++x) {
        float xPos = -sizeX + x * stepSizeX;
        float zPos = zStart + z * stepSizeZ;
        float zPosNext = zStart + (z + 1) * stepSizeZ;

        // Realistic dune heights with variation
        float y1 = getDuneHeight(xPos, zPos);
        float y2 = getDuneHeight(xPos, zPosNext);

        // Calculate normals for proper lighting
        float delta = 2.0f;
        float hL = getDuneHeight(xPos - delta, zPos);
        float hR = getDuneHeight(xPos + delta, zPos);
        float hD = getDuneHeight(xPos, zPos - delta);
        float hU = getDuneHeight(xPos, zPos + delta);

        float nx = (hL - hR) / (2.0f * delta);
        float nz = (hD - hU) / (2.0f * delta);
        float len = std::sqrt(nx * nx + 1.0f + nz * nz);
        glNormal3f(nx / len, 1.0f / len, nz / len);

        glVertex3f(xPos, y1, zPos);
        glVertex3f(xPos, y2, zPosNext);
      }
      glEnd();
    }
    glEnable(GL_LIGHTING);
  }

private:
  struct DebrisChunk {
    float x, z;
    float sx, sy, sz;
    float rotY;
    float gray; // Slight color variation
    bool isBox; // true = box, false = cylinder
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
    float angleDeg; // Angular position around center
    float radius;   // Orbit radius
    float height;   // Base height
    float speedDeg; // Angular speed per frame
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

  // ---------- Initialization ----------
  void initDebris() {
    // Debris scattered across the mesa top
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
      d.isBox = (frand(0.0f, 1.0f) < 0.7f); // 70% boxes, 30% cylinders
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
      s.height =
          150.0f + i * 15.0f; // Increased from 90.0f to avoid Vegas collision
      s.speedDeg = 0.12f + 0.04f * i;
      ships_.push_back(s);
    }
  }

  // ---------- Rendering ----------
  // drawDunes moved to public

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

    // Use stored shape type instead of re-randomizing every frame
    if (d.isBox) {
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

    // Tower column (dark gray)
    glPushMatrix();
    glTranslatef(c.x, baseY + c.height * 0.5f, c.z);
    glScalef(2.2f, c.height, 2.2f);
    glColor3f(0.40f, 0.40f, 0.42f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Horizontal arm
    glPushMatrix();
    float armY = baseY + c.height + 2.0f;
    glTranslatef(c.x, armY, c.z);
    glScalef(40.0f, 1.6f, 1.6f);
    glColor3f(0.45f, 0.45f, 0.47f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Hoist cable
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

    // Main crusher body
    glPushMatrix();
    glTranslatef(cr.x, y + 6.0f, cr.z);
    glScalef(cr.width, 12.0f, cr.depth);
    glColor3f(0.32f, 0.33f, 0.35f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Top guard rail
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

    // Feed port
    glPushMatrix();
    glTranslatef(cr.x, y + 5.0f, cr.z - cr.depth * 0.6f);
    glScalef(cr.width * 0.8f, 6.0f, cr.depth * 0.6f);
    glColor3f(0.26f, 0.26f, 0.28f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Inner teeth lines + side rivets
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

    // Discharge conveyor
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
    float y = s.height + std::sin(rad * 0.7f) * 2.0f; // Gentle bobbing

    // Whether it is dumping trash
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

    // Hull with tapered layers
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

    // Deck cargo hold
    glPushMatrix();
    glTranslatef(-5.0f, shipHeight * 0.4f, 0.0f);
    glScalef(shipLen * 0.7f, shipHeight * 0.6f, shipWidth * 0.8f);
    glColor3f(0.35f, 0.36f, 0.38f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Bridge / pilothouse
    glPushMatrix();
    glTranslatef(-shipLen * 0.15f, shipHeight * 0.9f, 0.0f);
    glScalef(shipLen * 0.25f, shipHeight * 0.8f, shipWidth * 0.6f);
    glColor3f(0.40f, 0.41f, 0.43f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Funnel
    glPushMatrix();
    glTranslatef(-shipLen * 0.05f, shipHeight * 1.4f, 0.0f);
    glColor3f(0.45f, 0.20f, 0.18f);
    glutSolidCylinder(1.4, 5.5, 10, 1);
    glTranslatef(0.0f, 5.5f, 0.0f);
    glColor3f(0.12f, 0.12f, 0.12f);
    glutSolidCylinder(1.2, 1.2, 10, 1);
    glPopMatrix();

    // Decoration strips removed for simplicity

    // Beveled bow
    glPushMatrix();
    glTranslatef(shipLen * 0.5f, shipHeight * 0.25f, 0.0f);
    glRotatef(20.0f, 0.0f, 1.0f, 0.0f);
    glScalef(shipLen * 0.12f, shipHeight * 0.9f, shipWidth * 0.85f);
    glColor3f(0.26f, 0.27f, 0.29f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Dumping trash: a stream of small cubes
    if (dumping) {
      glPushMatrix();
      glTranslatef(0.0f, -shipHeight * 0.5f, 0.0f); // Origin at ship bottom
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

    // Anti-collision light
    glColor3f(1.0f, 0.2f, 0.2f);
    glutSolidSphere(1.0, 12, 12); // Bow
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

// Global trash yard instance
TrashYard g_trashYard;

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

  // Overcast gray-yellow foggy lighting
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
  g_trashYard.update(0.016f); // Treat as ~60 FPS
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

  GLfloat specular[] = {0.25f, 0.25f, 0.25f, 1.0f};
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20.0f);

  // Overcast gray-yellow sky
  GLfloat fogColor[] = {0.45f, 0.42f, 0.32f, 1.0f};
  glClearColor(fogColor[0], fogColor[1], fogColor[2], fogColor[3]);

  // Fog uses global g_fogDensity
  glEnable(GL_FOG);
  glFogfv(GL_FOG_COLOR, fogColor);
  glFogi(GL_FOG_MODE, GL_EXP2);
  glFogf(GL_FOG_DENSITY, g_fogDensity);
  glHint(GL_FOG_HINT, GL_NICEST);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  g_trashYard.init();
}
