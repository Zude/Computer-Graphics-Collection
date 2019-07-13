/* ---- System Header einbinden ---- */
#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include <math.h>
#include <stdarg.h>
#include <stdio.h>

/* ---- Eigene Header einbinden ---- */
#include "drawing3D.h"
#include "levels.h"
#include "materials.h"
#include "scene.h"

/* ---- Variablen ---- */
// Bool ob Normalen gezeichnet werden soll oder nicht
extern GLboolean showNormals;
extern GLboolean Debug;

/* ---- Funktionen ---- */

/**
 * Zeichnet einen Pfeil mit Laenge 1 entlang der positiven z-Achse.
 */
static void
drawArrow3D()
{
  glBegin(GL_LINE_STRIP);
  {
    /* Achse */
    glVertex3f(0.0f, 0.0f, -0.5f);
    glVertex3f(0.0f, 0.0f, 0.5f);
    /* erster Pfeil */
    glVertex3f(0.05f, 0.0f, 0.45f);
    glVertex3f(-0.05f, 0.0f, 0.45f);
    glVertex3f(0.0f, 0.0f, 0.5f);
    /* zweiter Pfeil */
    glVertex3f(0.0f, 0.05f, 0.45f);
    glVertex3f(0.0f, -0.05f, 0.45f);
    glVertex3f(0.0f, 0.0f, 0.5f);
  }
  glEnd();
}

/**
 * Zeichne eine Fuge fuer den Wand Block
 */
void
drawBrickGap3D()
{
  glBegin(GL_QUAD_STRIP);
  {
    // setzt die Normale
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.5f, 0.5f, 0.0f);
    glVertex3f(-0.5f, 0.5f, 0.0f);

    glVertex3f(0.5f, 0.0f, 0.1f);
    glVertex3f(-0.5f, 0.0f, 0.1f);

    glVertex3f(0.5f, -0.5f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.0f);
  }
  glEnd();
}

void
drawGapPiece(Trans3f trans, Rot4f rot, Scale3f scale)
{
  setMaterial(WHITE);
  glPushMatrix();
  {
    glRotatef(rot[0], rot[1], rot[2], rot[3]);
    glTranslatef(trans[0], trans[1], trans[2]);
    glScalef(scale[0], scale[1], scale[2]);
    drawBrickGap3D();
  }
  glPopMatrix();
}

/**
 * Zeichnet alle Fugen fuer eine Wandseite, samt Muster
 */
void
drawAllBrickGaps3D(void)
{

  Rot4f r0 = { 0.0f, 0.0f, 0.0f, 0.0f };
  Rot4f r1 = { 90.0f, 0.0f, 0.0f, 1.0f };
  Scale3f s0 = { 1.0, 0.025, 0.025 };
  Scale3f s1 = { 0.25, 0.025, 0.025 };

  Trans3f t0 = { 0.0f, 0.25f, 0.0f };
  drawGapPiece(t0, r0, s0);

  Trans3f t1 = { 0.0f, 0.5f, 0.0f };
  drawGapPiece(t1, r0, s0);

  Trans3f t2 = { 0.0f, -0.5f, 0.0f };
  drawGapPiece(t2, r0, s0);

  Trans3f t3 = { 0.0f, -0.5f, 0.0f };
  drawGapPiece(t3, r1, s0);

  Trans3f t4 = { 0.0f, 0.5f, 0.0f };
  drawGapPiece(t4, r1, s0);

  Trans3f t5 = { 0.0f, -0.25f, 0.0f };
  drawGapPiece(t5, r0, s0);

  Trans3f tx = { 0.0f, 0.0f, 0.0f };
  drawGapPiece(tx, r0, s0);

  Trans3f t6 = { -0.375f, 0.0f, 0.0f };
  drawGapPiece(t6, r1, s1);

  Trans3f t7 = { 0.125f, 0.0f, 0.0f };
  drawGapPiece(t7, r1, s1);

  Trans3f t8 = { -0.125f, -0.25f, 0.0f };
  drawGapPiece(t8, r1, s1);

  Trans3f t9 = { -0.125f, 0.25f, 0.0f };
  drawGapPiece(t9, r1, s1);

  Trans3f t10 = { 0.375f, 0.25f, 0.0f };
  drawGapPiece(t10, r1, s1);

  Trans3f t11 = { 0.375f, -0.25f, 0.0f };
  drawGapPiece(t11, r1, s1);
}

