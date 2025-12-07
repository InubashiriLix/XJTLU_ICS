/**
 * Integrated future city scene for Blade Runner style assignment.
 * Uses only OpenGL + FreeGLUT and stitches together:
 *  - City (background mega city)
 *  - Dam (sea wall & ocean)
 *  - Vegas & Mesa (polluted wasteland across the sea)
 *  - Stella (memory research center with local snowfall)
 *  - Spinner (flying car patrol on a fixed path)
 *
 * Movement Controls:
 *  - WASD: FPS-style move (relative to view direction)
 *  - Mouse move: look around (yaw/pitch)
 *  - Arrow keys: rotate view
 *  - Q/E: move down/up (vertical)
 *  - Shift: speed boost
 *  - ESC: exit
 *
 * Weather Controls:
 *  - R: Toggle rain on/off
 *  - [ / ]: Decrease/increase rain intensity
 *  - F / G: Decrease/increase fog density
 *  - C / V: Decrease/increase cloud coverage
 *  - H: Show help
 *
 * Features:
 *  - First-person camera with smooth FPS-style movement
 *  - Dynamic weather system (rain, fog, clouds)
 *  - Automatic day-night cycle
 *  - Localized snow around Stelline memory center
 *  - Spinner follows a predefined patrol path, nose aligned to velocity
 */

#include <GL/freeglut.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>

// Embed original scene .cpp files as prefixed modules.

// ---- Import City.cpp as module City ----
#define PI City_PI
#define camX City_camX
#define camY City_camY
#define camZ City_camZ
#define yaw City_yaw
#define pitch City_pitch
#define MOVE_SPEED City_MOVE_SPEED
#define ROT_SPEED City_ROT_SPEED
#define g_isRoadRow City_g_isRoadRow
#define g_isRoadCol City_g_isRoadCol
#define applyCamera City_applyCamera
#define display City_display
#define drawBuildingBody City_drawBuildingBody
#define drawBuildingWindows City_drawBuildingWindows
#define drawBuildings City_drawBuildings
#define drawCity City_drawCity
#define drawGroundAndRoads City_drawGroundAndRoads
#define drawOverpass City_drawOverpass
#define drawTowers City_drawTowers
#define frand City_frand
#define gridToWorld City_gridToWorld
#define initCity City_initCity
#define initGL City_initGL
#define initRoadGrid City_initRoadGrid
#define keyboard City_keyboard
#define main City_main
#define moveForward City_moveForward
#define resetCamera City_resetCamera
#define reshape City_reshape
#define special City_special
#define strafe City_strafe
#define timer City_timer
#include "City.cpp"
#undef PI
#undef camX
#undef camY
#undef camZ
#undef yaw
#undef pitch
#undef MOVE_SPEED
#undef ROT_SPEED
#undef g_isRoadRow
#undef g_isRoadCol
#undef yaw
#undef pitch
#undef MOVE_SPEED
#undef ROT_SPEED
#undef applyCamera
#undef display
#undef drawBuildingBody
#undef drawBuildingWindows
#undef drawBuildings
#undef drawCity
#undef drawGroundAndRoads
#undef drawOverpass
#undef drawTowers
#undef frand
#undef gridToWorld
#undef initCity
#undef initGL
#undef initRoadGrid
#undef keyboard
#undef main
#undef moveForward
#undef resetCamera
#undef reshape
#undef special
#undef strafe
#undef timer

// ---- Import Dam.cpp as module Dam ----
#define PI Dam_PI
#define camX Dam_camX
#define camY Dam_camY
#define camZ Dam_camZ
#define yaw Dam_yaw
#define pitch Dam_pitch
#define MOVE_SPEED Dam_MOVE_SPEED
#define ROT_SPEED Dam_ROT_SPEED
#define applyCamera Dam_applyCamera
#define clampPitch Dam_clampPitch
#define display Dam_display
#define drawBox Dam_drawBox
#define drawCityGround Dam_drawCityGround
#define drawCitySideDetail Dam_drawCitySideDetail
#define drawCitySlide Dam_drawCitySlide
#define drawGlassRail Dam_drawGlassRail
#define drawOcean Dam_drawOcean
#define drawSeaWall Dam_drawSeaWall
#define drawServiceRoad Dam_drawServiceRoad
#define drawSlopeFoam Dam_drawSlopeFoam
#define drawSplashLine Dam_drawSplashLine
#define drawStairs Dam_drawStairs
#define drawTopLights Dam_drawTopLights
#define drawWallSegment Dam_drawWallSegment
#define drawWaveFoam Dam_drawWaveFoam
#define getForward2D Dam_getForward2D
#define idle Dam_idle
#define initGL Dam_initGL
#define keyboard Dam_keyboard
#define main Dam_main
#define moveForward Dam_moveForward
#define resetCamera Dam_resetCamera
#define reshape Dam_reshape
#define special Dam_special
#define strafe Dam_strafe
#define waveHeight Dam_waveHeight
#include "Dam.cpp"
#undef PI
#undef camX
#undef camY
#undef camZ
#undef yaw
#undef pitch
#undef MOVE_SPEED
#undef ROT_SPEED
#undef applyCamera
#undef clampPitch
#undef display
#undef drawBox
#undef drawCityGround
#undef drawCitySideDetail
#undef drawCitySlide
#undef drawGlassRail
#undef drawOcean
#undef drawSeaWall
#undef drawServiceRoad
#undef drawSlopeFoam
#undef drawSplashLine
#undef drawStairs
#undef drawTopLights
#undef drawWallSegment
#undef drawWaveFoam
#undef getForward2D
#undef idle
#undef initGL
#undef keyboard
#undef main
#undef moveForward
#undef resetCamera
#undef reshape
#undef special
#undef strafe
#undef waveHeight

// ---- Import Vegas.cpp as module Vegas ----
#define PI Vegas_PI
#define camX Vegas_camX
#define camY Vegas_camY
#define camZ Vegas_camZ
#define yaw Vegas_yaw
#define pitch Vegas_pitch
#define MOVE_SPEED Vegas_MOVE_SPEED
#define ROT_SPEED Vegas_ROT_SPEED
#define applyCamera Vegas_applyCamera
#define display Vegas_display
#define g_fogDensity Vegas_g_fogDensity
#define drawBarCounter Vegas_drawBarCounter
#define drawChair Vegas_drawChair
#define drawConcreteFrameAndSlabs Vegas_drawConcreteFrameAndSlabs
#define drawCurtainWall Vegas_drawCurtainWall
#define drawGlassBuildingBase Vegas_drawGlassBuildingBase
#define drawGround Vegas_drawGround
#define drawSlotMachine Vegas_drawSlotMachine
#define drawTable Vegas_drawTable
#define frand Vegas_frand
#define getForward2D Vegas_getForward2D
#define initGL Vegas_initGL
#define initScene Vegas_initScene
#define keyboard Vegas_keyboard
#define main Vegas_main
#define moveForward Vegas_moveForward
#define resetCamera Vegas_resetCamera
#define reshape Vegas_reshape
#define special Vegas_special
#define strafe Vegas_strafe
#define timer Vegas_timer
#include "Vegas.cpp"
#undef PI
#undef camX
#undef camY
#undef camZ
#undef yaw
#undef pitch
#undef MOVE_SPEED
#undef ROT_SPEED
#undef applyCamera
#undef display
#undef g_fogDensity
#undef drawBarCounter
#undef drawChair
#undef drawConcreteFrameAndSlabs
#undef drawCurtainWall
#undef drawGlassBuildingBase
#undef drawGround
#undef drawSlotMachine
#undef drawTable
#undef frand
#undef getForward2D
#undef initGL
#undef initScene
#undef keyboard
#undef main
#undef moveForward
#undef resetCamera
#undef reshape
#undef special
#undef strafe
#undef timer

