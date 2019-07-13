/* ---- System Header einbinden ---- */
#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include <math.h>
#include <string.h>

/* ---- Eigene Header einbinden ---- */
#include "debugGL.h"
#include "displaylists.h"
#include "displaylists3D.h"
#include "drawing.h"
#include "drawing3D.h"
#include "levels.h"
#include "logic.h"
#include "materials.h"
#include "scene.h"
#include "types.h"

extern GLboolean ShowInstructions;
extern GLboolean Debug;

#define CAMERASPEED 0.1f
#define HEAD_CAM_HEIGHT 0.05f

/* Begrenzung der vertikalen Kamera bewegung */
/* In Grad zur Y Achse */
#define CAM_UPPER_CUTOFF 2
#define CAM_LOWER_CUTOFF 85

#define CAM_MIN_ZOOM 0.5f
#define CAM_MAX_ZOOM 2.0f

/* Verschiedene Werte für Tooggles */
GLboolean showNormals = GL_FALSE;
static GLboolean animation = GL_TRUE;
static GLboolean outerCam = GL_TRUE;

static GLboolean headLamp = GL_TRUE;
static GLboolean globalLamp = GL_TRUE;

/* Abstand der Kamera zum Mittelpunkt */
static float distance = CAM_MAX_ZOOM;

static GLdouble camera[9] = { 0.0, 0.0, 0.0,   /* Augpunkt */
                              0.0, 0.0, 0.0,   /* Zentrum */
                              0.0, 1.0, 0.0 }; /* Up-Vektor */

/* Bewegung der Maus */
static int prevX = 0; // erhöhung in dx bedeutet vergrößerung des xz winkels
static int prevY = 0;

// Global für debug ausgabe
/* Winkel für Kameraposition */
float ax = 45.0f;
float az = 45.0f;
/* Veränderung in Mausposition */
int dx;
int dy;

static WaterBlockList* wblArray = NULL;
static int wblLength = 0;

float
distanceFromCam(Trans3f pos)
{
  /* Wurzel von (x2 - x1)² + (y2 - y2)² + (z2 - z1)² */
  float distance =
    sqrt(pow(pos[0] - camera[0], 2) + pow(pos[1] - camera[1], 2) +
         pow(pos[2] - camera[2], 2));
  return distance;
}

static void
wblAdd(Trans3f pos, Scale3f scale)
{
  wblArray = realloc(wblArray, (wblLength + 1) * sizeof(WaterBlockList));

  WaterBlockList new = { .pos[0] = pos[0],
                         .pos[1] = pos[1],
                         .pos[2] = pos[2],
                         .scale[0] = scale[0],
                         .scale[1] = scale[1],
                         .scale[2] = scale[2],
                         .distance = distanceFromCam(pos) };

  wblArray[wblLength] = new;

  wblLength++;
}

static void
wblFree(void)
{
  /* free(wblArray); */
  /* TODO richtiges free */
  wblLength = 0;
}

static int
cmpWbl(const void* a, const void* b)
{
  WaterBlockList* blockA = (WaterBlockList*)a;
  WaterBlockList* blockB = (WaterBlockList*)b;
  return ((blockA->distance > blockB->distance) -
          (blockA->distance < blockB->distance));
}

static void
drawWBL(void)
{
  /* Sortiere wb-Liste */
  /* Zeichne Block mit größter Distanz */

  qsort(wblArray, wblLength, sizeof(WaterBlockList), cmpWbl);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  for (int i = 0; i < wblLength; i++) {
    drawWaterBlock(wblArray[i]);
  }

  glDisable(GL_BLEND);

  wblFree();
}

/**
 * Zeichen-Funktion.
 * Stellt die Szene dar. Ein weisses Rechteck wird an der vorher im Logik-Modul
 * berechnet Position gezeichnet.
 * Ausgabe eines Quadrats.
 */
void
drawScene2D(void)
{
  glDisable(GL_DEPTH_TEST);
  // Jedes Feld nimmt 2.0f/LEVELSIZE Platz ein und damit das mittlere auf 0,0
  // liegt werden alle um ein halbes Feld verschoben

  glColor3f(0.0f, 0.0f, 0.0f);

  /* Hintergrund zeichnen */
  glPushMatrix();
  {
    glScalef(2.0f, 2.0f, 1.0f);
    drawQuad();
  }
  glPopMatrix();

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
            drawField(LV_SUGAR);
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
}