/**
 * Sortiert die Wasserblock Seiten basierend auf dem Abstand zur Kamera
*@param a: Wasserseite A
*@param b: Wasserseite B
 */
static int
cmpWS(const void* a, const void* b)
{
  WaterSide* blockA = (WaterSide*)a;
  WaterSide* blockB = (WaterSide*)b;
  return ((blockA->distance < blockB->distance) -
          (blockA->distance > blockB->distance));
}

/**
 * Sortiert die Wasserblock Seiten basierend auf dem Abstand zur Kamera
*@param water: Array mit wasserseiten
*@param node: Wasserblock
 */
static void
sortWaterSides(WaterSide* water, WaterBlockList node)
{
//Water Array durchlaufe und Distanz zur kamera berechen
  for (int i = 0; i < 6; i++) {
    if (fabsf(180.0f - water[i].rota[0]) < __DBL_EPSILON__) {

      float posArray[3];

      posArray[0] = node.pos[0];
      posArray[1] = node.pos[1];
      posArray[2] = node.pos[2] - (2.0f / LEVELSIZE * 0.5f);

      water[i].distance = distanceFromCam(posArray);
    } else if (fabsf(90.0f - water[i].rota[0]) < __DBL_EPSILON__) {

      float posArray[3];

      if (fabsf(1.0f - water[i].rota[1]) < __DBL_EPSILON__) {
        posArray[0] = node.pos[0];
        posArray[1] = node.pos[1] - (2.0f / LEVELSIZE * 0.5f);
        posArray[2] = node.pos[2];
      } else {
        posArray[0] = node.pos[0] + (2.0f / LEVELSIZE * 0.5f);
        posArray[1] = node.pos[1];
        posArray[2] = node.pos[2];
      }

      water[i].distance = distanceFromCam(posArray);
    } else if (fabsf(270.0f - water[i].rota[0]) < __DBL_EPSILON__) {

      float posArray[3];

      posArray[0] = node.pos[0] - (2.0f / LEVELSIZE * 0.5f);
      posArray[1] = node.pos[1];
      posArray[2] = node.pos[2];

      water[i].distance = distanceFromCam(posArray);
    } else if (fabsf(-90.0f - water[i].rota[0]) < __DBL_EPSILON__) {

      float posArray[3];

      posArray[0] = node.pos[0];
      posArray[1] = node.pos[1] + (2.0f / LEVELSIZE * 0.5f);
      posArray[2] = node.pos[2];

      water[i].distance = distanceFromCam(posArray);
    } else {

      float posArray[3];

      posArray[0] = node.pos[0];
      posArray[1] = node.pos[1];
      posArray[2] = node.pos[2] + (2.0f / LEVELSIZE * 0.5f);

      water[i].distance = distanceFromCam(posArray);
    }
  }

  qsort(water, 6, sizeof(WaterSide), cmpWS);
}


/**
 * Zeichnen der Wasserbloecke
*@param node: Wasserblock
 */
void
drawWaterBlock(WaterBlockList node)
{

  WaterSide water[] = {
    { 1.0f, { 180.0f, 0.0f, 1.0f, 0.0f } },
    { 1.0f, { 90.0f, 1.0f, 0.0f, 0.0f } },
    { 1.0f, { 90.0f, 0.0f, 1.0f, 0.0f } },
    { 1.0f, { 270.0f, 0.0f, 1.0f, 0.0f } },
    { 1.0f, { -90.0f, 1.0f, 0.0f, 0.0f } },
    { 1.0f, { 0.0f, 0.0f, 0.0f, 0.0f } },
  };

  sortWaterSides(water, node);

  glPushMatrix();
  {
    glTranslatef(node.pos[0], node.pos[1], node.pos[2]);
    glScalef(node.scale[0], node.scale[1], node.scale[2]);

    for (int i = 0; i < 6; i++) {

      glPushMatrix();
      {
        glRotatef(water[i].rota[0],
                  water[i].rota[1],
                  water[i].rota[2],
                  water[i].rota[3]);
        glTranslatef(0.0f, 0.0f, 0.5f);
        if (Debug) {
          setMaterial(i);
        } else {
          setMaterial(WATER);
        }

        drawSquare3D(6);
      }
      glPopMatrix();
    }
  }
  glPopMatrix();
}

