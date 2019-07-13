/* ---- System Header einbinden ---- */
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include <math.h>

/* ---- Eigene Header einbinden ---- */
#include "scene.h"
#include "logic.h"
#include "debugGL.h"
#include "types.h"

extern Ball gBall;
extern Player gPlayer;
extern Bat gAi;

//Variablen für die Hilfslinien

static GLboolean assists = GL_FALSE;

aLines Lines;

//Rotation für Animation
static int ball_Rotation = 0;

/**
 * Rotiert den Ball
 */
void incRot()
{
    ball_Rotation = (ball_Rotation + ROT_SPEED) % 360;
}

/**
 * Ändert die Farbe mit der gezeichnet wird
 */
void changeColor(colors color)
{
    float
        r = 0,
        b = 0,
        g = 0;

    switch (color)
    {
    case white:
        r = 255;
        b = 255;
        g = 255;
        glColor3f(r / 255.0f, g / 255.0f, b / 255.0f);
        break;
    case red:
        r = 255;
        b = 0;
        g = 0;
        glColor3f(r / 255.0f, g / 255.0f, b / 255.0f);
        break;
    case blue:
        r = 0;
        b = 255;
        g = 0;
        glColor3f(r / 255.0f, g / 255.0f, b / 255.0f);
        break;
    case green:
        r = 0;
        b = 0;
        g = 255;
        glColor3f(r / 255.0f, g / 255.0f, b / 255.0f);
        break;
    case yellow:
        r = 255;
        b = 255;
        g = 0;
        glColor3f(r / 255.0f, g / 255.0f, b / 255.0f);
        break;
    case cyan:
        r = 0;
        b = 255;
        g = 255;
        glColor3f(r / 255.0f, g / 255.0f, b / 255.0f);
        break;
    case magenta:
        r = 255;
        b = 0;
        g = 255;
        glColor3f(r / 255.0f, g / 255.0f, b / 255.0f);
        break;
    default:
        glColor3f(r / 255.0f, g / 255.0f, b / 255.0f);
    }
}

/**
 * Draws a normal 1x1 quad
 */
void drawQuad(void)
{
    glBegin(GL_QUADS);
    {
        glVertex2f(1.0f, 1.0f);

        glVertex2f(-1.0f, 1.0f);

        glVertex2f(-1.0f, -1.0f);

        glVertex2f(1.0f, -1.0f);
    }
    glEnd();
}

/**
 * Draws colored lines on the players bat
 * that show how the playball will hit the bat
 * and the new direction after the collision
 */
void drawAssists()
{
    /*
    1. Punkt liegt auf dem Schläger
        X Y der Kollision von logic
    2. Punkt liegt zwischen Ball und Schläger
        Eingangswinkel von Logic
    3. Punkt hat 90° Winkel zum Schläger
        90° offset von Logic
    4. Punkt hat invertierten Einfallswinkel vom Ball
        selbst berechnen
    */
    glLineWidth(4.0f);

    glBegin(GL_LINES);
    {
        changeColor(blue);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(Lines.entryVec[0], Lines.entryVec[1]);

        changeColor(yellow);

        glVertex2f(0.0f, 0.0f);
        glVertex2f(Lines.exitVec[0], Lines.exitVec[1]);

        changeColor(white);

        glVertex2f(0.0f, 0.0f);
        glVertex2f(Lines.lotVec[0], Lines.lotVec[1]);

        changeColor(green);
        //glVertex2f (0.0f, 0.0f);

        glVertex2f(Lines.entryVec[0], Lines.entryVec[1]);
        glVertex2f(Lines.exitVec[0], Lines.exitVec[1]);
    }
    glEnd();
}

/**
 * Draws the round version of the playball 
 */
void drawBRound()
{
    int points = 12;
    float slice = 2 * 3.14 / points;
    glBegin(GL_POLYGON);
    {
        for (int i = 0; i <= points; i++)
        {
            float angle = slice * i;
            float x = 1.0 * cos(angle);
            float y = 1.0 * sin(angle);

            glVertex2f(x, y);
        }
    }
    glEnd();
}

/**
 * Draws the star version of the playball
 */
