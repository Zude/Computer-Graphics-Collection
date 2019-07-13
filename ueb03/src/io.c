/* ---- System Header einbinden ---- */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
#include "displaylists3D.h"
#include "drawing.h"
#include "drawing3D.h"
#include "io.h"
#include "levels.h"
#include "logic.h"
#include "overlay.h"
#include "scene.h"
#include "types.h"

/* ---- Konstanten ---- */
#define ESC 27
#define ENTER 13

/* Größe der Minimap ansicht */
#define MINIMAP_X width / 4.0f
#define MINIMAP_Y height / 4.0f

/** Anzahl der Aufrufe der Timer-Funktion pro Sekunde */
#define TIMER_CALLS_PS 60

/* ---- Globale Werte ---- */
extern GLboolean Pause;
extern GLboolean ShowInstructions;
extern GLboolean Debug;

/*
 * Wechselt zwischen Fenster und Fullscreen Anzeige
 */
static void
toggleFullScreen(void)
{
  static GLboolean fullscreen = GL_FALSE;

  fullscreen = !fullscreen;

  if (fullscreen) {
    glutFullScreen();
    INFO(("Wechsel in den Vollbildmodus"));
  } else {
    glutReshapeWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    // Eventuell mal glutPositionWindow() ausprobieren
    INFO(("Wechsel in den Fenstermodus"));
  }
}

/**
 * Toggle für die Pausierung der Logik
 */
static void
togglePause(void)
{
  Pause = !Pause;

  INFO(("Pausierierung umgeschaltet\n"));
}

/**
 * Toggle für die Anzeige der Anleitung
 */
static void
toggleShowInstructions(void)
{
  ShowInstructions = !ShowInstructions;
}

/**
 * Toggle für Anzeige von Debug-Infos (Overlay, XYZ-Achsen,...)
 */
static void
toogleDebug(void)
{
  Debug = !Debug;
}

/**
 * Timer-Callback.
 * Initiiert Berechnung der aktuellen Position und Farben und anschliessendes
 * Neuzeichnen, setzt sich selbst erneut als Timer-Callback.
 * @param lastCallTime Zeitpunkt, zu dem die Funktion als Timer-Funktion
 *   registriert wurde (In).
 */
static void
cbTimer(int lastCallTime)
{
  /* Seit dem Programmstart vergangene Zeit in Millisekunden */
  int thisCallTime = glutGet(GLUT_ELAPSED_TIME);

  /* Seit dem letzten Funktionsaufruf vergangene Zeit in Sekunden */
  double interval = (double)(thisCallTime - lastCallTime) / 1000.0f;

  if (ShowInstructions || Pause) {
    updateLogic(0);
  } else {
    updateLogic(interval);
  }

  /* Wieder als Timer-Funktion registrieren */
  glutTimerFunc(1000 / TIMER_CALLS_PS, cbTimer, thisCallTime);

  /* Neuzeichnen anstossen */
  glutPostRedisplay();
}

/**
 * Verarbeitung eines Tasturereignisses.
 * ESC-Taste und q, Q beenden das Programm.
 * @param key Taste, die das Ereignis ausgeloest hat. (ASCII-Wert oder WERT des
 *        GLUT_KEY_<SPECIAL>.clear
 * @param status Status der Taste, GL_TRUE=gedrueckt, GL_FALSE=losgelassen.
 * @param isSpecialKey ist die Taste eine Spezialtaste?
 * @param x x-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung.
 * @param y y-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung.
 */
