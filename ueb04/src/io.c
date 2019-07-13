/* ---- System Header einbinden ---- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#include "scene.h"
#include "texture.h"
#include "types.h"
#include "waterSimulation.h"

/* ---- Konstanten ---- */
#define ESC 27
#define ENTER 13
#define PLUS 43
#define MINUS 45

/** Anzahl der Aufrufe der Timer-Funktion pro Sekunde */
#define TIMER_CALLS_PS 60

/* ---- Globale Werte ---- */
extern GLboolean Pause;
extern GLboolean Debug;
extern GLboolean ShowSpheres;
extern GLboolean ShowNormals;
extern GLboolean ShowLights;

/**
 * Gibt die Anleitung auf dem Terminal aus
 */
static void printInstructions(void) {
  printf("--------------------\n");
  printf("Steuerung für Wassersimulation \n");
  printf("--------------------\n");

  printf("\n");
  printf("--------------------\n");
  printf("Mausrad: Kamera distanz verändern\n");
  printf("Pfeiltasten: Kamera bewegen\n");
  printf("--------------------\n");
  printf("Linke Maustaste: Wasserhöhe veringern\n");
  printf("Rechte Maustaste: Wassserhöhe erhöhen\n");
  printf("p|P: Simulation Pausieren\n");
  printf("s|S: Picking-Spheren Ein-/Ausblenden\n");
  printf("t|T: Texturen Ein-/Ausschalten\n");
  printf("+|-: Kugeln Anzahl Erhöhen/Verringern\n");
  printf("--------------------\n");
  printf("F1: Wireframe Modus\n");
  printf("F2: Normalen Anzeigen\n");
  printf("F3: Beleuchtung Ein-/Auschalten\n");
  printf("F4: Vollbildmodus\n");
  printf("d|D: Debug Infos Ein-/Ausblenden\n");
  printf("h|H: Anleitung ausgeben\n");
  printf("q|Q|ESC: Anwendung beenden\n");
  printf("--------------------\n");
  printf("\n");
}

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
static void togglePause(void) { Pause = !Pause; }

/**
 * Toggle für Anzeige von Debug-Infos (Overlay, XYZ-Achsen,...)
 */
static void toggleDebug(void) { Debug = !Debug; }

/**
 * Wechselt die Anzeige von Spheren
 */
static void toggleSpheres(void) { ShowSpheres = !ShowSpheres; }

/**
 * Toggle für die Anzeige von Normalen
 */
static void toggleNormals(void) { ShowNormals = !ShowNormals; }

/**
 * Toggle für die Beleuchtung
 */
static void toggleLights(void) { ShowLights = !ShowLights; }

/**
 * Toggle für die Texturierung
 */