// ---- Import Mesa.cpp as module Mesa ----
#define PI Mesa_PI
#define camX Mesa_camX
#define camY Mesa_camY
#define camZ Mesa_camZ
#define yaw Mesa_yaw
#define pitch Mesa_pitch
#define MOVE_SPEED Mesa_MOVE_SPEED
#define ROT_SPEED Mesa_ROT_SPEED
#define applyCamera Mesa_applyCamera
#define display Mesa_display
#define g_fogDensity Mesa_g_fogDensity
#define draw Mesa_draw
#define drawCarWreck Mesa_drawCarWreck
#define drawCrane Mesa_drawCrane
#define drawCrusher Mesa_drawCrusher
#define drawDebrisChunk Mesa_drawDebrisChunk
#define drawDunes Mesa_drawDunes
#define drawMesa Mesa_drawMesa
#define drawShip Mesa_drawShip
#define frand Mesa_frand
#define getDuneHeight Mesa_getDuneHeight
#define getForward2D Mesa_getForward2D
#define getMesaY Mesa_getMesaY
#define init Mesa_init
#define initCars Mesa_initCars
#define initCranes Mesa_initCranes
#define initCrushers Mesa_initCrushers
#define initDebris Mesa_initDebris
#define initGL Mesa_initGL
#define initShips Mesa_initShips
#define keyboard Mesa_keyboard
#define main Mesa_main
#define moveForward Mesa_moveForward
#define reshape Mesa_reshape
#define special Mesa_special
#define strafe Mesa_strafe
#define timer Mesa_timer
#define tooClose Mesa_tooClose
#define tooCloseAny Mesa_tooCloseAny
#define update Mesa_update
#include "Mesa.cpp"
#undef PI
#undef camX
#undef camY
#undef camZ
#undef yaw
#undef pitch
#undef MOVE_SPEED
#undef ROT_SPEED
#undef applyCamera
#undef display
#undef g_fogDensity
#undef draw
#undef drawCarWreck
#undef drawCrane
#undef drawCrusher
#undef drawDebrisChunk
#undef drawDunes
#undef drawMesa
#undef drawShip
#undef frand
#undef getDuneHeight
#undef getForward2D
#undef getMesaY
#undef init
#undef initCars
#undef initCranes
#undef initCrushers
#undef initDebris
#undef initGL
#undef initShips
#undef keyboard
#undef main
#undef moveForward
#undef reshape
#undef special
#undef strafe
#undef timer
#undef tooClose
#undef tooCloseAny
#undef update

// ---- Import Stella.cpp as module Stella ----
#define PI Stella_PI
#define camX Stella_camX
#define camY Stella_camY
#define camZ Stella_camZ
#define yaw Stella_yaw
#define pitch Stella_pitch
#define MOVE_SPEED Stella_MOVE_SPEED
#define ROT_SPEED Stella_ROT_SPEED
#define applyCamera Stella_applyCamera
#define display Stella_display
#define drawAnnexBuildings Stella_drawAnnexBuildings
#define drawAnnexDoors Stella_drawAnnexDoors
#define drawAnnexWindows Stella_drawAnnexWindows
#define drawBollardLights Stella_drawBollardLights
#define drawCanopy Stella_drawCanopy
#define drawFallenMan Stella_drawFallenMan
#define drawFrontFacade Stella_drawFrontFacade
#define drawGround Stella_drawGround
#define drawMainBuilding Stella_drawMainBuilding
#define drawPlaza Stella_drawPlaza
#define drawSideWindows Stella_drawSideWindows
#define drawSnow Stella_drawSnow
#define drawSnowField Stella_drawSnowField
#define drawStairs Stella_drawStairs
#define drawStelline Stella_drawStelline
#define drawStripLights Stella_drawStripLights
#define frand Stella_frand
#define initGL Stella_initGL
#define initSnow Stella_initSnow
#define keyboard Stella_keyboard
#define main Stella_main
#define moveForward Stella_moveForward
#define resetCamera Stella_resetCamera
#define reshape Stella_reshape
#define special Stella_special
#define strafe Stella_strafe
#define timer Stella_timer
#define updateSnow Stella_updateSnow
#include "Stella.cpp"
#undef PI
#undef camX
#undef camY
#undef camZ
#undef yaw
#undef pitch
#undef MOVE_SPEED
#undef ROT_SPEED
#undef applyCamera
#undef display
#undef drawAnnexBuildings
#undef drawAnnexDoors
#undef drawAnnexWindows
#undef drawBollardLights
#undef drawCanopy
#undef drawFallenMan
#undef drawFrontFacade
#undef drawGround
#undef drawMainBuilding
#undef drawPlaza
#undef drawSideWindows
#undef drawSnow
#undef drawSnowField
#undef drawStairs
#undef drawStelline
#undef drawStripLights
#undef frand
#undef initGL
#undef initSnow
#undef keyboard
#undef main
#undef moveForward
#undef resetCamera
#undef reshape
#undef special
#undef strafe
#undef timer
#undef updateSnow

// ---- Import Spinner.cpp as module Spinner ----
#define PI Spinner_PI
#define camX Spinner_camX
#define camY Spinner_camY
#define camZ Spinner_camZ
#define yaw Spinner_yaw
#define pitch Spinner_pitch
#define MOVE_SPEED Spinner_MOVE_SPEED
#define ROT_SPEED Spinner_ROT_SPEED
#define applyCamera Spinner_applyCamera
#define display Spinner_display
#define draw Spinner_draw
#define drawBody Spinner_drawBody
#define drawBottomThrusters Spinner_drawBottomThrusters
#define drawCabin Spinner_drawCabin
#define drawDetails Spinner_drawDetails
#define drawGround Spinner_drawGround
#define drawLightBar Spinner_drawLightBar
#define drawRearEngine Spinner_drawRearEngine
#define drawSideEnginePod Spinner_drawSideEnginePod
#define drawSideEngines Spinner_drawSideEngines
#define initGL Spinner_initGL
#define keyboard Spinner_keyboard
#define main Spinner_main
#define reshape Spinner_reshape
#define special Spinner_special
#define timer Spinner_timer
#define update Spinner_update
#include "Spinner.cpp"
#undef PI
#undef camX
#undef camY
#undef camZ
#undef yaw
#undef pitch
#undef MOVE_SPEED
#undef ROT_SPEED
#undef applyCamera
#undef display
#undef draw
#undef drawBody
#undef drawBottomThrusters
#undef drawCabin
#undef drawDetails
#undef drawGround
#undef drawLightBar
#undef drawRearEngine
#undef drawSideEnginePod
#undef drawSideEngines
#undef initGL
#undef keyboard
#undef main
#undef reshape
#undef special
#undef timer
#undef update

// ---- Import main_LAPD.cpp as module LAPD ----
#define PI LAPD_PI
#define camX LAPD_camX
#define camY LAPD_camY
#define camZ LAPD_camZ
#define yaw LAPD_yaw
#define pitch LAPD_pitch
#define MOVE_SPEED LAPD_MOVE_SPEED
#define ROT_SPEED LAPD_ROT_SPEED
#define texConcrete LAPD_texConcrete
#define applyCamera LAPD_applyCamera
#define display LAPD_display
#define draw LAPD_draw
#define drawAntiCollisionLights LAPD_drawAntiCollisionLights
#define drawGround LAPD_drawGround
#define drawHelipad LAPD_drawHelipad
#define drawPodium LAPD_drawPodium
#define drawTexturedBox LAPD_drawTexturedBox
#define drawTopFrustum LAPD_drawTopFrustum
#define drawTowerShaft LAPD_drawTowerShaft
#define getForward2D LAPD_getForward2D
#define initGL LAPD_initGL
#define keyboard LAPD_keyboard
#define main LAPD_main
#define moveForward LAPD_moveForward
#define resetCamera LAPD_resetCamera
#define reshape LAPD_reshape
#define special LAPD_special
#define strafe LAPD_strafe
#include "main_LAPD.cpp"
#undef PI
#undef camX
#undef camY
#undef camZ
#undef yaw
#undef pitch
#undef MOVE_SPEED
#undef ROT_SPEED
#undef texConcrete
#undef applyCamera
#undef display
#undef draw
#undef drawAntiCollisionLights
#undef drawGround
#undef drawHelipad
#undef drawPodium
#undef drawTexturedBox
#undef drawTopFrustum
#undef drawTowerShaft
#undef getForward2D
#undef initGL
#undef keyboard
#undef main
#undef moveForward
#undef resetCamera
#undef reshape
#undef special
#undef strafe

// ---------------- Global state for integrated camera ----------------
// 新布局：Stella(+500) -> City(0) -> Dam(-550) -> Sea(-630~-1030) ->
// Mesa+Vegas(-1100) 统一宽度：800（城市宽度）
float gCamX = 150.0f; // Right side of LAPD
float gCamY = 80.0f;
float gCamZ = 200.0f;  // Front of LAPD
float gYaw = 225.0f;   // Looking toward LAPD (southwest)
float gPitch = -10.0f; // Slight downward angle

bool gKeyW = false, gKeyA = false, gKeyS = false, gKeyD = false;
bool gKeyQ = false, gKeyE = false;
bool gKeyShift = false; // Speed boost modifier

int gLastMouseX = 0, gLastMouseY = 0;
bool gFirstMouse = true;

int gPrevTimeMs = 0;

// Time-of-day: 0..24
float gTimeOfDay = 20.0f; // start at night

// Spinner patrol parameter (0..1 loop)
float gSpinnerT = 0.0f;

// Spinner position tracking for camera attachment
float gSpinnerX = 0.0f, gSpinnerY = 0.0f, gSpinnerZ = 0.0f;
float gSpinnerYaw = 0.0f;

