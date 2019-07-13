/* ---- System Header einbinden ---- */

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
/* ---- Eigene Header einbinden ---- */

#include "types.h"
#include "waterSimulation.h"

/* ---- Eigene Typen ---- */

/* ---- Eigene Konstanten ---- */
#define INIT_HEIGHT 0.0f
#define SIM_CALLS_PS 60

// Globale Var fuer Pause
extern GLboolean Pause;

// Hohe die beim Picken addiert/subtrahiert wird
#define PICK_HEIGHT 0.5
// Ausbreitungsgeschwindigkeit
#define EXPAND_VELOCITY 0.01
// Faktor zur abschwächung der geschwindigkeit
#define VELOCITY_FACTOR 0.9

/* ---- Eigene Variablen ---- */

// Zwischenspeichern der vergangenen Zeit fuer den idle Timer
int lastCallTime = 0;

// Array/Pointer zum speichern der Hoeheninformationen der Punkte
GLfloat *heightArray;
int heightArraySize = 0;

// Array/Pointer with the Simulation Velocity Data
GLfloat *velocityArray;
int velocityArraySize = 0;

// Aktuelle Array groesse als Faktor (3 == 3x3 array groesse)
int factor = 3;

/* ---- Funktionen Helper ---- */

/**
 * Setzt die Hohe des heightArrays an der Stelle Index auf newHeight
 * @param index: Index Poistion where to set the new Heighrt
 * @param newHeight: Value for the new Height
 */
static void setHeightAtIndex(int index, GLfloat newHeight) {
  assert(index >= 0);
  heightArray[index] = newHeight;
}

/**
 * Berechnet den Nachbar Index (oben, unten, rechts, links) in einem 1D Array
 * welcher einen 2D array repraesentieren soll
 * @param index: Index
 * @param factor: Size of inidicies factor*factor
 * @param dir: Direction of neighbor (left, right, top, bot)
 * @Returns the Index of the Neighbor, Returns Index if there is so vailid
 * neighbor. Returns -1 if invalid dir
 */
int getNeighborIndex(int index, int factor, CamDirection dir) {
  int arrSize = factor * factor;

  if (dir == dirLeft) {
    if (index != 0 && index / factor == (index - 1) / factor) {
      return (index - 1);
    } else {
      return index;
    }
  } else if (dir == dirRight) {
    if (index / factor == (index + 1) / factor) {
      return (index + 1);
    } else {
      return index;
    }
  } else if (dir == dirUp) {
    if (index - factor >= 0) {
      return (index - factor);
    } else {
      return index;
    }
  } else if (dir == dirDown) {
    if (index + factor < arrSize) {
      return (index + factor);
    } else {
      return index;
    }
  } else {
    return -1;
  }
}

/**
 * Fuehrt den Berechnungsschritt des Water Simulation Algorithmus aus
 * @param dt: Vergangene Zeit, seit der letzen Berechnung
 */
static void performWaterSimulationStep(double dt) {
  // Lokaler Array Initialisieren fuer zwischengespeicherte Hoehenwerte
  GLfloat *newHeightValArr;
  newHeightValArr = malloc(sizeof(GLfloat) * heightArraySize);
  /* copy(heightArray, 0, newHeighValArr, 0, heightArraySize); */
  memcpy(newHeightValArr, heightArray, sizeof(GLfloat) * heightArraySize);

  // Berechnungsschritt
  for (int i = 0; i < heightArraySize; i++) {
    GLfloat h = 1.0f / factor;
    // Kraft f ausrechnen
    GLfloat f =
        EXPAND_VELOCITY * EXPAND_VELOCITY *
        (heightArray[getNeighborIndex(i, sqrt(heightArraySize), dirLeft)] +
         heightArray[getNeighborIndex(i, sqrt(heightArraySize), dirRight)] +
         heightArray[getNeighborIndex(i, sqrt(heightArraySize), dirUp)] +
         heightArray[getNeighborIndex(i, sqrt(heightArraySize), dirDown)] -
         4 * heightArray[i]) /
        (h * h);

    // Neue Geschwindigkeit ausrechnen
    velocityArray[i] = velocityArray[i] + f * dt;
    // Neue Geschwindigkeit abschwaechen
    velocityArray[i] = velocityArray[i] * VELOCITY_FACTOR;

    // Neue Hohe ausrechnen und zwischenspeichern
    newHeightValArr[i] = heightArray[i] + velocityArray[i] * dt;
  }

  // Uebertrage Werte in das HeightArray
  /* copy(newHeighValArr, 0, heightArray, 0, heightArraySize); */
  memcpy(heightArray, newHeightValArr, sizeof(GLfloat) * heightArraySize);

  free(newHeightValArr);
  newHeightValArr = NULL;
}

