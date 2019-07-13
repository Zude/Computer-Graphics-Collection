#ifndef __SCENE_H__
#define __SCENE_H__
/* ---- System Header einbinden ---- */
#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

// TODO remove
#include "types.h"

/**
 * Zeichen-Funktion.
 * Stellt die Szene dar. Ein weisses Rechteck wird an der vorher im Logik-Modul
 * berechnet Position gezeichnet.
 * Ausgabe eines Quadrats.
 */
void drawScene2D(void);
void drawScene3D(void);

/**
 * Initialisierung der Szene (inbesondere der OpenGL-Statusmaschine).
 * Setzt Hintergrund- und Zeichenfarbe.
 * @return Rueckgabewert: im Fehlerfall 0, sonst 1.
 */
int initScene(void);

/**
 * (De-)aktiviert den Wireframe-Modus.
 */
void toggleWireframeMode(void);

void toggleNormals(void);

void toggleLighting(void);

void toggleHeadLamp(void);

void toggleAnimation(void);

void setMouseStart(int x, int y);

void moveCamera(int x, int y);

void zoomCamera(GLboolean zoom);

GLdouble *currentCamera(void);

// TODO remove, auch aus der io
void camMove(CGDirection dir);

void switchCam(void);

float distanceFromCam(Trans3f pos);

#endif
