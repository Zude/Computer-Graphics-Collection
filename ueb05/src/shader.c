/**
 * @file
 * Einfaches Beispielprogramm fuer OpenGL, GLUT & GLEW
 * sowie einen Shader in GLSL.
 * Vier texturierte Quadrate werden gezeigt.
 * @author copyright (C) Fachhochschule Wedel 1999-2018. All rights reserved.
 */

/* ---- System Header einbinden ---- */
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

/* ---- Eigene Header einbinden ---- */
#include "utility.h"
#include "debugGL.h"
#include "scene.h"

/* ---- Konstanten ---- */

/** Anzahl der Aufrufe der Timer-Funktion pro Sekunde */
#define TIMER_CALLS_PS 60

#define DEFAULT_WINDOW_WIDTH 600
#define DEFAULT_WINDOW_HEIGHT 600
/** Keycode der ESC-Taste */
#define ESC 27

extern GLboolean gPause;
extern GLboolean gSine;
extern GLboolean gTex;
extern GLboolean gHeightBool;
extern GLboolean gShadingModel;
extern GLboolean gDebug;


/* ---- Funktionen ---- */
static void printHelp(void){
  printf("---------------------------------\n");
  printf("-----------Anleitung-------------\n");
  printf("---------------------------------\n\n");
  printf("T : Texture toggle \n");
  printf("P : Pause toggle \n");
  printf("U : Heightmap toggle \n");
  printf("G : Shading toggle \n");
  printf("D : Debug toggle \n");
  printf("S : Sinuswelle toggle \n");
  printf("Q : Beenden \n");
  printf("F1 : Wireframe toggle \n");
  printf("F4 : Vollbild toggle \n");
  printf("---------------------------------\n");
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

/**
 * Initialisierung des OpenGL-Systems.
 * Setzt Shading, Hintergrund- und Zeichenfarbe.
 * @return Rueckgabewert: im Fehlerfall 0, sonst 1.
 */
static int
initOpenGL (void)
{
  /* DEBUG-Ausgabe */
  INFO (("Initialisiere OpenGL...\n"));

  /* Beim Aufruf von glClear werden alle Pixel des Framebuffers auf
   * diesen Wert gesetzt. */
  glClearColor(0, 0, 0, 0);

  /* Beim Aufrus von glClear werden alle Pixel des Depthbuffer auf
   * diesen Wert gesetzt */
  glClearDepth(1);

  /* Aktivieren des Tiefentests */
  glEnable(GL_DEPTH_TEST);

  /* Backfaceculling */
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);


  /* DEBUG-Ausgabe */
  INFO (("...fertig.\n\n"));

  /* Alles in Ordnung? */
  return 1;
}

/**
 * Timer-Callback.
 * Initiiert Berechnung der aktuellen Position und Farben und anschliessendes
 * Neuzeichnen, setzt sich selbst erneut als Timer-Callback.
 * @param lastCallTime Zeitpunkt, zu dem die Funktion als Timer-Funktion
 *   registriert wurde (In).
 */
static void
cbTimer (int lastCallTime)
{
	(void)lastCallTime;
  /* Seit dem Programmstart vergangene Zeit in Millisekunden */
  int thisCallTime = glutGet (GLUT_ELAPSED_TIME);

  /* Wieder als Timer-Funktion registrieren */
  glutTimerFunc (1000 / TIMER_CALLS_PS, cbTimer, thisCallTime);

  /* Neuzeichnen anstossen */
  glutPostRedisplay ();
}

/**
 * Zeichen-Callback.
 * Loescht die Buffer, ruft das Zeichnen der Szene auf und erzwingt die
 * Darstellung durch OpenGL.
 */
static void
cbDisplay (void)
{
  /* Buffer zuruecksetzen */
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Szene zeichnen */
  drawScene ();

  /* Objekt anzeigen */
  glutSwapBuffers();
}