/**
 * Zeichnet Koordinatenachsen (inklusive Beschriftung).
 */
void
drawAxes3D()
{
  /* Farben der Koordinatenachsen */
  /* GLfloat axesColor[] = {1.0f, 0.5f, 0.0f}; */
  GLfloat axesTextColor[] = { 1.0f, 1.0f, 0.0f };

  /* x-Achse */
  glPushMatrix();
  {
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glColor3f(1.0, 1.0, 1.0);
    glScalef(1.0f, 1.0f, 3.0f);
    drawArrow3D();
  }
  glPopMatrix();

  /* y-Achse */
  glPushMatrix();
  {
    glRotatef(270.0f, 1.0f, 0.0f, 0.0f);
    glColor3f(1.0, 1.0, 1.0);
    glScalef(1.0f, 1.0f, 3.0f);
    drawArrow3D();
  }
  glPopMatrix();

  /* z-Achse */
  glPushMatrix();
  {
    glScalef(1.0f, 1.0f, 3.0f);
    glColor3f(1.0, 1.0, 1.0);
    drawArrow3D();
  }
  glPopMatrix();

  /* Beschriftungen der Koordinatenachsen */
  glColor3fv(axesTextColor);
  glRasterPos3f(1.1f, 0.0f, 0.0f);
  glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'x');
  glRasterPos3f(0.0f, 1.1f, 0.0f);
  glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'y');
  glRasterPos3f(0.0f, 0.0f, 1.1f);
  glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'z');
}

// Dreht den Charakter um 90 Grad
void
turnAvatar3D()
{
  glPushMatrix();
  {
    glRotatef(90, 0, 0, 1);
  }
  glPopMatrix();
}

/**
 * Zeichnet den Spieler Avatar
 */
