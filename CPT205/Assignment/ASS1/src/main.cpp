/**
 * @file main.cc
 * @brief Entry point that sets up GLUT windowing and coordinates scene rendering.
 */

// stl
#include <vector>

// the background pandle elements
#include "Grass.hpp"
#include "Sky.hpp"
#include "Sun.hpp"

// the decoration panel elements
#include "Balloon.hpp"
#include "CentrerBuilding.hpp"
#include "Paper.hpp"
#include "TextLine.hpp"
#include "Tree.hpp"

// the card panel elements
#include "PaperWithText.hpp"

enum class SCENE_MODE {
    BG_ONLY,
    CONGETS,
};

static SCENE_MODE gSceneMode = SCENE_MODE::BG_ONLY;

static int gW = 1100, gH = 800;
static Sky* gSky = nullptr;
static Sun* gSun = nullptr;
static GrassField* gField = nullptr;
static CentralBuilding* gCB = nullptr;
static Paper* gPaper = nullptr;
static TextLine* gTextBar = nullptr;
static PaperWithText* gPaperText = nullptr;

// the decorations
static std::function<void(int, int, int, int, Balloon*)> g_reset_b_on_ex_up_cb =
    [](int x_lb, int y_lb, int w, int h, Balloon* b) {
        if (y_lb > gH) b->resetStatus();
    };
static std::function<void(int, int, int, int, Balloon*)> g_empty_b_cb;
static std::function<void(int, int, int, int, Balloon*)> g_suiside_b_cb =
    [](int x_lb, int y_lb, int w, int g, Balloon* b) {
        if (y_lb > gH) {
            b->killyourself();
        }
    };

static std::vector<Balloon> gRandomBalloonsVec;

static std::vector<Tree> gForest;

static bool gPause = false;
static float gTime = 0.f;

static void reshape(int w, int h) {
    // Force window size to stay at gW x gH
    if (w != gW || h != gH) {
        glutReshapeWindow(gW, gH);
        return;
    }

    glViewport(0, 0, gW, gH);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, gW, 0.0, gH, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if (gSky) gSky->setViewport((float)gW, (float)gH);
}
static void keyboard(unsigned char key, int, int) {
    switch (key) {
        case 'q': {
            exit(0);
        }
    }
}

static void mouse(int button, int state, int x, int y) {
    int flipped_y = gH - y;

    if ((button == GLUT_LEFT_BUTTON || button == GLUT_RIGHT_BUTTON) && state == GLUT_DOWN) {
        if (gSceneMode == SCENE_MODE::BG_ONLY) {
            if (gPaper && gPaper->containsPoint(x, flipped_y)) {
                gSceneMode = SCENE_MODE::CONGETS;
            } else {
                gRandomBalloonsVec.emplace_back(BalloonCfg::makeRandomOneWayVertical(gW, gH));
            }
        } else {
            if (gPaperText && !gPaperText->containsPoint(x, flipped_y)) {
                gSceneMode = SCENE_MODE::BG_ONLY;
            }
        }
    }
}

static void drawGround() {
    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT);
    glColor4f(0.82f, 0.95f, 0.78f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(gW, 0);
    glVertex2f(gW, 140);
    glVertex2f(0, 140);
    glEnd();
    glPopAttrib();
}

static void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (gSky) {
        if (gPause) gSky->setTime(gTime);
        gSky->render();
        gSun->render();
    }

    drawGround();
    if (gCB) gCB->render();
    for (auto& t : gForest) t.render();
    if (gField) gField->render();

    gPaper->render();

    if (!gRandomBalloonsVec.empty()) {
        for (auto& b : gRandomBalloonsVec) {
            b.render(g_suiside_b_cb);
        }
        // it will remove the dead balloons (exceeded the screen)
        gRandomBalloonsVec.erase(
            std::remove_if(gRandomBalloonsVec.begin(), gRandomBalloonsVec.end(),
                           [](const Balloon& b) { return !b.isAlive(); }),
            gRandomBalloonsVec.end());
    }

    gTextBar->render();

    if (gSceneMode == SCENE_MODE::CONGETS && gPaperText) gPaperText->render();

    glutSwapBuffers();
}

