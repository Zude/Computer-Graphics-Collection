/* ---- System Header einbinden ---- */
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

/* ---- Eigene Header einbinden ---- */
#include "logic.h"
#include "types.h"

/* ---- Konstanten ---- */
/** Geschwindigkeit (Schritte in X-Richtung pro Sekunde) */
#define X_STEPS_PS -0.5f
/** Geschwindigkeit (Schritte in Y-Richtung pro Sekunde) */
#define Y_STEPS_PS 0.0f
// Der Mindestabstand der benoetigt wird um eine Schlaegerkollision zu erkennen
#define MIN_BAT_COLLISION_DIST 0.05f

//Geschwindigkeit des Spieler Schlaegers
#define PLAYER_Y_SPEED 1.0f
#define ROTATION_AMOUNT 90.0f

// Geschwidigkeitsfaktor bei Kollision
#define COLL_SPEED 0.2f

/* ---- Globale Daten ---- */
/** Geschwindigkeitsvektor des Valls */

// Bewegung und rotation vom Spielerschläger
static GLboolean g_movement[4] = {GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE};

// Richtungsvektor der Spieler
static CGVector2f rVector = {0.0f, 0.0f};

static int assistsTimer = 120; //Updates nachdem die Hilfslinien wieder verschwinden

// Ortsvektor zum Schlaeger
CGVector2f oVector;
// Vektor des Lotes
CGVector2f lotVec;
// Vektor zwischen Ball und Schlaeger
CGVector2f ballBatVec;

static GLboolean ballDisplayMode;

Ball gBall = {
    .pos = {0.0f, 0.0f},
    .mov = {X_STEPS_PS, 0.0f},
    .speed = BALL_START_SPEED,
    .color = white,
    .type = bRound};

Bat gAi = {
    .pos = {BAT_X_DISTANCE, 0.0f},
    .color = white,
    .lifes = 3};

Player gPlayer = {
    .pos = {-BAT_X_DISTANCE, 0.0f},
    .color = white,
    .lifes = 3,
    .rot = 0.0f,
    .dir = {GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE}};

aLines gLines = {
    .pCollision = GL_FALSE,
    .entryVec = {0.0f, 0.0f},
    .exitVec = {0.0f, 0.0f},
    .lotVec = {0.0f, 0.0f},
    .collisionPoint = {0.0f, 0.0f},
    .speed = 0.0f};

aLines getLines()
{
    return gLines;
}
/**
 * Verändert Balltyp und Farbe
 * Sollte nach jeder Schlägerkollision aufgerufen werden
 */
void changeBall()
{
    if (!ballDisplayMode)
    {
        gBall.type = bRound;
    }
    else
    {
        gBall.type = (ball_Type)(rand() % bLast);
    }
    gBall.color = (colors)(rand() % cLast);
}

// NormalVec der Richtung
void calacBallVec(CGVector2f direction) //normBallDir
{
    direction[0] = direction[0] / sqrt(direction[0] * direction[0] + direction[1] * direction[1]);
    direction[1] = direction[1] / sqrt(direction[0] * direction[0] + direction[1] * direction[1]);
}

// Resetet den Timer
void resetAssists()
{
    assistsTimer--;

    if (assistsTimer < 0)
    {
        assistsTimer = 120;
        gLines.pCollision = GL_FALSE;
    }
}

/**
* Prueft ob der Ball mit dem Schlaeger collidiert
* @return 1 falls eine Kollision stattfindet, sonst 0
*/
int checkBallBatCollision()
{
    //berechnet ob das Lot innerhalb des schlaegers ist
    if (lotVec[1] >= (gPlayer.pos[1] - BAT_HEIGHT) && (lotVec[1] <= gPlayer.pos[1] + BAT_HEIGHT))
    {
        return 1;
    }
    return 0;
}

/**
 * Platziert den Ball auf einer hälfte mit einem zufälligen Winkel
 * @param side sideRight oder sideLeft 
 */
void placeBall(CGSide side)
{
    float a = (rand() % 90) - 45;
    if (side == sideLeft)
    {
        //gBall.pos[0] = (float) rand() / (RAND_MAX / 0.9f);
        //gBall.pos[1] = (float) rand() / (RAND_MAX / 0.9f);
        gBall.pos[0] = -0.4f;
        gBall.pos[1] = 0.0f;
        gBall.mov[0] = sin(-a);
        gBall.mov[1] = cos(-a);
    }
    else if (side == sideRight)
    {
        //gBall.pos[0] = (float) rand() / (RAND_MAX / 0.9f);
        gBall.pos[0] = 0.4f;
        gBall.pos[1] = 0.0f;
        gBall.mov[0] = sin(a);
        gBall.mov[1] = cos(a);
    }
}

