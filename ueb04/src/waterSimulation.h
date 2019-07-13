#ifndef __WATERSIMULATION_H_
#define __WATERSIMULATION_H_
#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include "types.h"

/**
 * Initialisiert das Array "heightArray" mit einem Groesse Factor und dem Wert
 * Value. Ausserdem wird das Vellocity Array auch Initialisiert
 * @param factor: Die Groeße des Arrays als Faktor (4 ist zb 4x4 Array)
 * @param startValue: Startwert fuer die Elemente des Arrays
 */
void initialiseHeightArray(int factor, GLfloat startValue);

/**
 * Aendert die Groesse des Heigharrays (und velocityArr)
 * @param factor: Die Groeße des Arrays als Faktor (4 ist zb 4x4 Array)
 * @param startValue: Startwert fuer die Elemente des Arrays
 */
void resizeHeightArray(GLboolean biggerBool);

/**
 * Gibt die aktuelle groesse des Heights Array als Factor zurueck
 * @param factor: Die Groeße des Arrays als Faktor (4 ist zb 4x4 Array)
 */
int getArrayFactor();

/**
 * Gibt den Speicher fuer heighArray (und velocityArray) frei
 */
void freeHeightArray();

/**
 * Pickt den HeightArray an der Stelle Index und erhoeht bzw verringer die Hoehe
 *  abhaengig von der Maustaste um einen Konstanten Wert PICK_HEIGHT
 * @param index: Index wo gepickt wird
 * @param leftMouse: Bool ob die linke Maus gedrueckt wurde, false bedeutet
 * rechte Maustaste
 */
void pickHeightAtIndex(int index, GLboolean leftMouse);

/**
 * Gibt die aktuelle Hoehe am Index des heightArray zurueck
 * @param index: Index wo gesucht wird
 * @return: Die Hoehe am index
 */
GLfloat getHeightAtIndex(int index);

/**
 * Returnt den Pointer zum Array
 * @return: Pointer zum Array
 */
GLfloat *getHeightArray();

/**
 * Fuehrt die Wassersimulation aus. Ein Simulationsschritt wird stepCountPS mal
 * durchgeführt
 * @param dt: vergangene Zeit seit letzem aufruf
 * soll
 */
void performWaterSimulation();

int getNeighborIndex(int index, int factor, CamDirection dir);
#endif