/**
 * Positioniert und aktiviert alle Lichter in der Scene
 */
void
drawLights(void)
{
  glEnable(GL_LIGHTING);

  /* Globale Beleuchtung */
  if (globalLamp) {
    float lightPos[] = { 5.0f, 10.0f, 4.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glEnable(GL_LIGHT0);
  }
  /* Kopf Lampe */
  if (headLamp) {
    float spotPos[] = {
      -1.0f + (1.0f / LEVELSIZE) + (2.0f / LEVELSIZE) * getPlayerY(),
      HEAD_CAM_HEIGHT / 2,
      1.0f - (1.0f / LEVELSIZE) - (2.0f / LEVELSIZE) * getPlayerX(),
      1.0f
    };
    /* float spotDir[] = {spotPos[0] + 1.0f, 0.0f, spotPos[2] + 1.0f}; */

    float* atDir = getDirCoords(spotPos, 5.0f, getPlayerDir());

    float spotDir[] = { atDir[0], HEAD_CAM_HEIGHT / 2, atDir[1] };

    glLightfv(GL_LIGHT1, GL_POSITION, spotPos);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotDir);

    glEnable(GL_LIGHT1);
  }
}

static void
drawAvatarAnimation(Trans3f newPos, Scale3f scale, int x, int y)
{
  // Vektor zwischen oldPos und newPos
  Trans3f distanceOldNew = { 0.0f, 0.0f, 0.0f };

  // Variable zum Speichern von Spielerpositionen bzw Transformationen
  static Trans3f oldPos = { 0.0f, 0.0f, 0.0f };

  // Berechnet den Abstand
  distanceOldNew[0] = newPos[0] - oldPos[0];
  distanceOldNew[2] = newPos[2] - oldPos[2];

  static GLboolean moving;
  if (!isPlayerStepAllowed()) {
    moving = GL_TRUE;
  } else {
    moving = GL_FALSE;
  }

  if (animation && moving) {
    // Verschiebt den Avatar abhaengig von der Altenpostion, abheangig von der
    // vergangenen Zeit von StepTime
    glPushMatrix();
    {
      glTranslatef(oldPos[0] + distanceOldNew[0] / getMaxStepTime() *
                                 getCurrentStepTime(),
                   0.0f,
                   oldPos[2] + distanceOldNew[2] / getMaxStepTime() *
                                 getCurrentStepTime());

      glScalef(2.0f / LEVELSIZE, 2.0f / LEVELSIZE, 2.0f / LEVELSIZE);

      glPushMatrix();
      {
        glRotatef(90.0f * getPlayerDir(), 0.0f, 1.0f, 0.0f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glTranslatef(0.0f, 0.0f, 0.5f);
        drawAvatar3D();
      }
      glPopMatrix();
    }
    glPopMatrix();
  } else {
    glPushMatrix();
    {
      glTranslatef(newPos[0], newPos[1], newPos[2]);
      glScalef(scale[0], scale[1], scale[2]);

      glPushMatrix();
      {
        glRotatef(90.0f * getPlayerDir(), 0.0f, 1.0f, 0.0f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glTranslatef(0.0f, 0.0f, 0.5f);
        drawAvatar3D();
      }
      glPopMatrix();
    }
    glPopMatrix();
  } // Wenn der Spieler sich nicht bewegt hatte, Speichere "alte" Position
  if (x == getPlayerY() && y == getPlayerX() && isPlayerStepAllowed()) {
    oldPos[0] = newPos[0];
    oldPos[1] = newPos[1];
    oldPos[2] = newPos[2];
  }
}

/**
 * Iteriert durch die Level-Daten und zeichnet die jeweiligen Objekte in 3D
 */
void
drawScene3D(void)
{

  glEnable(GL_DEPTH_TEST);

  /* Backfaceculling */
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  drawLights();
  // Iteration um das Level zu fuellen
  for (int y = 0; y <= LEVELSIZE - 1; y++) {
    for (int x = 0; x <= LEVELSIZE - 1; x++) {
      /* Position und Skalierung für aktuelles Feld, wird für alle weiteren
       * transformierungen genutzt */
      Trans3f pos = { -1.0f + (1.0f / LEVELSIZE) + (2.0f / LEVELSIZE) * x,
                      0.0f,
                      1.0f - (1.0f / LEVELSIZE) - (2.0f / LEVELSIZE) * y };

      Scale3f scale = { 2.0f / LEVELSIZE, 2.0f / LEVELSIZE, 2.0f / LEVELSIZE };

      glPushMatrix();
      {
        glTranslatef(pos[0], pos[1], pos[2]);
        glScalef(scale[0], scale[1], scale[2]);

        // Skaliere auflösenden Zucker
        if (getFieldAt(y, x) == LV_TOUCHED_SUGAR) {
          drawField3D(LV_FREE);
          glPushMatrix();
          {
            double time = sugarTimerAt(y, x);

            glTranslatef(0.0f, time - 1.0f, 0.0f);
            glScalef(time, time, time);
            drawField3D(LV_SUGAR);
          }
          glPopMatrix();

        } else if (getFieldAt(y, x) != LV_NEWWATER &&
                   getFieldAt(y, x) != LV_WATER) {
          drawField3D(getFieldAt(y, x));
        }
      }
      glPopMatrix();

      // Spieler bewegung / animation
      if (x == getPlayerY() && y == getPlayerX()) {
        drawAvatarAnimation(pos, scale, x, y);
      }

      /* Wasserblöcke merken und Deko zeichen */
      if (getFieldAt(y, x) == LV_NEWWATER || getFieldAt(y, x) == LV_WATER) {

        /* Wellen im Block und Boden */
        glPushMatrix();
        {
          glTranslatef(pos[0], pos[1], pos[2]);
          glScalef(scale[0], scale[1], scale[2]);
          /* drawWaterDeko(); */
          drawField3D(LV_FREE);
          drawField3D(LV_WATER);
        }
        glPopMatrix();
        wblAdd(pos, scale);
      }
    }
  }
  /* Wasserblöcke zeichnen, nachdem alle nicht transparenten Objekte
   * gezeichnet wurden */
  drawWBL();

  /* Licht wieder ausschalten */
  glDisable(GL_LIGHT0);
  glDisable(GL_LIGHT1);
  glDisable(GL_LIGHTING);

  /* Für 2D Szenen */
  glDisable(GL_CULL_FACE);

  if (Debug) {
    // Zeichner Hilfslinien fuer die Achsen
    drawAxes3D();
  }
}

/**
 * Initialisiert alle Lichtquellen
 */
void
initLights(void)
{
  GLfloat light0_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
  GLfloat light0_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat light0_specular[] = { 1.0, 1.0, 1.0, 1.0 };

  glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);

  GLfloat light1_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
  GLfloat light1_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat light1_specular[] = { 1.0, 1.0, 1.0, 1.0 };

  glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
  glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.5);
  glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.5);
  glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.2);

  /* Lichtquellen aktivieren */
  float exp = 50.0f;
  float cutoff = 50.0f;

  glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, exp);
  glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, cutoff);
}