// ---------------- Weather System ----------------
struct WeatherState {
  bool rainEnabled = true;
  float rainIntensity = 1.0f; // 0.0 - 2.0
  float fogDensity = 0.0005f; // 0.0 - 0.01 (global)
  float cloudCoverage = 0.5f; // 0.0 - 1.0
} gWeather;

// Per-area fog density multipliers (0.0 - 3.0)
struct FogDensityControls {
  float mesa = 1.0f;
  float beach = 1.0f;
  float desert = 1.0f;
  float vegas = 1.0f;
} gFogAreas;

// Rain particles for city area
struct RainDrop {
  float x, y, z;
  float vy; // fall speed
};
std::vector<RainDrop> gRain;
const int RAIN_COUNT = 2000;

// Camera basis in world space (prefixed to avoid conflicts with modules)
struct CamVec3 {
  float x, y, z;
};

CamVec3 gCamFront = {0.0f, 0.0f, -1.0f}; // forward
CamVec3 gCamRight = {1.0f, 0.0f, 0.0f};  // right
CamVec3 gCamUp = {0.0f, 1.0f, 0.0f};     // up
const CamVec3 CAM_WORLD_UP = {0.0f, 1.0f, 0.0f};

static CamVec3 camNormalize(CamVec3 v) {
  float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
  if (len < 1e-6f)
    return {0.0f, 0.0f, 0.0f};
  float inv = 1.0f / len;
  return {v.x * inv, v.y * inv, v.z * inv};
}

