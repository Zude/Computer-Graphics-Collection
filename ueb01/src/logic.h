#ifndef __LOGIG_H__
#define __LOGIG_H__

#include "types.h"


Ball gBall;

Bat gAi;

Player gPlayer;

aLines getLines();

void updateLogic();

/**
 * Setzt den Bewegunsstatus des Rechtecks.
 * @param direction Bewegungsrichtung deren Status veraendert werden soll.
 * @param status neuer Status der Bewegung: GL_TRUE->Rechteck bewegt sich in Richtung
 * direction, GL_FALSE->Rechteck bewegt sich nicht in Richtung direction.
 */
void setMovement(CGDirection direction, GLboolean status);

/**
 * Berechnet den Abstand zwischen Ball und Schlaegergeraden
 *  * @return gibt die Distanz in float zurueck
 */
float calcDistanceBallBat();

void updateLogic(double inverall);

void toggleBallDisplayMode();

void getLinesPos();

void getLinesBallAngle();

void getLinesAngle();

#endif