static void timer_cb(int) {
    if (!gPause) gTime += 0.016f;  // ~60 fps
    glutPostRedisplay();
    glutTimerFunc(16, timer_cb, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
    glutInitWindowSize(gW, gH);
    glutCreateWindow("Happy 20th Anniversary, XJTLU");

    // the SUN
    SunCfg sun_cfg = {.center = {70.f, 700.f},
                      .radius = 60.f,
                      .rays = 20,
                      .ray_length = 55.f,
                      .pulse_amp = 18.f,
                      .pulse_speed = 2.2f,
                      .rotate_deg_per_sec = 25.f,
                      .line_thickness = 4.f,
                      .core_color = Color4f(1.0f, 0.9f, 0.35f, 1.0f),
                      .ray_color = Color4f(1.0f, 0.80f, 0.15f, 1.0f)};
    gSun = new Sun(sun_cfg);

    // initialize the sky
    SkyCfg sky_cfg = {.width = (float)gW,
                      .height = (float)gH,

                      .bottom_color = Color4f(0.90f, 0.97f, 1.00f, 1.0f),
                      .top_color = Color4f(0.70f, 0.85f, 1.00f, 1.0f),

                      .layers = 3,
                      .clouds_per_layer = 5,
                      .wind_to_right = true,
                      .speed_min = 12.f,
                      .speed_max = 36.f,
                      .scale_min = 0.8f,
                      .scale_max = 1.6f,
                      .cloud_color = Color4f(1.0f, 1.0f, 1.0f, 0.93f),

                      .motive = true,
                      .speed_scale = 1,

                      .seed = -1};
    gSky = new Sky(sky_cfg);

    CentralBuildingCfg gCbCfg;
    gCbCfg.baseW = 1200;
    gCbCfg.baseH = 800;
    gCbCfg.originX = 0;
    gCbCfg.originY = 0;
    gCbCfg.scale = 1.0f;
    gCbCfg.seamSealPx = 3.0f;
    gCbCfg.seamSealList = {1, 5, 6, 7, 10, 11, 12};
    gCB = new CentralBuilding(gCbCfg);

    gField = new GrassField();
    GrassPatchCfg gGfCfg1;
    gGfCfg1.motive = true;
    gGfCfg1.speed = 2;
    gGfCfg1.color = COLORS::GREEN;
    gGfCfg1.win_dir_right = true;
    gField->addPatch(gGfCfg1)
        .setOrigin(100.f, 80.f)
        .setSize(420.f, 100.f)
        .setBladeCount(400)
        .setSegments(10)
        .setWindAmplitude(14.f);

    GrassPatchCfg gGfCfg2;
    gGfCfg2.motive = true;
    gGfCfg2.speed = 1;
    gGfCfg2.color = Color4f(0.50f, 0.75f, 0.30f, 1.0f);
    gGfCfg2.win_dir_right = false;
    gField->addPatch(gGfCfg2)
        .setOrigin(560.f, 70.f)
        .setSize(360.f, 160.f)
        .setBladeCount(480)
        .setSegments(9)
        .setWindAmplitude(10.f);

    GrassPatchCfg gGfCfg3;
    gGfCfg3.motive = true;
    gGfCfg3.speed = 1;
    gGfCfg3.color = Color4f(0.50f, 0.85f, 0.30f, 1.0f);
    gGfCfg3.win_dir_right = false;
    gField->addPatch(gGfCfg3)
        .setOrigin(20.f, 30.f)
        .setSize(360.f, 120.f)
        .setBladeCount(480)
        .setSegments(9)
        .setWindAmplitude(10.f);

    GrassPatchCfg gGfCfg4;
    gGfCfg4.motive = true;
    gGfCfg4.speed = 1;
    gGfCfg4.color = Color4f(0.50f, 0.75f, 0.30f, 1.0f);
    gGfCfg4.win_dir_right = false;
    gField->addPatch(gGfCfg4)
        .setOrigin(300.f, 20.f)
        .setSize(360.f, 160.f)
        .setBladeCount(480)
        .setSegments(4)
        .setWindAmplitude(10.f);

    GrassPatchCfg gGfCfg5;
    gGfCfg5.motive = true;
    gGfCfg5.speed = 1;
    gGfCfg5.color = Color4f(0.50f, 0.75f, 0.30f, 1.0f);
    gGfCfg5.win_dir_right = false;
    gField->addPatch(gGfCfg5)
        .setOrigin(700.f, 20.f)
        .setSize(360.f, 160.f)
        .setBladeCount(480)
        .setSegments(4)
        .setWindAmplitude(10.f);

    for (int i = 0; i < 10; i++) {
        gRandomBalloonsVec.emplace_back(BalloonCfg::makeRandomOneWayVertical(gW, gH));
    }

    gForest.emplace_back(TREE_CFG_PRELOADS::SPRING).setPosition(220, 110);
    gForest.emplace_back(TREE_CFG_PRELOADS::AUTUMN).setPosition(520, 110);
    gForest.emplace_back(TREE_CFG_PRELOADS::AUTUMN).setPosition(720, 110);
    gForest.emplace_back(TREE_CFG_PRELOADS::AUTUMN).setPosition(920, 110);
    gForest.emplace_back(TREE_CFG_PRELOADS::AUTUMN).setPosition(850, 110);
    gForest.emplace_back(TREE_CFG_PRELOADS::KANADE).setPosition(100, 110);

    PaperCfg gPCfg;
    gPCfg.cx = 500;
    gPCfg.cy = 80;
    gPCfg.width = 520.f;
    gPCfg.height = 360.f;
    gPCfg.rotDeg = -15.f;
    gPCfg.persp = 0.16f;
    gPCfg.curl = 34.f;
    gPCfg.drawEdge = true;
    gPCfg.drawShadow = true;
    gPaper = new Paper(gPCfg);
    gPaper->setSize(80.f, 80.f);

    gTextBar = new TextLine("Congratulations XJTLU 20th Anniversary", gW / 2, 500);

    PaperWithTextCfg gPTCfg;
    gPaperText = new PaperWithText(gPTCfg);

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutTimerFunc(16, timer_cb, 0);
    glClearColor(sky_cfg.bottom_color.r, sky_cfg.bottom_color.g, sky_cfg.bottom_color.b, 1.0f);
    glutMainLoop();
    return 0;
}