static CamVec3 camCross(CamVec3 a, CamVec3 b) {
  return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

void updateCameraBasis() {
  const float yawRad = gYaw * 3.14159265f / 180.0f;
  const float pitchRad = gPitch * 3.14159265f / 180.0f;

  // 匹配OpenGL旋转顺序的相机前向向量计算
  gCamFront.x = -std::sin(yawRad) * std::cos(pitchRad);
  gCamFront.y = std::sin(pitchRad);
  gCamFront.z = -std::cos(yawRad) * std::cos(pitchRad);
  gCamFront = camNormalize(gCamFront);

  gCamRight = camNormalize(camCross(gCamFront, CAM_WORLD_UP));
  gCamUp = camCross(gCamRight, gCamFront);
}

// ---------------- Parameters ----------------
const float CAM_MOVE_SPEED = 80.0f; // world units per second
const float CAM_MOUSE_SENS = 0.15f; // degrees per pixel
const float CAM_PITCH_LIMIT = 89.0f;
// 新增：方向键旋转速度（和 City 差不多）
const float KEY_ROT_SPEED = 2.0f;

const float DAYNIGHT_SPEED = 0.05f; // hours per second
const float SPINNER_SPEED = 0.02f;  // param units per second

// ---------------- Movement Boundaries ----------------
// World layout with coordinates:
// Stella: Z=+500 to +650
// City: Z=-400 to +400, X=-400 to +400
// Dam: Z=-550
// Ocean: Z=-630 to -1130
// Beach: Z=-1130 to -1400
// Desert/Mesa: Z=-1800 to -1250 (Mesa at -1400, local -400 to +150)
struct Boundaries {
  // Horizontal bounds (X direction) - matches desert width
  float minX = -450.0f;
  float maxX = 450.0f;

  // Vertical bounds (Y direction)
  float minY = 5.0f;   // Ground level (above ground)
  float maxY = 350.0f; // Below clouds (lowered from 380)

  // Depth bounds (Z direction) - covers entire scene including full desert
  float minZ = -1800.0f; // Desert back edge (Mesa at -1400, local -400)
  float maxZ = 650.0f;   // Behind Stella
} gBounds;

// Check if position is within valid area
bool isInBounds(float x, float y, float z) {
  // Check basic world boundaries
  if (x < gBounds.minX || x > gBounds.maxX)
    return false;
  if (y < gBounds.minY || y > gBounds.maxY)
    return false;
  if (z < gBounds.minZ || z > gBounds.maxZ)
    return false;

  // Additional area-specific height checks for terrain following

  // Desert/Mesa area (world Z: -1800 to -1250)
  if (z <= -1250.0f) {
    if (y < 40.0f)
      return false; // Above highest dune/mesa (raised from 35)
  }

  // Beach transition area (world Z: -1250 to -1130)
  else if (z <= -1130.0f) {
    if (y < 30.0f)
      return false; // Above beach/dune transition
  }

  // Ocean area (world Z: -1130 to -630)
  else if (z <= -630.0f) {
    if (y < 12.0f)
      return false; // Above water surface
  }

  // Dam area (world Z: -630 to -500)
  else if (z <= -500.0f) {
    if (y < 10.0f)
      return false; // Above dam structure
  }

  // City area (world Z: -400 to +400)
  else if (z <= 400.0f) {
    if (y < 5.0f)
      return false; // Above city ground
  }

  // Stella area (world Z: +400 to +650)
  else {
    if (y < 5.0f)
      return false; // Above ground
  }

  return true;
}

// ---------------- Helper: clamp ----------------
template <typename T> T clamp(T v, T lo, T hi) {
  return (v < lo) ? lo : (v > hi ? hi : v);
}

// ---------------- Spinner Camera Control ----------------
const float INTEGRATED_SPINNER_CAMERA_DISTANCE = 32.0f;
const float INTEGRATED_SPINNER_CAMERA_HEIGHT = 18.0f;  // 更高的位置
const float INTEGRATED_SPINNER_CAMERA_SIDE_OFFSET = 15.0f;  // 右侧偏移
const float INTEGRATED_SPINNER_MOVE_SPEED = 3.0f;
const float INTEGRATED_SPINNER_ASCEND_SPEED = 1.5f;

bool gSpinnerCameraAttached = false;
float gSavedCamX, gSavedCamY, gSavedCamZ;
float gSavedYaw, gSavedPitch;

void integratedUpdateSpinnerFollowCamera(float spinnerX, float spinnerY, float spinnerZ) {
  if (!gSpinnerCameraAttached)
    return;

  // Calculate camera position: right-top-back of spinner (右上后方)
  float radYaw = gYaw * 3.14159265f / 180.0f;
  float backX = std::sin(radYaw);     // backward direction
  float backZ = std::cos(radYaw);
  float rightX = std::cos(radYaw);    // right direction
  float rightZ = -std::sin(radYaw);
  
  // Position camera at: spinner + back*distance + right*offset + up*height
  gCamX = spinnerX + backX * INTEGRATED_SPINNER_CAMERA_DISTANCE + rightX * INTEGRATED_SPINNER_CAMERA_SIDE_OFFSET;
  gCamZ = spinnerZ + backZ * INTEGRATED_SPINNER_CAMERA_DISTANCE + rightZ * INTEGRATED_SPINNER_CAMERA_SIDE_OFFSET;
  gCamY = spinnerY + INTEGRATED_SPINNER_CAMERA_HEIGHT;
}

void integratedAttachCameraToSpinner(float spinnerX, float spinnerY, float spinnerZ, float spinnerYaw) {
  if (gSpinnerCameraAttached)
    return;

  gSavedCamX = gCamX;
  gSavedCamY = gCamY;
  gSavedCamZ = gCamZ;
  gSavedYaw = gYaw;
  gSavedPitch = gPitch;

  gSpinnerCameraAttached = true;
  gYaw = spinnerYaw;
  gPitch = -25.0f;  // 更大的俯角，从上方俯视spinner
  integratedUpdateSpinnerFollowCamera(spinnerX, spinnerY, spinnerZ);
}

void integratedDetachCameraFromSpinner() {
  if (!gSpinnerCameraAttached)
    return;

  gSpinnerCameraAttached = false;
  gCamX = gSavedCamX;
  gCamY = gSavedCamY;
  gCamZ = gSavedCamZ;
  gYaw = gSavedYaw;
  gPitch = gSavedPitch;
}

void integratedToggleSpinnerCamera(float spinnerX, float spinnerY, float spinnerZ, float spinnerYaw) {
  if (gSpinnerCameraAttached)
    integratedDetachCameraFromSpinner();
  else
    integratedAttachCameraToSpinner(spinnerX, spinnerY, spinnerZ, spinnerYaw);
}

// ---------------- FPS-style camera update ----------------
void updateCamera(float dt) {
  // If attached to spinner, control spinner movement instead
  if (gSpinnerCameraAttached) {
    // Use camera basis vectors for proper FPS-style movement (horizontal plane only)
    // Calculate horizontal forward and right vectors (no vertical component for spinner movement)
    float radYaw = gYaw * 3.14159265f / 180.0f;
    float frontX = -std::sin(radYaw);
    float frontZ = -std::cos(radYaw);
    float rightX = std::cos(radYaw);
    float rightZ = -std::sin(radYaw);

    float vx = 0.0f, vy = 0.0f, vz = 0.0f;

    // WASD controls for spinner movement (matching player camera movement)
    if (gKeyW) {
      vx += frontX;
      vz += frontZ;
    }
    if (gKeyS) {
      vx -= frontX;
      vz -= frontZ;
    }
    if (gKeyA) {
      vx -= rightX;
      vz -= rightZ;
    }
    if (gKeyD) {
      vx += rightX;
      vz += rightZ;
    }
    if (gKeyQ) {
      vy -= 1.0f;
    }
    if (gKeyE) {
      vy += 1.0f;
    }

    // Normalize and apply movement with boundary checking
    float len = std::sqrt(vx * vx + vz * vz);
    if (len > 1e-4f) {
      vx /= len;
      vz /= len;
    }

    // Calculate new position
    float newX = gSpinnerX + vx * INTEGRATED_SPINNER_MOVE_SPEED;
    float newY = gSpinnerY + vy * INTEGRATED_SPINNER_ASCEND_SPEED;
    float newZ = gSpinnerZ + vz * INTEGRATED_SPINNER_MOVE_SPEED;

    // Clamp Y separately (spinner minimum height)
    if (newY < 2.0f)
      newY = 2.0f;

    // Check boundaries and only move if valid
    if (isInBounds(newX, newY, newZ)) {
      gSpinnerX = newX;
      gSpinnerY = newY;
      gSpinnerZ = newZ;
    } else {
      // Try moving only in X if full movement blocked
      if (isInBounds(newX, gSpinnerY, gSpinnerZ)) {
        gSpinnerX = newX;
      }
      // Try moving only in Y if full movement blocked
      if (isInBounds(gSpinnerX, newY, gSpinnerZ)) {
        gSpinnerY = newY;
      }
      // Try moving only in Z if full movement blocked
      if (isInBounds(gSpinnerX, gSpinnerY, newZ)) {
        gSpinnerZ = newZ;
      }
    }

    // Update spinner yaw to match camera yaw
    gSpinnerYaw = gYaw;

    // Update camera position to follow spinner
    integratedUpdateSpinnerFollowCamera(gSpinnerX, gSpinnerY, gSpinnerZ);
    updateCameraBasis();
    return;
  }

  // Normal free camera movement
  float vx = 0.0f, vy = 0.0f, vz = 0.0f;

  // W/S 前后移动（看向哪儿就走向哪儿）
  if (gKeyW) {
    vx += gCamFront.x;
    vy += gCamFront.y;
    vz += gCamFront.z;
  }
  if (gKeyS) {
    vx -= gCamFront.x;
    vy -= gCamFront.y;
    vz -= gCamFront.z;
  }

  // A/D 左右平移
  if (gKeyA) {
    vx -= gCamRight.x;
    vy -= gCamRight.y;
    vz -= gCamRight.z;
  }
  if (gKeyD) {
    vx += gCamRight.x;
    vy += gCamRight.y;
    vz += gCamRight.z;
  }

  // Q/E 垂直升降
  if (gKeyQ) {
    vy -= 1.0f;
  }
  if (gKeyE) {
    vy += 1.0f;
  }

  float len = std::sqrt(vx * vx + vy * vy + vz * vz);
  if (len > 1e-4f) {
    vx /= len;
    vy /= len;
    vz /= len;

    // Apply speed boost with Shift
    float speed = CAM_MOVE_SPEED;
    if (gKeyShift) {
      speed *= 3.0f; // 3x faster with Shift
    }

    // Calculate new position
    float newX = gCamX + vx * speed * dt;
    float newY = gCamY + vy * speed * dt;
    float newZ = gCamZ + vz * speed * dt;

    // Check boundaries and only move if valid
    if (isInBounds(newX, newY, newZ)) {
      gCamX = newX;
      gCamY = newY;
      gCamZ = newZ;
    } else {
      // Try moving only in X if full movement blocked
      if (isInBounds(newX, gCamY, gCamZ)) {
        gCamX = newX;
      }
      // Try moving only in Y if full movement blocked
      if (isInBounds(gCamX, newY, gCamZ)) {
        gCamY = newY;
      }
      // Try moving only in Z if full movement blocked
      if (isInBounds(gCamX, gCamY, newZ)) {
        gCamZ = newZ;
      }
    }
  }

  // Clamp position to absolute bounds as safety net
  gCamX = clamp(gCamX, gBounds.minX, gBounds.maxX);
  gCamY = clamp(gCamY, gBounds.minY, gBounds.maxY);
  gCamZ = clamp(gCamZ, gBounds.minZ, gBounds.maxZ);
}

// ---------------- Mouse look ----------------
void mouseLook(int x, int y) {
  if (gFirstMouse) {
    gLastMouseX = x;
    gLastMouseY = y;
    gFirstMouse = false;
    return;
  }
  int dx = x - gLastMouseX;
  int dy = y - gLastMouseY;
  gLastMouseX = x;
  gLastMouseY = y;

  gYaw += dx * CAM_MOUSE_SENS;
  gPitch -= dy * CAM_MOUSE_SENS;

  if (gYaw > 360.0f)
    gYaw -= 360.0f;
  else if (gYaw < -360.0f)
    gYaw += 360.0f;

  gPitch = clamp(gPitch, -CAM_PITCH_LIMIT, CAM_PITCH_LIMIT);

  updateCameraBasis();
}

// ---------------- Apply integrated camera ----------------
void applyIntegratedCamera() {
  glRotatef(-gPitch, 1.0f, 0.0f, 0.0f);
  glRotatef(-gYaw, 0.0f, 1.0f, 0.0f);
  glTranslatef(-gCamX, -gCamY, -gCamZ);
}

// ---------------- Day/Night lighting ----------------
void setupDayNightLighting() {
  float t = gTimeOfDay / 24.0f; // 0..1

  float dayFactor = std::exp(-std::pow((t - 0.3f) / 0.25f, 2.0f));
  float nightFactor = std::exp(-std::pow((t - 0.8f) / 0.25f, 2.0f));

  // Sky color affected by cloud coverage
  float cloudDarkening = 1.0f - gWeather.cloudCoverage * 0.3f;
  GLfloat clearColor[4] = {
      (0.02f + 0.08f * dayFactor + 0.1f * nightFactor) * cloudDarkening,
      (0.02f + 0.10f * dayFactor + 0.04f * nightFactor) * cloudDarkening,
      (0.05f + 0.18f * dayFactor + 0.20f * nightFactor) * cloudDarkening, 1.0f};
  glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);

  // Fog system
  if (gWeather.fogDensity > 0.0001f) {
    glEnable(GL_FOG);
    GLfloat fogColor[] = {clearColor[0] * 1.2f, clearColor[1] * 1.2f,
                          clearColor[2] * 1.2f, 1.0f};
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogi(GL_FOG_MODE, GL_EXP2);
    glFogf(GL_FOG_DENSITY, gWeather.fogDensity);
    glHint(GL_FOG_HINT, GL_NICEST);
  } else {
    glDisable(GL_FOG);
  }

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);

  // Sun (directional) - dimmed by clouds
  float lightReduction = 1.0f - gWeather.cloudCoverage * 0.5f;
  GLfloat sunDir[] = {-0.4f, 0.8f, -0.2f, 0.0f};
  GLfloat sunDiffuse[] = {(0.4f + 0.6f * dayFactor) * lightReduction,
                          (0.4f + 0.6f * dayFactor) * lightReduction,
                          (0.45f + 0.55f * dayFactor) * lightReduction, 1.0f};
  GLfloat sunAmbient[] = {0.10f + 0.20f * dayFactor, 0.12f + 0.22f * dayFactor,
                          0.16f + 0.26f * dayFactor, 1.0f};
  GLfloat sunSpecular[] = {0.6f * dayFactor * lightReduction,
                           0.6f * dayFactor * lightReduction,
                           0.6f * dayFactor * lightReduction, 1.0f};

  glLightfv(GL_LIGHT0, GL_POSITION, sunDir);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, sunDiffuse);
  glLightfv(GL_LIGHT0, GL_AMBIENT, sunAmbient);
  glLightfv(GL_LIGHT0, GL_SPECULAR, sunSpecular);

  // Moon / city glow
  GLfloat moonDir[] = {0.3f, 0.6f, 0.2f, 0.0f};
  GLfloat moonDiffuse[] = {0.1f + 0.5f * nightFactor,
                           0.15f + 0.6f * nightFactor,
                           0.25f + 0.8f * nightFactor, 1.0f};
  GLfloat moonAmbient[] = {0.05f + 0.25f * nightFactor,
                           0.06f + 0.28f * nightFactor,
                           0.08f + 0.30f * nightFactor, 1.0f};
  GLfloat moonSpecular[] = {0.2f * nightFactor, 0.2f * nightFactor,
                            0.3f * nightFactor, 1.0f};

  glLightfv(GL_LIGHT1, GL_POSITION, moonDir);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, moonDiffuse);
  glLightfv(GL_LIGHT1, GL_AMBIENT, moonAmbient);
  glLightfv(GL_LIGHT1, GL_SPECULAR, moonSpecular);
}

