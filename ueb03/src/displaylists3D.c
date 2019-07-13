/* ---- System Header einbinden ---- */
#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include <math.h>

/* ---- Eigene Header einbinden ---- */
#include "debugGL.h"
#include "displaylists3D.h"
#include "drawing3D.h"
#include "materials.h"
#include "types.h"

#define DL_3D_RANGE 5 // Anzahl der Displaylisten
static GLuint dlWalls;
static GLuint dlSugar;
static GLuint dlSand;
static GLuint dlFree;
static GLuint dlWaterDeko;

/* TODO subDivs als define? Sollten am Ende eh alle gleich sein oder? */
static void
createDLWalls3D(float subDivs);
static void
createDLFree3D(void);
static void
createDLSand3D(void);
static void
createDLSugar3D(float subDivs);
static void
createDLWaterDeko3D(void);

static void
drawDekoQuad3D(float x, float y, float sx, float sy)
{
  glPushMatrix();
  {
    glTranslatef(x, y, 0.0f);
    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
    glScalef(sx, sy, 1.0f);
    drawQuad3D();
  }
  glPopMatrix();
}

void
initDisplayLists3D(void)
{
  dlWalls = glGenLists((GLsizei)DL_3D_RANGE);
  if (dlWalls != 0) {
    createDLWalls3D(4);
    dlFree = dlWalls + 1;
    createDLFree3D();
    dlSand = dlWalls + 2;
    createDLSand3D();
    dlSugar = dlWalls + 3;
    createDLSugar3D(4);
    dlWaterDeko = dlWalls + 4;
    createDLWaterDeko3D();
  } else {
    CG_ERROR(("Konnte Displaylisten nicht erzeugen\n"));
  }
}

/**
 * Zeichnet die Displayliste (3D) für den ausgewählten Typen
 */
void
drawField3D(lvFieldType type)
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
      drawTarget3D();
      break;
    case LV_FREE:
      glCallList(dlFree);
      break;
    case LV_WATER:
      glCallList(dlWaterDeko);
      break;
    case LV_BLACK:
    default:
      /* TODO einfach keinen Block zeichnen oder? */
      // drawQuad3D();
      break;
  }
}

static void
drawWallSection(Rot4f rot, Trans3f trans, int subDivs)
{
  glPushMatrix();
  {
    glRotatef(rot[0], rot[1], rot[2], rot[3]);
    glTranslatef(trans[0], trans[1], trans[2]);
    setMaterial(RED);
    drawSquare3D(subDivs);
    setMaterial(WHITE);
    drawAllBrickGaps3D();
  }
  glPopMatrix();
}

/**
 * Display List für LV_WALL in 3D
 */
static void
createDLWalls3D(float subDivs)
{
  glNewList(dlWalls, GL_COMPILE);

  Trans3f t = { 0.0f, 0.0f, 0.5f };
  /* Rueckseitenflaeche */
  Rot4f r0 = { 180.0f, 0.0f, 1.0f, 0.0f };
  drawWallSection(r0, t, subDivs);

  /* Bodenflaeche */
  Rot4f r1 = { 90.0f, 1.0f, 0.0f, 0.0f };
  drawWallSection(r1, t, subDivs);

  /* rechte Seitenflaeche */
  Rot4f r2 = { 90.0f, 0.0f, 1.0f, 0.0f };
  drawWallSection(r2, t, subDivs);

  /* linke Seitenflaeche */
  Rot4f r3 = { 270.0f, 0.0f, 1.0f, 0.0f };
  drawWallSection(r3, t, subDivs);

  /* Deckelflaeche */
  Rot4f r4 = { -90.0f, 1.0f, 0.0f, 0.0f };
  drawWallSection(r4, t, subDivs);

  /* Frontflaeche */
  Rot4f r5 = { 0.0f, 0.0f, 0.0f, 0.0f };
  drawWallSection(r5, t, subDivs);

  glEndList();
}

/**
 * Display List für LV_FREE (3D)
 */
static void
createDLFree3D(void)
{
  glNewList(dlFree, GL_COMPILE);

  setMaterial(GREY);

  glPushMatrix();
  {
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, -0.5f);
    drawSquare3D(6);
  }
  glPopMatrix();

  glEndList();
}

static void
drawSandBall(Trans3f trans)
{
  setMaterial(SAND);
  glPushMatrix();
  {
    glScalef(0.4, 0.4, 0.4);
    glTranslatef(trans[0], trans[1], trans[2]);
    GLUquadric* quad;
    quad = gluNewQuadric();
    gluSphere(quad, 0.5, 100, 20);
    gluDeleteQuadric(quad);
  }
  glPopMatrix();
}

