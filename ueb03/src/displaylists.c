/* ---- System Header einbinden ---- */
#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

/* ---- Eigene Header einbinden ---- */
#include "debugGL.h"
#include "displaylists.h"
#include "drawing.h"

#define BRICK_LENGTH 0.5f
#define BRICK_HEIGHT 0.25f

#define DL_RANGE 5 // Anzahl der Displaylisten
static GLuint dlWalls;
static GLuint dlSugar;
static GLuint dlSand;
static GLuint dlFree;
static GLuint dlWater;

static void
createDLWalls(void);
static void
createDLFree(void);
static void
createDLSand(void);
static void
createDLSugar(void);
static void
createDLWater(void);

void
initDisplayLists(void)
{
  dlWalls = glGenLists((GLsizei)DL_RANGE);
  if (dlWalls != 0) {
    createDLWalls();
    dlFree = dlWalls + 1;
    createDLFree();
    dlSand = dlWalls + 2;
    createDLSand();
    dlSugar = dlWalls + 3;
    createDLSugar();
    dlWater = dlWalls + 4;
    createDLWater();
  } else {
    CG_ERROR(("Konnte Displaylisten nicht erzeugen\n"));
  }
}

/**
 * Zeichnet die Displayliste für den ausgewählten Typen
 */
void
drawField(lvFieldType type)
{
  switch (type) {
    case LV_WALL:
      glCallList(dlWalls);
      break;
    case LV_SAND:
      glCallList(dlSand);
      break;
    case LV_SUGAR:
      glCallList(dlSugar);
      break;
    case LV_TARGET:
      drawTarget();
      break;
    case LV_WATER:
      glCallList(dlWater);
      break;
    case LV_FREE:
      glCallList(dlFree);
      break;
    case LV_BLACK:
    default:
      glColor3f(0.0f, 0.0f, 0.0f);
      drawQuad();
      break;
  }
}

static void
fillBackground(void)
{
  glPushMatrix();
  {
    glScalef(2.0f, 2.0f, 1.0f);
    drawQuad();
  }
  glPopMatrix();
}

/**
 * Zeichne einen Stein an der X,Y Position
 * x = 0..4 y = 0..3
 * @params x,y Position
 */
static void
drawBrick(int x, int y)
{
  GLboolean shortBrick = GL_FALSE;

  glPushMatrix();
  {
    switch (y) {
      default:
      case 0:
        glTranslatef(0.0f, 1.0f - BRICK_HEIGHT, 0.0f);
        break;
      case 1:
        glTranslatef(0.0f, BRICK_HEIGHT, 0.0f);
        break;
      case 2:
        glTranslatef(0.0f, -BRICK_HEIGHT, 0.0f);
        break;
      case 3:
        glTranslatef(0.0f, -1.0f + BRICK_HEIGHT, 0.0f);
        break;
    }

    switch (x) {
      default:
      case 0:
        glTranslatef(-0.87f + BRICK_LENGTH / 4, 0.0f, 0.0f);
        shortBrick = GL_TRUE;
        break;
      case 1:
        glTranslatef(-BRICK_LENGTH, 0.0f, 0.0f);
        break;
      case 2:
        glTranslatef(0.0f, 0.0f, 0.0f);
        break;
      case 3:
        glTranslatef(BRICK_LENGTH, 0.0f, 0.0f);
        break;
      case 4:
        glTranslatef(0.87f - BRICK_LENGTH / 4, 0.0f, 0.0f);
        shortBrick = GL_TRUE;
        break;
    }

    if (shortBrick) {
      glScalef(BRICK_LENGTH * 0.95, BRICK_HEIGHT * 1.8, 1.0f);
    } else {
      glScalef(BRICK_LENGTH * 1.9, BRICK_HEIGHT * 1.8, 1.0f);
    }
    drawQuad();
  }
  glPopMatrix();
}

/**
 * Display List für LV_WALL
 */
