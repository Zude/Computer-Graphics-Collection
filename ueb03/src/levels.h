#ifndef __LEVELS_H__
#define __LEVELS_H__

/* ---- System Header einbinden ---- */
#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include "types.h"

/* Hoehe/Breite des (quadratischen) Levels */
#define LEVELSIZE (8)
#define LEVELCOUNT 4
/*
 * Art von Levelfeldern
 * LV_OUTER: Außenmauer
 * LV_FREE: leeres Feld
 * LV_BLACK: schwarzes Feld
 * LV_WALL: Unzerstörbares Feld
 * LV_TARGET: Zielfeld
 * LV_WATER : Wasser
 * LV_SAND : Sandsack, bewegbares Feld
 * LV_SUGAR : Zuckerstück, bewegbares Feld
 */
typedef enum lvFieldType {
  LV_INVALID,
  LV_FREE,
  LV_BLACK,
  LV_WALL,
  LV_TARGET,
  LV_WATER,
  LV_NEWWATER,
  LV_SAND,
  LV_SUGAR,
  LV_TOUCHED_SUGAR
} lvFieldType;

// Sind NEWWATER und TOUCHED_SUGAR wirklich nötig?

/* Spielfeld */
typedef lvFieldType level[LEVELSIZE][LEVELSIZE];

/* Zeiger auf ein Spielfeld */
typedef lvFieldType (*levelPointer)[LEVELSIZE];

/* Informationen ueber ein spezifisches */
typedef struct fieldInformation {
  int x;
  int y;
  lvFieldType type;
} fieldInformation;

/* INFO
 * Avatar hat eigene Koordinaten und befindet sich nicht
 * im Array vom Spielfeld. Ansonsten gäbe es eine doppelte
 * Belegung mit den leeren Feldern.
 */

/**
 * Setzt gCurrentLevel auf eines der Level
 * @param level Die Nummer des Levels
 * @return GLboolean FALSE falls das Level nicht geladen werden konnte, z.B.
 * weil es nicht exisiert
 */
levelPointer loadLevel(int level);

#endif
