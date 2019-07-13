#ifndef __TYPES_H__
#define __TYPES_H__

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

/*
  Beinhaltet globale Pointer, Structuren und Enums
*/

GLboolean Usage;  // Bei GL_TRUE wird die Usage gezeichnet und das Spiel
                  // pausiert
GLboolean Pause;

#define DEFAULT_WINDOW_WIDTH 600
#define DEFAULT_WINDOW_HEIGHT 600

/** Bewegungsrichtungen. */
typedef enum { dirLeft, dirRight, dirUp, dirDown } CGDirection;

/* Avatar vom Spieler */
typedef struct {
  int x;
  int y;
} Avatar;

typedef struct SugarList {
  int x;
  int y;
  float timer;
  struct SugarList *next;
} SugarList;

typedef struct WaterList {
  int x;
  int y;
  struct WaterList *next;
} WaterList;

#endif
