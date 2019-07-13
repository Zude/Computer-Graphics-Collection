#include "map.h"
#include "types.h"
#include "waterSimulation.h"

#define TEX_RANGE_A 0.0f
#define TEX_RANGE_B 1.0f
#define COORD_RANGE_A -1.0f
#define COORD_RANGE_B 1.0f

GLdouble mapRange(GLdouble a1, GLdouble a2, GLdouble b1, GLdouble b2,
                  GLdouble s) {
  return b1 + (s - a1) * (b2 - b1) / (a2 - a1);
}

GLdouble coordToTex(GLdouble coord) {
  return mapRange(COORD_RANGE_A, COORD_RANGE_B, TEX_RANGE_A, TEX_RANGE_B,
                  coord);
}

GLdouble indexToCoord(int index) {
  return mapRange(0, getArrayFactor() - 1, COORD_RANGE_A, COORD_RANGE_B,
                  (GLdouble)index);
}
