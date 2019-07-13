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
#include "io.h"
#include "logic.h"
#include "scene.h"
#include "types.h"

/* ---- Konstanten ---- */
/** Anzahl der Aufrufe der Timer-Funktion pro Sekunde */
#define TIMER_CALLS_PS 60

/* ---- Globale Werte ---- */
extern GLboolean Pause;
extern GLboolean Usage;

/*
 * Wechselt zwischen Fenster und Fullscreen Anzeige
 */
static void toggleFullScreen(void) {
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
static void togglePause(void) {
  Pause = !Pause;

  INFO(("Pausierierung umgeschaltet\n"));
}

/**
 * Toggle für die Anzeige der Anleitung
 */
static void toggleUsage(void) { Usage = !Usage; }

/**
 * Timer-Callback.
 * Initiiert Berechnung der aktuellen Position und Farben und anschliessendes
 * Neuzeichnen, setzt sich selbst erneut als Timer-Callback.
 * @param lastCallTime Zeitpunkt, zu dem die Funktion als Timer-Funktion
 *   registriert wurde (In).
 */
static void cbTimer(int lastCallTime) {
  /* Seit dem Programmstart vergangene Zeit in Millisekunden */
  int thisCallTime = glutGet(GLUT_ELAPSED_TIME);

  /* Seit dem letzten Funktionsaufruf vergangene Zeit in Sekunden */
  double interval = (double)(thisCallTime - lastCallTime) / 1000.0f;

  if (Usage || Pause) {
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
 * Setzen der Projektionsmatrix.
 * Setzt die Projektionsmatrix unter Berücksichtigung des Seitenverhaeltnisses
 * des Anzeigefensters, sodass das Seitenverhaeltnisse der Szene unveraendert
 * bleibt und gleichzeitig entweder in x- oder y-Richtung der Bereich von -1
 * bis +1 zu sehen ist.
 * @param aspect Seitenverhaeltnis des Anzeigefensters (In).
 */
static void setProjection(GLdouble aspect) {
  /* Nachfolgende Operationen beeinflussen Projektionsmatrix */
  glMatrixMode(GL_PROJECTION);
  /* Matrix zuruecksetzen - Einheitsmatrix laden */
  glLoadIdentity();

  /* Koordinatensystem bleibt quadratisch */
  if (aspect <= 1) {
    gluOrtho2D(-1.0, 1.0,                    /* links, rechts */
               -1.0 / aspect, 1.0 / aspect); /* unten, oben */
  } else {
    gluOrtho2D(-1.0 * aspect, 1.0 * aspect, /* links, rechts */
               -1.0, 1.0);                  /* unten, oben */
  }
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
static void handleKeyboardEvent(int key, int status, GLboolean isSpecialKey,
                                int x, int y) {
  (void)x;
  (void)y;
/** Keycode der ESC-Taste */
#define ESC 27
#define ENTER 13
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
          toggleFullScreen();
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
          toggleUsage();
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
        case '2':
        case '3':
        case '4':
          initLv(key);
          break;
          /* Zum nächsten Level gehen */
        case ENTER:
          nextLevel();
          break;
      }
    }
  }
}

/**
 * Callback fuer Tastendruck.
 * Ruft Ereignisbehandlung fuer Tastaturereignis auf.
 * @param key betroffene Taste (In).
 * @param x x-Position der Maus zur Zeit des Tastendrucks (In).
 * @param y y-Position der Maus zur Zeit des Tastendrucks (In).
 */
static void cbKeyboard(unsigned char key, int x, int y) {
  handleKeyboardEvent(key, GLUT_DOWN, GL_FALSE, x, y);
}

/**
 * Callback fuer Druck auf Spezialtasten.
 * Ruft Ereignisbehandlung fuer Tastaturereignis auf.
 * @param key betroffene Taste (In).
 * @param x x-Position der Maus zur Zeit des Tastendrucks (In).
 * @param y y-Position der Maus zur Zeit des Tastendrucks (In).
 */
static void cbSpecial(int key, int x, int y) {
  handleKeyboardEvent(key, GLUT_DOWN, GL_TRUE, x, y);
}

/**
 * Callback fuer Loslassen von Spezialtasten.
 * Ruft Ereignisbehandlung fuer Tastaturereignis auf.
 * @param key betroffene Taste (In).
 * @param x x-Position der Maus zur Zeit des Loslassens (In).
 * @param y y-Position der Maus zur Zeit des Loslassens (In).
 */
static void cbSpecialUp(int key, int x, int y) {
  handleKeyboardEvent(key, GLUT_UP, GL_TRUE, x, y);
}

/**
 * Callback fuer Aenderungen der Fenstergroesse.
 * Initiiert Anpassung der Projektionsmatrix an veränderte Fenstergroesse.
 * @param w Fensterbreite (In).
 * @param h Fensterhoehe (In).
 */
static void cbReshape(int w, int h) {
  /* Das ganze Fenster ist GL-Anzeigebereich */
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);

  /* Anpassen der Projektionsmatrix an das Seitenverhältnis des Fensters */
  setProjection((GLdouble)w / (GLdouble)h);

  glPointSize(14000.0f / glutGet(GLUT_WINDOW_WIDTH));
}

/**
 * Zeichen-Callback.
 * Loescht die Buffer, ruft das Zeichnen der Szene auf und tauscht den Front-
 * und Backbuffer.
 */
static void cbDisplay(void) {
  /* Buffer zuruecksetzen */
  glClear(GL_COLOR_BUFFER_BIT);

  /* Nachfolgende Operationen beeinflussen Modelviewmatrix */
  glMatrixMode(GL_MODELVIEW);
  /* Matrix zuruecksetzen - Einheitsmatrix laden */
  glLoadIdentity();

  /* Szene zeichnen */
  drawScene();

  /* Objekt anzeigen */
  /* glFlush (); */  /* fuer SingleBuffering */
  glutSwapBuffers(); /* fuer DoubleBuffering */
}

/**
 * Registrierung der GLUT-Callback-Routinen.
 */
static void registerCallbacks(void) {
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

  glutReshapeFunc(cbReshape);
}

/**
 * Initialisiert das Programm (inkl. I/O und OpenGL) und startet die
 * Ereignisbehandlung.
 * @param title Beschriftung des Fensters
 * @param width Breite des Fensters
 * @param height Hoehe des Fensters
 * @return ID des erzeugten Fensters, 0 im Fehlerfall
 */
int initAndStartIO(char *title, int width, int height) {
  int windowID = 0;

  /* Kommandozeile immitieren */
  int argc = 1;
  char *argv = "cmd";

  srand(time(0));

  Usage = GL_FALSE;
  Pause = GL_FALSE;

  // Erstes Level laden
  initLv('1');

  /* Glut initialisieren */
  glutInit(&argc, &argv);

  /* DEBUG-Ausgabe */
  INFO(("Erzeuge Fenster...\n"));

  /* Initialisieren des Fensters */
  /* glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB); */ /* fuer SingleBuffering */
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);        /* fuer DoubleBuffering */
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