/**
 * Display List für LV_SAND (3D)
 */
static void
createDLSand3D(void)
{
  glNewList(dlSand, GL_COMPILE);

  // Sack 1
  Trans3f t1 = { 0.6f, -1.0f, 0.6f };
  drawSandBall(t1);
  // Sack 2
  Trans3f t2 = { 0.6f, -1.0f, -0.6f };
  drawSandBall(t2);
  // Sack 3
  Trans3f t3 = { -0.6f, -1.0f, -0.0f };
  drawSandBall(t3);
  // Sack 4
  Trans3f t4 = { 0.0f, -0.5f, 0.0f };
  drawSandBall(t4);

  // Boden

  drawField3D(LV_FREE);

  glEndList();
}

static void
drawSugarSection(Rot4f rot, Trans3f trans, int subDivs)
{
  float dots[16][2] = { { -0.2f, 0.3f },  { -0.8f, -0.4f }, { -0.7f, 0.2f },
                        { -0.2f, -0.3f }, { -0.6f, 0.5f },  { -0.7f, -0.8f },
                        { 0.2f, -0.2f },  { 0.8f, 0.3f },   { 0.7f, -0.1f },
                        { 0.1f, 0.4f },   { 0.6f, -0.5f },  { 0.7f, 0.8f },
                        { 0.1f, 0.7f },   { -0.1f, -0.8f }, { 0.4f, 0.3f },
                        { 0.9f, -0.7f } };

  setMaterial(WHITE);

  glPushMatrix();
  {
    glRotatef(rot[0], rot[1], rot[2], rot[3]);
    glTranslatef(trans[0], trans[1], trans[2]);
    drawSquare3D(subDivs);
  }
  glPopMatrix();

  // Punkte
  setMaterial(GREY);

  glPushMatrix();
  {
    glRotatef(rot[0], rot[1], rot[2], rot[3]);
    glTranslatef(trans[0], trans[1], trans[2] + 0.03f);
    glScalef(0.4f, 0.4f, 0.4f);
    for (int i = 0; i <= 16; i++) {
      drawDekoQuad3D(dots[i][0], dots[i][1], 0.25f, 0.25f);
    }
  }
  glPopMatrix();
}

/**
 * Dislay List für LV_SUGAR
 */
static void
createDLSugar3D(float subDivs)
{
  glNewList(dlSugar, GL_COMPILE);

  Trans3f t = { 0.0f, 0.0f, 0.5f };

  Rot4f r0 = { 180.0f, 0.0f, 1.0f, 0.0f };
  drawSugarSection(r0, t, subDivs);

  Rot4f r1 = { 90.0f, 1.0f, 0.0f, 0.0f };
  drawSugarSection(r1, t, subDivs);

  /* rechte Seitenflaeche */
  Rot4f r2 = { 90.0f, 0.0f, 1.0f, 0.0f };
  drawSugarSection(r2, t, subDivs);

  /* linke Seitenflaeche */
  Rot4f r3 = { 270.0f, 0.0f, 1.0f, 0.0f };
  drawSugarSection(r3, t, subDivs);

  /* Deckelflaeche */
  Rot4f r4 = { -90.0f, 1.0f, 0.0f, 0.0f };
  drawSugarSection(r4, t, subDivs);

  /* Frontflaeche */
  Rot4f r5 = { 0.0f, 0.0f, 0.0f, 0.0f };
  drawSugarSection(r5, t, subDivs);

  // Boden
  glPushMatrix();
  {
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, 0.5f);
    drawQuad3D();
  }
  glPopMatrix();

  glEndList();
}


static void
createDLWaterDeko3D(void) {

  glNewList(dlWaterDeko, GL_COMPILE);

  setMaterial(WHITE);

  glLineWidth(6.0f);

  int l = 4;
  Trans3f pos[] = { { -0.2f, 0.2f, -0.15f },
                    { -0.5f, 0.0f, 0.2f },
                    { 0.0f, 0.4f, -0.3f },
                    { -0.4f, -0.4f, 0.4f } };

  for (int a = 0; a < l; a++) {
    glPushMatrix();
    {
      glTranslatef(pos[a][0], pos[a][1], pos[a][2]);

      glScalef(0.05f, 0.05f, 0.05f);
      glBegin(GL_LINE_STRIP);
      {
        for (float i = 0.0f; i < 12; i = i + 0.3) { // Feinheit der Linie
          glVertex3f(i * 0.8f, sin(i), 0.0f);
          /* glVertex3f(i * 0.8f, sin(i) + 0.3, 0.0f);  // Breite der Linie */
        }
      }
      glEnd();
    }
    glPopMatrix();
  }
glEndList();
}
