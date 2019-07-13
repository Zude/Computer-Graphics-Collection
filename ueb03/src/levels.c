/* ---- System Header einbinden ---- */
#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include <string.h>

/* ---- Eigene Header einbinden ---- */
#include "debugGL.h"
#include "levels.h"

/* ---- Alle Level ---- */
// Level Vorlage
level lvEmpty = { { LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK },
                  { LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK },
                  { LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK },
                  { LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK },
                  { LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK },
                  { LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK },
                  { LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK },
                  { LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK,
                    LV_BLACK } };

level lv1 = {
  { LV_WALL, LV_WALL, LV_WALL, LV_WALL, LV_WALL, LV_WALL, LV_WALL, LV_WALL },
  { LV_WALL, LV_FREE, LV_SAND, LV_FREE, LV_FREE, LV_FREE, LV_FREE, LV_WALL },
  { LV_WALL, LV_SAND, LV_WALL, LV_WATER, LV_FREE, LV_SAND, LV_FREE, LV_WALL },
  { LV_WALL, LV_FREE, LV_SAND, LV_SUGAR, LV_WALL, LV_WALL, LV_FREE, LV_WALL },
  { LV_WALL, LV_WATER, LV_SAND, LV_SUGAR, LV_FREE, LV_FREE, LV_WALL, LV_WALL },
  { LV_WALL, LV_FREE, LV_FREE, LV_WALL, LV_WALL, LV_FREE, LV_FREE, LV_WALL },
  { LV_WALL, LV_FREE, LV_FREE, LV_FREE, LV_FREE, LV_FREE, LV_TARGET, LV_WALL },
  { LV_WALL, LV_WALL, LV_WALL, LV_WALL, LV_WALL, LV_WALL, LV_WALL, LV_WALL }
};

level lv2 = {
  { LV_WALL, LV_WALL, LV_WALL, LV_WALL, LV_WALL, LV_WALL, LV_WALL, LV_WALL },
  { LV_WALL, LV_FREE, LV_FREE, LV_FREE, LV_WALL, LV_WATER, LV_WALL, LV_WALL },
  { LV_WALL, LV_FREE, LV_FREE, LV_SAND, LV_FREE, LV_FREE, LV_FREE, LV_WALL },
  { LV_WALL, LV_FREE, LV_SAND, LV_SAND, LV_FREE, LV_SAND, LV_FREE, LV_WALL },
  { LV_WALL, LV_WALL, LV_FREE, LV_SAND, LV_FREE, LV_SAND, LV_FREE, LV_WALL },
  { LV_WALL, LV_WALL, LV_SUGAR, LV_SUGAR, LV_WALL, LV_SUGAR, LV_FREE, LV_WALL },
  { LV_WALL, LV_WATER, LV_FREE, LV_FREE, LV_FREE, LV_FREE, LV_TARGET, LV_WALL },
  { LV_WALL, LV_WALL, LV_WALL, LV_WALL, LV_WALL, LV_WALL, LV_WALL, LV_WALL }
};

level lv3 = {
  { LV_WALL, LV_WALL, LV_WALL, LV_WALL, LV_WALL, LV_WALL, LV_BLACK, LV_BLACK },
  { LV_WALL, LV_FREE, LV_SAND, LV_FREE, LV_SAND, LV_WALL, LV_WALL, LV_BLACK },
  { LV_WALL, LV_SAND, LV_SAND, LV_SAND, LV_SAND, LV_FREE, LV_WALL, LV_BLACK },
  { LV_WALL, LV_WATER, LV_SUGAR, LV_FREE, LV_SAND, LV_FREE, LV_WALL, LV_BLACK },
  { LV_WALL, LV_WALL, LV_FREE, LV_SAND, LV_SAND, LV_FREE, LV_WALL, LV_WALL },
  { LV_BLACK, LV_WALL, LV_WALL, LV_WALL, LV_FREE, LV_FREE, LV_TARGET, LV_WALL },
  { LV_BLACK, LV_BLACK, LV_BLACK, LV_WALL, LV_WALL, LV_FREE, LV_WALL, LV_WALL },
  { LV_BLACK,
    LV_BLACK,
    LV_BLACK,
    LV_BLACK,
    LV_WALL,
    LV_WALL,
    LV_WALL,
    LV_BLACK }
};

level lv4 = {
  { LV_WALL, LV_WALL, LV_WALL, LV_WALL, LV_WALL, LV_WALL, LV_WALL, LV_WALL },
  { LV_WALL, LV_FREE, LV_FREE, LV_FREE, LV_FREE, LV_WALL, LV_WATER, LV_WALL },
  { LV_WALL, LV_FREE, LV_FREE, LV_FREE, LV_SUGAR, LV_SUGAR, LV_FREE, LV_WALL },
  { LV_WALL, LV_FREE, LV_FREE, LV_FREE, LV_FREE, LV_SUGAR, LV_FREE, LV_WALL },
  { LV_WALL, LV_FREE, LV_SUGAR, LV_FREE, LV_FREE, LV_SUGAR, LV_FREE, LV_WALL },
  { LV_WALL,
    LV_FREE,
    LV_SUGAR,
    LV_SUGAR,
    LV_SUGAR,
    LV_SUGAR,
    LV_FREE,
    LV_WALL },
  { LV_WALL, LV_WATER, LV_WALL, LV_FREE, LV_FREE, LV_FREE, LV_TARGET, LV_WALL },
  { LV_WALL, LV_WALL, LV_WALL, LV_WALL, LV_WALL, LV_WALL, LV_WALL, LV_WALL }
};

levelPointer
loadLevel(int level)
{
  INFO(("Lade Level Nr %i\n...", level));

  levelPointer copiedLevel = malloc(sizeof(lvEmpty));

  switch (level) {
    case 4:
      memcpy(copiedLevel, lv4, sizeof(lvEmpty));
      break;

    case 3:
      memcpy(copiedLevel, lv3, sizeof(lvEmpty));
      break;

    case 2:
      memcpy(copiedLevel, lv2, sizeof(lvEmpty));
      break;

    case 1:
    default:
      memcpy(copiedLevel, lv1, sizeof(lvEmpty));
  }

  return copiedLevel;
}