/**
 * Debug-Callback.
 * @param type Art der Debug-Information. Einer der folgenden Werte:
 *     DEBUG_TYPE_ERROR_ARB
 *     DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB
 *     DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB
 *     DEBUG_TYPE_PORTABILITY_ARB
 *     DEBUG_TYPE_PERFORMANCE_ARB
 *     DEBUG_TYPE_OTHER_ARB
 * @param message Debug-Nachricht.
 */
#ifdef GLEW_CORRECT
static void
cbDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
  if (type == GL_DEBUG_TYPE_ERROR_ARB)
    {
      printf("ARB_debug_output: %s\n", message);
      exit(1);
    }
}
#endif

/**
 * Callback fuer Tastendruck.
 * Ruft Ereignisbehandlung fuer Tastaturereignis auf.
 * @param key betroffene Taste (In).
 * @param x x-Position der Maus zur Zeit des Tastendrucks (In).
 * @param y y-Position der Maus zur Zeit des Tastendrucks (In).
 */
static void
cbKeyboard (unsigned char key, int x, int y)
{
	(void)x;
	(void)y;
  switch (key)
  {

    case 't':
    case 'T':
      gTex = !gTex;
      break;

    case 'p':
    case 'P':
      gPause = !gPause;
      break;

    case 'u':
    case 'U':
      gHeightBool = !gHeightBool;
      break;

    case 'g':
    case 'G':
      gShadingModel = !gShadingModel;
      break;

    case 'd':
    case 'D':
      gDebug = !gDebug;
      break;

    case 'h':
    case 'H':
      printHelp();
      break;

    case 's':
    case 'S':
      gSine = !gSine;
      break;
    /* Programm beenden */
    case 'q':
    case 'Q':
    case ESC:
      exit (0);
      break;
  }
}
/**
 * Callback fuer Tastendruck von Speziellen Tasten.
 * Ruft Ereignisbehandlung fuer Tastaturereignis auf.
 * @param key betroffene Taste (In).
 * @param x x-Position der Maus zur Zeit des Tastendrucks (In).
 * @param y y-Position der Maus zur Zeit des Tastendrucks (In).
 */
static void
cbSpecial (int key, int x, int y){

  switch (key) {
    case GLUT_KEY_F1:
      toggleWireframeMode();
      glutPostRedisplay();
      break;
    case GLUT_KEY_F4:
      toggleFullScreen();
      break;
  }

}

/**
 * Registrierung der GLUT-Callback-Routinen.
 */
static void
registerCallbacks (void)
{
  /* DEBUG-Ausgabe */
  INFO (("Registriere Callbacks...\n"));

  /* Ob die extension ARB_debug_output bereit steht hängt vom Treiber
   * ab. Deswegen muss zur Laufzeit geprüft werden, ob das Callback
   * registriert werden kann. */
  if (GLEW_ARB_debug_output)
    {
      /* Verhindert, dass die OpenGL Implementierung das Callback
       * asynchron aufruft. Damit ist sichergestellt, dass der Callstack
       * im Falle eines Fehlers bereit steht.  */
      glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

      /* In den Rechenzentren ist im Sommersemester 2016 eine veraltete
       * GLEW-Version installiert. Die dort erwartete Funktions-
       * signatur des Callback-Handlers stimmt nicht mit der hier
       * implementierten überein. Wenn ihr (Studenten) auf euren
       * Laptops arbeitet, ist es jedoch durchaus möglich, dass dieser
       * Code fehlerfrei kompiliert. Definiert einfach mal das Makro
       * und testet das. */
#ifdef GLEW_CORRECT
      /* Wenn dieses Callback gesetzt ist, wird die übergebene Funktion
       * immer dann aufgerufen, wenn Fehler auftreten oder weitere
       * nennenswerte Informationen ausgegeben werden müssen. Ihr
       * müsst dann nicht immer glGetError aufrufen und bekommt
       * zusätzlich auch noch einige Zusatzinformationen, die das
       * Debuggen vereinfachen. */
      glDebugMessageCallback(cbDebugOutput, NULL);
#endif

      /* Aktivieren der Extension */
      glEnable(GL_DEBUG_OUTPUT);
    }

  /* Display-Callback (wird ausgefuehrt, wenn neu gezeichnet wird
   * z.B. nach Erzeugen oder Groessenaenderungen des Fensters) */
  glutDisplayFunc (cbDisplay);

  /* Tasten-Druck-Callback - wird ausgefuehrt, wenn eine Taste gedrueckt wird */
  glutKeyboardFunc (cbKeyboard);

  glutSpecialFunc (cbSpecial);

    /* Timer-Callback - wird einmalig nach msescs Millisekunden ausgefuehrt */
  glutTimerFunc (1000 / TIMER_CALLS_PS,         /* msecs - bis Aufruf von func */
                 cbTimer,                       /* func  - wird aufgerufen    */
                 glutGet (GLUT_ELAPSED_TIME));  /* value - Parameter, mit dem
                                                   func aufgerufen wird */

  /* DEBUG-Ausgabe */
  INFO (("...fertig.\n\n"));
}

