#ifndef __LOGIC_H__
#define __LOGIC_H__

#include "levels.h"
#include "types.h"

/**
 * Updated die gesamte Spiellogic
 * @param interval Abgelaufene Zeit seit dem letzten Update
 */
void updateLogic(double interval);

/**
 * Versucht den Avatar zu bewegen
 * @param inputDir Richtung in die der Avatar bewegt werden soll
 */
void movePlayer(CGDirection inputDir);

/**
 * Gibt die X Koordinate des Spielers zurueck
 * @return die X Koordinate
 */
int getPlayerX();

/**
 * Gibt currentStep Time zurueck
 */
double getCurrentStepTime();

/**
 * Gibt die Maximale Step Time des Spielers zurueck
 */
double getMaxStepTime();

/**
 * Gibt zurueck, ob die Spielerbewegung aktuell erlaub ist
 */
GLboolean isPlayerStepAllowed();

/**
 * Gib die Y Koordinate des Spielers zurueck
 * @return die Y Koordinate
 */
int getPlayerY();

CGDirection getPlayerDir();

lvFieldType getFieldAt(int x, int y);

void initLv(int level);

GLboolean isAlive();

GLboolean getPlayerWin();

void resetLevel();

void nextLevel();

double sugarTimerAt(int x, int y);

float *getDirCoords(float *start, float distance, CGDirection dir);

#endif
