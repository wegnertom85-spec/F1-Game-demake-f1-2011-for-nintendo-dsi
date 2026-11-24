#include <nds.h>
#include <stdio.h>
#include <math.h>

/*
 * F1 Melbourne DSi - prototype
 * Original homebrew code; not an extraction or modification of F1 2011.
 * Build target: Nintendo DS/DSi in DS-compatible mode using devkitARM/libnds.
 *
 * Controls:
 *   LEFT/RIGHT or D-pad: steer
 *   A: accelerate
 *   B: brake
 *   X: switch team (Lotus / Mercedes)
 *   START: pause/resume
 */

#define PI 3.14159265358979323846f
#define TRACK_SEGMENTS 48
#define TRACK_RADIUS_X 18.0f
#define TRACK_RADIUS_Z 27.0f

typedef struct {
    float x, z;
} Point2;

static float car_s = 0.0f;
static float lateral = 0.0f;
static float speed = 0.0f;
static int team = 0;              /* 0 = Lotus Renault GP, 1 = Mercedes GP */
static int paused = 0;
static int frame = 0;

static Point2 track[TRACK_SEGMENTS];

static void make_track(void) {
    /*
     * Low-poly closed circuit inspired by Albert Park's flowing layout.
     * This is a playable placeholder layout, not a surveyed-accurate
     * reproduction of the real circuit.
     */
    int i;
    for (i = 0; i < TRACK_SEGMENTS; ++i) {
        float a = (2.0f * PI * (float)i) / (float)TRACK_SEGMENTS;
        float rmod = 1.0f + 0.12f * sinf(3.0f * a) + 0.06f * cosf(5.0f * a);
        track[i].x = cosf(a) * TRACK_RADIUS_X * rmod;
        track[i].z = sinf(a) * TRACK_RADIUS_Z * rmod;
    }
}

static Point2 sample_track(float s) {
    float u = s * (float)TRACK_SEGMENTS;
    int i = ((int)u) % TRACK_SEGMENTS;
    int j = (i + 1) % TRACK_SEGMENTS;
    float t = u - floorf(u);
    Point2 p;
    p.x = track[i].x + (track[j].x - track[i].x) * t;
    p.z = track[i].z + (track[j].z - track[i].z) * t;
    return p;
}

static Point2 track_tangent(float s) {
    Point2 a = sample_track(s);
    Point2 b = sample_track(s + 0.002f);
    Point2 t = { b.x - a.x, b.z - a.z };
    float len = sqrtf(t.x*t.x + t.z*t.z);
    if (len < 0.0001f) len = 1.0f;
    t.x /= len; t.z /= len;
    return t;
}

static void vertex(float x, float y, float z) {
    glVertex3v16(floattof32(x), floattof32(y), floattof32(z));
}

static void quad(float x1,float z1,float x2,float z2,
                 float y, int r,int g,int b) {
    glColor3b(r,g,b);
    glBegin(GL_QUADS);
    vertex(x1,y,z1); vertex(x2,y,z2);
    vertex(x2,y+0.01f,z2); vertex(x1,y+0.01f,z1);
    glEnd();
}

static void draw_ground(void) {
    glColor3b(38,105,48);
    glBegin(GL_QUADS);
    vertex(-55,-0.15f,-65); vertex(55,-0.15f,-65);
    vertex(55,-0.15f,65); vertex(-55,-0.15f,65);
    glEnd();
}