/**
* Prueft, ob das Rechteck mit dem Rahmen kollidiert ist.
* @return Rahmenseite, mit der kollidiert wurde.
*/
static CGSide
checkCollision(void)
{
    // collisionsseiten
    CGSide res = sideNone;
    // Ball trifft auf Schlaeger
    if (
        gBall.mov[0] < 0.0f &&
        calcDistanceBallBat() <= MIN_BAT_COLLISION_DIST &&
        checkBallBatCollision())
    {
        res = playerBat;
    }

    /* Ball fliegt nach oben und
    die obere Seite des Balls ueberschreitet den oberen Rand */
    else if (gBall.mov[1] > 0.0f &&
             gBall.pos[1] + BALL_RADIUS >= WALL_Y_DISTANCE)
    {
        res = sideTop;
    }

    /* Ball fliegt nach unten und
    die untere Seite des Balls ueberschreitet den unteren Rand */
    else if (gBall.mov[1] < 0.0f &&
             gBall.pos[1] - BALL_RADIUS <= -WALL_Y_DISTANCE)
    {
        res = sideBottom;
    }

    // Ball trifft den KI schlaeger
    else if (
        (gBall.mov[0] > 0.0f) &&
        (gBall.pos[1] <= (gAi.pos[1] + BAT_HEIGHT)) &&
        (gBall.pos[1] >= (gAi.pos[1] - BAT_HEIGHT)) &&
        (gAi.pos[0] - gBall.pos[0] - BALL_RADIUS <= 0.0f))
    {
        res = aiBat;
    }

    // Ball geht links raus
    else if (gBall.mov[0] < 0.0f && gBall.pos[0] < -1.0f)
    {
        res = sideLeft;
    }

    // Ball geht rechts raus
    else if (gBall.mov[0] > 0.0f && gBall.pos[0] > 1.0f)
    {
        res = sideRight;
    }

    return res;
}

void calcAiPosition(double interval)
{
    // Verschiebt den AI Schlaeger nach Oben
    if (gBall.pos[1] > gAi.pos[1])
    {
        if ((gBall.pos[1] - gAi.pos[1]) > MAX_AI_SPEED * (double)interval)
        {
            gAi.pos[1] += MAX_AI_SPEED * (double)interval;
        }
        else
        {
            gAi.pos[1] += (gBall.pos[1] - gAi.pos[1]);
        }
    }
    // Verschiebt denn AI Schlaeger nach unten
    else if (gBall.pos[1] < gAi.pos[1])
    {
        if ((gAi.pos[1] - gBall.pos[1]) > MAX_AI_SPEED * (double)interval)
        {
            gAi.pos[1] -= MAX_AI_SPEED * (double)interval;
        }
        else
        {
            gAi.pos[1] -= (gAi.pos[1] - gBall.pos[1]);
        }
    }
}

//TODO besserer name
void scoring(CGSide side)
{
    // Wenn der Ball links raus geht ziehe den Spieler Leben ab und spawn neu
    if (side == sideLeft)
    {
        //TODO bessere Position und zufalls Winkel
        gPlayer.lifes -= 1;
        fprintf(stdout, "Du hast noch: %i Leben!!!\n", gPlayer.lifes);
        if (gPlayer.lifes <= 0)
        {
            fprintf(stdout, "KI HAT GEWONNEN! L2P\n");
            exit(0);
        }
        else
        {
            placeBall(side);
            calacBallVec(gBall.mov);
            gBall.speed = BALL_START_SPEED;
        }
    }
    // Wenn der Ball rechts raus geht ziehe AI leben ab und spawn neu
    else if (side == sideRight)
    {
        gAi.lifes -= 1;
        fprintf(stdout, "Ai hat noch: %i Leben!!!\n", gAi.lifes);
        if (gAi.lifes <= 0)
        {
            fprintf(stdout, "DU HAST GEWONNEN! GGEZWP\n");
            exit(0);
        }
        else
        {
            placeBall(side);
            calacBallVec(gBall.mov);
            gBall.speed = BALL_START_SPEED;
        }
    }
}