// Apply area-specific fog density
void applyAreaFog(float multiplier) {
  if (gWeather.fogDensity > 0.0001f && multiplier > 0.0f) {
    float areaDensity = gWeather.fogDensity * multiplier;
    glFogf(GL_FOG_DENSITY, areaDensity);
  }
}

// ---------------- Spinner patrol path ----------------
void evalSpinnerPath(float t, float &x, float &y, float &z) {
  // Clamp 0..1
  if (t < 0.0f)
    t = 0.0f;
  if (t > 1.0f)
    t = 1.0f;

  float angle = t * 2.0f * 3.14159265f;
  float radius = 220.0f + 40.0f * std::sin(2.0f * angle);

  // 以城市中心（z=0）为圆心绕圈
  x = radius * std::cos(angle);
  z = radius * std::sin(angle);

  // 高度上下起伏，避免穿楼
  y = 200.0f + 40.0f * std::sin(3.0f * angle);
}

// ---------------- Draw unified ground (Stella to Dam) ----------------
void drawUnifiedGround() {
  // 统一地面：从Stella(+500)到Dam底部(-550)
  // 总范围：Z从+500到-550，宽度约1000（匹配城市宽度800+边缘）
  glDisable(GL_LIGHTING);
  glColor3f(0.25f, 0.25f, 0.28f); // 深灰色混凝土地面

  glBegin(GL_QUADS);
  float halfWidth =
      450.0f; // 半宽800，总宽1600 (large enough for rotated Stella buildings)
  float zStart = 600.0f; // Stella后方扩展 (extended from 550 for Stella length)
  float zEnd = -600.0f;  // Dam底部后方一点
  float y = 0.0f;        // 地面高度

  glNormal3f(0.0f, 1.0f, 0.0f);
  glVertex3f(-halfWidth, y, zStart);
  glVertex3f(halfWidth, y, zStart);
  glVertex3f(halfWidth, y, zEnd);
  glVertex3f(-halfWidth, y, zEnd);
  glEnd();

  glEnable(GL_LIGHTING);
}

// 只绘制城市道路（不绘制地面底色）
void drawCityRoadsOnly() {
  glDisable(GL_LIGHTING);
  glBegin(GL_QUADS);
  glColor3f(0.08f, 0.09f, 0.11f);

  const float CITY_HALF_SIZE = 400.0f;
  const int GRID_N = 25;
  const float CELL_SIZE = 32.0f;
  const float roadHalf = CELL_SIZE * 0.4f;

  // 水平道路
  for (int j = 0; j < GRID_N; ++j) {
    if (!City_g_isRoadRow[j])
      continue;
    float z = City_gridToWorld(j);
    glVertex3f(-CITY_HALF_SIZE - 10.0f, 0.01f, z - roadHalf);
    glVertex3f(CITY_HALF_SIZE + 10.0f, 0.01f, z - roadHalf);
    glVertex3f(CITY_HALF_SIZE + 10.0f, 0.01f, z + roadHalf);
    glVertex3f(-CITY_HALF_SIZE - 10.0f, 0.01f, z + roadHalf);
  }

  // 垂直道路
  for (int i = 0; i < GRID_N; ++i) {
    if (!City_g_isRoadCol[i])
      continue;
    float x = City_gridToWorld(i);
    glVertex3f(x - roadHalf, 0.01f, -CITY_HALF_SIZE - 10.0f);
    glVertex3f(x + roadHalf, 0.01f, -CITY_HALF_SIZE - 10.0f);
    glVertex3f(x + roadHalf, 0.01f, CITY_HALF_SIZE + 10.0f);
    glVertex3f(x - roadHalf, 0.01f, CITY_HALF_SIZE + 10.0f);
  }

  glEnd();

  // 中心广场
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

// ---------------- Stella Buffer Area (Transition/Connection Zone)
// ----------------
void drawStellaBuffer() {
  // Buffer zone connecting Stella to the city
  // This area creates a visual transition with roads, lights, barriers, etc.

  // 1) Access road from city to Stella (along Z-axis)
  glDisable(GL_LIGHTING);
  glColor3f(0.12f, 0.13f, 0.15f); // Dark asphalt
  glBegin(GL_QUADS);
  float roadWidth = 30.0f;
  float roadStart = 150.0f; // From Stella perimeter
  float roadEnd = 400.0f;   // To city edge
  glVertex3f(-roadWidth, 0.02f, roadStart);
  glVertex3f(roadWidth, 0.02f, roadStart);
  glVertex3f(roadWidth, 0.02f, roadEnd);
  glVertex3f(-roadWidth, 0.02f, roadEnd);
  glEnd();

  // Road markings (dashed center line)
  glColor3f(0.85f, 0.85f, 0.50f); // Yellow line
  glLineWidth(2.0f);
  glBegin(GL_LINES);
  for (float z = roadStart; z < roadEnd; z += 20.0f) {
    glVertex3f(0.0f, 0.05f, z);
    glVertex3f(0.0f, 0.05f, z + 10.0f);
  }
  glEnd();

  // 2) Street lights along the road
  glColor3f(0.25f, 0.25f, 0.28f); // Dark metal
  for (float z = roadStart + 40.0f; z < roadEnd; z += 80.0f) {
    for (int side = -1; side <= 1; side += 2) {
      float x = side * (roadWidth + 5.0f);

      // Light post
      glPushMatrix();
      glTranslatef(x, 7.0f, z);
      glScalef(0.8f, 14.0f, 0.8f);
      glutSolidCube(1.0);
      glPopMatrix();

      // Light fixture
      glPushMatrix();
      glTranslatef(x, 14.0f, z);
      glScalef(2.0f, 1.0f, 2.0f);
      glColor3f(0.95f, 0.90f, 0.70f); // Warm light color
      glutSolidCube(1.0);
      glColor3f(0.25f, 0.25f, 0.28f);
      glPopMatrix();
    }
  }

  // 3) Parking area / vehicle staging zone
  glColor3f(0.20f, 0.21f, 0.23f); // Concrete parking lot
  float parkingX = 60.0f;
  float parkingZ = 250.0f;
  float parkingSize = 40.0f;

  glBegin(GL_QUADS);
  glVertex3f(parkingX, 0.01f, parkingZ - parkingSize);
  glVertex3f(parkingX + parkingSize, 0.01f, parkingZ - parkingSize);
  glVertex3f(parkingX + parkingSize, 0.01f, parkingZ + parkingSize);
  glVertex3f(parkingX, 0.01f, parkingZ + parkingSize);
  glEnd();

  // Parking space lines
  glColor3f(0.80f, 0.80f, 0.80f); // White lines
  glLineWidth(1.5f);
  for (int i = 0; i < 5; ++i) {
    float zLine = parkingZ - parkingSize + i * 20.0f;
    glBegin(GL_LINES);
    glVertex3f(parkingX, 0.02f, zLine);
    glVertex3f(parkingX + parkingSize, 0.02f, zLine);
    glEnd();
  }

  // 4) Security checkpoint booth (mid-point between city and Stella)
  float checkpointZ = 300.0f;

  // Booth structure
  glColor3f(0.35f, 0.35f, 0.38f); // Security booth gray
  glPushMatrix();
  glTranslatef(-roadWidth - 8.0f, 2.0f, checkpointZ);
  glScalef(6.0f, 4.0f, 8.0f);
  glutSolidCube(1.0);
  glPopMatrix();

  // Booth window
  glColor3f(0.20f, 0.30f, 0.40f); // Tinted glass
  glPushMatrix();
  glTranslatef(-roadWidth - 7.5f, 2.5f, checkpointZ);
  glScalef(0.2f, 2.0f, 4.0f);
  glutSolidCube(1.0);
  glPopMatrix();

  // Barrier gates (raised position)
  glColor3f(0.90f, 0.30f, 0.20f); // Red/white barrier
  for (int side = -1; side <= 1; side += 2) {
    glPushMatrix();
    glTranslatef(side * roadWidth * 0.5f, 2.0f, checkpointZ);
    glRotatef(70.0f, 0.0f, 0.0f, 1.0f * side); // Raised angle
    glScalef(roadWidth * 0.6f, 0.5f, 1.0f);
    glutSolidCube(1.0);
    glPopMatrix();
  }

  // 5) Planters / decorative elements
  glColor3f(0.35f, 0.40f, 0.30f); // Dark green planters
  for (float z = 200.0f; z < 380.0f; z += 60.0f) {
    for (int side = -1; side <= 1; side += 2) {
      float x = side * (roadWidth + 15.0f);

      // Planter box
      glPushMatrix();
      glTranslatef(x, 0.5f, z);
      glScalef(4.0f, 1.0f, 4.0f);
      glutSolidCube(1.0);
      glPopMatrix();

      // Simple "tree" representation
      glColor3f(0.25f, 0.20f, 0.15f); // Brown trunk
      glPushMatrix();
      glTranslatef(x, 2.5f, z);
      glScalef(0.6f, 4.0f, 0.6f);
      glutSolidCube(1.0);
      glPopMatrix();

      glColor3f(0.20f, 0.35f, 0.20f); // Dark green foliage
      glPushMatrix();
      glTranslatef(x, 5.0f, z);
      glutSolidSphere(2.5f, 8, 8);
      glPopMatrix();

      glColor3f(0.35f, 0.40f, 0.30f); // Back to planter color
    }
  }

  // 6) Signage
  glColor3f(0.15f, 0.30f, 0.50f); // Blue sign background
  glPushMatrix();
  glTranslatef(-roadWidth - 20.0f, 5.0f, 350.0f);
  glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
  glScalef(0.3f, 4.0f, 8.0f);
  glutSolidCube(1.0);
  glPopMatrix();

  glEnable(GL_LIGHTING);
}

// ---------------- Cloud system ----------------
struct Cloud {
  float x, y, z;
  float size;
  float alpha;
};
std::vector<Cloud> gClouds;

void initClouds() {
  gClouds.clear();
  for (int i = 0; i < 30; ++i) {
    Cloud c;
    c.x = -800.0f + (float)std::rand() / RAND_MAX * 1600.0f;
    c.y = 250.0f + (float)std::rand() / RAND_MAX * 150.0f;
    c.z = -1000.0f + (float)std::rand() / RAND_MAX * 2000.0f;
    c.size = 40.0f + (float)std::rand() / RAND_MAX * 60.0f;
    c.alpha = 0.15f + (float)std::rand() / RAND_MAX * 0.25f;
    gClouds.push_back(c);
  }
}

void updateClouds(float dt) {
  for (auto &c : gClouds) {
    c.x += 5.0f * dt; // Drift slowly
    if (c.x > 800.0f) {
      c.x = -800.0f;
      c.z = -1000.0f + (float)std::rand() / RAND_MAX * 2000.0f;
    }
  }
}

void drawClouds() {
  if (gWeather.cloudCoverage < 0.01f)
    return;

  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDepthMask(GL_FALSE); // Don't write to depth buffer for clouds

  int drawCount = (int)(gClouds.size() * gWeather.cloudCoverage);
  for (int i = 0; i < drawCount && i < (int)gClouds.size(); ++i) {
    const auto &c = gClouds[i];

    // Draw cloud as multiple overlapping spheres
    float baseAlpha = c.alpha * gWeather.cloudCoverage;
    glColor4f(0.85f, 0.85f, 0.92f, baseAlpha);

    glPushMatrix();
    glTranslatef(c.x, c.y, c.z);

    // Main cloud body (3 spheres)
    glutSolidSphere(c.size * 0.7f, 10, 10);

    glTranslatef(c.size * 0.5f, 0.0f, 0.0f);
    glColor4f(0.88f, 0.88f, 0.95f, baseAlpha * 0.8f);
    glutSolidSphere(c.size * 0.6f, 10, 10);

    glTranslatef(-c.size * 1.0f, c.size * 0.2f, 0.0f);
    glColor4f(0.90f, 0.90f, 0.97f, baseAlpha * 0.7f);
    glutSolidSphere(c.size * 0.55f, 10, 10);

    glPopMatrix();
  }

  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);
  glEnable(GL_LIGHTING);
}

