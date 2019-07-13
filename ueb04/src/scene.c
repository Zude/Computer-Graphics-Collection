/* ---- System Header einbinden ---- */
#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include <math.h>

/* ---- Makros ---- */
#ifndef M_PI
/** Die mathematische Konstante PI */
#define M_PI 3.14159265358979323846
#endif

#ifndef DEG2RAD
/** Winkelumrechnung von Grad nach Radiant */
#define DEG2RAD(x) ((x) / 180.0 * M_PI)
/** Winkelumrechnung von Radiant nach Grad */
#define RAD2DEG(x) ((x) / M_PI * 180.0)
#endif

extern GLboolean Debug;
extern GLboolean ShowSpheres;
extern GLboolean ShowNormals;
extern GLboolean ShowLights;

/* ---- Eigene Header einbinden ---- */
#include "debugGL.h"
#include "drawing.h"
#include "map.h"
#include "scene.h"
#include "sphere.h"
#include "texture.h"
#include "types.h"
#include "waterSimulation.h"

/**
 * Zeichnet einen Pfeil mit Laenge 1 entlang der positiven z-Achse.
 */
static void drawArrow(void) {
  glBegin(GL_LINE_STRIP);
  {
    /* Achse */
    glVertex3f(0.0f, 0.0f, -0.5f);
    glVertex3f(0.0f, 0.0f, 0.5f);
    /* erster Pfeil */
    glVertex3f(0.05f, 0.0f, 0.45f);
    glVertex3f(-0.05f, 0.0f, 0.45f);
    glVertex3f(0.0f, 0.0f, 0.5f);
    /* zweiter Pfeil */
    glVertex3f(0.0f, 0.05f, 0.45f);
    glVertex3f(0.0f, -0.05f, 0.45f);
    glVertex3f(0.0f, 0.0f, 0.5f);
  }
  glEnd();
}

/**
 * Zeichnet Koordinatenachsen (inklusive Beschriftung).
 */
static void drawAxes(void) {
  /* Farben der Koordinatenachsen */
  Col axesColor = {1.0f, 0.5f, 0.0f};
  Col axesTextColor = {1.0f, 1.0f, 0.0f};

  glColor3dv(axesColor);

  /* x-Achse */
  glPushMatrix();
  {
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glScalef(1.0f, 1.0f, 2.0f);
    drawArrow();
  }
  glPopMatrix();

  /* y-Achse */
  glPushMatrix();
  {
    glRotatef(270.0f, 1.0f, 0.0f, 0.0f);
    glScalef(1.0f, 1.0f, 2.0f);
    drawArrow();
  }
  glPopMatrix();

  /* z-Achse */
  glPushMatrix();
  {
    glScalef(1.0f, 1.0f, 2.0f);
    drawArrow();
  }
  glPopMatrix();

  /* Beschriftungen der Koordinatenachsen */

  glColor3dv(axesTextColor);
  glRasterPos3f(1.1f, 0.0f, 0.0f);
  glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'x');
  glRasterPos3f(0.0f, 1.1f, 0.0f);
  glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'y');
  glRasterPos3f(0.0f, 0.0f, 1.1f);
  glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'z');
}

/**
 * Aktiviert die Beleuchtung und zeichnet alle
 * Lichtquellen
 */
static void drawLights(void) {
  glEnable(GL_LIGHTING);

  /* Globale Beleuchtung */
  float lightPos[] = {0.0f, 2.0f, 5.0f, 1.0f};

  glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

  glEnable(GL_LIGHT0);
}

/**
 * Zeichnet die Scene
 */
void drawScene(void) {

  if (ShowLights) {
    drawLights();
  }

  if (getTexturingStatus()) {
    /* Texturierung aktivieren */
    glEnable(GL_TEXTURE_2D);
  }

  bindTexture(texLand);
  glPushMatrix();
  {
    glScalef(ISLAND_SCALE, ISLAND_SCALE, ISLAND_SCALE);
    drawIsland();
  }
  glPopMatrix();

  WaterPoint wa[getArrayFactor() * getArrayFactor()];

  for (int y = 0; y < getArrayFactor(); y++) {
    for (int x = 0; x < getArrayFactor(); x++) {

      wa[(getArrayFactor() * y) + x][0] = indexToCoord(x);
      wa[(getArrayFactor() * y) + x][1] =
          getHeightAtIndex((getArrayFactor() * y) + x);
      wa[(getArrayFactor() * y) + x][2] = indexToCoord(y);
    }
  }

  bindTexture(texWater);

  drawWater(wa, getArrayFactor());

  if (Debug) {
    bindTexture(texFace);
    drawSkyBox();
  }
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);

  /* Keine Beleuchtung etc für Kugeln */
  if (ShowSpheres) {
    drawSpheres(wa, getArrayFactor());
  }

  if (ShowNormals) {
    drawNormals();
  }

  if (Debug) {
    drawAxes();
  }

  /* drawNormals(); */
}