void calcLot(GLfloat *lotVec, GLfloat *norVec, GLfloat *mirrorPoint, GLfloat *goalVec, GLfloat *ballBatVec)
{
    // Vektoren fuer Zwischenergebnisse
    CGVector2f connectVec;

    // Zwischenergebnis
    float r = 1.0f;

    // Subtrahier Ortsvektor und Ballposition
    connectVec[0] = (lotVec[0]) - (mirrorPoint[0]);
    connectVec[1] = (lotVec[1]) - (mirrorPoint[1]);

    // Berechnet Parameter R als Lambda der Geradengleichung
    r = ((connectVec[0] * (norVec[0]) + connectVec[1] * (norVec[1])) * (-1)) / ((norVec[0]) * (norVec[0]) + (norVec[1]) * (norVec[1]));

    // Berechnet ein Zwischenergebniss
    (goalVec[0]) = (lotVec[0]) + r * (norVec[0]);
    (goalVec[1]) = (lotVec[1]) + r * (norVec[1]);

    // Berechnet den Zielvektor zwischen Lot und Ball
    (ballBatVec[0]) = (goalVec[0]) - (mirrorPoint[0]);
    (ballBatVec[1]) = (goalVec[1]) - (mirrorPoint[1]);
}

/**
* Reagiert auf Kollisionen des Rechtecks mit dem Rahmen.
* @param side Rahmenseite, mit der kollidiert wurde.
*/
static void
handleCollision(CGSide side)
{
    /* Bewegung in Y-Richtung umkehren */
    if (side == sideTop || side == sideBottom)
    {
        gBall.mov[1] *= -1;
        gBall.speed += COLL_SPEED;
    }

    else if (side == sideLeft || side == sideRight)
    {
        scoring(side);
    }

    else if (side == aiBat)
    {
        gBall.mov[0] *= -1;
        gBall.speed += COLL_SPEED;
        changeBall();
    }

    //Spieler Schlaeger wird getroffen
    else if (side == playerBat)
    {
        changeBall();

        //Normalvektor zum Richtungsvektor
        CGVector2f normVec;

        if (gPlayer.rot <= 0.0f)
        {
            normVec[0] = rVector[1];
            normVec[1] = rVector[0] * (-1);
        }
        else
        {
            normVec[0] = rVector[1] * (-1);
            normVec[1] = rVector[0];
        }

        gLines.lotVec[0] = normVec[0];
        gLines.lotVec[1] = normVec[1];

        // Zwischenergebniss
        CGVector2f goalVec;

        // Berechnet den zu Spiegelnen Punkt (Lotpunk + Bewegungsvektor)
        CGVector2f mirrorPoint = {lotVec[0] + gBall.mov[0] * (-1), lotVec[1] + gBall.mov[1] * (-1)};

        // Berechne das Lot
        calcLot((lotVec), (normVec), (mirrorPoint), (goalVec), (ballBatVec));

        // Ortsvektor zum gespiegelten Punkt
        CGVector2f goalPoint = {mirrorPoint[0] + 2 * (ballBatVec[0]), mirrorPoint[1] + 2 * (ballBatVec[1])};

        gLines.entryVec[0] = gBall.mov[0] * -1;
        gLines.entryVec[1] = gBall.mov[1] * -1;

        // Passt den geschwindigkeitsvektor des Balles an
        gBall.mov[0] = goalPoint[0] - lotVec[0];
        gBall.mov[1] = goalPoint[1] - lotVec[1];

        //fprintf(stdout, "BALLSPEED: %F, %F\n", gBall.mov[0], gBall.mov[1]);
        //fprintf(stdout, "SPEED %f\n", gBall.speed);

        //Variablen für die Hilfslinien
        gLines.pCollision = GL_TRUE;
        gLines.speed = gBall.speed;
        gLines.exitVec[0] = gBall.mov[0];
        gLines.exitVec[1] = gBall.mov[1];
        gLines.collisionPoint[0] = lotVec[0];
        gLines.collisionPoint[1] = lotVec[1];
	fprintf(stdout, "Geschwindigkeit: %f\n", gBall.speed);
    }
}