// ---------------- Rain system ----------------
void initRain() {
  gRain.clear();
  gRain.reserve(RAIN_COUNT);
  for (int i = 0; i < RAIN_COUNT; ++i) {
    RainDrop r;
    // Rain only in city area: X=[-500, 500], Z=[-500, 600]
    r.x = -500.0f + (float)std::rand() / RAND_MAX * 1000.0f;
    r.z = -500.0f + (float)std::rand() / RAND_MAX * 1100.0f;
    r.y = (float)std::rand() / RAND_MAX * 300.0f;
    r.vy = 150.0f + (float)std::rand() / RAND_MAX * 50.0f; // fall speed
    gRain.push_back(r);
  }
}

void updateRain(float dt) {
  if (!gWeather.rainEnabled)
    return;

  for (auto &r : gRain) {
    r.y -= r.vy * dt * gWeather.rainIntensity;

    // Reset when hitting ground
    if (r.y < 0.0f) {
      r.y = 300.0f + (float)std::rand() / RAND_MAX * 100.0f;
      r.x = -500.0f + (float)std::rand() / RAND_MAX * 1000.0f;
      r.z = -500.0f + (float)std::rand() / RAND_MAX * 1100.0f;
    }
  }
}

void drawRain() {
  if (!gWeather.rainEnabled || gWeather.rainIntensity < 0.01f)
    return;

  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glBegin(GL_LINES);
  float alpha = 0.3f * gWeather.rainIntensity;
  glColor4f(0.7f, 0.7f, 0.8f, alpha); // Semi-transparent blue-gray

  int drawCount = (int)(RAIN_COUNT * gWeather.rainIntensity);
  for (int i = 0; i < drawCount && i < (int)gRain.size(); ++i) {
    const auto &r = gRain[i];
    glVertex3f(r.x, r.y, r.z);
    glVertex3f(r.x, r.y - 3.0f * gWeather.rainIntensity,
               r.z); // Rain streak length
  }
  glEnd();

  glDisable(GL_BLEND);
  glEnable(GL_LIGHTING);
}