void drawBStar()
{
    int points = 16;
    float slice = 2 * 3.14 / points;
    glBegin(GL_TRIANGLE_FAN);
    {
        glVertex2f(0, 0);
        for (int i = 0; i <= points; i++)
        {
            float x = 0;
            float y = 0;

            float angle = slice * i;
            if (i % 2)
            {
                x = 1.0 * cos(angle);
                y = 1.0 * sin(angle);
            }
            else
            {
                x = 0.5 * cos(angle);
                y = 0.5 * sin(angle);
            }
            glVertex2f(x, y);
        }
    }
    glEnd();
}

/**
 * Draws the triangle version of the playball
 */
void drawBTriangle()
{
    int points = 3;
    float slice = 2 * 3.14 / points;

    glBegin(GL_TRIANGLES);
    {
        for (int i = 0; i < 3; i++)
        {
            float angle = slice * i;

            float x = 1.0 * cos(angle);
            float y = 1.0 * sin(angle);
            glVertex2f(x, y);
        }
    }
    glEnd();
}

/**
 * Selects the function thats used to draw the playball
 * @param type which type should be drawn
 */
void drawBall(int type)
{

    changeColor(gBall.color);

    switch (type)
    {
    case bRound:
        drawBRound();
        break;
    case bStar:
        drawBStar();
        break;
    case bTriangle:
        drawBTriangle();
        break;
    default:
        drawBRound();
        break;
    }
}

/**
 * Draws the entire scene
 */
void drawScene(void)
{
    incRot();

    //Untere Wand
    glPushMatrix();
    {
        glTranslatef(0.0, -WALL_Y_DISTANCE, 0.0);
        glScalef(WALL_LENGTH, WALL_HEIGHT, 1.0);
        drawQuad();
    }
    glPopMatrix();

    //Obere Wand
    glPushMatrix();
    {
        glTranslatef(0.0, WALL_Y_DISTANCE, 0.0);
        glScalef(WALL_LENGTH, WALL_HEIGHT, 1.0);
        drawQuad();
    }
    glPopMatrix();

    //KI Schlaeger
    changeColor(gAi.color);
    glPushMatrix();
    {
        glTranslatef(BAT_X_DISTANCE, gAi.pos[1], 0.0);
        glScalef(BAT_LENGTH, BAT_HEIGHT, 1.0);
        drawQuad();
    }
    glPopMatrix();

    changeColor(gPlayer.color);
    //Spieler Schlaeger
    glPushMatrix();
    {
        glTranslatef(-BAT_X_DISTANCE, gPlayer.pos[1], 0.0);
        glRotatef(gPlayer.rot, 0, 0, 1);
        glScalef(BAT_LENGTH, BAT_HEIGHT, 1.0);
        drawQuad();
    }
    glPopMatrix();

    //Winkelhilfe

    Lines = getLines();
    if (assists && Lines.pCollision)
    {
	//fprintf(stdout, "Geschwindigkeit: %f\n", Lines.speed);
        glPushMatrix();
        {
            glTranslatef(Lines.collisionPoint[0], Lines.collisionPoint[1], 0.0);
            glScalef(0.2, 0.2, 1.0);
            drawAssists();
        }
        glPopMatrix();
    }

    //Ball
    glPushMatrix();
    {
        glTranslated(gBall.pos[0], gBall.pos[1], 0.0);
        glRotatef(ball_Rotation, 0, 0, 1);
        glScalef(BALL_RADIUS, BALL_RADIUS, 0.0);
        drawBall(gBall.type);
    }
    glPopMatrix();
}

/**
 * Initialisierung der Szene (inbesondere der OpenGL-Statusmaschine).
 * Setzt Hintergrund- und Zeichenfarbe.
 * @return Rueckgabewert: im Fehlerfall 0, sonst 1.
 */
int initScene(void)
{
    /* Setzen der Farbattribute */
    /* Hintergrundfarbe */
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    /* Zeichenfarbe */
    glColor3f(1.0f, 1.0f, 1.0f);

    /* Linienbreite */
    glLineWidth(3.0f);

    /* Alles in Ordnung? */
    return (GLGETERROR == GL_NO_ERROR);
}

/**
 * (De-)aktiviert den Wireframe-Modus.
 */
void toggleWireframeMode(void)
{
    /* Flag: Wireframe: ja/nein */
    static GLboolean wireframe = GL_FALSE;

    /* Modus wechseln */
    wireframe = !wireframe;

    if (wireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

/**
 * Toggles the assisting lines
 */
void toggleAssistingLines()
{
    assists = !assists;
}