/**
 * Initialisierung der Szene (inbesondere der OpenGL-Statusmaschine).
 * Setzt Hintergrund- und Zeichenfarbe.
 * @return Rueckgabewert: im Fehlerfall 0, sonst 1.
 */
int
initScene(void)
{
  /* Setzen der Farbattribute */
  /* Hintergrundfarbe */
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  /* Automatische Normalisierung */
  glEnable(GL_NORMALIZE);

  initDisplayLists();
  initDisplayLists3D();
  initLights();

  /* Alles in Ordnung? */
  return (GLGETERROR == GL_NO_ERROR);
}

/**
 * (De-)aktiviert den Wireframe-Modus.
 */
void
toggleWireframeMode(void)
{
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

void
camMove(CGDirection dir)
{
  switch (dir) {
    case dirUp:
      az = az - 3;
      break;
    case dirDown:
      az = az + 3;
      break;
    case dirLeft:
      ax = ax + 3;
      break;
    case dirRight:
      ax = ax - 3;
      break;
  }

  ax = (int)ax % 360;
  /* Winkel limitieren */
  if (az >= CAM_LOWER_CUTOFF) {
    az = CAM_LOWER_CUTOFF;
  } else if (az <= CAM_UPPER_CUTOFF) {
    az = CAM_UPPER_CUTOFF;
  }
}

/**
 * Setz X/Y Maus Position für Bewegungsberechnung
 * @param X Y Position im Fenster
 */
void
setMouseStart(int x, int y)
{
  prevX = x;
  prevY = y;
}

/**
 * Bewege die Kamera
 * @param X Y Position im Fenster
 */
void
moveCamera(int x, int y)
{
  /* Bewegung = aktuelle - letzte */
  dx = x - prevX;
  dy = y - prevY;

  /* Aktuelle Pos als letzte setzen */
  prevX = x;
  prevY = y;

  /* Verändere Winkel anhand der Mausbewegung */
  if (dx > 0) {
    ax = ax + dx * CAMERASPEED;
  } else {
    ax = ax - dx * CAMERASPEED;
  }

  /* Verhindert das ax ungewohlt wächst */
  ax = (int)ax % 360;

  if (dy > 0) { // Maus geht nach unten
    az = az + dy * CAMERASPEED;
  } else {
    az = az - dy * CAMERASPEED;
  }

  /* Winkel limitieren */
  if (az > CAM_LOWER_CUTOFF) {
    az = CAM_LOWER_CUTOFF;
  } else if (az < CAM_UPPER_CUTOFF) {
    az = CAM_UPPER_CUTOFF;
  }
}

void
zoomCamera(GLboolean zoom)
{
  if (zoom) {
    distance -= 0.2f;
  } else {
    distance += 0.2f;
  }

  if (distance > CAM_MAX_ZOOM) {
    distance = CAM_MAX_ZOOM;
  } else if (distance < CAM_MIN_ZOOM) {
    distance = CAM_MIN_ZOOM;
  }
}

/**
 * Berechnet Position der aktuell ausgewählten Kamera
 * @return GLdouble[9] Array mit Werten für LookAt aufruf
 */
GLdouble*
currentCamera(void)
{
  if (outerCam) {
    /* Winkel zu Radiant */
    float rx = ax * M_PI / 180;
    float rz = az * M_PI / 180;

    /* Kreis auf X Z Achse */
    float nx = distance * sin(rz) * cos(rx);
    float ny = distance * sin(rz) * sin(rx);
    float nz = distance * cos(rz);
    /* ny und nz verstauscht weil ny oben ist */

    camera[0] = nx;
    camera[1] = nz;
    camera[2] = ny;

    /* Kamera immer zum Mittelpunk ausrichten */
    camera[3] = 0.0f;
    camera[4] = 0.0f;
    camera[5] = 0.0f;

  } else {
    // Avatar Position
    camera[0] = -1.0f + (1.0f / LEVELSIZE) + (2.0f / LEVELSIZE) * getPlayerY();
    camera[1] = HEAD_CAM_HEIGHT;
    camera[2] = 1.0f - (1.0f / LEVELSIZE) - (2.0f / LEVELSIZE) * getPlayerX();
    // 1 in letze Bewegungsrichtung schauen

    float start[3];
    start[0] = camera[0];
    start[1] = camera[1];
    start[2] = camera[2];

    float* goal = getDirCoords(start, 1.0f, getPlayerDir());

    camera[3] = goal[0];
    camera[5] = goal[1];

    camera[4] = HEAD_CAM_HEIGHT;
  }
  return camera;
}

/**
 * Wechselt die aktuelle Camera
 */
void
switchCam(void)
{
  outerCam = !outerCam;
}

void
toggleNormals(void)
{
  showNormals = !showNormals;
  initDisplayLists3D();
}

void
toggleLighting(void)
{
  globalLamp = !globalLamp;
}

void
toggleHeadLamp(void)
{
  headLamp = !headLamp;
}

void
toggleAnimation(void)
{
  animation = !animation;
}