static void
handleKeyboardEvent(int key, int status, GLboolean isSpecialKey, int x, int y)
{
  (void)x;
  (void)y;
  /* Taste gedrueckt */
  if (status == GLUT_DOWN) {
    /* Spezialtaste gedrueckt */
    if (isSpecialKey) {
      switch (key) {
          /* (De-)Aktivieren des Wireframemode */
        case GLUT_KEY_F1:
          toggleWireframeMode();
          glutPostRedisplay();
          break;
        case GLUT_KEY_F2:
          toggleNormals();
          break;
        case GLUT_KEY_F3:
          toggleLighting();
          break;
        case GLUT_KEY_F4:
          toggleFullScreen();
          break;
        case GLUT_KEY_F5:
          toggleHeadLamp();
          break;
        /*Avatar bewegen*/
        case GLUT_KEY_UP:
          movePlayer(dirUp);
          break;
        case GLUT_KEY_DOWN:
          movePlayer(dirDown);
          break;
        case GLUT_KEY_LEFT:
          movePlayer(dirLeft);
          break;
        case GLUT_KEY_RIGHT:
          movePlayer(dirRight);
          break;
      }
    }
    /* normale Taste gedrueckt */
    else {
      switch (key) {
          /* Programm beenden */
        case 'q':
        case 'Q':
        case ESC:
          exit(0);
          break;

          /* Anleitung anzeigen */
        case 'h':
        case 'H':
          toggleShowInstructions();
          break;

        /* Programm pausieren */
        case 'p':
        case 'P':
          togglePause();
          break;
        /* Level neustarten */
        case 'r':
        case 'R':
          resetLevel();
          break;
          /* Level auswählen */
        case '1':
          initLv(1);
          break;
        case '2':
          initLv(2);
          break;
        case '3':
          initLv(3);
          break;
        case '4':
          initLv(4);
          break;
          /* Zum nächsten Level gehen */
        case ENTER:
          nextLevel();
          break;

        case 'c':
        case 'C':
          switchCam();
          break;
        case 'a':
        case 'A':
          toggleAnimation();
          break;

          /* Kamera bewegung mit der Tastatur */
        case 'u':
          camMove(dirLeft);
          break;
        case 'i':
          camMove(dirRight);
          break;
        case 'j':
          camMove(dirUp);
          break;
        case 'k':
          camMove(dirDown);
          break;

        case 'd':
        case 'D':
          toogleDebug();
          break;
      }
    }
  }
}

static void
handleMouseEvent(int key, int state, int x, int y)
{
  if (state == GLUT_DOWN) {
    switch (key) {
      case GLUT_LEFT_BUTTON:
        setMouseStart(x, y);
        break;
      case 3: // Mausrad hoch
        zoomCamera(GL_TRUE);
        break;
      case 4: // Maurad runter
        zoomCamera(GL_FALSE);
        break;
    }
  }
}

static void
cbMouseMovement(int x, int y)
{
  moveCamera(x, y);
}

static void
cbMouseClick(int key, int state, int x, int y)
{
  handleMouseEvent(key, state, x, y);
}

/**
 * Callback fuer Tastendruck.
 * Ruft Ereignisbehandlung fuer Tastaturereignis auf.
 * @param key betroffene Taste (In).
 * @param x x-Position der Maus zur Zeit des Tastendrucks (In).
 * @param y y-Position der Maus zur Zeit des Tastendrucks (In).
 */
static void
cbKeyboard(unsigned char key, int x, int y)
{
  handleKeyboardEvent(key, GLUT_DOWN, GL_FALSE, x, y);
}

/**
 * Callback fuer Druck auf Spezialtasten.
 * Ruft Ereignisbehandlung fuer Tastaturereignis auf.
 * @param key betroffene Taste (In).
 * @param x x-Position der Maus zur Zeit des Tastendrucks (In).
 * @param y y-Position der Maus zur Zeit des Tastendrucks (In).
 */
static void
cbSpecial(int key, int x, int y)
{
  handleKeyboardEvent(key, GLUT_DOWN, GL_TRUE, x, y);
}

/**
 * Callback fuer Loslassen von Spezialtasten.
 * Ruft Ereignisbehandlung fuer Tastaturereignis auf.
 * @param key betroffene Taste (In).
 * @param x x-Position der Maus zur Zeit des Loslassens (In).
 * @param y y-Position der Maus zur Zeit des Loslassens (In).
 */
static void
cbSpecialUp(int key, int x, int y)
{
  handleKeyboardEvent(key, GLUT_UP, GL_TRUE, x, y);
}