/**
 * Initialisiert das Array "velocityArray" mit einem Groesse Factor und dem Wert
 * Value.
 * @param factor: Die Groeße des Arrays als Faktor (4 ist zb 4x4 Array)
 * @param startValue: Startwert fuer die Elemente des Arrays
 */
static void initialiseVelocityArray(GLfloat startValue) {
  velocityArray = malloc(sizeof(GLfloat) * factor * factor);
  velocityArraySize = factor * factor;
  for (int i = 0; i < velocityArraySize; i++) {
    velocityArray[i] = startValue;
  }
}

/**
 * Aendert die Groesse des Heigharrays (und velocityArr)
 * @param factor: Die Groeße des Arrays als Faktor (4 ist zb 4x4 Array)
 * @param startValue: Startwert fuer die Elemente des Arrays
 */
void resizevelocityArray(int oldFactor) {
  GLfloat *tempVelocity;
  tempVelocity = malloc(sizeof(GLfloat) * oldFactor * oldFactor);

  memcpy(tempVelocity, velocityArray, sizeof(GLfloat) * oldFactor * oldFactor);

  velocityArray = realloc(velocityArray, sizeof(GLfloat) * factor * factor);

  velocityArraySize = factor * factor;

  for (int i = 0; i < velocityArraySize; i++) {
    velocityArray[i] = INIT_HEIGHT;
  }

  if (oldFactor > factor) {
    for (int y = 0; y < factor; y++) {
      for (int x = 0; x < factor; x++) {
        velocityArray[y * factor + x] = tempVelocity[y * oldFactor + x];
      }
    }

  } else {
    for (int y = 0; y < oldFactor; y++) {
      for (int x = 0; x < oldFactor; x++) {
        velocityArray[y * factor + x] = tempVelocity[y * oldFactor + x];
      }
    }
  }

  free(tempVelocity);
  tempVelocity = NULL;
}

/**
 * Speicher Freigeben fuer velocityArray
 */
void freevelocityArray() {
  free(velocityArray);
  velocityArray = NULL;
  velocityArraySize = 0;
}

/* ---- Funktionen Public ---- */

int getArrayFactor() { return factor; }

void initialiseHeightArray(int sfactor, GLfloat startValue) {
  assert(sfactor >= 0);
  factor = sfactor;
  heightArray = malloc(sizeof(GLfloat) * factor * factor);
  heightArraySize = factor * factor;
  for (int i = 0; i < heightArraySize; i++) {
    heightArray[i] = startValue;
  }
  initialiseVelocityArray(startValue);
}

void resizeHeightArray(GLboolean biggerBool) {
  int oldFactor = factor;

  if ((factor + 1) * (factor + 1) <= MAX_WATER ||
      (factor - 1) * (factor - 1) <= MIN_WATER) {

    if (biggerBool) {
      factor++;
    } else {
      factor--;
    }
    GLfloat *tempHeight;
    tempHeight = malloc(sizeof(GLfloat) * heightArraySize);

    memcpy(tempHeight, heightArray, sizeof(GLfloat) * heightArraySize);

    heightArray = realloc(heightArray, sizeof(GLfloat) * factor * factor);

    heightArraySize = factor * factor;

    for (int i = 0; i < heightArraySize; i++) {
      heightArray[i] = INIT_HEIGHT;
    }

    if (!biggerBool) {
      for (int y = 0; y < factor; y++) {
        for (int x = 0; x < factor; x++) {
          heightArray[y * factor + x] = tempHeight[y * oldFactor + x];
        }
      }

    } else {

      for (int y = 0; y < oldFactor; y++) {
        for (int x = 0; x < oldFactor; x++) {
          heightArray[y * factor + x] = tempHeight[y * oldFactor + x];
        }
      }
    }
    free(tempHeight);
    tempHeight = NULL;
    resizevelocityArray(oldFactor);
  }
}

void freeHeightArray() {
  free(heightArray);
  heightArray = NULL;
  heightArraySize = 0;
  freevelocityArray();
}

void pickHeightAtIndex(int index, GLboolean leftMouse) {
  if (leftMouse) {
    setHeightAtIndex(index, getHeightAtIndex(index) + PICK_HEIGHT);
  } else {
    setHeightAtIndex(index, getHeightAtIndex(index) + PICK_HEIGHT * (-1));
  }
}

GLfloat getHeightAtIndex(int index) { return heightArray[index]; }

GLfloat *getHeightArray() { return heightArray; }

void performWaterSimulation() {
  if (!Pause) {

    // Aktuell vergangene Zeit nach Programmstart
    double thisCallTime = (double)glutGet(GLUT_ELAPSED_TIME);

    // Zeit seit letzem idle aufruf
    double interval =
        (double)(thisCallTime - lastCallTime) / 1000.0f * SIM_CALLS_PS;

    performWaterSimulationStep(interval);
  }
  // letze vergangene Zeit nach Programmstart
  lastCallTime = glutGet(GLUT_ELAPSED_TIME);
}
