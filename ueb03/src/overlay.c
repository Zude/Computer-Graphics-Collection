/* ---- System Header einbinden ---- */
#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

/* ---- Eigene Header einbinden ---- */
#include "drawing.h"
#include "logic.h"
#include "overlay.h"
#include "scene.h"
#include "types.h"

extern GLboolean Debug;

extern float ax;
extern float az;
extern int dx;
extern int dy;

/* ---- Konstanten ---- */
/* Einrückungen für den Text */
#define INDENT1 0.1f
#define INDENT2 0.13f
/* Position der ersten Linie */
#define FIRSTLINE 0.15f
/* Abstand zwischen den Linien */
#define NEXTLINE 0.02f

static void
drawBackground(void)
{
  glPushMatrix();
  {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.3f, 0.3f, 0.3f, 0.85f);
    glScalef(3.0f, 3.0f, 1.0f);
    drawQuad();
    glDisable(GL_BLEND);
  }
  glPopMatrix();
}

/**
 * Zeichnet die Anleitung
 */
static void
drawInstructions(void)
{
  float curr = 0.0f;

  GLfloat textColor[3] = { 1.0f, 1.0f, 1.0f };

  curr = FIRSTLINE;
  drawString(INDENT1, curr, textColor, "Anleitung");

  curr += NEXTLINE;
  drawString(INDENT1, curr, textColor, "F1:");
  curr += NEXTLINE;
  drawString(INDENT2, curr, textColor, "Toggle Wireframe Modus");
  curr += NEXTLINE;

  drawString(INDENT1, curr, textColor, "F2:");
  curr += NEXTLINE;
  drawString(INDENT2, curr, textColor, "Toggle Normals");
  curr += NEXTLINE;

  drawString(INDENT1, curr, textColor, "F3:");
  curr += NEXTLINE;
  drawString(INDENT2, curr, textColor, "Toggle globale Beleuchtung");
  curr += NEXTLINE;

  drawString(INDENT1, curr, textColor, "F4:");
  curr += NEXTLINE;
  drawString(INDENT2, curr, textColor, "Toggle Fullscreenmodus");
  curr += NEXTLINE;

  drawString(INDENT1, curr, textColor, "F5:");
  curr += NEXTLINE;
  drawString(INDENT2, curr, textColor, "Toggle Kopflampe");
  curr += NEXTLINE;

  drawString(INDENT1, curr, textColor, "C|c");
  curr += NEXTLINE;
  drawString(INDENT2, curr, textColor, "Kamera wechseln");
  curr += NEXTLINE;

  drawString(INDENT1, curr, textColor, "A|A");
  curr += NEXTLINE;
  drawString(INDENT2, curr, textColor, "Toggle Animationen");
  curr += NEXTLINE;

  drawString(INDENT1, curr, textColor, "H|h");
  curr += NEXTLINE;
  drawString(INDENT2, curr, textColor, "Zeige diese Anleitung an");
  curr += NEXTLINE;

  drawString(INDENT1, curr, textColor, "P|p:");
  curr += NEXTLINE;
  drawString(INDENT2, curr, textColor, "Programm pausieren");
  curr += NEXTLINE;

  drawString(INDENT1, curr, textColor, "Q|q|Esc:");
  curr += NEXTLINE;
  drawString(INDENT2, curr, textColor, "Programm beenden");
  curr += NEXTLINE;

  drawString(INDENT1, curr, textColor, "1|2|3|...");
  curr += NEXTLINE;
  drawString(INDENT2, curr, textColor, "Lade das Level mit dieser Nummer");
  curr += NEXTLINE;

  drawString(INDENT1, curr, textColor, "R|r");
  curr += NEXTLINE;
  drawString(INDENT2, curr, textColor, "Aktuelles Level neustarten");
  curr += NEXTLINE;
}

/**
 * Zeichnet alle Elemente fürs Overlay (Anleitung, GameOver-Screen usw)
 */
void
drawOverlay2D(void)
{
  glDisable(GL_DEPTH_TEST);

  /* Ausgaben fürs Testen */
  if (Debug) {
    GLdouble* camera = currentCamera();
    /* Ausgabe fürs Kamera testen */
    GLfloat textColor[3] = { 1.0f, 1.0f, 1.0f };
    drawString(0.1f, 0.1f, textColor, "AX: %f  AZ %f ", ax, az);
    drawString(0.1f,
               0.15f,
               textColor,
               "Pos: X: %f  Y: %f  Z: %f",
               camera[0],
               camera[1],
               camera[2]);
    drawString(0.1f, 0.2f, textColor, "DX: %d DY: %d", dx, dy);
    drawString(0.1f,
               0.25f,
               textColor,
               "Look: X: %f  Y: %f  Z: %f",
               camera[3],
               camera[4],
               camera[5]);
  }

  /* Pausiert */
  if (Pause) {
    drawBackground();
    GLfloat textColor[3] = { 1.0f, 1.0f, 1.0f };
    drawString(0.5f, 0.5f, textColor, "Pausiert");
  }
  /* Level beendet */
  else if (getPlayerWin()) {
    drawBackground();

    GLfloat textColor[3] = { 0.1f, 1.0f, 0.1f };

    drawString(0.5f, 0.5f, textColor, "GEWONNEN!");
    drawString(0.5f, 0.55f, textColor, "Enter zum Fortfahren");
  }
  /* Gameover */
  else if (!isAlive()) {
    drawBackground();

    GLfloat textColor[3] = { 1.0f, 0.1f, 0.1f };

    drawString(0.5f, 0.5f, textColor, "GAME OVER");
    drawString(0.5f, 0.55f, textColor, "R zum neustarten");
  }
  /* Anleitung */
  else if (ShowInstructions) {
    drawBackground();
    drawInstructions();
  }
}