/**
* Berechnet die Parameter der Geradengleichung des Schlaegers
*/
void calcBatLine()
{
    //Ortsvektoren zum Schlaeger initialisieren
    oVector[0] = -BAT_X_DISTANCE + BAT_LENGTH;
    oVector[1] = gPlayer.pos[1];

    //Winkel der Schlaegergeraden zur X achse
    float angle = 90.0f + gPlayer.rot;

    //Steigung der geraden
    float tilt = tan(angle * M_PI / 180.0f);

    //Ein zweiter Punkt auf der Geraden
    CGVector2f secondPoint = {oVector[0] + 1, oVector[1] + tilt};

    //Der Richtungsvektor der geraden initialisieren
    rVector[0] = secondPoint[0] - oVector[0];
    rVector[1] = secondPoint[1] - oVector[1];
}

/**
* Berechnet der Abstand zwischen Ball und Schlaeger
* @return gibt die Distanz in float zurueck
*/
float calcDistanceBallBat()
{
    // Vektoren zwischen Ortvektor der Schlaegergeraden und Ball
    calcBatLine();

    // Berechnet das Lot und die Paramter
    calcLot(oVector, rVector, gBall.pos, lotVec, ballBatVec);

    // Berechet die Laenge des Zielvektors
    return sqrt(ballBatVec[0] * ballBatVec[0] + ballBatVec[1] * ballBatVec[1]);
}

/**
 * Berechnet die Bewegung vom Ball
 * @param interval  
 */
void calcBallPosition(double interval)
{
    calcBatLine();
    calacBallVec(gBall.mov);
    if (gBall.speed > MAX_BALL_SPEED)
    {
        gBall.speed = MAX_BALL_SPEED;
    }

    gBall.pos[0] += gBall.mov[0] * gBall.speed * (float)interval;
    gBall.pos[1] += gBall.mov[1] * gBall.speed * (float)interval;
}

/**
* Setzt den Bewegunsstatus des Spielerschlägers.
* @param direction Bewegungsrichtung deren Status veraendert werden soll.
* @param status neuer Status der Bewegung: GL_TRUE.Rechteck bewegt sich in Richtung
* direction, GL_FALSE.Rechteck bewegt sich nicht in Richtung direction.
*/
void setMovement(CGDirection direction, GLboolean status)
{
    if (direction <= dirDown)
    {
        g_movement[direction] = status;
    }
}

/**
* Berechnet neue Position des Spielers. SchlaegerPosition und Winkel.
* @param interval Dauer der Bewegung in Sekunden.
*/
void calcBatPosition(double interval)
{
    if (g_movement[dirUp])
    {
        gPlayer.pos[1] += PLAYER_Y_SPEED * (float)interval;
        if (gPlayer.pos[1] + BAT_HEIGHT >= WALL_Y_DISTANCE)
        {
            gPlayer.pos[1] = WALL_Y_DISTANCE - BAT_HEIGHT;
        }
    }
    if (g_movement[dirDown])
    {
        gPlayer.pos[1] -= PLAYER_Y_SPEED * (float)interval;
        if (gPlayer.pos[1] - BAT_HEIGHT <= -WALL_Y_DISTANCE)
        {
            gPlayer.pos[1] = -WALL_Y_DISTANCE + BAT_HEIGHT;
        }
    }

    //Rotation
    if (g_movement[dirLeft])
    {
        gPlayer.rot += ROTATION_AMOUNT * (float)interval;
        if (gPlayer.rot >= 35.0f)
        {
            gPlayer.rot = 35.0f;
        }
    }
    if (g_movement[dirRight])
    {
        gPlayer.rot -= ROTATION_AMOUNT * (float)interval;
        if (gPlayer.rot <= -35.0f)
        {
            gPlayer.rot = -35.0f;
        }
    }
}

/**
 * Wechselt den Anzeige Modus vom Ball
 * zwischen Rund und Zufällig
 */
void toggleBallDisplayMode()
{
    ballDisplayMode = !ballDisplayMode;
}

/**
 * 
 *  Check für kollisionen
 *  Update Ball
 *  Update Spieler
 *  Update Ai
 */
void updateLogic(double interval)
{

    //printf("Coords: X: %f Y: %f\n", gBall.pos[0], gBall.pos[1]);

    CGSide collision = checkCollision();

    if (collision != sideNone)
    {
        handleCollision(collision);
    }

    if (gLines.pCollision)
    {
        resetAssists();
    }

    // Aktuallisiert die BallPosition
    calcBallPosition(interval);

    // Aktuallisiert die Schlaegerosition
    calcBatPosition(interval);

    // AKtuallisiert die Schlaegerposition
    calcAiPosition(interval);
}