/**
 * Setzt einen Viewport für 3-dimensionale Darstellung
 * mit perspektivischer Projektion und legt eine Kamera fest.
 * Ruft das zeichnen der Szene in diesem Viewport auf.
 *
 * @param x, y Position des Viewports im Fenster - (0, 0) ist die untere linke
 * Ecke
 * @param width, height Breite und Höhe des Viewports
 * @param lookAt enthält die für glLookAt benötigten Daten zur Kamera (Augpunkt,
 * Zentrum, Up-Vektor)
 */
static void
set3DViewport(GLint x, GLint y, GLint width, GLint height, GLdouble lookAt[9])
{
  /* Seitenverhältnis bestimmen */
  double aspect = (double)width / height;

  /* Folge Operationen beeinflussen die Projektionsmatrix */
  glMatrixMode(GL_PROJECTION);

  /* Einheitsmatrix laden */
  glLoadIdentity();

  /* Viewport-Position und -Ausdehnung bestimmen */
  glViewport(x, y, width, height);

  /* Perspektivische Darstellung */
  gluPerspective(70,     /* Öffnungswinkel */
                 aspect, /* Seitenverhältnis */
                 0.1,    /* nahe Clipping-Ebene */
                 100);   /* ferne Clipping-Ebene */

  /* Folge Operationen beeinflussen die Modelviewmatrix */
  glMatrixMode(GL_MODELVIEW);

  /* Einheitsmatrix laden */
  glLoadIdentity();

  /* Kameraposition */
  gluLookAt(lookAt[0],
            lookAt[1],
            lookAt[2], /* Augpunkt */
            lookAt[3],
            lookAt[4],
            lookAt[5], /* Zentrum */
            lookAt[6],
            lookAt[7],
            lookAt[8]); /* Up-Vektor */

  /* Szene zeichnen lassen */
  drawScene3D();
}
/**
 * Setzt einen Viewport für 2-dimensionale Darstellung.
 * Ruft das zeichnen der Szene in diesem Viewport auf.
 *
 * @param x, y Position des Viewports im Fenster - (0, 0) ist die untere linke
 * Ecke
 * @param width, height Breite und Höhe des Viewports
 */
static void
set2DViewport(GLint x,
              GLint y,
              GLint width,
              GLint height,
              void (*drawPtr)(void))
{
  /* Seitenverhältnis bestimmen */
  double aspect = (double)width / height;

  /* Folge Operationen beeinflussen die Projektionsmatrix */
  glMatrixMode(GL_PROJECTION);

  /* Einheitsmatrix laden */
  glLoadIdentity();

  /* Viewport-Position und -Ausdehnung bestimmen */
  glViewport(x, y, width, height);

  /* Das Koordinatensystem bleibt immer quadratisch */
  if (aspect <= 1) {
    gluOrtho2D(-1,
               1, /* left, right */
               -1 / aspect,
               1 / aspect); /* bottom, top */
  } else {
    gluOrtho2D(-1 * aspect,
               1 * aspect, /* left, right */
               -1,
               1); /* bottom, top */
  }

  /* Folge Operationen beeinflussen die Modelviewmatrix */
  glMatrixMode(GL_MODELVIEW);

  /* Einheitsmatrix laden */
  glLoadIdentity();

  /* Szene zeichnen lassen */
  /* drawScene2D(); */
  drawPtr();
}

/**
 * Zeichen-Callback.
 * Loescht die Buffer, ruft das Zeichnen der Szene auf und tauscht den Front-
 * und Backbuffer.
 */
static void
cbDisplay(void)
{
  /* Buffer zuruecksetzen */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Fensterdimensionen auslesen */
  int width = glutGet(GLUT_WINDOW_WIDTH);
  int height = glutGet(GLUT_WINDOW_HEIGHT);

  set3DViewport(0,
                0, /* x, y */
                width,
                height,           /* breite, hoehe */
                currentCamera()); /* Kamera */

  /* 2D-Viewport setzen */
  set2DViewport(width - MINIMAP_X,
                height - MINIMAP_Y, /* x, y */
                MINIMAP_X,
                MINIMAP_Y,
                drawScene2D); /* breite, hoehe */

  /* GUI Overlay Viewport */
  set2DViewport(0, 0, width, height, drawOverlay2D);

  glutSwapBuffers();
}