static void toggleTextures(void) {
  int tex_status = getTexturingStatus();
  setTexturingStatus(!tex_status);
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
static void set3DViewport(GLint x, GLint y, GLint width, GLint height,
                          GLdouble lookAt[9]) {
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
  gluLookAt(lookAt[0], lookAt[1], lookAt[2],  /* Augpunkt */
            lookAt[3], lookAt[4], lookAt[5],  /* Zentrum */
            lookAt[6], lookAt[7], lookAt[8]); /* Up-Vektor */
}

/**
 * Verarbeitung der Picking-Ergebnisse.
 * Findet den Treffer, der dem Betrachter am naechsten liegt und gibt die
 * Namen dieses Treffers aus.
 * @param numHits Anzahl der getroffenen Objekte (In).
 * @param buffer Buffer, in dem die Treffer gespeichert sind (In).
 * @return GLuint Index der angeklicken Kugel
 */
static GLuint processHits(GLint numHits, GLuint buffer[]) {
  /* Anzahl der Namen des Treffers, der am naechsten zum Betrachter ist */
  GLint numOfClosestNames = 0;
  /* Anzahl der Namen des Treffers, der am naechsten zum Betrachter ist */
  GLuint *ptrClosestNames = NULL;
  /* Laufvariable - Nummer des akt. verarbeiteten Treffers */
  GLint i;
  /* Laufvariable - Zeiger auf den Beginn des Hit-Records */
  GLuint *ptr = (GLuint *)buffer;
  /* DepthBuffer - Wert */
  GLuint minZ = 0xffffffff;
  /* Rückgabewert */
  GLuint res = MAX_WATER + 10;

  if (numHits > 0) {
    /* Alle Treffer verarbeiten */
    for (i = 0; i < numHits; i++) {
      /* Ist der Treffer naeher dran, als der bisher naechste? */
      if (*(ptr + 1) < minZ) {
        /* Anzahl der Namen merken */
        numOfClosestNames = *ptr;
        /* Zeiger auf den ersten Namen merken */
        ptrClosestNames = ptr + 3;
        /* als neuen minimalen Tiefenwert merken */
        minZ = *(ptr + 1);
      }
      /* zum Beginn des naechsten Hit-Records springen */
      /* Schrittweite: Anzahl-Feld + minZ-Feld + maxZ-Feld + Namen-Felder */
      ptr += 3 + numOfClosestNames;
    }

    for (i = 0; i < numOfClosestNames; i++, ptrClosestNames++) {
      res = *ptrClosestNames;
    }
  }

  return res;
}
/**
 * Picking. Auswahl von Szenenobjekten durch Klicken mit der Maus.
 */
static void pick(int x, int y, GLboolean left) {
  /** Groesse des Buffers fuer Picking Ergebnisse */
#define SELECTBUFSIZE 512

  /* Viewport (Darstellungsbereich des Fensters) */
  GLint viewport[4];

  /* Puffer fuer Picking-Ergebnis */
  GLuint buffer[SELECTBUFSIZE];

  /* Anzahl der getroffenen Namen beim Picking */
  GLint numHits;

  /* aktuellen Viewport ermitteln */
  glGetIntegerv(GL_VIEWPORT, viewport);

  /* Puffer festlegen, Name-Stack leeren und Rendermode wechseln */
  glSelectBuffer(SELECTBUFSIZE, buffer);
  glInitNames();
  glRenderMode(GL_SELECT);

  int width = glutGet(GLUT_WINDOW_WIDTH);
  int height = glutGet(GLUT_WINDOW_HEIGHT);

  double aspect = (double)width / height;

  /* Folge Operationen beeinflussen die Projektionsmatrix */
  glMatrixMode(GL_PROJECTION);

  /* Einheitsmatrix laden */
  glLoadIdentity();

  gluPickMatrix(x, height - y, 5, 5, viewport);

  /* Perspektivische Darstellung */
  gluPerspective(70,     /* Öffnungswinkel */
                 aspect, /* Seitenverhältnis */
                 0.1,    /* nahe Clipping-Ebene */
                 100);   /* ferne Clipping-Ebene */

  /* Folge Operationen beeinflussen die Modelviewmatrix */
  glMatrixMode(GL_MODELVIEW);

  /* Einheitsmatrix laden */
  glLoadIdentity();

  GLdouble *lookAt = currentCamera();

  /* Kameraposition */
  gluLookAt(lookAt[0], lookAt[1], lookAt[2],  /* Augpunkt */
            lookAt[3], lookAt[4], lookAt[5],  /* Zentrum */
            lookAt[6], lookAt[7], lookAt[8]); /* Up-Vektor */

  /* drawScene(); */
  drawPickingScene();

  /* Zeichnen beenden und auswerten */
  glFlush();

  set3DViewport(0, 0,             /* x, y */
                width, height,    /* breite, hoehe */
                currentCamera()); /* Kamera */

  numHits = glRenderMode(GL_RENDER);

  /*   GLGETERROR; */

  GLuint res = processHits(numHits, buffer);

  if (res > MAX_WATER) {
    printf("Kein Hit");
  } else {
    if (left) {
      printf("Linker klick auf: %d \n", res);
    } else {
      printf("Rechter klick auf: %d \n", res);
    }
    pickHeightAtIndex(res, left);
  }
}

/**
 * Berechnung der Frames pro Sekunde.
 * @return aktuelle Framerate.
 */
static float frameRate(void) {
  /* Zeitpunkt der letzten Neuberechnung */
  static int timebase = 0;
  /* Anzahl der Aufrufe seit letzter Neuberechnung */
  static int frameCount = 0;
  /* Zuletzt berechneter FPS-Wert */
  static float fps = 0.0;
  /* aktuelle "Zeit" */
  static int time = 0;

  /* Diesen Aufruf hinzuzaehlen */
  frameCount++;

  /* seit dem Start von GLUT vergangene Zeit ermitteln */
  time = glutGet(GLUT_ELAPSED_TIME);

  /* Eine Sekunde ist vergangen */
  if (time - timebase > 1000) {
    /* FPS-Wert neu berechnen */
    fps = frameCount * 1000.0f / (time - timebase);

    /* Zureuecksetzen */
    timebase = time;
    frameCount = 0;
  }

  /* Aktuellen FPS-Wert zurueckgeben */
  return fps;
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
        toggleLights();
        break;
      case GLUT_KEY_F4:
        toggleFullScreen();
        break;

      /* Kamera bewegen */
      case GLUT_KEY_UP:
        camDirMove(dirUp);
        break;
      case GLUT_KEY_DOWN:
        camDirMove(dirDown);
        break;
      case GLUT_KEY_LEFT:
        camDirMove(dirLeft);
        break;
      case GLUT_KEY_RIGHT:
        camDirMove(dirRight);
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
        printInstructions();
        break;

        /* Kugeln ein-/ausblenden */
      case 's':
      case 'S':
        toggleSpheres();
        break;

      /* Programm pausieren */
      case 'p':
      case 'P':
        togglePause();
        break;

      case 't':
      case 'T':
        toggleTextures();
        break;

      case PLUS:
        resizeHeightArray(GL_TRUE);
        break;
      case MINUS:
        resizeHeightArray(GL_FALSE);
        break;

        /* Debug Anzeige wechseln */
      case 'd':
      case 'D':
        toggleDebug();
        break;
      }
    }
  }
}

