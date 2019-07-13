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
#include "drawing.h"

/**
 * Draws a normal 1x1 quad
 */
void
drawQuad(void)
{
  glBegin(GL_QUADS);
  {
    glNormal3f(0.0f, 0.0f, 1.0f);

    glVertex2f(0.5f, 0.5f);

    glVertex2f(0.5f, -0.5f);

    glVertex2f(-0.5f, -0.5f);

    glVertex2f(-0.5f, 0.5f);
  }
  glEnd();
}

/**
 * Zeichnet einen Einheitskreis
 */
void
drawCircle(void)
{
  // TODO so okay? Wird der Speicher wieder frei gegeben? Lieber manuell?
  gluDisk(gluNewQuadric(), 0, 1, 24, 1);
}

/**
 * Zeichnet eine Sinuswelle
 */
void
drawSinWave(void)
{
  glBegin(GL_TRIANGLE_STRIP);
  {
    for (float i = 0.0f; i < 12; i = i + 0.3) { // Feinheit der Linie
      glVertex2f(i * 0.8, sin(i));
      glVertex2f(i * 0.8, sin(i) + 0.3); // Breite der Linie
    }
  }
  glEnd();
}

/**
 * Zeichnet das Zielfeld
 */
void
drawTarget(void)
{

  glPushMatrix();
  {
    glColor3f(1.0f, 1.0f, 1.0f);
    glScalef(2.0f, 2.0f, 1.0f);
    drawQuad();
  }
  glPopMatrix();
  glPushMatrix();
  {

    glColor3f(0.4f, 0.3f, 0.4f);
    glScalef(1.8f, 1.8f, 1.0f);
    drawQuad();
  }
  glPopMatrix();
}

/**
 * Zeichnet den Spieler Avatar
 */
void
drawAvatar(void)
{
  // Körper
  glPushMatrix();
  {
    glColor3f(1.0f, 1.0f, 1.0f);
    glScalef(0.3f, 0.35f, 1.0f);
    drawCircle();
  }
  glPopMatrix();

  // Kopf
  glPushMatrix();
  {
    glColor3f(0.5f, 0.5f, 1.0f);
    glTranslatef(0.0f, 0.3f, 0.0f);
    glScalef(0.3f, 0.3f, 1.0f);
    drawCircle();
  }
  glPopMatrix();

  glPushMatrix();
  {
    glColor3f(0.6f, 0.6f, 1.0f);
    glTranslatef(0.0f, 0.3f, 0.0f);
    glScalef(0.25f, 0.25f, 1.0f);
    drawCircle();
  }
  glPopMatrix();

  // Augen
  glPushMatrix();
  {
    glColor3f(0.0f, 0.0f, 0.0f);
    glTranslatef(0.1f, 0.3f, 0.0f);
    glScalef(0.03f, 0.05f, 1.0f);
    drawCircle();
  }
  glPopMatrix();

  glPushMatrix();
  {
    glColor3f(0.0f, 0.0f, 0.0f);
    glTranslatef(-0.1f, 0.3f, 0.0f);
    glScalef(0.03f, 0.05f, 1.0f);
    drawCircle();
  }
  glPopMatrix();

  // Arme
  glPushMatrix();
  {
    glColor3f(1.0f, 0.0f, 1.0f);

    glScalef(0.2f, 0.2f, 1.0f);

    glPushMatrix();
    {
      glTranslatef(1.0f, 0.0f, 0.0f);
      glScalef(0.5f, 0.5f, 1.0f);
      drawCircle();
    }
    glPopMatrix();

    glPushMatrix();
    {
      glTranslatef(-1.0f, 0.0f, 0.0f);
      glScalef(0.5f, 0.5f, 1.0f);
      drawCircle();
    }
    glPopMatrix();
  }
  glPopMatrix();

  // Beine und Hose
  glColor3f(0.0f, 1.0f, 1.0f);
  glPushMatrix();
  {
    glTranslatef(0.1f, -0.35f, 0.0f);

    glRotatef(30, 0.0f, 0.0f, 1.0f);
    glScalef(0.055f, 0.12f, 1.0f);

    drawQuad();
  }
  glPopMatrix();

  glPushMatrix();
  {
    glTranslatef(-0.1f, -0.35f, 0.0f);

    glRotatef(150, 0.0f, 0.0f, 1.0f);
    glScalef(0.055f, 0.12f, 1.0f);

    drawQuad();
  }
  glPopMatrix();

  glPushMatrix();
  {
    glTranslatef(0.0f, -0.22f, 0.0f);
    glScalef(0.23f, 0.13f, 1.0f);
    drawCircle();
  }
  glPopMatrix();

  // Füße
  glPushMatrix();
  {
    glColor3f(1.0f, 0.0f, 1.0f);

    glScalef(0.4f, 0.15f, 1.0f);

    glPushMatrix();
    {
      glTranslatef(0.6f, -3.2f, 0.0f);
      glScalef(0.4f, 0.4f, 1.0f);
      drawCircle();
    }
    glPopMatrix();

    glPushMatrix();
    {
      glTranslatef(-0.6f, -3.2f, 0.0f);
      glScalef(0.4f, 0.4f, 1.0f);
      drawCircle();
    }
    glPopMatrix();
  }
  glPopMatrix();
}

