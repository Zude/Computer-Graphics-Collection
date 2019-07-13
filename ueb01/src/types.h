#ifndef __TYPES_H__
#define __TYPES_H__
/**
 * @file
 * Typenschnittstelle.
 * Das Modul kapselt die Typdefinitionen und globalen Konstanten des Programms.
 *
 * Bestandteil eines Beispielprogramms fuer Animationen mit OpenGL & GLUT.
 *
 * @author copyright (C) Fachhochschule Wedel 1999-2011. All rights reserved.
 */


/* ---- System Header einbinden ---- */
#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

/* ---- Konstanten fürs Layout ---- */
#define ROT_SPEED 5
#define BALL_RADIUS 0.05f

#define WALL_Y_DISTANCE 0.95f
#define WALL_HEIGHT 0.05f
#define WALL_LENGTH 1.9f

#define BAT_LENGTH 0.05f
#define BAT_HEIGHT 0.15f
#define BAT_X_DISTANCE 0.92f

#define HILFSLINIEN 0.1f

/* ---- Konstanten für die Logik ---- */

#define MAX_AI_SPEED 0.5f
#define MAX_BALL_SPEED 2.0f
#define BALL_START_SPEED 1.0f

/* ---- Typedeklarationen ---- */
/** Punkt im 2D-Raum */
typedef GLfloat CGPoint2f[2];

/** Vektor im 2D-Raum */
typedef GLfloat CGVector2f[2];

// Enum fuer die versch. Balltypen
typedef enum ball_Type
{
    bRound,
    bStar,
    bTriangle,
    bLast
} ball_Type;

// Verschiedene Farben
typedef enum colors
{
    white,
    red,
    blue,
    green,
    yellow,
    cyan,
    magenta,
    cLast
} colors;



/** Kollisionsseiten. */
enum e_Side
{
    sideNone,
    playerBat,
    sideBottom,
    sideTop,
    aiBat,
    sideLeft,
    sideRight
};

/** Datentyp fuer Kollisionsseiten. */
typedef enum e_Side CGSide;

/** Bewegungsrichtungen. */
enum e_Direction
{
    dirLeft,
    dirRight,
    dirUp,
    dirDown
};

/** Datentyp fuer Bewegungsrichtungen. */
typedef enum e_Direction CGDirection;


typedef struct Ball
{
    CGPoint2f pos;
    CGVector2f mov;// Bewegung, nicht geschwindigkeit
    double speed;
    colors color;
    ball_Type type;
} Ball;


typedef struct Bat
{
    CGPoint2f pos;
    colors color;
    int lifes;
} Bat;

typedef struct Player
{
    CGPoint2f pos;
    colors color;
    int lifes;
    float rot;
    CGDirection dir[4];
} Player;


typedef struct aLines
{
    GLboolean pCollision;
    CGVector2f entryVec;
    CGVector2f exitVec;
    CGVector2f lotVec;
    CGPoint2f collisionPoint;
    float speed; 
} aLines;
#endif