/**
 * Erzeugt ein Fenster.
 * @param title Beschriftung des Fensters
 * @param width Breite des Fensters
 * @param height Hoehe des Fensters
 * @return ID des erzeugten Fensters, 0 im Fehlerfall
 */
static int
createWindow (char *title, int width, int height)
{
  int windowID = 0;

  /* DEBUG-Ausgabe */
  INFO (("Erzeuge Fenster...\n"));

  /* Initialisieren des Fensters */
  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize (width, height);
  glutInitWindowPosition (0, 0);

  /* Es wird explizit ein Context mit der Version 3.3 und dem Kern-
   * Profil angefordert. */
  glutInitContextVersion(3, 3);
  glutInitContextProfile(GLUT_CORE_PROFILE);

  windowID = glutCreateWindow (title);

  /* DEBUG-Ausgabe */
  INFO (("...fertig..\n\n"));

  return windowID;
}

/**
 * Hauptprogramm.
 * Initialisiert Fenster, Anwendung und Callbacks, startet glutMainLoop.
 * @param argc Anzahl der Kommandozeilenparameter (In).
 * @param argv Kommandozeilenparameter (In).
 * @return Rueckgabewert im Fehlerfall ungleich Null.
 */
int
main (int argc, char **argv)
{
  GLenum error;

  /* Glut initialisieren */
  glutInit (&argc, argv);


  gPause = GL_FALSE;
  gSine = GL_FALSE;
  gTex = GL_TRUE;
  gHeightBool = GL_TRUE;
  gShadingModel = GL_TRUE;


  /* Erzeugen des Fensters */
  if (!createWindow ("Mein erstes OpenGL3.3-Programm", DEFAULT_WINDOW_HEIGHT, DEFAULT_WINDOW_WIDTH))
    {
      fprintf (stderr, "Initialisierung des Fensters fehlgeschlagen!");
      exit (1);
    }

  /* Initialisieren von GLEW.
   * Wenn glewExperimental auf GL_TRUE gesetzt wird, läd GLEW
   * zusätzliche Extensions. In diesem konkreten Programm wird dies
   * beispielsweise benötigt, um glGenVertexArrays() aufzurufen. (Ohne
   * glewExperimental auf GL_TRUE zu setzen tritt ein Speicherzugriffs-
   * fehler auf.) */
  glewExperimental = GL_TRUE;
  error = glewInit();
  if (error != GLEW_OK)
    {
      fprintf (stderr, "Initialisierung von GLEW fehlgeschlagen!");
      exit (1);
    }

  /* Initialen OpenGL-Zustand setzen */
  if (!initOpenGL())
    {
      fprintf (stderr, "Initialisierung von OpenGL fehlgeschlagen!");
      exit (1);
    }

  /* registriere Callbacks */
  registerCallbacks ();

  /* Initialisiere Szene */
  initScene();

  /* Hauptschleife der Ereignisbehandlung starten */
  glutMainLoop ();

  return 0;
}