/**
 * Zeichnen einer Zeichfolge in den Vordergrund. Gezeichnet wird mit Hilfe von
 * <code>glutBitmapCharacter(...)</code>. Kann wie <code>printf genutzt
 * werden.</code>
 * @param x x-Position des ersten Zeichens 0 bis 1 (In).
 * @param y y-Position des ersten Zeichens 0 bis 1 (In).
 * @param color Textfarbe (In).
 * @param format Formatstring fuer die weiteren Parameter (In).
 */
void
drawString(GLfloat x, GLfloat y, GLfloat* color, char* format, ...)
{
  GLint matrixMode; /* Zwischenspeicher akt. Matrixmode */
  va_list args;     /* variabler Teil der Argumente */
  char buffer[255]; /* der formatierte String */
  char* s;          /* Zeiger/Laufvariable */
  va_start(args, format);
  vsnprintf(buffer, 255, format, args);
  va_end(args);

  /* aktuelle Zeichenfarbe (u.a. Werte) sichern */
  glPushAttrib(GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT);

  /* aktuellen Matrixmode speichern */
  glGetIntegerv(GL_MATRIX_MODE, &matrixMode);
  glMatrixMode(GL_PROJECTION);

  /* aktuelle Projektionsmatrix sichern */
  glPushMatrix();

  /* neue orthogonale 2D-Projektionsmatrix erzeugen */
  glLoadIdentity();
  gluOrtho2D(0.0, 1.0, 1.0, 0.0);

  glMatrixMode(GL_MODELVIEW);

  /* aktuelle ModelView-Matrix sichern */
  glPushMatrix();

  /* neue ModelView-Matrix zuruecksetzen */
  glLoadIdentity();

  /* Tiefentest ausschalten */
  glDisable(GL_DEPTH_TEST);

  /* Licht ausschalten */
  glDisable(GL_LIGHTING);

  /* Nebel ausschalten */
  glDisable(GL_FOG);

  /* Blending ausschalten */
  glDisable(GL_BLEND);

  /* Texturierung ausschalten */
  glDisable(GL_TEXTURE_1D);
  glDisable(GL_TEXTURE_2D);
  /* glDisable (GL_TEXTURE_3D); */

  /* neue Zeichenfarbe einstellen */
  glColor4fv(color);

  /* an uebergebenene Stelle springen */
  glRasterPos2f(x, y);

  /* Zeichenfolge zeichenweise zeichnen */
  for (s = buffer; *s; s++)

  {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *s);
  }

  /* alte ModelView-Matrix laden */
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);

  /* alte Projektionsmatrix laden */
  glPopMatrix();

  /* alten Matrixmode laden */
  glMatrixMode(matrixMode);

  /* alte Zeichenfarbe und Co. laden */
  glPopAttrib();
}