/**
 * Zeichnet die Scene für das Picking
 */
void drawPickingScene(void) {

  WaterPoint wa[getArrayFactor() * getArrayFactor()];

  for (int y = 0; y < getArrayFactor(); y++) {
    for (int x = 0; x < getArrayFactor(); x++) {

      wa[(getArrayFactor() * y) + x][0] = indexToCoord(x);
      wa[(getArrayFactor() * y) + x][1] =
          getHeightAtIndex((getArrayFactor() * y) + x);
      wa[(getArrayFactor() * y) + x][2] = indexToCoord(y);
    }
  }

  drawSpheres(wa, getArrayFactor());
}

/**
 * Initialisiert alle Lichtquellen
 */
void initLights(void) {
  GLfloat light0_ambient[] = {0.2, 0.2, 0.2, 1.0};
  GLfloat light0_diffuse[] = {0.7, 0.1, 0.4, 1.0};
  GLfloat light0_specular[] = {1.0, 1.0, 1.0, 1.0};

  glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
}

/**
 * Initialisierung der Szene (inbesondere der OpenGL-Statusmaschine).
 * Setzt Hintergrund- und Zeichenfarbe.
 * @return Rueckgabewert: im Fehlerfall 0, sonst 1.
 */
int initScene(void) {
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  initSphereDL(2);
  initLights();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_COLOR_MATERIAL);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);

  /* Backfaceculling */
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  return (GLGETERROR == GL_NO_ERROR);
}

/**
 * (De-)aktiviert den Wireframe-Modus.
 */
void toggleWireframeMode(void) {
  /* Flag: Wireframe: ja/nein */
  static GLboolean wireframe = GL_FALSE;

  /* Modus wechseln */
  wireframe = !wireframe;

  if (wireframe) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}

/* Variablen für Kamera */
GLdouble camera[9];
GLfloat ax = 45;
GLfloat az = 45;

GLfloat distance = 2.0f;

/**
 * Bewegt die Camera in die angegebene Richtung
 * @param dir: Richtungs enum
 */
void camDirMove(CamDirection dir) {
  switch (dir) {
  case dirUp:
    az = az - 3;
    break;
  case dirDown:
    az = az + 3;
    break;
  case dirLeft:
    ax = ax + 3;
    break;
  case dirRight:
    ax = ax - 3;
    break;
  }

  ax = (int)ax % 360;
  /* Winkel limitieren */
  if (az >= CAM_LOWER_CUTOFF) {
    az = CAM_LOWER_CUTOFF;
  } else if (az <= CAM_UPPER_CUTOFF) {
    az = CAM_UPPER_CUTOFF;
  }
}

/**
 * Verändert die Distanz der Kamera
 * @param zoom: Boolean ob die Kamera distanz verringert oder vergrößert werden
 * soll
 */
void zoomCamera(GLboolean zoom) {
  if (zoom) {
    distance -= 0.2f;
  } else {
    distance += 0.2f;
  }

  if (distance > CAM_MAX_ZOOM) {
    distance = CAM_MAX_ZOOM;
  } else if (distance < CAM_MIN_ZOOM) {
    distance = CAM_MIN_ZOOM;
  }
}

GLdouble *currentCamera(void) {
  /* Winkel zu Radiant */
  float rx = DEG2RAD(ax);
  float rz = DEG2RAD(az);

  /* Kreis auf X Z Achse */
  float nx = distance * sin(rz) * cos(rx);
  float ny = distance * sin(rz) * sin(rx);
  float nz = distance * cos(rz);
  /* ny und nz verstauscht weil ny oben ist */

  camera[0] = nx;
  camera[1] = nz;
  camera[2] = ny;

  /* Kamera immer zum Mittelpunk ausrichten */
  camera[3] = 0.0f;
  camera[4] = 0.0f;
  camera[5] = 0.0f;

  /* Y Achse ist oben */
  camera[6] = 0.0f;
  camera[7] = 1.0f;
  camera[8] = 0.0f;

  return camera;
}