// ---------------- Draw integrated scene ----------------
void drawIntegratedScene() {
  // 新布局（沿Z轴，统一宽度800）：
  // Stella(+500) -> City(0) -> [空隙] -> Dam(-550) -> Sea(-630 to -1030) ->
  // Mesa+Vegas(-1100)

  float timeSec = glutGet(GLUT_ELAPSED_TIME) * 0.001f;

  // 0) 绘制统一地面（Stella到Dam底部）
  drawUnifiedGround();

  // 1) Stella 记忆中心：在城市后方（+Z方向），旋转180度朝向城市
  glPushMatrix();
  glTranslatef(0.0f, 0.0f, 500.0f);
  glRotatef(180.0f, 0.0f, 1.0f, 0.0f); // 旋转180度
  // Stella_drawGround(); // 已取消独立ground
  // Stella_drawSnowField(); // 已取消snow ground

  // Buffer area: Security fence/wall around Stella complex
  drawStellaBuffer();

  Stella_drawStelline();
  Stella_drawAnnexBuildings();
  Stella_drawPlaza();
  Stella_drawStairs();
  Stella_drawSnow(); // 局部下雪
  glPopMatrix();

  // 2) City + LAPD：在世界中心（z=0）
  glPushMatrix();
  // City_drawCity(); // 不使用，因为它包含ground
  // 分别绘制城市组件，不绘制ground
  drawCityRoadsOnly(); // 绘制道路（在统一地面之上）
  City_drawBuildings();
  City_drawOverpass();
  City_drawTowers();
  // LAPD_drawGround(); // 取消独立ground，使用统一地面
  lapdBuilding.LAPD_draw();
  glPopMatrix();

  // 3) Dam 大坝：离城市150单位（z=-550，城市边界-400，间隙150）
  //    大坝城市侧延伸约52，所以总位置约-550避免穿模
  glPushMatrix();
  glTranslatef(0.0f, 0.0f, -550.0f);
  Dam_drawSeaWall();
  glPopMatrix();

  // 4) Ocean 海洋：在大坝前方（深度约400，缩短到原来1/3）
  //    OCEAN_NEAR_Z=-80相对大坝，所以海洋从-630到-1030
  glPushMatrix();
  glTranslatef(0.0f, 0.0f, -550.0f); // 海洋相对大坝绘制
  Dam_drawOcean(timeSec);
  Dam_drawWaveFoam(timeSec);
  Dam_drawSplashLine(timeSec);
  Dam_drawSlopeFoam(timeSec);
  glPopMatrix();

  // 4.5) 沙滩区域：固定在海洋边缘，不随Mesa移动
  //      海洋结束于world Z=-1130，沙滩从这里开始延伸到Mesa
  glPushMatrix();
  glTranslatef(0.0f, 0.0f, -1130.0f); // 海洋终点位置
  
  // Apply beach-specific fog
  applyAreaFog(gFogAreas.beach);

  // 绘制沙滩地面 - 延伸到Mesa边缘
  glDisable(GL_LIGHTING);
  glColor3f(0.76f, 0.70f, 0.50f); // 沙色

  float beachWidth = 450.0f; // 沙滩宽度（X方向）
  float beachDepth =
      270.0f; // 沙滩深度（Z方向） - 延伸到Mesa边缘（-1130 to -1400 = 270）
  int steps = 30;
  float stepZ = beachDepth / steps;

  // 从海洋边缘(z=0)向Mesa(z=-270)渐变高度
  for (int i = 0; i < steps; ++i) {
    float z0 = -i * stepZ;
    float z1 = -(i + 1) * stepZ;

    // 高度从海平面(5.0f)渐变到沙丘高度(30.0f)
    float t0 = (float)i / steps;
    float t1 = (float)(i + 1) / steps;
    // 使用smoothstep
    t0 = t0 * t0 * (3.0f - 2.0f * t0);
    t1 = t1 * t1 * (3.0f - 2.0f * t1);

    float y0 = 5.0f + (30.0f - 5.0f) * t0; // 从海平面到沙丘高度
    float y1 = 5.0f + (30.0f - 5.0f) * t1;

    glBegin(GL_TRIANGLE_STRIP);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-beachWidth, y0, z0);
    glVertex3f(beachWidth, y0, z0);
    glVertex3f(-beachWidth, y1, z1);
    glVertex3f(beachWidth, y1, z1);
    glEnd();
  }

  glEnable(GL_LIGHTING);
  glPopMatrix();

  // 5) Mesa + Vegas：在海的对岸，共享沙丘地面
  //    使用Mesa的沙丘作为统一地面，Vegas和Mesa靠得很近
  //    Moved farther: -1260 -> -1400 (away from ocean, with gap)
  glPushMatrix();
  glTranslatef(0.0f, 0.0f, -1400.0f);
  
  // Apply desert/mesa fog
  applyAreaFog(gFogAreas.desert);

  // 绘制统一的沙丘地面（Mesa的沙丘覆盖整个区域）
  g_trashYard.Mesa_draw();

  // Vegas建筑放在沙丘上，靠近Mesa（右侧，靠近中心）
  // Raise Vegas to match the new dune height (y=30)
  glPushMatrix();
  glTranslatef(200.0f, 30.0f, 0.0f); // Updated from 20.0f to 30.0f
  
  // Apply Vegas-specific fog
  applyAreaFog(gFogAreas.vegas);
  
  // 不绘制Vegas的独立地面，只绘制建筑
  Vegas_drawGlassBuildingBase();
  Vegas_drawConcreteFrameAndSlabs();
  glPopMatrix();

  glPopMatrix();

  // 6) Spinner 巡航：绕城市中心（z=0）巡航
  // If camera is attached to spinner, use manual position; otherwise use patrol path
  if (gSpinnerCameraAttached) {
    // Use manually controlled spinner position
    glPushMatrix();
    glTranslatef(gSpinnerX, gSpinnerY, gSpinnerZ);
    glRotatef(gSpinnerYaw, 0.0f, 1.0f, 0.0f);
    g_spinner.Spinner_draw();
    glPopMatrix();
  } else {
    // Use automatic patrol path
    float t0 = gSpinnerT;
    float t1 = gSpinnerT + 0.002f;
    float x0, y0, z0, x1, y1, z1;
    evalSpinnerPath(t0, x0, y0, z0);
    evalSpinnerPath(t1, x1, y1, z1);

    float dx = x1 - x0;
    float dz = z1 - z0;
    float yawDeg = std::atan2(dx, -dz) * 180.0f / 3.14159265f;

    // Update tracked spinner position for potential camera attachment
    gSpinnerX = x0;
    gSpinnerY = y0;
    gSpinnerZ = z0;
    gSpinnerYaw = yawDeg;

    glPushMatrix();
    glTranslatef(x0, y0, z0);
    glRotatef(yawDeg, 0.0f, 1.0f, 0.0f); // 车头对准运动方向
    g_spinner.Spinner_draw();
    glPopMatrix();
  }
}

// ---------------- GLUT callbacks ----------------
void displayIntegrated() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  applyIntegratedCamera();

  setupDayNightLighting();

  drawIntegratedScene();

  // Draw atmospheric effects
  drawClouds();
  drawRain();

  glutSwapBuffers();
}

void reshapeIntegrated(int w, int h) {
  if (h == 0)
    h = 1;
  float aspect = (float)w / (float)h;

  glViewport(0, 0, w, h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, aspect, 1.0, 5000.0);

  glMatrixMode(GL_MODELVIEW);
}

// Keyboard handlers
void keyboardDown(unsigned char key, int, int) {
  switch (key) {
  case 27: // ESC
    std::exit(0);
    break;
  case 'w':
  case 'W':
    gKeyW = true;
    break;
  case 's':
  case 'S':
    gKeyS = true;
    break;
  case 'a':
  case 'A':
    gKeyA = true;
    break;
  case 'd':
  case 'D':
    gKeyD = true;
    break;
  case 'q':
  case 'Q':
    gKeyQ = true;
    break;
  case 'e':
  case 'E':
    gKeyE = true;
    break;

  // Spinner camera toggle
  case '\\':
    integratedToggleSpinnerCamera(gSpinnerX, gSpinnerY, gSpinnerZ, gSpinnerYaw);
    printf("Spinner camera: %s\n", gSpinnerCameraAttached ? "ATTACHED" : "DETACHED");
    break;

  // Weather controls
  case 'r':
  case 'R':
    // Toggle rain
    gWeather.rainEnabled = !gWeather.rainEnabled;
    printf("Rain: %s\n", gWeather.rainEnabled ? "ON" : "OFF");
    break;
  case '[':
    // Decrease rain intensity
    gWeather.rainIntensity = clamp(gWeather.rainIntensity - 0.2f, 0.0f, 2.0f);
    printf("Rain intensity: %.1f\n", gWeather.rainIntensity);
    break;
  case ']':
    // Increase rain intensity
    gWeather.rainIntensity = clamp(gWeather.rainIntensity + 0.2f, 0.0f, 2.0f);
    printf("Rain intensity: %.1f\n", gWeather.rainIntensity);
    break;
  case 'f':
  case 'F':
    // Decrease fog density
    gWeather.fogDensity = clamp(gWeather.fogDensity - 0.0005f, 0.0f, 0.01f);
    printf("Fog density: %.4f\n", gWeather.fogDensity);
    break;
  case 'g':
  case 'G':
    // Increase fog density
    gWeather.fogDensity = clamp(gWeather.fogDensity + 0.0005f, 0.0f, 0.01f);
    printf("Fog density: %.4f\n", gWeather.fogDensity);
    break;
  case 'c':
  case 'C':
    // Decrease cloud coverage
    gWeather.cloudCoverage = clamp(gWeather.cloudCoverage - 0.1f, 0.0f, 1.0f);
    printf("Cloud coverage: %.1f\n", gWeather.cloudCoverage);
    break;
  case 'v':
  case 'V':
    // Increase cloud coverage
    gWeather.cloudCoverage = clamp(gWeather.cloudCoverage + 0.1f, 0.0f, 1.0f);
    printf("Cloud coverage: %.1f\n", gWeather.cloudCoverage);
    break;
  
  // Area-specific fog controls
  case '1':
    // Decrease Mesa fog
    gFogAreas.mesa = clamp(gFogAreas.mesa - 0.25f, 0.0f, 3.0f);
    printf("Mesa fog: %.2fx\n", gFogAreas.mesa);
    break;
  case '2':
    // Increase Mesa fog
    gFogAreas.mesa = clamp(gFogAreas.mesa + 0.25f, 0.0f, 3.0f);
    printf("Mesa fog: %.2fx\n", gFogAreas.mesa);
    break;
  case '3':
    // Decrease Beach fog
    gFogAreas.beach = clamp(gFogAreas.beach - 0.25f, 0.0f, 3.0f);
    printf("Beach fog: %.2fx\n", gFogAreas.beach);
    break;
  case '4':
    // Increase Beach fog
    gFogAreas.beach = clamp(gFogAreas.beach + 0.25f, 0.0f, 3.0f);
    printf("Beach fog: %.2fx\n", gFogAreas.beach);
    break;
  case '5':
    // Decrease Desert fog
    gFogAreas.desert = clamp(gFogAreas.desert - 0.25f, 0.0f, 3.0f);
    printf("Desert fog: %.2fx\n", gFogAreas.desert);
    break;
  case '6':
    // Increase Desert fog
    gFogAreas.desert = clamp(gFogAreas.desert + 0.25f, 0.0f, 3.0f);
    printf("Desert fog: %.2fx\n", gFogAreas.desert);
    break;
  case '7':
    // Decrease Vegas fog
    gFogAreas.vegas = clamp(gFogAreas.vegas - 0.25f, 0.0f, 3.0f);
    printf("Vegas fog: %.2fx\n", gFogAreas.vegas);
    break;
  case '8':
    // Increase Vegas fog
    gFogAreas.vegas = clamp(gFogAreas.vegas + 0.25f, 0.0f, 3.0f);
    printf("Vegas fog: %.2fx\n", gFogAreas.vegas);
    break;
  
  case 'h':
  case 'H':
    // Print help
    printf("\n=== Camera Controls ===\n");
    printf("\\: Toggle spinner camera (FPS controls)\n");
    printf("WASD: Move forward/left/back/right\n");
    printf("Q/E: Move down/up\n");
    printf("Arrow keys: Rotate view\n");
    printf("Shift: Speed boost (free camera only)\n");
    printf("\n=== Weather Control Keys ===\n");
    printf("R: Toggle rain on/off\n");
    printf("[ / ]: Decrease/increase rain intensity\n");
    printf("F / G: Decrease/increase global fog density\n");
    printf("C / V: Decrease/increase cloud coverage\n");
    printf("\n=== Area Fog Controls ===\n");
    printf("1 / 2: Decrease/increase Mesa fog (%.2fx)\n", gFogAreas.mesa);
    printf("3 / 4: Decrease/increase Beach fog (%.2fx)\n", gFogAreas.beach);
    printf("5 / 6: Decrease/increase Desert fog (%.2fx)\n", gFogAreas.desert);
    printf("7 / 8: Decrease/increase Vegas fog (%.2fx)\n", gFogAreas.vegas);
    printf("\n=== Other Keys ===\n");
    printf("H: Show this help\n");
    printf("P: Show current position\n");
    printf("\n=== Movement Boundaries ===\n");
    printf("X: %.0f to %.0f\n", gBounds.minX, gBounds.maxX);
    printf("Y: %.0f to %.0f (ground to clouds)\n", gBounds.minY, gBounds.maxY);
    printf("Z: %.0f to %.0f (Stella to Mesa)\n", gBounds.minZ, gBounds.maxZ);
    printf("Current position: (%.1f, %.1f, %.1f)\n", gCamX, gCamY, gCamZ);
    printf("Spinner camera: %s\n", gSpinnerCameraAttached ? "ATTACHED" : "DETACHED");
    printf("============================\n\n");
    break;
  case 'p':
  case 'P':
    // Print current position
    printf("Position: (%.1f, %.1f, %.1f) | Yaw: %.1f | Pitch: %.1f\n", gCamX,
           gCamY, gCamZ, gYaw, gPitch);
    break;

  default:
    break;
  }
}