static void draw_track(void) {
    int i;
    /* Asphalt ribbon, red/white kerbs, and outer barriers. */
    for (i = 0; i < TRACK_SEGMENTS; ++i) {
        Point2 a = track[i];
        Point2 b = track[(i+1)%TRACK_SEGMENTS];
        Point2 t = { b.x-a.x, b.z-a.z };
        float len = sqrtf(t.x*t.x+t.z*t.z);
        float nx, nz;
        if (len < 0.001f) continue;
        nx = -t.z/len; nz = t.x/len;

        /* Road width 7.2 world units. */
        glColor3b(72,74,78);
        glBegin(GL_QUADS);
        vertex(a.x+nx*3.6f,0.0f,a.z+nz*3.6f);
        vertex(b.x+nx*3.6f,0.0f,b.z+nz*3.6f);
        vertex(b.x-nx*3.6f,0.0f,b.z-nz*3.6f);
        vertex(a.x-nx*3.6f,0.0f,a.z-nz*3.6f);
        glEnd();

        /* Kerb strips. */
        glColor3b((i/2)%2 ? 220:245, (i/2)%2 ? 220:35, (i/2)%2 ? 220:35);
        glBegin(GL_QUADS);
        vertex(a.x+nx*4.0f,0.015f,a.z+nz*4.0f);
        vertex(b.x+nx*4.0f,0.015f,b.z+nz*4.0f);
        vertex(b.x+nx*3.6f,0.015f,b.z+nz*3.6f);
        vertex(a.x+nx*3.6f,0.015f,a.z+nz*3.6f);
        vertex(a.x-nx*3.6f,0.015f,a.z-nz*3.6f);
        vertex(b.x-nx*3.6f,0.015f,b.z-nz*3.6f);
        vertex(b.x-nx*4.0f,0.015f,b.z-nz*4.0f);
        vertex(a.x-nx*4.0f,0.015f,a.z-nz*4.0f);
        glEnd();

        /* Simple dark barrier ribbons. */
        glColor3b(155,160,165);
        glBegin(GL_QUADS);
        vertex(a.x+nx*5.0f,0.25f,a.z+nz*5.0f);
        vertex(b.x+nx*5.0f,0.25f,b.z+nz*5.0f);
        vertex(b.x+nx*4.85f,0.25f,b.z+nz*4.85f);
        vertex(a.x+nx*4.85f,0.25f,a.z+nz*4.85f);
        glEnd();
    }
}

static void draw_trackside(void) {
    int i;
    /* Lightweight scenery markers: trees, grandstands and lamp posts. */
    for (i = 0; i < TRACK_SEGMENTS; i += 2) {
        Point2 p = track[i];
        Point2 t = track_tangent((float)i/(float)TRACK_SEGMENTS);
        float nx = -t.z, nz = t.x;
        float side = (i % 4) ? 1.0f : -1.0f;
        float x = p.x + nx * 8.0f * side;
        float z = p.z + nz * 8.0f * side;

        if ((i % 6) == 0) {
            /* Tree trunk */
            glColor3b(95,65,35);
            glBegin(GL_QUADS);
            vertex(x-0.18f,0.0f,z); vertex(x+0.18f,0.0f,z);
            vertex(x+0.18f,1.3f,z); vertex(x-0.18f,1.3f,z);
            glEnd();
            glColor3b(25,105,40);
            glBegin(GL_TRIANGLES);
            vertex(x,3.2f,z); vertex(x-1.1f,1.0f,z-0.5f);
            vertex(x+1.1f,1.0f,z+0.5f);
            glEnd();
        } else {
            /* Grandstand block */
            glColor3b(95,100,110);
            glBegin(GL_QUADS);
            vertex(x-1.4f,0.0f,z-0.8f); vertex(x+1.4f,0.0f,z-0.8f);
            vertex(x+1.4f,1.7f,z+0.8f); vertex(x-1.4f,1.7f,z+0.8f);
            glEnd();
            glColor3b(210,210,205);
            glBegin(GL_QUADS);
            vertex(x-1.4f,1.7f,z-0.8f); vertex(x+1.4f,1.7f,z-0.8f);
            vertex(x+1.4f,1.9f,z+0.8f); vertex(x-1.4f,1.9f,z+0.8f);
            glEnd();
        }
    }
}