void
drawAvatar3D(void)
{

  setMaterial(AVATAR1);
  // Kopf
  glPushMatrix();
  {
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTranslatef(0.0f, 0.0f, -1.0f);
    glScalef(0.5, 0.5, 0.5);
    GLUquadric* quad;
    quad = gluNewQuadric();
    gluSphere(quad, 0.5, 100, 20);
    gluDeleteQuadric(quad);
  }
  glPopMatrix();

  setMaterial(AVATAR2);
  // Körper
  glPushMatrix();
  {
    glTranslatef(0.0f, 0.0f, -0.6f);

    /* Rueckseitenflaeche */
    glPushMatrix();
    {
      glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
      glScalef(0.5f, 0.5f, 0.5f);
      glTranslatef(0.0f, 0.0f, 0.5f);
      drawSquare3D(4);
    }
    glPopMatrix();

    /* Bodenflaeche */
    glPushMatrix();
    {
      glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
      glScalef(0.5f, 0.5f, 0.5f);
      glTranslatef(0.0f, 0.0f, 0.5f);
      drawSquare3D(4);
    }
    glPopMatrix();

    /* rechte Seitenflaeche */
    glPushMatrix();
    {
      glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
      glScalef(0.5f, 0.5f, 0.5f);
      glTranslatef(0.0f, 0.0f, 0.5f);
      drawSquare3D(4);
    }
    glPopMatrix();

    /* linke Seitenflaeche */
    glPushMatrix();
    {
      glRotatef(270.0f, 0.0f, 1.0f, 0.0f);
      glScalef(0.5f, 0.5f, 0.5f);
      glTranslatef(0.0f, 0.0f, 0.5f);
      drawSquare3D(4);
    }
    glPopMatrix();

    /* Deckelflaeche */
    glPushMatrix();
    {
      glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
      glScalef(0.5f, 0.5f, 0.5f);
      glTranslatef(0.0f, 0.0f, 0.5f);
      drawSquare3D(4);
    }
    glPopMatrix();

    /* Frontflaeche */
    glPushMatrix();
    {
      glScalef(0.5f, 0.5f, 0.5f);
      glTranslatef(0.0f, 0.0f, 0.5f);
      drawSquare3D(4);
    }
    glPopMatrix();
  }
  glPopMatrix();

  setMaterial(AVATAR3);
  // Bein 1
  glPushMatrix();
  {
    glNormal3f(0.0f, 0.0f, 1.0f);
    glScalef(0.4, 0.4, 0.4);
    glTranslatef(0.0f, 0.3f, -0.4f);
    GLUquadric* quad;
    quad = gluNewQuadric();
    gluSphere(quad, 0.5, 100, 20);
    gluDeleteQuadric(quad);
  }
  glPopMatrix();
  // Bein 2
  glPushMatrix();
  {
    glNormal3f(0.0f, 0.0f, 1.0f);
    glScalef(0.4, 0.4, 0.4);
    glTranslatef(0.0f, -0.3f, -0.4f);
    GLUquadric* quad;
    quad = gluNewQuadric();
    gluSphere(quad, 0.5, 100, 20);
    gluDeleteQuadric(quad);
  }
  glPopMatrix();
  // Arm 1
  glPushMatrix();
  {
    glNormal3f(0.0f, 0.0f, 1.0f);
    glScalef(0.4, 0.4, 0.4);
    glTranslatef(0.0f, 0.5f, -1.5f);
    GLUquadric* quad;
    quad = gluNewQuadric();
    gluSphere(quad, 0.5, 100, 20);
    gluDeleteQuadric(quad);
  }
  glPopMatrix();
  // Arm 2
  glPushMatrix();
  {
    glNormal3f(0.0f, 0.0f, 1.0f);
    glScalef(0.4, 0.4, 0.4);
    glTranslatef(0.0f, -0.5f, -1.5f);
    GLUquadric* quad;
    quad = gluNewQuadric();
    gluSphere(quad, 0.5, 100, 20);
  }
  glPopMatrix();
}

/**
 * Draws a normal 1x1 quad
 */
void
drawQuad3D(void)
{
  glBegin(GL_QUADS);
  {
    glNormal3f(0.0f, 0.0f, 1.0f);

    glVertex3f(0.5f, 0.5f, 0.0f);

    glVertex3f(0.5f, -0.5f, 0.0f);

    glVertex3f(-0.5f, -0.5f, 0.0f);

    glVertex3f(-0.5f, 0.5f, 0.0f);
  }
  glEnd();
}

/**
 * Zeichnet ein Quadrat mit der Kantenlaenge 1, das aus mehreren kleineren
 * Quadraten zusammen gesetzt ist.
 * @param subDivs Anzahl der Unterteilungsschritte der Kanten.
 */
void
drawSquare3D(GLint subDivs)
{
  int x, y;

  for (y = 0; y < subDivs + 1; y++) {
    glBegin(GL_QUAD_STRIP);
    glNormal3f(0.0f, 0.0f, 1.0f);
    for (x = 0; x <= subDivs + 1; x++) {
      glVertex3f(
        -0.5f + x / (subDivs + 1.0f), 0.5f - y / (subDivs + 1.0f), 0.0f);

      glVertex3f(
        -0.5f + x / (subDivs + 1.0f), 0.5f - (y + 1) / (subDivs + 1.0f), 0.0f);
    }
    glEnd();
  }
  // Wenn Normalen zeichnen erlaubt, zeichne normalen
  if (showNormals) {
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 1.0f);
    glEnd();
  }
}

/**
 * Zeichnet das Zielfeld in 3D
 */
void
drawTarget3D(void)
{
  setMaterial(GREY);
  glPushMatrix();
  {
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, 0.5f);
    drawQuad3D();
  }
  glPopMatrix();

  setMaterial(GREEN);
  glPushMatrix();
  {
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, -0.49f);
    glScalef(0.9f, 0.9f, 1.0f);
    drawSquare3D(6);
  }
  glPopMatrix();
}