static void handleMouseEvent(int key, int state, int x, int y) {
  if (state == GLUT_DOWN) {
    switch (key) {
    case GLUT_LEFT_BUTTON:
      pick(x, y, GL_TRUE);
      break;
    case GLUT_RIGHT_BUTTON:
      pick(x, y, GL_FALSE);
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

/* ----------------------------------------------------------------- */
/*   ____    _    _     _     ____    _    ____ _  ______   */
/*  / ___|  / \  | |   | |   | __ )  / \  / ___| |/ / ___|  */
/* | |     / _ \ | |   | |   |  _ \ / _ \| |   | ' /\___ \  */
/* | |___ / ___ \| |___| |___| |_) / ___ \ |___| . \ ___) | */
/*  \____/_/   \_\_____|_____|____/_/   \_\____|_|\_\____/  */
/* ----------------------------------------------------------------- */

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

  /* Wieder als Timer-Funktion registrieren */
  glutTimerFunc(1000 / TIMER_CALLS_PS, cbTimer, thisCallTime);

  /* Neuzeichnen anstossen */
  glutPostRedisplay();
}

static void cbMouseClick(int key, int state, int x, int y) {
  handleMouseEvent(key, state, x, y);
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
 * Zeichen-Callback.
 * Loescht die Buffer, ruft das Zeichnen der Szene auf und tauscht den Front-
 * und Backbuffer.
 */
static void cbDisplay(void) {
  static float fps = 0;
  /* Buffer zuruecksetzen */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Fensterdimensionen auslesen */
  int width = glutGet(GLUT_WINDOW_WIDTH);
  int height = glutGet(GLUT_WINDOW_HEIGHT);

  set3DViewport(0, 0,             /* x, y */
                width, height,    /* breite, hoehe */
                currentCamera()); /* Kamera */

  /* Szene zeichnen lassen */
  drawScene();

  glutSwapBuffers();

  fps = frameRate();
  char buf[10];
  gcvt(fps, 5, buf);

  char str[20] = "Framerate: ";
  glutSetWindowTitle(strcat(str, buf));
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

  glutIdleFunc(performWaterSimulation);

  /* Tasten-Druck-Callback - wird ausgefuehrt, wenn eine Taste gedrueckt wird
   */
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

  glutMouseFunc(cbMouseClick);
}

/* ----------------------------------------------------------------- */
/*  ___ _   _ ___ _____  */
/* |_ _| \ | |_ _|_   _| */
/*  | ||  \| || |  | |   */
/*  | || |\  || |  | |   */
/* |___|_| \_|___| |_|   */
/* ----------------------------------------------------------------- */

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

  /* Wichtige toogle Werte  */
  Pause = GL_FALSE;
  Debug = GL_FALSE;
  ShowNormals = GL_FALSE;
  ShowSpheres = GL_TRUE;
  ShowLights = GL_TRUE;

  /* Glut initialisieren */
  glutInit(&argc, &argv);

  /* DEBUG-Ausgabe */
  INFO(("Erzeuge Fenster...\n"));

  /* Initialisieren des Fensters */
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(width, height);
  glutInitWindowPosition(0, 0);

  initialiseHeightArray(WATER_INIT_FACTOR, 0.0f);

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
      INFO(("Lade und initialisiere Texturen...\n"));

      if (initTextures()) {
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