void keyboardUp(unsigned char key, int, int) {
  switch (key) {
  case 'w':
  case 'W':
    gKeyW = false;
    break;
  case 's':
  case 'S':
    gKeyS = false;
    break;
  case 'a':
  case 'A':
    gKeyA = false;
    break;
  case 'd':
  case 'D':
    gKeyD = false;
    break;
  case 'q':
  case 'Q':
    gKeyQ = false;
    break;
  case 'e':
  case 'E':
    gKeyE = false;
    break;
  default:
    break;
  }
}

void mouseMotion(int x, int y) { mouseLook(x, y); }

// ---------------- Arrow keys: rotate view ----------------
void specialIntegrated(int key, int, int) {
  switch (key) {
  case GLUT_KEY_LEFT:
    gYaw -= KEY_ROT_SPEED;
    break;
  case GLUT_KEY_RIGHT:
    gYaw += KEY_ROT_SPEED;
    break;
  case GLUT_KEY_UP:
    gPitch += KEY_ROT_SPEED;
    break;
  case GLUT_KEY_DOWN:
    gPitch -= KEY_ROT_SPEED;
    break;
  case GLUT_KEY_SHIFT_L:
  case GLUT_KEY_SHIFT_R:
    gKeyShift = true;
    break;
  default:
    break;
  }

  // pitch 限制在 [-89, 89]
  gPitch = clamp(gPitch, -CAM_PITCH_LIMIT, CAM_PITCH_LIMIT);

  // wrap yaw to avoid drift and keep basis in sync
  if (gYaw > 360.0f)
    gYaw -= 360.0f;
  else if (gYaw < -360.0f)
    gYaw += 360.0f;

  updateCameraBasis();
}

void specialUpIntegrated(int key, int, int) {
  switch (key) {
  case GLUT_KEY_SHIFT_L:
  case GLUT_KEY_SHIFT_R:
    gKeyShift = false;
    break;
  default:
    break;
  }
}

void idleIntegrated() {
  int now = glutGet(GLUT_ELAPSED_TIME);
  float dt = (now - gPrevTimeMs) * 0.001f;
  if (dt < 0.0f)
    dt = 0.0f;
  if (dt > 0.1f)
    dt = 0.1f; // clamp large jump
  gPrevTimeMs = now;

  updateCamera(dt);

  // Day/night cycle (always auto)
  gTimeOfDay += DAYNIGHT_SPEED * dt;
  if (gTimeOfDay > 24.0f)
    gTimeOfDay -= 24.0f;

  // Spinner patrol (only when camera is not attached)
  if (!gSpinnerCameraAttached) {
    gSpinnerT += SPINNER_SPEED * dt;
    if (gSpinnerT > 1.0f)
      gSpinnerT -= 1.0f;
  }

  // Spinner hover / siren animation
  g_spinner.Spinner_update(dt);

  // Snow update
  Stella_updateSnow(dt);

  // Weather updates
  updateClouds(dt);
  updateRain(dt);

  // Mesa trash yard animation
  g_trashYard.Mesa_update(dt);

  glutPostRedisplay();
}

// ---------------- Integrated init ----------------
void initIntegratedGL() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);
  glShadeModel(GL_SMOOTH);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);

  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Initialize original modules (textures, random layouts, snow, trash yard...)
  City_initCity();
  City_initRoadGrid();
  City_initGL();

  Dam_initGL();
  Vegas_initScene();
  Vegas_initGL();
  Mesa_initGL();
  Stella_initGL();
  Spinner_initGL();

  // Initialize weather systems
  initRain();
  initClouds();

  // Prime camera basis to match initial yaw/pitch
  updateCameraBasis();

  gPrevTimeMs = glutGet(GLUT_ELAPSED_TIME);

  // Print initial help
  printf("\n=== BR2077 Integrated City ===\n");
  printf("\n--- Movement Controls ---\n");
  printf("WASD: Move (FPS style)\n");
  printf("Mouse: Look around\n");
  printf("Q/E: Move down/up\n");
  printf("Shift: Speed boost\n");
  printf("Arrow keys: Rotate view\n");
  printf("\n--- Weather Controls ---\n");
  printf("R: Toggle rain on/off\n");
  printf("[ / ]: Decrease/increase rain intensity\n");
  printf("F / G: Decrease/increase global fog density\n");
  printf("C / V: Decrease/increase cloud coverage\n");
  printf("\n--- Area Fog Controls ---\n");
  printf("1 / 2: Mesa fog (%.2fx)\n", gFogAreas.mesa);
  printf("3 / 4: Beach fog (%.2fx)\n", gFogAreas.beach);
  printf("5 / 6: Desert fog (%.2fx)\n", gFogAreas.desert);
  printf("7 / 8: Vegas fog (%.2fx)\n", gFogAreas.vegas);
  printf("\n--- Other Keys ---\n");
  printf("H: Show help\n");
  printf("P: Show position\n");
  printf("\n--- Movement Boundaries ---\n");
  printf("X: %.0f to %.0f\n", gBounds.minX, gBounds.maxX);
  printf("Y: %.0f to %.0f (ground to clouds)\n", gBounds.minY, gBounds.maxY);
  printf("Z: %.0f to %.0f\n", gBounds.minZ, gBounds.maxZ);
  printf("Starting position: (%.1f, %.1f, %.1f)\n", gCamX, gCamY, gCamZ);
  printf("============================\n\n");
}

// ---------------- main ----------------
int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(1280, 720);
  glutCreateWindow("BR2077 Integrated City - FreeGLUT");

  initIntegratedGL();

  glutDisplayFunc(displayIntegrated);
  glutReshapeFunc(reshapeIntegrated);
  glutKeyboardFunc(keyboardDown);
  glutKeyboardUpFunc(keyboardUp);
  glutSpecialFunc(specialIntegrated);     // 新增：方向键控制视角
  glutSpecialUpFunc(specialUpIntegrated); // Handle Shift key release
  glutPassiveMotionFunc(mouseMotion);
  glutMotionFunc(mouseMotion);
  glutIdleFunc(idleIntegrated);

  glutSetCursor(GLUT_CURSOR_NONE);

  glutMainLoop();
  return 0;
}