static void
createDLWalls(void)
{
  glNewList(dlWalls, GL_COMPILE);

  // Zeichne Hintergrund
  glColor3f(1.0f, 1.0f, 1.0f);
  fillBackground();

  // Steine in rot Zeichnen
  glColor3f(1.0f, 0.0f, 0.0f);

  drawBrick(0, 0);
  drawBrick(2, 0);
  drawBrick(4, 0);

  drawBrick(1, 1);
  drawBrick(3, 1);

  drawBrick(0, 2);
  drawBrick(2, 2);
  drawBrick(4, 2);

  drawBrick(1, 3);
  drawBrick(3, 3);

  glEndList();
}

static void
drawDekoQuad(float x, float y, float sx, float sy)
{
  glPushMatrix();
  {
    glTranslatef(x, y, 0.0f);
    glScalef(sx, sy, 1.0f);
    drawQuad();
  }
  glPopMatrix();
}

/**
 * Dislay List für LV_SUGAR
 */
static void
createDLSugar(void)
{
  glNewList(dlSugar, GL_COMPILE);

  glColor3f(1.0f, 1.0f, 1.0f);
  fillBackground();

  glColor3f(0.7f, 0.7f, 0.7f);

  float dots[16][2] = { { -0.2f, 0.3f },  { -0.8f, -0.4f }, { -0.7f, 0.2f },
                        { -0.2f, -0.3f }, { -0.6f, 0.5f },  { -0.7f, -0.8f },
                        { 0.2f, -0.2f },  { 0.8f, 0.3f },   { 0.7f, -0.1f },
                        { 0.1f, 0.4f },   { 0.6f, -0.5f },  { 0.7f, 0.8f },
                        { 0.1f, 0.7f },   { -0.1f, -0.8f }, { 0.4f, 0.3f },
                        { 0.9f, -0.7f } };

  for (int i = 0; i <= 16; i++) {
    drawDekoQuad(dots[i][0], dots[i][1], 0.05f, 0.05f);
  }

  glEndList();
}

/**
 * Display List für LV_FREE
 */
static void
createDLFree(void)
{
  glNewList(dlFree, GL_COMPILE);

  glColor3f(0.2f, 0.2f, 0.2f);
  fillBackground();

  glEndList();
}

static void
drawSandPiece(float x, float y)
{
  glPushMatrix();
  {
    glTranslatef(x, y, 0.0f);
    glScalef(0.5f, 0.5f, 1.0f);
    drawCircle();
  }
  glPopMatrix();
}

/**
 * Display List für LV_SAND
 */
static void
createDLSand(void)
{
  glNewList(dlSand, GL_COMPILE);

  glColor3f(0.5f, 0.3f, 0.0f);
  fillBackground();

  glColor3f(0.9f, 0.8f, 0.5f);

  glPushMatrix();
  {
    // Kreise zu Eclipsen formen
    glScalef(0.9f, 0.6f, 1.0f);
    drawSandPiece(-0.6f, -1.0f);
    drawSandPiece(-0.35f, -0.55f);
    drawSandPiece(0.35f, -0.55f);
    drawSandPiece(0.0f, -1.0f);
    drawSandPiece(0.6f, -1.0f);
    drawSandPiece(0.0f, 0.0f);
  }
  glPopMatrix();

  glEndList();
}

static void
drawWaterWave(float x, float y)
{
  glPushMatrix();
  {
    glTranslatef(x, y, 0.0f);
    glScalef(0.1f, 0.1f, 1.0f);
    drawSinWave();
  }
  glPopMatrix();
}

/**
 * Display List für LV_WATER
 */
static void
createDLWater(void)
{
  glNewList(dlWater, GL_COMPILE);

  glColor3f(0.2f, 0.2f, 1.0f);
  fillBackground();

  glColor3f(0.0f, 0.0f, 0.5f);

  drawWaterWave(-0.4f, 0.6f);
  drawWaterWave(-0.9f, 0.1f);
  drawWaterWave(-0.1f, -0.4f);
  drawWaterWave(-0.7f, -0.7f);

  glEndList();
}