/**
 * Registrierung der GLUT-Callback-Routinen.
 */
static void
registerCallbacks(void)
{
  /* Timer-Callback - wird einmalig nach msescs Millisekunden ausgefuehrt */
  glutTimerFunc(1000 / TIMER_CALLS_PS,       /* msecs - bis Aufruf von func */
                cbTimer,                     /* func  - wird aufgerufen    */
                glutGet(GLUT_ELAPSED_TIME)); /* value - Parameter, mit dem
                                                   func aufgerufen wird */

  /* Tasten-Druck-Callback - wird ausgefuehrt, wenn eine Taste gedrueckt wird */
  glutKeyboardFunc(cbKeyboard);

  /* Spezialtasten-Druck-Callback - wird ausgefuehrt, wenn Spezialtaste
   * (F1 - F12, Links, Rechts, Oben, Unten, Bild-Auf, Bild-Ab, Pos1, Ende oder
   * Einfuegen) gedrueckt wird */
  glutSpecialFunc(cbSpecial);

  /* Spezialtasten-Loslass-Callback - wird ausgefuehrt, wenn eine Spezialtaste
   * losgelassen wird */
  glutSpecialUpFunc(cbSpecialUp);

  /* Display-Callback - wird an mehreren Stellen imlizit (z.B. im Anschluss an
   * Reshape-Callback) oder explizit (durch glutPostRedisplay) angestossen */
  glutDisplayFunc(cbDisplay);

  /*
   * Callbakc für Mausbewegung, während eine der Maustasten gedrückt ist
   */
  glutMotionFunc(cbMouseMovement);

  glutMouseFunc(cbMouseClick);
}

/**
 * Initialisiert das Programm (inkl. I/O und OpenGL) und startet die
 * Ereignisbehandlung.
 * @param title Beschriftung des Fensters
 * @param width Breite des Fensters
 * @param height Hoehe des Fensters
 * @return ID des erzeugten Fensters, 0 im Fehlerfall
 */
int
initAndStartIO(char* title, int width, int height)
{
  int windowID = 0;

  /* Kommandozeile immitieren */
  int argc = 1;
  char* argv = "cmd";

  srand(time(0));

  ShowInstructions = GL_FALSE;
  Pause = GL_FALSE;

  // Erstes Level laden
  initLv(1);

  /* Glut initialisieren */
  glutInit(&argc, &argv);

  /* DEBUG-Ausgabe */
  INFO(("Erzeuge Fenster...\n"));

  /* Initialisieren des Fensters */
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(width, height);
  glutInitWindowPosition(0, 0);

  /* Fenster erzeugen */
  windowID = glutCreateWindow(title);

  if (windowID) {
    /* DEBUG-Ausgabe */
    INFO(("...fertig.\n\n"));

    /* DEBUG-Ausgabe */
    INFO(("Initialisiere Szene...\n"));

    if (initScene()) {
      /* DEBUG-Ausgabe */
      INFO(("...fertig.\n\n"));

      /* DEBUG-Ausgabe */
      INFO(("Registriere Callbacks...\n"));

      registerCallbacks();

      /* DEBUG-Ausgabe */
      INFO(("...fertig.\n\n"));

      /* DEBUG-Ausgabe */
      INFO(("Trete in Schleife der Ereignisbehandlung ein...\n"));

      glutMainLoop();
    } else {
      /* DEBUG-Ausgabe */
      INFO(("...fehlgeschlagen.\n\n"));

      glutDestroyWindow(windowID);
      windowID = 0;
    }
  } else {
    /* DEBUG-Ausgabe */
    INFO(("...fehlgeschlagen.\n\n"));
  }

  return windowID;
}
