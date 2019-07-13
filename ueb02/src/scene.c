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
#include "levels.h"
#include "logic.h"
#include "scene.h"
#include "types.h"

extern GLboolean Usage;

#define DL_RANGE 5  // Anzahl der Displaylisten
static GLuint dlWalls;
static GLuint dlSugar;
static GLuint dlSand;
static GLuint dlFree;
static GLuint dlWater;

/**
 * Draws a normal 1x1 quad
 */
static void drawQuad(void) {
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
 * Zeichnet einen Einheitskreis
 */
static void drawCircle(void) {
  // TODO so okay? Wird der Speicher wieder frei gegeben? Lieber manuell?
  gluDisk(gluNewQuadric(), 0, 1, 24, 1);
}

/**
 * Zeichnet das Zielfeld
 */
static void drawTarget(void) {
  glColor3f(1.0f, 1.0f, 1.0f);
  drawQuad();
  glColor3f(0.4f, 0.3f, 0.4f);

  glPushMatrix();
  {
    glScalef(0.9f, 0.9f, 1.0f);
    drawQuad();
  }
  glPopMatrix();
}

/**
 * Display List für LV_WALL
 */
static void createDLWalls(void) {
  glNewList(dlWalls, GL_COMPILE);

  // Zeichne Hintergrund
  glColor3f(1.0f, 1.0f, 1.0f);
  drawQuad();

  // Steine in rot Zeichnen
  glColor3f(1.0f, 0.0f, 0.0f);

  // Zeichne Steine
  glPushMatrix();
  {  // Start Steine

#define BRICK_LENGTH 1.0f
#define BRICK_HEIGHT 0.5f
#define BRICK_BORDER 0.04f

    glPushMatrix();
    {
      glTranslatef(-1.0f + BRICK_LENGTH / 4, 1.0f - BRICK_HEIGHT / 2, 0.0f);
      glScalef(BRICK_LENGTH / 4.2, BRICK_HEIGHT / 2.1, 1.0f);
      drawQuad();
    }
    glPopMatrix();

    glPushMatrix();
    {
      glTranslatef(0.0f, 1.0f - BRICK_HEIGHT / 2, 0.0f);
      glScalef(BRICK_LENGTH / 2.1, BRICK_HEIGHT / 2.1, 1.0f);
      drawQuad();
    }
    glPopMatrix();

    glPushMatrix();
    {
      glTranslatef(1.0f - BRICK_LENGTH / 4, 1.0f - BRICK_HEIGHT / 2, 0.0f);
      glScalef(BRICK_LENGTH / 4.2, BRICK_HEIGHT / 2.1, 1.0f);
      drawQuad();
    }
    glPopMatrix();

    //----

    glPushMatrix();
    {
      glTranslatef(-BRICK_LENGTH / 2, BRICK_HEIGHT / 2, 0.0f);
      glScalef(BRICK_LENGTH / 2.1, BRICK_HEIGHT / 2.1, 1.0f);
      drawQuad();
    }
    glPopMatrix();

    glPushMatrix();
    {
      glTranslatef(BRICK_LENGTH / 2, BRICK_HEIGHT / 2, 0.0f);
      glScalef(BRICK_LENGTH / 2.1, BRICK_HEIGHT / 2.1, 1.0f);
      drawQuad();
    }
    glPopMatrix();

    // Untere hälfte

    glPushMatrix();
    {
      glTranslatef(-1.0f + BRICK_LENGTH / 4, -BRICK_HEIGHT / 2, 0.0f);
      glScalef(BRICK_LENGTH / 4.2, BRICK_HEIGHT / 2.1, 1.0f);
      drawQuad();
    }
    glPopMatrix();

    glPushMatrix();
    {
      glTranslatef(0.0f, -BRICK_HEIGHT / 2, 0.0f);
      glScalef(BRICK_LENGTH / 2.1, BRICK_HEIGHT / 2.1, 1.0f);
      drawQuad();
    }
    glPopMatrix();

    glPushMatrix();
    {
      glTranslatef(1.0f - BRICK_LENGTH / 4, -BRICK_HEIGHT / 2, 0.0f);
      glScalef(BRICK_LENGTH / 4.2, BRICK_HEIGHT / 2.1, 1.0f);
      drawQuad();
    }
    glPopMatrix();

    //----

    glPushMatrix();
    {
      glTranslatef(-BRICK_LENGTH / 2, -1.0f + BRICK_HEIGHT / 2, 0.0f);
      glScalef(BRICK_LENGTH / 2.1, BRICK_HEIGHT / 2.1, 1.0f);
      drawQuad();
    }
    glPopMatrix();

    glPushMatrix();
    {
      glTranslatef(BRICK_LENGTH / 2, -1.0f + BRICK_HEIGHT / 2, 0.0f);
      glScalef(BRICK_LENGTH / 2.1, BRICK_HEIGHT / 2.1, 1.0f);
      drawQuad();
    }
    glPopMatrix();

  }  // Ende Steine
  glPopMatrix();

  glEndList();
}

/**
 * Dislay List für LV_SUGAR
 */
static void createDLSugar(void) {
  glNewList(dlSugar, GL_COMPILE);

  glColor3f(1.0f, 1.0f, 1.0f);
  drawQuad();

  glColor3f(0.7f, 0.7f, 0.7f);

  glPushMatrix();
  {
    glTranslatef(-0.6f, 0.6f, 0.0f);
    glScalef(0.1f, 0.1f, 1.0f);
    drawQuad();
  }
  glPopMatrix();
  glPushMatrix();
  {
    glTranslatef(-0.8f, 0.4f, 0.0f);
    glScalef(0.1f, 0.1f, 1.0f);
    drawQuad();
  }
  glPopMatrix();
  glPushMatrix();
  {
    glTranslatef(0.9f, 0.2f, 0.0f);
    glScalef(0.1f, 0.1f, 1.0f);
    drawQuad();
  }
  glPopMatrix();
  glPushMatrix();
  {
    glTranslatef(-0.1f, 0.2f, 0.0f);
    glScalef(0.1f, 0.1f, 1.0f);
    drawQuad();
  }
  glPopMatrix();
  glPushMatrix();
  {
    glTranslatef(-0.6f, -0.3f, 0.0f);
    glScalef(0.1f, 0.1f, 1.0f);
    drawQuad();
  }
  glPopMatrix();
  glPushMatrix();
  {
    glTranslatef(0.4f, 0.9f, 0.0f);
    glScalef(0.1f, 0.1f, 1.0f);
    drawQuad();
  }
  glPopMatrix();
  glPushMatrix();
  {
    glTranslatef(-0.1f, -0.5f, 0.0f);
    glScalef(0.1f, 0.1f, 1.0f);
    drawQuad();
  }
  glPopMatrix();
  glPushMatrix();
  {
    glTranslatef(0.6f, -0.6f, 0.0f);
    glScalef(0.1f, 0.1f, 1.0f);
    drawQuad();
  }
  glPopMatrix();
  glPushMatrix();
  {
    glTranslatef(-0.7f, -0.0f, 0.0f);
    glScalef(0.1f, 0.1f, 1.0f);
    drawQuad();
  }
  glPopMatrix();
  glPushMatrix();
  {
    glTranslatef(0.7f, -0.6f, 0.0f);
    glScalef(0.1f, 0.1f, 1.0f);
    drawQuad();
  }
  glPopMatrix();
  glPushMatrix();
  {
    glTranslatef(-0.6f, -0.9f, 0.0f);
    glScalef(0.1f, 0.1f, 1.0f);
    drawQuad();
  }
  glPopMatrix();
  glPushMatrix();
  {
    glTranslatef(-0.9f, -0.2f, 0.0f);
    glScalef(0.1f, 0.1f, 1.0f);
    drawQuad();
  }
  glPopMatrix();
  glPushMatrix();
  {
    glTranslatef(0.9f, 0.2f, 0.0f);
    glScalef(0.1f, 0.1f, 1.0f);
    drawQuad();
  }
  glPopMatrix();
  glPushMatrix();
  {
    glTranslatef(0.6f, 0.3f, 0.0f);

    glScalef(0.1f, 0.1f, 1.0f);
    drawQuad();
  }
  glPopMatrix();

  glEndList();
}

/**
 * Display List für LV_FREE
 */
static void createDLFree(void) {
  glNewList(dlFree, GL_COMPILE);

  glColor3f(0.2f, 0.2f, 0.2f);
  drawQuad();

  glEndList();
}

/**
 * Display List für LV_SAND
 */
static void createDLSand(void) {
  glNewList(dlSand, GL_COMPILE);

  glColor3f(0.5f, 0.3f, 0.0f);
  drawQuad();

  glColor3f(0.9f, 0.8f, 0.5f);

  glPushMatrix();
  {
    // Kreise zu Eclipsen formen
    glScalef(0.9f, 0.6f, 1.0f);

    glPushMatrix();
    {
      glTranslatef(-0.6f, -1.0f, 0.0f);
      glScalef(0.5f, 0.5f, 1.0f);
      drawCircle();
    }
    glPopMatrix();

    glPushMatrix();
    {
      glTranslatef(0.0f, -1.0f, 0.0f);
      glScalef(0.5f, 0.5f, 1.0f);
      drawCircle();
    }
    glPopMatrix();
    glPushMatrix();
    {
      glTranslatef(0.6f, -1.0f, 0.0f);
      glScalef(0.5f, 0.5f, 1.0f);
      drawCircle();
    }
    glPopMatrix();

    glPushMatrix();
    {
      glTranslatef(-0.35f, -0.55f, 0.0f);
      glScalef(0.5f, 0.5f, 1.0f);
      drawCircle();
    }
    glPopMatrix();
    glPushMatrix();
    {
      glTranslatef(0.35f, -0.55f, 0.0f);
      glScalef(0.5f, 0.5f, 1.0f);
      drawCircle();
    }
    glPopMatrix();

    glPushMatrix();
    {
      glTranslatef(0.0f, 0.0f, 0.0f);
      glScalef(0.5f, 0.5f, 1.0f);
      drawCircle();
    }
    glPopMatrix();
  }
  glPopMatrix();

  glEndList();
}

static void drawSinWave(void) {
  glBegin(GL_LINE_STRIP);
  {
    for (float i = 0.0f; i < 13; i = i + 0.1) {
      glVertex2f(i * 0.8, sin(i));
    }
  }
  glEnd();
}

/**
 * Display List für LV_WATER
 */
static void createDLWater(void) {
  glNewList(dlWater, GL_COMPILE);

  glColor3f(0.2f, 0.2f, 1.0f);
  drawQuad();

  glColor3f(0.0f, 0.0f, 0.5f);
  glLineWidth(6.0f);

  glPushMatrix();
  {
    glTranslatef(-0.9f, 0.0f, 0.0f);
    glScalef(0.1f, 0.1f, 1.0f);
    drawSinWave();
  }
  glPopMatrix();

  glPushMatrix();
  {
    glTranslatef(-0.6f, 0.6f, 0.0f);
    glScalef(0.1f, 0.1f, 1.0f);
    drawSinWave();
  }
  glPopMatrix();

  glPushMatrix();
  {
    glTranslatef(-0.1f, -0.3f, 0.0f);
    glScalef(0.1f, 0.1f, 1.0f);
    drawSinWave();
  }
  glPopMatrix();

  glPushMatrix();
  {
    glTranslatef(-0.4f, -0.7f, 0.0f);
    glScalef(0.1f, 0.1f, 1.0f);
    drawSinWave();
  }
  glPopMatrix();

  glLineWidth(1.0f);
  glEndList();
}

/**
 * Erzeugt alle notwenigen Displaylisten
 */
static void initDisplayLists(void) {
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
 * Zeichnet den Spieler Avatar
 */
static void drawAvatar(void) {
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
 * Zeichnet die Displayliste für den ausgewählten Typen
 */
static void drawField(lvFieldType type) {
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
/**
 * Zeichnen einer Zeichfolge in den Vordergrund. Gezeichnet wird mit Hilfe von
 * <code>glutBitmapCharacter(...)</code>. Kann wie <code>printf genutzt
 * werden.</code>
 * @param x x-Position des ersten Zeichens 0 bis 1 (In).
 * @param y y-Position des ersten Zeichens 0 bis 1 (In).
 * @param color Textfarbe (In).
 * @param format Formatstring fuer die weiteren Parameter (In).
 */
void drawString(GLfloat x, GLfloat y, GLfloat *color, char *format, ...) {
  GLint matrixMode; /* Zwischenspeicher akt. Matrixmode */
  va_list args;     /* variabler Teil der Argumente */
  char buffer[255]; /* der formatierte String */
  char *s;          /* Zeiger/Laufvariable */
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

/**
 * Zeichnet die Usage
 */
static void drawUsage(void) {
  glColor3f(0.0f, 0.0f, 0.0f);
  drawQuad();

  GLfloat textColor[3] = {1.0f, 1.0f, 1.0f};

  drawString(0.1f, 0.15f, textColor, "Anleitung");

  drawString(0.1f, 0.25f, textColor, "F1:");
  drawString(0.12f, 0.3f, textColor, "Toggle Wireframe Modus");

  drawString(0.1f, 0.35f, textColor, "F2:");
  drawString(0.12f, 0.4f, textColor, "Toggle Fullscreen");

  drawString(0.1f, 0.45f, textColor, "H|h");
  drawString(0.12f, 0.5f, textColor, "Zeige diese Anleitung an");

  drawString(0.1f, 0.55f, textColor, "P|p:");
  drawString(0.12f, 0.6f, textColor, "Programm pausieren");

  drawString(0.1f, 0.65f, textColor, "Q|q|Esc:");
  drawString(0.12f, 0.7f, textColor, "Programm beenden");

  drawString(0.1f, 0.75f, textColor, "1|2|3|...");
  drawString(0.12f, 0.8f, textColor, "Lade das Level mit dieser Nummer");

  drawString(0.1f, 0.85f, textColor, "R|r");
  drawString(0.12f, 0.9f, textColor, "Aktuelles Level neustarten");
}

/**
 * Zeichnet den Game Over Bildschirm
 */
static void drawGameOver(void) {
  glColor3f(0.0f, 0.0f, 0.0f);
  drawQuad();

  GLfloat textColor[3] = {1.0f, 0.1f, 0.1f};

  drawString(0.5f, 0.5f, textColor, "GAME OVER");
  drawString(0.5f, 0.55f, textColor, "R zum neustarten");
}

/**
 * Zeichnet den Sieges Bildschirm
 */
static void drawWinningScreen(void) {
  glColor3f(0.0f, 0.0f, 0.0f);
  drawQuad();

  GLfloat textColor[3] = {0.1f, 1.0f, 0.1f};

  drawString(0.5f, 0.5f, textColor, "GEWONNEN!");
  drawString(0.5f, 0.55f, textColor, "Enter zum Fortfahren");
}

/**
 * Zeichen-Funktion.
 * Stellt die Szene dar. Ein weisses Rechteck wird an der vorher im Logik-Modul
 * berechnet Position gezeichnet.
 * Ausgabe eines Quadrats.
 */
void drawScene(void) {
  // Jedes Feld nimmt 2.0f/LEVELSIZE Platz ein und damit das mittlere auf 0,0
  // liegt werden alle um ein halbes Feld verschoben
  if (getPlayerWin()) {
    drawWinningScreen();
  } else if (!isAlive()) {
    drawGameOver();
  } else if (Usage) {
    drawUsage();
  } else {
    for (int y = 0; y <= LEVELSIZE - 1; y++) {
      for (int x = 0; x <= LEVELSIZE - 1; x++) {
        glPushMatrix();
        {
          glTranslatef(-1.0f + (1.0f / LEVELSIZE) + (2.0f / LEVELSIZE) * x,
                       1.0f - (1.0f / LEVELSIZE) - (2.0f / LEVELSIZE) * y,
                       0.0f);
          glScalef(0.98f / LEVELSIZE, 0.98f / LEVELSIZE, 1.0f);

          // Skaliere auflösenden Zucker
          if (getFieldAt(x, y) == LV_TOUCHED_SUGAR) {
            drawField(LV_FREE);
            glPushMatrix();
            {
              double time = sugarTimerAt(x, y);
              // double time = 0.4f;
              glScalef(time, time, 1.0f);
              glCallList(dlSugar);
            }
            glPopMatrix();

          } else {
            drawField(getFieldAt(x, y));
          }

          if (y == getPlayerY() && x == getPlayerX()) {
            drawAvatar();
          }
        }
        glPopMatrix();
      }
    }

    // Visuelle Anzeige das das Spiel aktuell pausiert ist
    if (Pause) {
      GLfloat textColor[3] = {1.0f, 1.0f, 1.0f};
      drawString(0.48f, 0.48f, textColor, "Pausiert");
    }
  }
}

/**
 * Initialisierung der Szene (inbesondere der OpenGL-Statusmaschine).
 * Setzt Hintergrund- und Zeichenfarbe.
 * @return Rueckgabewert: im Fehlerfall 0, sonst 1.
 */
int initScene(void) {
  /* Setzen der Farbattribute */
  /* Hintergrundfarbe */
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  /* Zeichenfarbe */
  glColor3f(1.0f, 1.0f, 1.0f);

  /* Linienbreite */
  glLineWidth(3.0f);

  initDisplayLists();

  /* Alles in Ordnung? */
  return (GLGETERROR == GL_NO_ERROR);
}

/**
 * (De-)aktiviert den Wireframe-Modus.
 */
void toggleWireframeMode(void) {
  /* Flag: Wireframe: ja/nein */
  static GLboolean wireframe = GL_FALSE;

  /* Modus wechseln */
  wireframe = !wireframe;

  if (wireframe) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}