static void draw_car(void) {
    int r = team == 0 ? 205 : 185;
    int g = team == 0 ? 165 : 195;
    int b = team == 0 ? 25  : 205;

    /* Simplified low-poly nose/engine cover visible from TV camera. */
    glColor3b(r,g,b);
    glBegin(GL_TRIANGLES);
    vertex(-0.48f,0.20f,0.8f); vertex(0.48f,0.20f,0.8f); vertex(0.0f,0.38f,2.1f);
    vertex(-0.48f,0.20f,0.8f); vertex(0.0f,0.38f,2.1f); vertex(-0.22f,0.30f,1.35f);
    vertex(0.48f,0.20f,0.8f); vertex(0.22f,0.30f,1.35f); vertex(0.0f,0.38f,2.1f);
    glEnd();

    glColor3b(25,25,28);
    glBegin(GL_QUADS);
    vertex(-0.20f,0.38f,0.95f); vertex(0.20f,0.38f,0.95f);
    vertex(0.16f,0.42f,1.55f); vertex(-0.16f,0.42f,1.55f);
    glEnd();
}

static void draw_scene(void) {
    Point2 p = sample_track(car_s);
    Point2 t = track_tangent(car_s);
    float angle = atan2f(t.x,t.z);
    float ca = cosf(angle), sa = sinf(angle);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(58.0f, 256.0f/192.0f, 0.5f, 110.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* TV onboard view: camera just above the airbox, looking forward. */
    gluLookAt(p.x - sa*0.4f + ca*lateral, 1.55f,
              p.z - ca*0.4f - sa*lateral,
              p.x + ca*9.0f + ca*lateral, 0.8f,
              p.z + sa*9.0f - sa*lateral,
              0.0f, 1.0f, 0.0f);

    glClearColor(80,155,205);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw_ground();
    draw_track();
    draw_trackside();
    draw_car();
}

static void update_game(void) {
    scanKeys();
    int down = keysHeld();

    if (keysDown() & KEY_START) paused = !paused;
    if (keysDown() & KEY_X) team = 1-team;
    if (paused) return;

    if (down & KEY_A) speed += 0.0018f;
    else speed -= 0.0007f;
    if (down & KEY_B) speed -= 0.0030f;
    if (speed < 0.0f) speed = 0.0f;
    if (speed > 0.035f) speed = 0.035f;

    if (down & KEY_LEFT) lateral -= 0.055f * (0.3f + speed*20.0f);
    if (down & KEY_RIGHT) lateral += 0.055f * (0.3f + speed*20.0f);
    lateral *= 0.94f;
    if (lateral > 2.5f) lateral = 2.5f;
    if (lateral < -2.5f) lateral = -2.5f;

    car_s += speed * 0.0018f;
    if (car_s >= 1.0f) car_s -= 1.0f;
    ++frame;
}

int main(void) {
    videoSetMode(MODE_0_3D);
    vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankB(VRAM_B_TEXTURE);
    vramSetBankC(VRAM_C_MAIN_BG);
    consoleDemoInit();

    glInit();
    glEnable(GL_ANTIALIAS);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glClearColor(80,155,205);
    glClearPolyId(0);
    glClearDepth(0x7FFF);
    glViewport(0,0,255,191);

    make_track();

    iprintf("\\x1b[2J");
    iprintf("F1 MELBOURNE DSi PROTOTYPE\\n");
    iprintf("A: GAS   B: BRAKE\\n");
    iprintf("LEFT/RIGHT: STEER\\n");
    iprintf("X: CHANGE TEAM  START: PAUSE\\n");
    iprintf("TEAM: LOTUS RENAULT GP\\n");

    while (1) {
        update_game();
        draw_scene();

        /* Keep the HUD/team label on the text layer. */
        consoleClear();
        iprintf("F1 MELBOURNE  |  %s\\n", team == 0 ? "LOTUS RENAULT GP" : "MERCEDES GP");
        iprintf("A GAS  B BRAKE  LEFT/RIGHT STEER\\n");
        iprintf("X CHANGE TEAM  START %s\\n", paused ? "RESUME" : "PAUSE");
        if (paused) iprintf("\\nPAUSED");
        glFlush(0);
        swiWaitForVBlank();
    }
    return 0;
}
